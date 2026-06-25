// retour vers le menu principal
void retourMenuPrincipalDepuisSequenceur() {
  sequenceurEnLecture = false;
  modeActuel = NAVIGATION_STEPS;
  etatCourant = ETAT_MENU_PRINCIPAL;
  dessinerMenuPrincipal();
}

// action à faire quand le clic court est validé
void traiterClicCourtEncodeur() {
  // depuis le menu principal, le clic sert à choisir une action
  if (etatCourant == ETAT_MENU_PRINCIPAL) {
    if (choixMenuPrincipal == 0) {
      for (int p = 0; p < nbBoutons; p++) {
        for (int e = 0; e < nbEtapes; e++) {
          grille[p][e] = false;
        }
      }
 
      niveauReverb = 0.20;
      niveauBitcrush = 0.0;
      tempoBPM = 120;
      tempoMs = 60000UL / (tempoBPM * 4);
 
      appliquerParametresAudio();
 
      etatCourant = ETAT_SEQUENCEUR;
      sequenceurEnLecture = true;
      modeActuel = NAVIGATION_STEPS;
 
      dessinerSequenceur();
    } else {
      scannerCarteSD();
      indexFichierACharger = 0;
      etatCourant = ETAT_CHARGEMENT;
      dessinerMenuChargement();
    }
  }
 
  // depuis le chargement, le clic charge le fichier sélectionné
  else if (etatCourant == ETAT_CHARGEMENT) {
    if (indexFichierACharger == totalFichiersTrouves || totalFichiersTrouves == 0) {
      etatCourant = ETAT_MENU_PRINCIPAL;
      dessinerMenuPrincipal();
    } else {
      chargerGrilleDepuisSD(listeFichiersSD[indexFichierACharger]);
 
      etatCourant = ETAT_SEQUENCEUR;
      sequenceurEnLecture = true;
      modeActuel = NAVIGATION_STEPS;
 
      dessinerSequenceur();
    }
  }
  
  // confirme ou on annule la suppression
  else if (etatCourant == ETAT_CONFIRM_SUPPR) {
    if (choixConfirmSuppr == 1) {
      SD.remove(listeFichiersSD[indexFichierACharger].c_str());
      scannerCarteSD();
      indexFichierACharger = 0;
    }
 
    etatCourant = ETAT_CHARGEMENT;
    dessinerMenuChargement();
  }
 
  // pendant la sauvegarde, le clic passe à la lettre suivante
  else if (etatCourant == ETAT_SAUVEGARDE) {
    indexLettreSaisie++;
 
    if (indexLettreSaisie >= 8) indexLettreSaisie = 0;
 
    dessinerClavierSauvegarde();
  }
 
  // dans le séquenceur, le clic active ou désactive une case
  else if (etatCourant == ETAT_SEQUENCEUR) {
    if (modeActuel == DECLENCHE_SAUVEGARDE) {
      sequenceurEnLecture = false;
      etatCourant = ETAT_SAUVEGARDE;
      indexLettreSaisie = 0;
 
      dessinerClavierSauvegarde();
    }
 
    else if (modeActuel == NAVIGATION_STEPS) {
      grille[pisteActive][etapeSelectionnee] = !grille[pisteActive][etapeSelectionnee];
      dessinerSequenceur();
    }
  }
}