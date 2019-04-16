#ifndef IINFOBASE_H
#define IINFOBASE_H
#include "obmanager.h"

class IHealth
{
public:
    virtual void Attach(IManager *) = 0;
    virtual void Detach(IManager *) = 0;
    virtual void Notify() = 0;
};

#endif // SUBJECT_H
