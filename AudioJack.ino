// GESTION AUTO JACK / HAUT-PARLEUR
// prise jack branchée ?
bool jackBranche() {
  int etat = digitalRead(pinDetectJack);

  if (JACK_ACTIVE_LOW) {
    return etat == LOW;
  } else {
    return etat == HIGH;
  }
}

// active ou coupe le haut-parleur selon la présence du jack
void gererSortiesAudio(bool force) {
  bool branche = jackBranche();

  if (force || branche != dernierEtatJack) {
    dernierEtatJack = branche;

    // la sortie casque/jack reste toujours active
    sgtl5000.unmuteHeadphone();
    sgtl5000.volume(0.75);

    if (branche) {
      // jack branche: on coupe seulement le haut-parleur interne / LINE OUT.
      sgtl5000.muteLineout();

      Serial.println("Jack branche : prise jack active, haut-parleur interne coupe");
    } else {
      // jack debranche: le haut-parleur interne reste actif.
      sgtl5000.unmuteLineout();
      sgtl5000.lineOutLevel(13);

      Serial.println("Jack debranche : prise jack active, haut-parleur interne actif");
    }
  }
}