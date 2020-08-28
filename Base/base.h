#ifndef BASE_H
#define BASE_H

#include <exception>
#include "glog/logging.h"
#if __amd64 ==1
#define AMD64 0
#define PROXYOK 0
#endif
#define MAXRETRY 1
#define ADB 0
//#define NETDISK 1
enum EType
{
    Adb,
    Local,
    Proxy,
    ES,
    NetDisk,
    Offline,
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
inline bool CheckStatus(EType stype,int status){ return ((status>>stype)&1); }
#endif // BASE_H
