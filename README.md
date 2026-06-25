# JAMA – Mini Groovebox

Projet pluridisciplinaire ISEN2 – Conception d'une mini-groovebox

# Membres du projet

- Mélyscéane DUPUIS
- Alina CERRUTI
- Juliette VIDAL
- Adam TARQUINI

Projet ISEN Méditerranée – ISEN2

## Présentation

JAMA est une mini groovebox de type step-séquenceur de batterie développée dans le cadre du projet pluridisciplinaire ISEN2.

L'objectif est de proposer un instrument électronique autonome, compact et intuitif permettant de créer des rythmes sans ordinateur.

Le système repose sur une Teensy 4.1 associée à un audio shield, permettant la lecture d'échantillons audio (.wav), leur traitement en temps réel ainsi que leur restitution via une sortie Jack ou un haut-parleur intégré.

L'interface utilisateur est composée :

- d'un écran OLED;
- de 4 boutons de sélection des pistes;
- d'un bouton Play/Pause;
- de 2 potentiomètres;
- d'un encodeur rotatif avec bouton poussoir.

Le projet permet de :

- créer une boucle rythmique sur 8 pas;
- jouer simultanément jusqu'à 4 sons;
- régler le tempo;
- régler le volume;
- appliquer une réverbération;
- appliquer un effet Bitcrusher;
- sauvegarder et recharger des boucles sur carte microSD.

# Fonctionnalités

- Séquenceur 8 pas;
- 4 pistes de batterie:
  - Kick
  - Clap
  - HiHat
  - Snare
- Lecture d'échantillons WAV depuis carte SD;
- Polyphonie 4 sons;
- Réglage du BPM;
- Contrôle du volume;
- Effet Reverb;
- Effet Bitcrusher;
- Sauvegarde des patterns;
- Chargement des patterns;
- Détection automatique d'une prise Jack;
- Interface OLED.

# Plateforme matérielle

Le projet utilise :

- Teensy 4.1;
- Audio shield;
- Écran OLED SSD1306 I2C (128×64);
- Encodeur rotatif;
- 2 potentiomètres;
- 5 boutons poussoirs;
- Carte microSD;
- Haut-parleur;
- Sortie Jack 3.5 mm;
- Amplificateur.

# Bibliothèques utilisées

Le projet utilise les bibliothèques suivantes :

- Audio;
- Wire;
- SPI;
- SD;
- SerialFlash;
- Encoder;
- Adafruit GFX;
- Adafruit SSD1306.

Toutes les bibliothèques sont disponibles depuis le Teensyduino ou le gestionnaire de bibliothèques Arduino.

# Compilation

## Prérequis

Installer:

- Arduino IDE;
- Teensyduino;
- Bibliothèques listées ci-dessus.

Sélectionner: Board > Teensy 4.1

Compiler le projet puis téléverser le firmware sur la Teensy.

# Carte SD

La carte microSD doit contenir les fichiers suivants :

kick.wav
clap.wav
hihat.wav
snare.wav

Les sauvegardes des séquences sont automatiquement créées au format :

LOOP01.txt
LOOP02.txt

# Utilisation

## Menu principal

- Créer une nouvelle boucle
- Charger une boucle existante

## Séquenceur

Les boutons permettent de sélectionner une piste :

- Kick
- Clap
- HiHat
- Snare

Le potentiomètre Tempo règle le BPM.
Le potentiomètre Volume règle le volume général.

L'encodeur permet :

- navigation dans les steps;
- réglage de la Reverb;
- réglage du Bitcrusher;
- accès à la sauvegarde.

Le bouton Play/Pause lance ou arrête la lecture.
