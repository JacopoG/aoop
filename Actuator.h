class Actuator {
  public:

    int pin;



    void init (int _pin) {
      pin = _pin;
      pinMode(pin, OUTPUT);
    }



    void set (bool voltage) {
      digitalWrite(pin, voltage);
    }

};
