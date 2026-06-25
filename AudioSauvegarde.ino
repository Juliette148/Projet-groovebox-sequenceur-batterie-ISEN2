// met à jour les effets audio (réverbération et bitcrusher)
// selon les valeurs actuellement sélectionnées
void appliquerParametresAudio() {
  mixerFinal.gain(0, 1.0 - (niveauReverb * 0.40));
  mixerFinal.gain(1, niveauReverb * 0.65);

  if (niveauBitcrush == 0.0) {
    bitcrusher.bits(16);
    bitcrusher.sampleRate(44100);
  } else {
    int depthBits = map(niveauBitcrush * 100, 0, 100, 16, 4);
    int sampleRateHz = map(niveauBitcrush * 100, 0, 100, 44100, 2000);

    bitcrusher.bits(depthBits);
    bitcrusher.sampleRate(sampleRateHz);
  }
}

// lance le fichier audio qui correspond à la piste choisie
void jouerSon(int i) {
  switch (i) {
    case 0: playWav1.play(nomFichiers[0]); break;
    case 1: playWav2.play(nomFichiers[1]); break;
    case 2: playWav3.play(nomFichiers[2]); break;
    case 3: playWav4.play(nomFichiers[3]); break;
  }
}

// cherche les fichiers de sauvegarde disponibles sur la carte SD
void scannerCarteSD() {
  totalFichiersTrouves = 0;

  File racine = SD.open("/");

  while (true) {
    File entree = racine.openNextFile();

    if (!entree) break;

    if (!entree.isDirectory()) {
      String nomStr = entree.name();

      if (nomStr.endsWith(".TXT") || nomStr.endsWith(".txt")) {
        if (totalFichiersTrouves < 10) {
          listeFichiersSD[totalFichiersTrouves] = nomStr;
          totalFichiersTrouves++;
        }
      }
    }

    entree.close();
  }

  racine.close();
}

// enregistre la grille actuelle dans un fichier texte
void sauvegarderGrilleSurSD(const char* nomFichier) {
  String nomComplet = String(nomFichier);
  nomComplet.trim();
  nomComplet += ".txt";

  SD.remove(nomComplet.c_str());

  File fichierSauvegarde = SD.open(nomComplet.c_str(), FILE_WRITE);

  if (fichierSauvegarde) {
    for (int p = 0; p < nbBoutons; p++) {
      for (int e = 0; e < nbEtapes; e++) {
        fichierSauvegarde.print(grille[p][e] ? "1" : "0");
      }
      fichierSauvegarde.println();
    }

    fichierSauvegarde.println(tempoBPM);
    fichierSauvegarde.print(niveauReverb);
    fichierSauvegarde.print(" ");
    fichierSauvegarde.println(niveauBitcrush);

    fichierSauvegarde.close();
  }
}

// recharge une boucle sauvegardée depuis la carte SD
void chargerGrilleDepuisSD(String nomFichier) {
  File fichierSauvegarde = SD.open(nomFichier.c_str());

  if (fichierSauvegarde) {
    for (int p = 0; p < nbBoutons; p++) {
      String ligne = fichierSauvegarde.readStringUntil('\n');
      ligne.trim();

      for (int e = 0; e < nbEtapes; e++) {
        if (e < (int)ligne.length()) {
          grille[p][e] = (ligne.charAt(e) == '1');
        }
      }
    }

    if (fichierSauvegarde.available()) {
      String ligneTempo = fichierSauvegarde.readStringUntil('\n');
      ligneTempo.trim();

      if (ligneTempo.length() > 0) {
        tempoBPM = ligneTempo.toInt();
        tempoMs = 60000UL / (tempoBPM * 4);
      }
    }

    if (fichierSauvegarde.available()) {
      niveauReverb = fichierSauvegarde.parseFloat();
      niveauBitcrush = fichierSauvegarde.parseFloat();
      appliquerParametresAudio();
    }

    fichierSauvegarde.close();
  }
}