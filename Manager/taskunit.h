#ifndef TASKUNIT_H
#define TASKUNIT_H
#include "abstask.h"


class TaskUnit : public absTask
{
public:
    TaskUnit();
    virtual void gettask();
    virtual void run();
    virtual void after();

private:

};

#endif // TASKUNIT_H

