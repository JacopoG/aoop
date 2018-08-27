#include <Arduino.h>

class Sensor;

/*void SensorFakeCallback (Sensor* fake) {
}*/


typedef void (*sensorCallback)(Sensor*);

class Sensor {

  public:



    int pin;

    sensorCallback onHighCallback = NULL;

    sensorCallback onLowCallback = NULL;

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

      //onHighCallback = SensorFakeCallback;
      //onLowCallback = SensorFakeCallback;
      //onChangeStatusCallback = SensorFakeCallback;
    };



    bool getStatus () {
      return digitalRead(pin);
    };



    void run () {
      bool newStatus = getStatus();
      if (pastStatus != newStatus) {
          if (onChangeStatusCallback) {
              onChangeStatusCallback(this);
          }

        if (newStatus == HIGH) {
            if (onHighCallback) {
                onHighCallback(this);
            }
        }

        if (newStatus == LOW) {
            if (onLowCallback) {
                onLowCallback(this);
            }
        }
      }

      pastStatus = newStatus;
    };


    
};

