#ifndef APPCONF_H
#define APPCONF_H
#include <string>
using std::string;
class APPCONF
{
public:
    static APPCONF* getInstance()
    {
        return appconf;
    }
public:
    string refpath;
private:
    static APPCONF* appconf;
};

#endif // APPCONF_H
