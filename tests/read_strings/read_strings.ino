//--- lecture d'un nombre reçu par le Terminal et positionnement du servomoteur --- 
  if (Serial.available()>0) { // si caractère dans la file d'attente

    //---- lecture du nombre reçu 
    while (Serial.available()>0) { // tant que buffer pas vide pour lire d'une traite tous les caractères reçus

      ReceptionOctet= Serial.read(); // renvoie le 1er octet présent dans la file attente série (-1 si aucun) 
      ReceptionOctet=ReceptionOctet-48; // transfo valeur ASCII en valeur décimale

      if ((ReceptionOctet>=0)&&(ReceptionOctet<=9))     ReceptionNombre = (ReceptionNombre*10)+ReceptionOctet; 
      // si valeur reçue correspond à un chiffre on calcule nombre

      delay(1); // pause pour laisser le temps au Serial.available de recevoir les caractères

    } // fin while
  }
