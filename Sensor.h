class Sensor;

void SensorFakeCallback (Sensor* fake) {
}


typedef void (*sensorCallback)(Sensor*);

class Sensor {

  public:



    int pin;

    sensorCallback onHighCallback;

    sensorCallback onLowCallback;

    sensorCallback onChangeStatusCallback;

    bool pastStatus;



    void init (int _pin, bool pullup = true) {
      pin = _pin;

      if (pullup) {
        pinMode(pin, INPUT_PULLUP);
      }
      else {
        pinMode(pin, INPUT);
      }

      pastStatus = getStatus();

      onHighCallback = SensorFakeCallback;
      onLowCallback = SensorFakeCallback;
      onChangeStatusCallback = SensorFakeCallback;
    };



    bool getStatus () {
      return digitalRead(pin);
    };



    void run () {
      bool newStatus = getStatus();
      if (pastStatus != newStatus) {
        onChangeStatusCallback(this);

        if (newStatus == HIGH) {
          onHighCallback(this);
        }

        if (newStatus == LOW) {
          onLowCallback(this);
        }
      }

      pastStatus = newStatus;
    };


    
};

