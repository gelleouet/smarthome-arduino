#include <Arduino.h>
#include "Carillon.h"


Carillon::Carillon(char* name, int pinBoutonPoussoir, int pinSonnerie) : AbstractDevice(name) {
    _pinBoutonPoussoir = pinBoutonPoussoir;
    _pinSonnerie = pinSonnerie;
    _lastRing = 0;
}


Carillon::~Carillon() {

}


bool Carillon::isMuteTimerElapsed() {
  return isTimerElapsed(_lastRing, MUTE_TIMER);
}


bool Carillon::canProcessPin(int pin) {
  return pin == _pinSonnerie;
}


void Carillon::processPin(int pin, int value) {
  if (pin == _pinSonnerie) {
    if (value) {
      dring();
    }
  }
}


void Carillon::init() {
  // pin en sortie configuré avec une valeur HIGH au démarrage pour être
  // compatible avec carte relai sainsmart qui fonctionne "à l'envers"
  pinMode(_pinSonnerie, OUTPUT);
  digitalWrite(_pinSonnerie, HIGH);

  // les pins en entrée sont configurés avec une résistance pullup
  // pour gérer les perturbations (faux signal du à un pin "flottant")
  // la valeur d'init doit aussi être ajustée en conséquence
  pinMode(_pinBoutonPoussoir, INPUT_PULLUP);
  _valeurBoutonPoussoir = HIGH;

  // envoi d'un message pour indiquer le pin de commande
  sendValue(_pinSonnerie, 0);
}


void Carillon::process() {
  // état du bouton poussoir
  // on va pouvoir déclencher la commande directe quand le BP est activé
  // on ne lancera la commande et l'envoi du message qu'à l'appui du BP et pas
  // au relachement
  int valPin = readDebounce(_pinBoutonPoussoir);

  if (valPin != -1 && valPin != _valeurBoutonPoussoir) {
    _valeurBoutonPoussoir = valPin;

    // trigger de la commande et envoi message ssi la valeur interprétée est positive
    // !! le message est envoyé pour le pin de la commande moteur et non pas du BP
    // le BP ne sera pas visualisé sur application, et on veut savoir si event sur
    // le composant principal (ie commande)
    if (convertInputValue(_valeurBoutonPoussoir)) {
      // !! on actionnne la commande avant le message pour être plus réactif
      if (!_mute && isMuteTimerElapsed()) {
        dring();
        _lastRing = millis();
      }

      // même si _mute on recoit le message pour recevoir par exemple des alertes
      sendValue(_pinSonnerie, convertInputValue(_valeurBoutonPoussoir));
    }
  }
}


void Carillon::dring() {
  // activation du pin pendant le TIMER
  // les valeurs sont inversées à cause carte sainsmart
  digitalWrite(_pinSonnerie, LOW); // LOW = activation relai
  // FIXME : ne pas utiliser le delay mais floagguer l'action
  // et la traiter au cycle suivant (avec date)
  delay(BOUTON_POUSSOIR_TIMER);
  digitalWrite(_pinSonnerie, HIGH); // HIGH = relachement relai
}


void Carillon::mute(int value) {
  _mute = value ? true : false;
}
