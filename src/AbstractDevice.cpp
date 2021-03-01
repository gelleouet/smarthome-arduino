#include "AbstractDevice.h"
#include <Arduino.h>


AbstractDevice::AbstractDevice(char* name) {
  _name = name;
  }


AbstractDevice::~AbstractDevice() {

}


void AbstractDevice::sendValue(int pin, int value) {
  Serial.print("{\"mac\":\"arduino");
  Serial.print(pin);
  Serial.print("\",\"value\":");
  Serial.print(value);
  Serial.println("}");
}


int AbstractDevice::readDebounce(int pin) {
  // 2 lecture avec pause pour gerer les parasites
  int firstRead = digitalRead(pin);
  delay(DEBOUNCE);
  int secondRead = digitalRead(pin);

  if (firstRead == secondRead) {
    return firstRead;
  } else {
    return -1;
  }
}


int AbstractDevice::convertInputValue(int value) {
  return value == HIGH ? 0 : 1;
}


bool AbstractDevice::isTimerElapsed(unsigned long startTime, unsigned long timer) {
  // si aucune action précédente, alors pas de timer activé
  if (!startTime) {
    return true;
  }
  // la fonction millis reprend à 0 quand la taille du unsigned lon est dépassée
  // donc now peut être < à startTime
  unsigned long elapsedTime = millis() - startTime;

  return abs(elapsedTime) > timer;
}
