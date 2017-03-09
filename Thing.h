#include <Arduino.h>



struct Modules {
    Module *module;
    int count;
};



class Module {
    virtual void loop () = 0;
};



class Thing {

    Modules *modules;


    void setModules (Modules *_modules) {
        modules = _modules;
    };



    /*void setup () {
    };*/



    void loop () {
        for (int i = 0; i < modules.count; i = i + 1;) {
            modules->module[i]->loop();
        }
    };

}

