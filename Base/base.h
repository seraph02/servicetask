#ifndef BASE_H
#define BASE_H

#include <exception>
#include "glog/logging.h"
#if __amd64 ==1
#define PROXYOK 0
#endif
#define MAXRETRY 3

enum EType
{
    Adb,
    Local,
    Proxy,
    ES,
    NetDisk,
    Dev,


};
enum EStatus
{
    Off,
    On
};
struct CStatus
{
    EStatus status;

};
#endif // BASE_H
