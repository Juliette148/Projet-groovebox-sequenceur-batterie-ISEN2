#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Encoder.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
 
// dimensions de l’écran OLED
#define LARGEUR_ECRAN 128
#define HAUTEUR_ECRAN 64
 
// création de l'objet pour l'écran
Adafruit_SSD1306 ecran(LARGEUR_ECRAN, HAUTEUR_ECRAN, &Wire, -1);

//AUDIO
// les 4 lecteurs servent à lancer plusieurs sons en même temps
AudioPlaySdWav         playWav1;
AudioPlaySdWav         playWav2;
AudioPlaySdWav         playWav3;
AudioPlaySdWav         playWav4;
 
// chaîne audio: mixage, effets puis sortie vers la carte audio
AudioMixer4            mixer;
AudioEffectBitcrusher  bitcrusher;
AudioEffectFreeverb    reverb;
AudioMixer4            mixerFinal;
 
AudioOutputI2S         i2s;
AudioControlSGTL5000   sgtl5000;
 
AudioConnection        patchCord1(playWav1, 0, mixer, 0);
AudioConnection        patchCord2(playWav2, 0, mixer, 1);
AudioConnection        patchCord3(playWav3, 0, mixer, 2);
AudioConnection        patchCord4(playWav4, 0, mixer, 3);
 
AudioConnection        patchCordBit(mixer, 0, bitcrusher, 0);
AudioConnection        patchCord5(bitcrusher, 0, mixerFinal, 0);
AudioConnection        patchCord6(bitcrusher, 0, reverb, 0);
AudioConnection        patchCord7(reverb, 0, mixerFinal, 1);
 
AudioConnection        patchCord8(mixerFinal, 0, i2s, 0);
AudioConnection        patchCord9(mixerFinal, 0, i2s, 1);
 
//DETECTION PRISE JACK
// broche DETECT/SWITCH de la prise jack vers pin 33
// GND de la prise jack vers GND Teensy
const int pinDetectJack = 33;
 
// avec INPUT_PULLUP :
// jack non branché = HIGH
// jack branché     = LOW
const bool JACK_ACTIVE_LOW = true;
 
int dernierEtatJack = -1;
 
//BOUTONS
// chaque bouton correspond à une piste/un son
const int nbBoutons = 4;
const int pinsBoutons[nbBoutons] = {24, 26, 28, 30};
bool dernierEtat[nbBoutons] = {HIGH, HIGH, HIGH, HIGH};
 
//BOUTON PLAY/PAUSE
// bouton séparé pour démarrer ou arrêter la lecture
const int pinBoutonPlayPause = 32;
bool sequenceurEnLecture = false;
 
//POTENTIOMÈTRES
// potentiomètres pour régler le volume et le tempo
const int pinPotVolume = A2;
const int pinPotTempo  = A1;
 
float dernierVolume = -1.0;
float volumeAffichage = 0.5;
 
//ENCODEUR
// encodeur rotatif utilisé pour naviguer dans les différents menus de l'interface
Encoder monEncodeur(39, 40);
long anciennePosition = 0;
 
const int pinClicEncodeur = 37;
bool dernierEtatClic = HIGH;
 
//MACHINE À ÉTATS GLOBAUX
// les différents écrans/états du programme
enum EtatMachine {
  ETAT_MENU_PRINCIPAL,
  ETAT_SEQUENCEUR,
  ETAT_SAUVEGARDE,
  ETAT_CHARGEMENT,
  ETAT_CONFIRM_SUPPR
};
 
EtatMachine etatCourant = ETAT_MENU_PRINCIPAL;
 
// modes internes au séquenceur
// modes disponibles quand on est dans le séquenceur
enum ModeSelection {
  NAVIGATION_STEPS,
  REVERB_FX,
  BITCRUSHER_FX,
  DECLENCHE_SAUVEGARDE
};
 
ModeSelection modeActuel = NAVIGATION_STEPS;
 
unsigned long tempsAppuiClic = 0;
const unsigned long delaiDoubleClic = 350;
 
bool clicCourtEnAttente = false;
unsigned long tempsDernierClicCourt = 0;
 
float niveauReverb = 0.20;
float niveauBitcrush = 0.0;
 
//FICHIERS AUDIO
// noms des fichiers audio présents sur la carte SD
const char* nomFichiers[nbBoutons] = {
  "kick.wav",
  "clap.wav",
  "hihat.wav",
  "snare.wav"
};
 
//SÉQUENCEUR
// la boucle contient 8 étapes
const int nbEtapes = 8;
 
// grille du séquenceur: une ligne par son, une colonne par étape
bool grille[nbBoutons][nbEtapes];
 
int pisteActive = 0;
int etapeSelectionnee = 0;
int etapeCourante = 0;
 
unsigned long dernierTempsEtape = 0;
 
int tempoBPM = 120;
unsigned long tempoMs = 60000UL / (tempoBPM * 4);
 
//VARIABLES DE SAUVEGARDE
// nom par défaut proposé pour sauvegarder une boucle
char nomFichierSaisie[9] = "LOOP01  ";
 
int indexLettreSaisie = 0;
int choixMenuPrincipal = 0;
int indexFichierACharger = 0;
 
String listeFichiersSD[10];
int totalFichiersTrouves = 0;
 
int choixConfirmSuppr = 0; // 0 = ANNULER, 1 = SUPPRIMER
 
//IMAGE MENU: CLÉ DE SOL
const unsigned char epd_bitmap_cl__de_sol [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x12, 0x00, 0x00, 0x12, 0x00, 0x00,
  0x16, 0x00, 0x00, 0x14, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x38, 0x00, 0x00, 0x70, 0x00, 0x00, 0xe0,
  0x00, 0x00, 0xcc, 0x00, 0x00, 0x9f, 0x00, 0x01, 0xb1, 0x00, 0x00, 0xa1, 0x80, 0x00, 0x95, 0x00,
  0x00, 0x41, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x72, 0x00, 0x00,
  0x32, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};
 
const int epd_bitmap_allArray_LEN = 1;
const unsigned char* epd_bitmap_allArray[1] = {
  epd_bitmap_cl__de_sol
};

bool jackBranche();
void gererSortiesAudio(bool force = false);
void jouerSon(int i);
void appliquerParametresAudio();
void scannerCarteSD();
void sauvegarderGrilleSurSD(const char* nomFichier);
void retourMenuPrincipalDepuisSequenceur();
void traiterClicCourtEncodeur();
void chargerGrilleDepuisSD(String nomFichier);

//SETUP
 // fonction lancée une seule fois au démarrage
void setup() {
  // ouverture du port série pour afficher quelques messages de debug
  Serial.begin(9600);
  delay(1000);
 
  analogReadResolution(10);
 
  // mise en entrée des boutons avec résistance interne
  for (int i = 0; i < nbBoutons; i++) {
    pinMode(pinsBoutons[i], INPUT_PULLUP);
  }
 
  pinMode(pinClicEncodeur, INPUT_PULLUP);
  pinMode(pinBoutonPlayPause, INPUT_PULLUP);
 
  // détection jack
  pinMode(pinDetectJack, INPUT_PULLUP);
 
  for (int p = 0; p < nbBoutons; p++) {
    for (int e = 0; e < nbEtapes; e++) {
      grille[p][e] = false;
    }
  }
 
  // démarrage de l’écran OLED
  if (!ecran.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("ERREUR : OLED !");
  }
 
  AudioMemory(40);
 
  // initialisation de l'Audio Shield
  if (!sgtl5000.enable()) {
    Serial.println("ERREUR : Audio Shield SGTL5000 non detecte !");
    while (1);
  }
 
  // volume de la prise jack/sortie casque
  sgtl5000.volume(0.75);
  sgtl5000.unmuteHeadphone();
 
  // sortie line out pour le mini-ampli/haut-parleur
  sgtl5000.unmuteLineout();
  sgtl5000.lineOutLevel(13);
 
  // applique directement le bon mode:
  // jack branché = HP coupé
  // jack débranché = HP actif
  gererSortiesAudio(true);
 
  mixer.gain(0, 0.5);
  mixer.gain(1, 0.5);
  mixer.gain(2, 0.5);
  mixer.gain(3, 0.5);
 
  bitcrusher.bits(16);
  bitcrusher.sampleRate(44100);
 
  mixerFinal.gain(0, 0.95);
  mixerFinal.gain(1, 0.20);
 
  reverb.roomsize(0.75);
  reverb.damping(0.10);
 
  // la carte SD doit être disponible pour les sons et les sauvegardes
  if (!SD.begin(BUILTIN_SDCARD)) {
    Serial.println("ERREUR : SD non trouvee !");
    while (1);
  }
 
  monEncodeur.write(0);
  anciennePosition = 0;
 
  dessinerMenuPrincipal();
}

//LOOP
// boucle principale: elle tourne en continu
void loop() {
  // vérifie en permanence si une enceinte/prise jack est branchée
  gererSortiesAudio();
 
  //les réglages en temps réel se font seulement dans le séquenceur
  if (etatCourant == ETAT_SEQUENCEUR) {
    int valeurPotTempo = analogRead(pinPotTempo);
    int nouveauTempoBPM = map(valeurPotTempo, 0, 1023, 60, 180);
 
    if (nouveauTempoBPM != tempoBPM) {
      tempoBPM = nouveauTempoBPM;
      tempoMs = 60000 / (tempoBPM * 4);
 
      dessinerSequenceur();
    }
 
    unsigned long tempsActuel = millis();
 
    // si temps est écoulé, on passe à l’étape suivante
    if (sequenceurEnLecture && tempsActuel - dernierTempsEtape >= tempoMs) {
      dernierTempsEtape = tempsActuel;
 
      etapeCourante++;
 
      if (etapeCourante >= nbEtapes) {
        etapeCourante = 0;
      }
 
      // on regarde quelles pistes doivent jouer sur cette étape
      for (int p = 0; p < nbBoutons; p++) {
        if (grille[p][etapeCourante]) {
          jouerSon(p);
        }
      }
 
      dessinerSequenceur();
    }
 
    // lecture du potentiomètre de volume
    int valeurPotVolume = analogRead(pinPotVolume);
    float volumeActuel = valeurPotVolume / 1023.0;
 
    if (volumeActuel < 0.02) volumeActuel = 0.0;
    if (volumeActuel > 1.0)  volumeActuel = 1.0;
 
    if (abs(volumeActuel - dernierVolume) > 0.02) {
      mixer.gain(0, volumeActuel);
      mixer.gain(1, volumeActuel);
      mixer.gain(2, volumeActuel);
      mixer.gain(3, volumeActuel);
 
      dernierVolume = volumeActuel;
      volumeAffichage = volumeActuel;
 
      dessinerSequenceur();
    }
  }
 
  //BOUTON PLAY/PAUSE
  static bool boutonDejaAppuye = false;
 
  bool etatPlayPauseActuel = digitalRead(pinBoutonPlayPause);
 
  if (etatPlayPauseActuel == LOW && !boutonDejaAppuye) {
    boutonDejaAppuye = true;
 
    if (etatCourant == ETAT_SAUVEGARDE) {
      modeActuel = NAVIGATION_STEPS;
      etatCourant = ETAT_SEQUENCEUR;
      sequenceurEnLecture = true;
 
      dessinerSequenceur();
    }
 
    else if (etatCourant == ETAT_SEQUENCEUR) {
      sequenceurEnLecture = !sequenceurEnLecture;
 
      if (sequenceurEnLecture) {
        dernierTempsEtape = millis();
      }
 
      dessinerSequenceur();
    }
 
    delay(200);
  }
 
  if (etatPlayPauseActuel == HIGH) {
    boutonDejaAppuye = false;
  }
 
  //ENCODEUR: ROTATIONS
  // lecture de la rotation de l’encodeur
  long nouvellePosition = monEncodeur.read();
  long ecart = nouvellePosition - anciennePosition;
 
  // écart
  if (abs(ecart) >= 4) {
    int direction = (ecart > 0) ? 1 : -1;
    anciennePosition = nouvellePosition;
 
    if (etatCourant == ETAT_MENU_PRINCIPAL) {
      choixMenuPrincipal = (choixMenuPrincipal == 0) ? 1 : 0;
      dessinerMenuPrincipal();
    }
 
    else if (etatCourant == ETAT_CHARGEMENT) {
      if (totalFichiersTrouves == 0) {
        indexFichierACharger = 0;
      } else {
        indexFichierACharger += direction;
 
        if (indexFichierACharger < 0) {
          indexFichierACharger = totalFichiersTrouves;
        }
 
        if (indexFichierACharger > totalFichiersTrouves) {
          indexFichierACharger = 0;
        }
      }
 
      dessinerMenuChargement();
    }
 
    else if (etatCourant == ETAT_CONFIRM_SUPPR) {
      choixConfirmSuppr = (choixConfirmSuppr == 0) ? 1 : 0;
      dessinerConfirmSuppr();
    }
 
    else if (etatCourant == ETAT_SAUVEGARDE) {
      char c = nomFichierSaisie[indexLettreSaisie];
 
      if (direction == 1) {
        if (c == ' ') c = 'A';
        else if (c == 'Z') c = '0';
        else if (c == '9') c = ' ';
        else c++;
      } else {
        if (c == ' ') c = '9';
        else if (c == '0') c = 'Z';
        else if (c == 'A') c = ' ';
        else c--;
      }
 
      nomFichierSaisie[indexLettreSaisie] = c;
 
      dessinerClavierSauvegarde();
    }
 
    else if (etatCourant == ETAT_SEQUENCEUR) {
      if (modeActuel == REVERB_FX) {
        niveauReverb += direction * 0.05;
 
        if (niveauReverb < 0.0) niveauReverb = 0.0;
        if (niveauReverb > 1.0) niveauReverb = 1.0;
 
        appliquerParametresAudio();
        dessinerSequenceur();
      }
 
      else if (modeActuel == BITCRUSHER_FX) {
        niveauBitcrush += direction * 0.05;
 
        if (niveauBitcrush < 0.0) niveauBitcrush = 0.0;
        if (niveauBitcrush > 1.0) niveauBitcrush = 1.0;
 
        appliquerParametresAudio();
        dessinerSequenceur();
      }
 
      else if (modeActuel == NAVIGATION_STEPS) {
        if (direction == 1 && etapeSelectionnee < nbEtapes - 1) {
          etapeSelectionnee++;
        } else if (direction == -1 && etapeSelectionnee > 0) {
          etapeSelectionnee--;
        }
 
        dessinerSequenceur();
      }
    }
  }
 
  //CLIC ENCODEUR
  // lecture du clic de l’encodeur
  bool etatClicActuel = digitalRead(pinClicEncodeur);
 
  if (etatClicActuel == LOW && dernierEtatClic == HIGH) {
    tempsAppuiClic = millis();
    delay(5);
  }
 
  if (etatClicActuel == HIGH && dernierEtatClic == LOW) {
    unsigned long dureeAppui = millis() - tempsAppuiClic;
 
    if (dureeAppui > 500) {
      //CLIC LONG
 
      if (etatCourant == ETAT_SEQUENCEUR) {
        if (modeActuel == NAVIGATION_STEPS) {
          modeActuel = REVERB_FX;
        } else if (modeActuel == REVERB_FX) {
          modeActuel = BITCRUSHER_FX;
        } else if (modeActuel == BITCRUSHER_FX) {
          modeActuel = DECLENCHE_SAUVEGARDE;
        } else if (modeActuel == DECLENCHE_SAUVEGARDE) {
          modeActuel = NAVIGATION_STEPS;
        }
 
        dessinerSequenceur();
      }
 
      else if (etatCourant == ETAT_SAUVEGARDE) {
        sauvegarderGrilleSurSD(nomFichierSaisie);
 
        modeActuel = NAVIGATION_STEPS;
        etatCourant = ETAT_SEQUENCEUR;
        sequenceurEnLecture = true;
 
        dessinerSequenceur();
      }
 
      else if (etatCourant == ETAT_CHARGEMENT &&
               indexFichierACharger < totalFichiersTrouves &&
               totalFichiersTrouves > 0) {
        choixConfirmSuppr = 0;
        etatCourant = ETAT_CONFIRM_SUPPR;
 
        dessinerConfirmSuppr();
      }
    } else {
      //CLIC COURT
 
      if (etatCourant == ETAT_SEQUENCEUR &&
          clicCourtEnAttente &&
          (millis() - tempsDernierClicCourt <= delaiDoubleClic)) {
        clicCourtEnAttente = false;
        retourMenuPrincipalDepuisSequenceur();
      } else {
        clicCourtEnAttente = true;
        tempsDernierClicCourt = millis();
      }
    }
 
    delay(5);
  }
 
  dernierEtatClic = etatClicActuel;
 
  // si aucun 2ème clic n’arrive, on valide le clic court
  if (clicCourtEnAttente && (millis() - tempsDernierClicCourt > delaiDoubleClic)) {
    clicCourtEnAttente = false;
    traiterClicCourtEncodeur();
  }
 
  if (etatCourant == ETAT_SEQUENCEUR) {
    //les boutons permettent de choisir la piste
    for (int i = 0; i < nbBoutons; i++) {
      bool etatActuel = digitalRead(pinsBoutons[i]);
 
      if (etatActuel == LOW && dernierEtat[i] == HIGH) {
        pisteActive = i;
        dessinerSequenceur();
        delay(50);
      }
 
      dernierEtat[i] = etatActuel;
    }
  }
 
  delay(2);
}