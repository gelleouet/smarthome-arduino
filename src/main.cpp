#include <Arduino.h>
#include "main.h"
#include "AbstractDevice.h"
#include "PorteGarage.h"
#include "Carillon.h"


const int MAXBUFFER = 32;
char _buffer[MAXBUFFER];
// volatile car variables modifiées depuis l'interrupt serialEvent
volatile int _idxBuffer = 0;
volatile bool _bufferFilled = true;

const int NB_DEVICES = 2;
AbstractDevice** _devices;

/**
 * Init programme
 */
void setup() {
  // création des impls
  // !! on utilise un tableau de pointer pour gérer plusieurs types d'impl
  // c'est pour cela que la taille du pointeur est la même sinon ca ne marcherait pas
  _devices = (AbstractDevice**) calloc(NB_DEVICES, sizeof(AbstractDevice*));
  _devices[0] = new PorteGarage((char*)"porteGarage", 25, 26, 27, 35);
  _devices[1] = new Carillon((char*)"carillon", 24, 32);

  // init connexion
  // !! A faire avant le init des devices car ils peuvent déjà envoyer des messages
  // prepare le buffer pour réception des messages
  resetBuffer();
  Serial.begin(9600);

  // init des devices
  for (int idx = 0; idx < NB_DEVICES; idx++) {
    _devices[idx]->init();
  }
}


/**
 * Programme principal
 */
void loop() {
  // demande process à chaque device
  for (int idx = 0; idx < NB_DEVICES; idx++) {
    _devices[idx]->process();
  }

  // attente info du controller
  if (_bufferFilled) {
    parseBuffer();
    resetBuffer();
  }
}


/**
 * Interruption déclenchée dès que le buffer Série
 * contient des données
 * serialEvent est définie par l'arduino (le nom est figé)
 */
void serialEvent() {
  while (Serial.available()) {
    if (readBuffer()) {
      _bufferFilled = true;
    }
  }
}


/**
 * Réinitialise le buffer Série
 */
void resetBuffer() {
  memset(_buffer, 0, MAXBUFFER);
  _idxBuffer = 0;
  _bufferFilled = false;
}


/**
 * Lecture d'un seul caractère sur le buffer série
 */
bool readBuffer() {
   _buffer[_idxBuffer] = (char) Serial.read();

   if (_buffer[_idxBuffer] == '\n' || _buffer[_idxBuffer] == '\r') {
     _buffer[_idxBuffer] = '\0';
     return true;
   } else {
     if (_idxBuffer < (MAXBUFFER-2)) {
        _idxBuffer++;
     } else {
        resetBuffer();
     }

     return false;
   }
}


/**
 * Exécution des commandes recues dans le buffer Série
 */
void parseBuffer() {
  char *split = strtok(_buffer, ":");
  int pin = -1;
  int valeur = -1;

  // lecture pin
  if (split != NULL) {
    pin = atoi(split);
    split = strtok(NULL, ":");
  }

  // lecture valeur
  if (split != NULL) {
    valeur = atoi(split);
  }

  // recherche d'un device pour traiter le message
  for (int idx = 0; idx < NB_DEVICES; idx++) {
    if (_devices[idx]->canProcessPin(pin)) {
      _devices[idx]->processPin(pin, valeur);
    }

  }
}
