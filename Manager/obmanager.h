#ifndef IMANAGE_H
#define IMANAGE_H

class IManager
{
public:
    virtual void Update(int status) = 0;
    virtual void run()=0;
    virtual ~IManager(){}
};

#endif // IMANAGE_H
