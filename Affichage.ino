// dessine un bouton de menu avec un style différent s’il est sélectionné
void dessinerBoutonMenu(int x, int y, int w, int h, const char* texte, bool selectionne) {
  if (selectionne) {
    ecran.fillRect(x, y, w, h, SSD1306_WHITE);
    ecran.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  } else {
    ecran.drawRect(x, y, w, h, SSD1306_WHITE);
    ecran.setTextColor(SSD1306_WHITE);
  }

  ecran.setCursor(x + 6, y + 4);
  ecran.print(texte);

  ecran.setTextColor(SSD1306_WHITE);
}

// affichage de l’écran d’accueil
void dessinerMenuPrincipal() {
  ecran.clearDisplay();
  ecran.setTextSize(1);
  ecran.setTextColor(SSD1306_WHITE);

  ecran.drawBitmap(0, 0, epd_bitmap_cl__de_sol, 24, 24, SSD1306_WHITE);
  ecran.drawBitmap(104, 0, epd_bitmap_cl__de_sol, 24, 24, SSD1306_WHITE);

  ecran.setTextSize(2);
  ecran.setCursor(40, 4);
  ecran.print("JAMA");
  ecran.setCursor(41, 4);
  ecran.print("JAMA");
  ecran.setTextSize(1);

  dessinerBoutonMenu(12, 31, 104, 14, "CREER NOUVEAU", choixMenuPrincipal == 0);
  dessinerBoutonMenu(12, 49, 104, 14, "CHARGER BOUCLE", choixMenuPrincipal == 1);

  ecran.display();
}

// affiche l’écran où on choisit le nom de la sauvegarde
void dessinerClavierSauvegarde() {
  ecran.clearDisplay();
  ecran.setTextSize(1);
  ecran.setTextColor(SSD1306_WHITE);

  ecran.setCursor(0, 0);
  ecran.print("NOMMER + ENREGISTRER");

  ecran.setTextSize(2);
  ecran.setCursor(15, 20);

  for (int i = 0; i < 8; i++) {
    ecran.print(nomFichierSaisie[i]);
  }

  ecran.setTextSize(1);
  ecran.setCursor(15 + (indexLettreSaisie * 12), 34);
  ecran.print("^");

  ecran.setCursor(0, 46);
  ecran.print("[P/P] = NON SAVE\n[Clic L] = SAUVEGARDE");

  ecran.display();
}

// affiche la liste des boucles enregistrées
void dessinerMenuChargement() {
  ecran.clearDisplay();
  ecran.setTextSize(1);
  ecran.setTextColor(SSD1306_WHITE);

  ecran.setCursor(8, 2);
  ecran.print("GERER LES BOUCLES");

  if (indexFichierACharger == totalFichiersTrouves || totalFichiersTrouves == 0) {
    dessinerBoutonMenu(10, 24, 108, 16, "RETOUR MENU", true);
  } else {
    dessinerBoutonMenu(6, 18, 116, 16, listeFichiersSD[indexFichierACharger].c_str(), true);

    ecran.setCursor(26, 42);
    ecran.print("Fichier ");
    ecran.print(indexFichierACharger + 1);
    ecran.print("/");
    ecran.print(totalFichiersTrouves);

    ecran.drawRect(18, 52, 92, 10, SSD1306_WHITE);
    ecran.setCursor(22, 53);
    ecran.print("CLIC L = SUPPR");
  }

  ecran.display();
}

// écran de confirmation avant suppression d’une boucle
void dessinerConfirmSuppr() {
  ecran.clearDisplay();
  ecran.setTextSize(1);
  ecran.setTextColor(SSD1306_WHITE);

  ecran.setCursor(6, 2);
  ecran.print("SUPPRIMER LE FICHIER ?");

  ecran.setCursor(8, 14);
  ecran.print(listeFichiersSD[indexFichierACharger]);

  dessinerBoutonMenu(8, 32, 112, 12, "ANNULER", choixConfirmSuppr == 0);
  dessinerBoutonMenu(8, 48, 112, 12, "SUPPRIMER", choixConfirmSuppr == 1);

  ecran.display();
}

// affiche la grille principale du séquenceur
void dessinerSequenceur() {
  ecran.clearDisplay();
  ecran.setTextSize(1);
  ecran.setTextColor(SSD1306_WHITE);

  ecran.setCursor(0, 0);
  ecran.print("PISTE:");
  ecran.println(nomFichiers[pisteActive]);

  if (sequenceurEnLecture) {
    ecran.fillTriangle(116, 2, 116, 12, 126, 7, SSD1306_WHITE);
  } else {
    ecran.fillRect(116, 2, 3, 10, SSD1306_WHITE);
    ecran.fillRect(123, 2, 3, 10, SSD1306_WHITE);
  }

  ecran.setCursor(0, 12);

  if (modeActuel == REVERB_FX) {
    ecran.print("RVB");
    ecran.drawRect(25, 12, 45, 7, SSD1306_WHITE);

    int largeurReverb = niveauReverb * 43;
    ecran.fillRect(26, 13, largeurReverb, 5, SSD1306_WHITE);

    ecran.setCursor(76, 12);
    ecran.print("[FX RVB]");
  } else if (modeActuel == BITCRUSHER_FX) {
    ecran.print("CRU");
    ecran.drawRect(25, 12, 45, 7, SSD1306_WHITE);

    int largeurBitcrush = niveauBitcrush * 43;
    ecran.fillRect(26, 13, largeurBitcrush, 5, SSD1306_WHITE);

    ecran.setCursor(76, 12);
    ecran.print("[FX CRU]");
  } else if (modeActuel == DECLENCHE_SAUVEGARDE) {
    ecran.print("SAVE");

    ecran.setCursor(76, 12);
    ecran.print("[CLIC L]");
  } else {
    ecran.print("VOL");
    ecran.drawRect(25, 12, 45, 7, SSD1306_WHITE);

    int largeurVolume = volumeAffichage * 43;
    ecran.fillRect(26, 13, largeurVolume, 5, SSD1306_WHITE);

    ecran.setCursor(76, 12);
    ecran.print("[NAVIG]");
  }

  int tailleCase = 10;
  int espace = 4;
  int departX = 8;
  int departY = 30;

  for (int i = 0; i < nbEtapes; i++) {
    int x = departX + i * (tailleCase + espace);

    if (grille[pisteActive][i]) ecran.fillRect(x, departY, tailleCase, tailleCase, SSD1306_WHITE);
    else                        ecran.drawRect(x, departY, tailleCase, tailleCase, SSD1306_WHITE);

    if (i == etapeSelectionnee) {
      ecran.drawLine(x, departY + tailleCase + 3, x + tailleCase, departY + tailleCase + 3, SSD1306_WHITE);
    }

    if (i == etapeCourante) {
      ecran.fillCircle(x + tailleCase / 2, departY - 4, 2, SSD1306_WHITE);
    }
  }

  ecran.setCursor(0, 54);
  ecran.print("Step:");
  ecran.print(etapeSelectionnee + 1);

  ecran.setCursor(55, 54);
  ecran.print("BPM:");
  ecran.print(tempoBPM);

  ecran.display();
}