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
    void setq(string qt)
    {
        this->q = qt;
    }
    string getkey(){return key;}
    string getq(){return q;}
    absresult* run(string body);
    virtual ~absAPP(){}
protected:
    absControl* control;
    string key;
    string q;
};

#endif // ABSAPP_H

