
#include <Servo.h> 
#include "I2C.h"
#include "bma180.h"
#include "itg3200.h"
#include "triMath.h"

#define SERVO_PIN                  3

#define ACC_WEIGHT 0.080   // Accelerometer data weight relative to gyro's weight of 1
#define ACC_SCALE_WEIGHT 2   // Gradually drive accelerometer weight to zero. For example, a value of 5 here will drive the accelerometer weight to zero if the magnitude of the measured acceleration differs by more than 1/5 gravity.
#define MASTER_DT            7000   // 7000 us interval = 143 Hz master loop.
#define CONTROL_LOOP_INTERVAL   1   // 1x master.
#define ACC_READ_INTERVAL       5   // Read accelerometer every 5th loop.
#define COMM_LOOP_INTERVAL      5   // 1/5x master.
#define DOGLIFE 400   // Watchdog life in milliseconds.

int armCount;   // Arm status counter.
int loopCount;   // Count system loops.
int16_t pwmShift[4], pwmOut[4];   // 10 bit PWM output duty cycle.
float trimAngle[2];   // Trim.
float bodyDCM[3][3];   // Current body orientation calculated by IMU.
float targetAngPos[3], targetAngVel[3], pidAngPos[3], pidAngVel[3], currentAngPos[3];
float ang_pos_xy_cap, ang_vel_xy_cap;

float accScale;
float accWeight;   // Variable accelerometer weight.
//float accVar;   // Accelerometer variance for Kalman filter.

float aVec[3];   // Accelerometer read.
float aVecLast[3];   // Last accelerometer read.
float gVec[3];   // Gyro read.
float mVec[3];   // Magnetometer read.

float kbb[3];   // K body unit vector expressed in body coordinates.
float kgb[3];   // K global unit vector expressed in body coordinates.
float jgb[3];   // J global unit vector expressed in body coordinates.

float wA[3];   // Corrective rotation vector based on acceleration vector.
float wM[3];   // Corrective rotation vector based on magnetic north vector.
float wdt[3];    // Angular displacement vector = w * dt, where w is the angular velocity vector and dt is the time elapsed.

float gyroDCM[3][3];   // DCM based on gyro readings, corrected with wA.
float trimDCM[3][3];   // DCM used to offset gyroDCM to produce bodyDCM.
float dDCM[3][3];   // First used to store the change in DCM to update the current DCM. Repurposed during orthonormalization to store the correction vectors for the i and j unit vectors.
float errDCM;   // DCM error for which we need orthonormalization.


int16_t gZero[3];   // Zero values
//Servo myservo;  // create servo object to control a servo 
int i = 0; 

BMA180 acc;
ITG3200 gyro;

void setup() {
  // initialize the serial communication:
  Serial.begin(9600);
  I2c.begin();

  //GyroAndAcceleroInit(500);

  gyro.calibrate(500);

  //myservo.attach(SERVO_PIN);
  //myservo.write(pos);

  Serial.println("Sensors have been initialized"); 
} 


void IMUupdate() {
  // ========================================================================
  // Accelerometer
  //     Frame of reference: BODY
  //     Units: G (gravitational acceleration)
  //     Purpose: Measure the acceleration vector aVec with components
  //              codirectional with the i, j, and k vectors. Note that the
  //              gravitational vector is the negative of the K vector.
  // ========================================================================
#ifdef ACC_WEIGHT
  if (loopCount % ACC_READ_INTERVAL == 0) {
    acc.poll();
    for (int i=0; i<3; i++) {
      aVec[i] = acc.get(i);
    }

    // Take weighted average.
#ifdef ACC_SELF_WEIGHT
    for (int i=0; i<3; i++) {
      aVec[i] = ACC_SELF_WEIGHT * aVec[i] + (1-ACC_SELF_WEIGHT) * aVecLast[i];
      aVecLast[i] = aVec[i];

      // Kalman filtering?
      //aVecLast[i] = acc.get(i);
      //kalmanUpdate(aVec[i], accVar, aVecLast[i], ACC_UPDATE_SIG);
      //kalmanPredict(aVec[i], accVar, 0.0, ACC_PREDICT_SIG);
    }
#endif // ACC_SELF_WEIGHT
    accScale = vNorm(aVec);

    // Reduce accelerometer weight if the magnitude of the measured
    // acceleration is significantly greater than or less than 1 g.
    //
    // TODO: Magnitude of acceleration should be reported over telemetry so
    // the "cutoff" value (the constant before the ABS() below) for
    // disregaring acceleration input can be more accurately determined.
#ifdef ACC_SCALE_WEIGHT
    // For some reason, 1 gravity has magnitude of 1.05.
    accScale = (1.0 - MIN(1.0, ACC_SCALE_WEIGHT * ABS(accScale - 1.0)));
    accWeight = ACC_WEIGHT * accScale;
#else
    accWeight = ACC_WEIGHT;

    // Ignore accelerometer if it measures anything 0.5g past gravity.
    if (accScale > 1.5 || accScale < 0.5) {
      accWeight = 0;
    }
#endif // ACC_SCALE_WEIGHT

    // Uncomment the loop below to get accelerometer readings in order to
    // obtain trimAngle.
    //if (loopCount % COMM_LOOP_INTERVAL == 0) {
    //    sp("A(");
    //    sp(aVec[0]*1000); sp(", ");
    //    sp(aVec[1]*1000); sp(", ");
    //    sp(aVec[2]*1000);
    //    sp(")  ");
    //}

    // Express K global unit vector in BODY frame as kgb for use in drift
    // correction (we need K to be described in the BODY frame because
    // gravity is measured by the accelerometer in the BODY frame).
    // Technically we could just create a transpose of gyroDCM, but since
    // we don't (yet) have a magnetometer, we don't need the first two rows
    // of the transpose. This saves a few clock cycles.
    for (int i=0; i<3; i++) {
      kgb[i] = gyroDCM[i][2];
    }

    // Calculate gyro drift correction rotation vector wA, which will be
    // used later to bring KB closer to the gravity vector (i.e., the
    // negative of the K vector). Although we do not explicitly negate the
    // gravity vector, the cross product below produces a rotation vector
    // that we can later add to the angular displacement vector to correct
    // for gyro drift in the X and Y axes.
    vCrossP(kgb, aVec, wA);

    // Divide by ACC_READ_INTERVAL since the acceleration vector is not
    // updated every loop.
    for (int i=0; i<3; i++) {
      wA[i] /= ACC_READ_INTERVAL;
    }
  }
#endif // ACC_WEIGHT


  // ========================================================================
  // Gyroscope
  //     Frame of reference: BODY
  //     Units: rad/s
  //     Purpose: Measure the rotation rate of the body about the body's i,
  //              j, and k axes.
  // ========================================================================
  gyro.poll();
  for (int i=0; i<3; i++) {
    gVec[i] = gyro.get(i);
  }
  //if (loopCount % COMM_LOOP_INTERVAL == 0) {
  //    sp("G(");
  //    sp(gVec[0]); sp(", ");
  //    sp(gVec[1]); sp(", ");
  //    sp(gVec[2]);
  //    spln(")");
  //}

  // Scale gVec by elapsed time (in seconds) to get angle w*dt in
  // radians, then compute weighted average with the accelerometer and
  // magnetometer correction vectors to obtain final w*dt.
  for (int i=0; i<3; i++) {
    float numerator   = gVec[i] * MASTER_DT/1000000;
    float denominator = 1.0;

#ifdef ACC_WEIGHT
    if (loopCount % ACC_READ_INTERVAL == 0) {
      numerator   += accWeight * wA[i];
      denominator += accWeight;
    }
#endif // ACC_WEIGHT

    wdt[i] = numerator / denominator;
  }

  // ========================================================================
  // Direction Cosine Matrix
  //     Frame of reference: GLOBAL
  //     Units: None (unit vectors)
  //     Purpose: Calculate the components of the body's i, j, and k unit
  //              vectors in the global frame of reference.
  // ========================================================================
  // Skew the rotation vector and sum appropriate components by combining the
  // skew symmetric matrix with the identity matrix. The math can be
  // summarized as follows:
  //
  // All of this is calculated in the BODY frame. If w is the angular
  // velocity vector, let wdt (w*dt) be the angular displacement vector of
  // the DCM over a time interval dt. Let wdt_i, wdt_j, and wdt_k be the
  // components of wdt codirectional with the i, j, and k unit vectors,
  // respectively. Also, let dr be the linear displacement vector of the DCM
  // and dr_i, dr_j, and dr_k once again be the i, j, and k components,
  // respectively.
  //
  // In very small dt, certain vectors approach orthogonality, so we can
  // assume that (draw this out for yourself!):
  //
  //     dr_x = <    0,  dw_k, -dw_j>,
  //     dr_y = <-dw_k,     0,  dw_i>, and
  //     dr_z = < dw_j, -dw_i,     0>,
  //
  // which can be expressed as the rotation matrix:
  //
  //          [     0  dw_k -dw_j ]
  //     dr = [ -dw_k     0  dw_i ]
  //          [  dw_j -dw_i     0 ].
  //
  // This can then be multiplied by the current DCM and added to the current
  // DCM to update the DCM. To minimize the number of calculations performed
  // by the processor, however, we can combine the last two steps by
  // combining dr with the identity matrix to produce:
  //
  //              [     1  dw_k -dw_j ]
  //     dr + I = [ -dw_k     1  dw_i ]
  //              [  dw_j -dw_i     1 ],
  //
  // which we multiply with the current DCM to produce the updated DCM
  // directly.
  //
  // It may be helpful to read the Wikipedia pages on cross products and
  // rotation representation.
  // ========================================================================
  dDCM[0][0] =       1;
  dDCM[0][1] =  wdt[2];
  dDCM[0][2] = -wdt[1];
  dDCM[1][0] = -wdt[2];
  dDCM[1][1] =       1;
  dDCM[1][2] =  wdt[0];
  dDCM[2][0] =  wdt[1];
  dDCM[2][1] = -wdt[0];
  dDCM[2][2] =       1;

  // Multiply the current DCM with the change in DCM and update.
  mProduct(dDCM, gyroDCM, gyroDCM);
  IMUorthonormalize(gyroDCM);

#ifdef ACC_WEIGHT
  trimDCM[0][0] =             1;
  trimDCM[0][1] =             0;
  trimDCM[0][2] = -trimAngle[1];
  trimDCM[1][0] =             0;
  trimDCM[1][1] =             1;
  trimDCM[1][2] =  trimAngle[0];
  trimDCM[2][0] =  trimAngle[1];
  trimDCM[2][1] = -trimAngle[0];
  trimDCM[2][2] =             1;

  // Rotate gyroDCM with trimDCM.
  mProduct(trimDCM, gyroDCM, bodyDCM);
  //orthonormalize(bodyDCM);   // TODO: This shouldn't be necessary.
#else
  for (int i=0; i<3; i++) {
    for (int j=0; j<3; j++) {
      bodyDCM[i][j] = gyroDCM[i][j];
    }
  }
#endif // ACC_WEIGHT
}

void IMUorthonormalize(float inputDCM[3][3]) {
  // Takes 700 us.
  // Orthogonalize the i and j unit vectors (DCMDraft2 Eqn. 19).
  errDCM = vDotP(inputDCM[0], inputDCM[1]);
  vScale(inputDCM[1], -errDCM/2, dDCM[0]);   // i vector correction
  vScale(inputDCM[0], -errDCM/2, dDCM[1]);   // j vector correction
  vAdd(inputDCM[0], dDCM[0], inputDCM[0]);
  vAdd(inputDCM[1], dDCM[1], inputDCM[1]);

  // k = i x j
  vCrossP(inputDCM[0], inputDCM[1], inputDCM[2]);

  // Normalize all three vectors.
  vNorm(inputDCM[0]);
  vNorm(inputDCM[1]);
  vNorm(inputDCM[2]);
}


void loop() 
{ 
  //  AccelerometerRead(); 
  //  GyroRead(); 
  IMUupdate();
  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 3; i++){
      sp(bodyDCM[i][j]);
      sp(" ");
    }
    spn(" ");
  }
  delay(200); // slow down output   
}

unsigned long nextRuntime = micros();

void loop_(){
  delay(2000);
       for(int o = 0; o > 3; o++){
           for(int p = 0; p > 3; p++){
              Serial.print(gyroDCM[o][p]); 
           }
           Serial.println("");
        }
   
   
  if (micros() >= nextRuntime) {
    // ================================================================
    // System loop
    // ================================================================
    IMUupdate();   // Run this ASAP when loop starts so gyro integration is as accurate as possible.
    nextRuntime += MASTER_DT;   // Update next loop start time.

    // ================================================================
    // Func loop
    // ================================================================

    if (loopCount % COMM_LOOP_INTERVAL == 0) {
    }
    else {
      /* The pilot communicates with base and updates pwmOut
       * according to the joystick axis values it receives. */
    }

    if (loopCount % COMM_LOOP_INTERVAL == 1) {
    }
    else if (loopCount % COMM_LOOP_INTERVAL == 2) {
    }
    else if (loopCount % COMM_LOOP_INTERVAL == 3) {
    }
    else if (loopCount % COMM_LOOP_INTERVAL == 4) {
    }

    loopCount++;
    loopCount = loopCount % 100;
  } // endif
}



