#include <Arduino.h>
#include "PorteGarage.h"


PorteGarage::PorteGarage(char* name, int pinCapteurOuvert, int pinCapteurFerme, int pinBoutonPoussoir,
  int pinCommandeMoteur) : AbstractDevice(name) {
    _pinCapteurOuvert = pinCapteurOuvert;
    _pinCapteurFerme = pinCapteurFerme;
    _pinBoutonPoussoir = pinBoutonPoussoir;
    _pinCommandeMoteur = pinCommandeMoteur;
}


PorteGarage::~PorteGarage() {

}


bool PorteGarage::canProcessPin(int pin) {
  return pin == _pinCommandeMoteur;
}


void PorteGarage::processPin(int pin, int value) {
  if (pin == _pinCommandeMoteur) {
    if (value) {
      openClose();
    }
  }
}


void PorteGarage::init() {
  // pin en sortie configuré avec une valeur HIGH au démarrage pour être
  // compatible avec carte relai sainsmart qui fonctionne "à l'envers"
  pinMode(_pinCommandeMoteur, OUTPUT);
  digitalWrite(_pinCommandeMoteur, HIGH);

  // les pins en entrée sont configurés avec une résistance pullup
  // pour gérer les perturbations (faux signal du à un pin "flottant")
  // la valeur d'init doit aussi être ajustée en conséquence
  pinMode(_pinCapteurOuvert, INPUT_PULLUP);
  pinMode(_pinCapteurFerme, INPUT_PULLUP);
  pinMode(_pinBoutonPoussoir, INPUT_PULLUP);
  _valeurCapteurOuvert = HIGH;
  _valeurCapteurFerme = HIGH;
  _valeurBoutonPoussoir = HIGH;

  // envoi d'un message pour indiquer le pin de commande
  sendValue(_pinCommandeMoteur, 0);
}


void PorteGarage::process() {
  // état du capteur ouvert
  int valPin = readDebounce(_pinCapteurOuvert);
  bool canSendState = false;

  if (valPin != -1 && valPin != _valeurCapteurOuvert) {
    _valeurCapteurOuvert = valPin;
    sendValue(_pinCapteurOuvert, convertInputValue(_valeurCapteurOuvert));
    canSendState = true;
  }

  // état du capteur fermé
  valPin = readDebounce(_pinCapteurFerme);

  if (valPin != -1 && valPin != _valeurCapteurFerme) {
    _valeurCapteurFerme = valPin;
    sendValue(_pinCapteurFerme, convertInputValue(_valeurCapteurFerme));
    canSendState = true;
  }

  // état du bouton poussoir
  // on va pouvoir déclencher la commande directe quand le BP est activé
  // on ne lancera la commande et l'envoi du message qu'à l'appui du BP et pas
  // au relachement
  valPin = readDebounce(_pinBoutonPoussoir);

  if (valPin != -1 && valPin != _valeurBoutonPoussoir) {
    _valeurBoutonPoussoir = valPin;

    // trigger de la commande et envoi message ssi la valeur interprétée est positive
    // !! le message est envoyé pour le pin de la commande moteur et non pas du BP
    // le BP ne sera pas visualisé sur application, et on veut savoir si event sur
    // le composant principal (ie commande)
    if (convertInputValue(_valeurBoutonPoussoir)) {
      // !! on actionnne la commande avant le message pour être plus réactif
      openClose();
      sendValue(_pinCommandeMoteur, convertInputValue(_valeurBoutonPoussoir));
      canSendState = true;
    }
  }

  // Envoi d'un message complet pour nouvelle implémentation device PorteGarage
  // avec les metavalues
  if (canSendState) {
    sendState();
  }
}


void PorteGarage::openClose() {
  // activation du pin pendant le TIMER
  // les valeurs sont inversées à cause carte sainsmart
  digitalWrite(_pinCommandeMoteur, LOW); // LOW = activation relai
  // FIXME : ne pas utiliser le delay mais floagguer l'action
  // et la traiter au cycle suivant (avec date)
  delay(BOUTON_POUSSOIR_TIMER);
  digitalWrite(_pinCommandeMoteur, HIGH); // HIGH = relachement relai
}


void PorteGarage::sendState() {
  Serial.print("{\"mac\":\"");
  Serial.print(_name);
  Serial.print("\",\"implClass\":\"");
  Serial.print(PORTE_GARAGE_IMPL);
  Serial.print("\",\"value\":");
  Serial.print(_valeurBoutonPoussoir);
  Serial.print(",\"metavalues\":{");
  Serial.print("\"capteurOuvert\":{\"value\":");
  Serial.print(_valeurCapteurOuvert);
  Serial.print("},\"capteurFerme\":{\"value\":");
  Serial.print(_valeurCapteurFerme);
  Serial.print("}");
  Serial.println("}}");
}
