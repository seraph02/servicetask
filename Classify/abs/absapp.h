#ifndef ABSAPP_H
#define ABSAPP_H
#include "abscontrol.h"

//APP
class absAPP
{
public:
    void setkey(string key)
    {
        this->key = key;
    }
    string getkey(){return key;}
    absresult* run(string body);
    virtual ~absAPP(){}
protected:
    absControl* control;
    string key;
};

#endif // ABSAPP_H

