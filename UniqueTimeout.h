#include <SimpleTimer.h>

class UniqueTimeout {
public:

  SimpleTimer cron;

  int timerID = 0;

  void set (long timer, timer_callback callback) {

    if (cron.isEnabled(timerID)) {
        cron.restartTimer(timerID);
    }
    else {
      timerID = cron.setTimeout(timer, callback);
    }

  };

  void run () {
    cron.run();
  }

};
