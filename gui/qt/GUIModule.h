#ifndef GUIMODULE_H
#define GUIMODULE_H

#include "module.h"
class event;
class GUIModule: public module{
private:
    static const char* myName;
    public:
    void initialize(uint32&);
    void update(bot_info*){}
    void pushEvent(event* ){}
    const char* getCommonName(){
        return myName;
    }

};


#endif // GUIMODULE_H
