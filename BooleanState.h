typedef void (*booleanStateCallback)(bool);


void fakeBooleanState (bool fake) {
}

class BooleanState {
public:

  bool pastState;

  bool newState;

  booleanStateCallback onChange;



  BooleanState  (bool _state) {
    pastState = _state;
    newState = _state;
    onChange = fakeBooleanState;
  }



  void run () {
    if (pastState != newState) {
      onChange(newState);
    }
    pastState = newState;
  }






  void set (bool _newState) {
    newState = _newState;
  }



  bool toggle () {
    newState = ! pastState;
    return newState;
  }



};
