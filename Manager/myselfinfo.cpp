#include "myselfinfo.h"
#include "jsoncpp2pb.h"
#include <boost/algorithm/string.hpp>
MyHealth::MyHealth()
{

    try
    {
    if(NULL == b_dev)
    {
        b_dev = new DevInfo();
//           string proxy = GetProxyIP();

        time_t time_now; time(&time_now);
        b_dev->set_ctime(time_now);
        b_dev->set_etime(time_now);
//            b_dev->set_process(0);
//            b_dev->set_error(0);
//            b_dev->set_complete(0);
//            b_dev->set_count(0);
        b_dev->set_type(0);

    }
    if(b_dev->mac().size()<1){b_dev->set_mac(GetMAC()); }
    if(b_dev->nodeid().size()<1){b_dev->set_nodeid(GetIMEI()); }
    if(b_dev->ip().size()<1){b_dev->set_ip(GetLocalIP()); }
    if(b_dev->dname().size()<1){b_dev->set_dname(GetDevName()); }
    }
    catch(...)
    {

    }

}
MyHealth::~MyHealth()
{
    if(NULL != b_dev)
    {
        delete b_dev;
        b_dev=NULL;
    }
}
void MyHealth::SetDevInfo(DevInfo *info)
{
    try
    {
    string str_tmp = pb2json(*info);
    SetDevInfo(str_tmp);
    }
    catch(...)
    {

    }
}
void MyHealth::SetDevInfo(string strinfo)
{
    try
    {
    json2pb(*b_dev,strinfo);
    }
    catch(...)
    {

    }
}

string MyHealth::GetIMEI()
{
#ifdef AMD64
    string strret=GetMAC();

    boost::replace_all(strret,":","");
    boost::replace_all(strret,"\n","");
    return strret;
#else
    string strcmd = "adb shell service call iphonesubinfo 1 | sed -n '/[0-9]\\./p' |sed ':a;N;s/\\n/\\t/;ba;'|sed 's/0x[0-9]\\{8\\}\\|)//g'|sed 's/[0-9a-f]\\{8\\}//g' |sed 's/[[:punct:][:space:]]//g'";
    string strret= RunShell(strcmd.c_str());

//        LOG(INFO)<<"runshell :"<<strcmd.c_str()<< " => " << strret;
    if(strret.compare("-1")!=0 &&strret.size()>1)
    {
        strret.erase(strret.find_last_not_of("\n") + 1);
    }
    else
    {
        strret = "";
    }
    return strret;
#endif
}
string MyHealth::GetMAC()
{
#ifdef AMD64
    string strcmd = "cat /sys/class/net/*/address | sed \"s/00:00:00:00:00:00//\"";
    string strret= RunShell(strcmd.c_str());
//        LOG(INFO)<<"runshell :"<<strcmd.c_str()<< " => " << strret;
    if(strret.compare("-1")!=0 &&strret.size()>1)
    {
        strret.erase(strret.find_last_not_of("\n") + 1);
    }
    else
    {
        strret = "0000000000";
    }
    return strret;
#else
    string strcmd = "adb shell cat /sys/class/net/wlan0/address";
    string strret= RunShell(strcmd.c_str());
//        LOG(INFO)<<"runshell :"<<strcmd.c_str()<< " => " << strret;
    if(strret.compare("-1")!=0 &&strret.size()>1)
    {
        strret.erase(strret.find_last_not_of("\n") + 1);
    }
    else
    {
        strret = "0000000000";
    }
    return strret;
#endif
}
string MyHealth::GetLocalIP()
{
#ifdef AMD64
    string strcmd = "ifconfig 2> /dev/null |sed 's/.*127.0.0.1.*//g'|sed -n '/Bcast\\|broadcast/p' |sed 's/.*\\(addr:\\|inet \\)\\([0-9.]*\\).*/\\2/g'";
    string strret= RunShell(strcmd.c_str());
    if(strret.compare("-1")!=0 &&strret.size()>1)
    {
        strret.erase(strret.find_last_not_of("\n") + 1);
    }
    else
    {
        strret = "127.0.0.1";
    }
#else
    string strcmd = "adb shell ifconfig 2> /dev/null |sed 's/.*127.0.0.1.*//g'|sed -n '/Bcast\\|broadcast/p' |sed 's/.*\\(addr:\\|inet \\)\\([0-9.]*\\).*/\\2/g'";
    string strret= RunShell(strcmd.c_str());
//        LOG(INFO)<<"runshell :"<<strcmd.c_str()<< " => " << strret;
    if(strret.compare("-1")!=0 &&strret.size()>1)
    {
        strret.erase(strret.find_last_not_of("\n") + 1);
    }
    else
    {
        strret = "127.0.0.1";
    }
#endif
    return strret;
}
string MyHealth::GetProxyIP()
{
    string strcmd = "adb shell ifconfig tun0 2> /dev/null |sed -n '/inet addr:/p' |sed 's/.*addr:\\([0-9.]*\\).*/\\1/g'";
    string strret= RunShell(strcmd.c_str());
    LOG(INFO)<<"runshell :"<<strcmd.c_str()<< " => " << strret;
    if(strret.compare("-1")!=0 &&strret.size()>1)
    {
        strret.erase(strret.find_last_not_of("\n") + 1);
    }
    else
    {
        strret = "127.0.0.1";
    }
    return strret;
}
string MyHealth::GetDevName()
{
#ifdef AMD64
    string strret = "AMD64";
#else
    string strcmd = "adb shell getprop ro.product.brand";
    string strret= RunShell(strcmd.c_str());
//        LOG(INFO)<<"runshell :"<<strcmd.c_str()<< " => " << strret;
    if(strret.compare("-1")!=0 &&strret.size()>1)
    {
        strret.erase(strret.find_last_not_of("\n") + 1);
    }
    else
    {
        strret = "";
    }
#endif
    return strret;
}
