/**
 * AbstractDevice
 * ==============
 *
 * Classe parent pour les implémentations objet
 *
 * @author : Gregory Elleouet <gregory.elleouet@gmail.com>
 */

#ifndef AbstractDevice_h
#define AbstractDevice_h


// Fonction callback pour l'envoi de valeurs ou d'événements
typedef void (*TriggerCallback)(int pin, int value);


const unsigned long DEBOUNCE = 5;
const unsigned long BOUTON_POUSSOIR_TIMER = 500;


class AbstractDevice {

  protected:
    char* _name;

    /**
     * Lecture d'un pin avec debounce (double lecture avec un temps d'intervalle)
     *
     * 0 ou 1 si lecture ok, sinon -1
     */
    int readDebounce(int pin);
    /**
     * Conversion valeur input à envoyer à l'application à cause du pullup
     *
     * 0 si pin = HIGH, 1 si pin = LOW
     */
    int convertInputValue(int value);
    /**
     * Envoi d'une valeur au contrôleur connecté
     *
    */
    void sendValue(int pin, int value);
    /**
     * Vérif si un timer est écoulé
     *
    */
    bool isTimerElapsed(unsigned long startTime, unsigned long timer);


  public:
    AbstractDevice(char* name);
    virtual ~AbstractDevice();


    /**
     * A appeler une seule fois (ie setup)
     *
    */
    virtual void init() = 0;

    /**
     * A appeler à chaque cycle pour vérifier le nouvel état du device
     * et déclencher le event nécessaires
     *
    */
    virtual void process() = 0;

    /**
     * A appeler pour traiter un message sur un pin
     *
    */
    virtual void processPin(int pin, int value) = 0;

    /**
     * Pour savoir si un device peut traiter un message pour un pin donné
     *
    */
    virtual bool canProcessPin(int pin) = 0;
};

#endif
