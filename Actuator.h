class Actuator {
  public:

    int pin;



    void init (int _pin, bool voltage = LOW) {
      pin = _pin;
      pinMode(pin, OUTPUT);
      set(voltage);
    }



    void set (bool voltage) {
      digitalWrite(pin, voltage);
    }



    bool toggle () {
      bool newVoltage = ! digitalRead(pin);
      set(newVoltage);
      return newVoltage;
    }

};
