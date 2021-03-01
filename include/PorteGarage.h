/**
 * PorteGarage
 * ==============
 *
 * Implémentation PorteGarage
 *
 * Commande externe par bouton poussoir
 * Capteur porte totalement ouverte
 * Capteur porte totalement fermée
 * Commande moteur
 *
 * @author : Gregory Elleouet <gregory.elleouet@gmail.com>
 */

#ifndef PorteGarage_h
#define PorteGarage_h

#include "AbstractDevice.h"


#define PORTE_GARAGE_IMPL "smarthome.automation.deviceType.PorteGarage"


class PorteGarage : public AbstractDevice {

  private:
    int _pinCapteurOuvert;
    int _pinCapteurFerme;
    int _pinBoutonPoussoir;
    int _pinCommandeMoteur;

    int _valeurCapteurOuvert;
    int _valeurCapteurFerme;
    int _valeurBoutonPoussoir;


  public:
    PorteGarage(char* name, int pinCapteurOuvert, int pinCapteurFerme, int pinBoutonPoussoir,
      int pinCommandeMoteur);
    ~PorteGarage();

    void init();
    void process();
    bool canProcessPin(int pin);
    void processPin(int pin, int value);
    /**
     * Ouverture ou fermeture (en fonction état de la porte de garage)
     * On ne contrôle pas l'état de la porte, on déclenche simplement la commande
     */
    void openClose();
    /**
     * Envoi d'un message complet avec l'état de la porte.
     * Ce message complet est construit avec les metavalues de l'objet
     */
    void sendState();
};

#endif
