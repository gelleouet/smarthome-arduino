/**
 * Carillon
 * ==============
 *
 * Implémentation Carillon
 *
 * Commande externe par bouton poussoir
 * Commande moteur
 * Gestion d'un timer pour éviter de se faire "spammer" la sonnette entre 2 actions
 * du bouton poussoir
 * Gestion d'un "mute" pour désactiver la sonnette à distance (ex : en fonction horaire
 * configurable sur application)
 *
 * @author : Gregory Elleouet <gregory.elleouet@gmail.com>
 */

#ifndef Carillon_h
#define Carillon_h

#include "AbstractDevice.h"


const unsigned long MUTE_TIMER = 10000; // 10sec


class Carillon : public AbstractDevice {

  private:
    int _pinBoutonPoussoir;
    int _pinSonnerie;
    int _valeurBoutonPoussoir;
    bool _mute;
    unsigned long _lastRing;

    bool isMuteTimerElapsed();


  public:
    Carillon(char* name, int pinBoutonPoussoir, int pinSonnerie);
    ~Carillon();

    void init();
    void process();
    bool canProcessPin(int pin);
    void processPin(int pin, int value);
    /**
     * Déclenchement sonnerie
     */
    void dring();
    /**
     * Activation / Désactivation du mute
     */
    void mute(int value);
};

#endif
