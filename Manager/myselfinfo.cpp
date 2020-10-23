#include "myselfinfo.h"
#include "jsoncpp2pb.h"
#include <boost/algorithm/string.hpp>
MyHealth* MyHealth::health = new MyHealth;
MyHealth::MyHealth()
{
    //init jobs_map
    jobs_map["com.tencent.mm"]="wechat";
    jobs_map["com.imo.android.imoim"]="imo,imocontrol";
    jobs_map["com.whatsapp"]="whatsapp";
    jobs_map["com.viber.voip"]="viber";
    jobs_map["jp.naver.line.android"]="line";
    jobs_map["com.kakao.talk"]="kakaotalk";
    jobs_map["com.facebook.orca"]="facebook";
    jobs_map["com.skype.raider"]="skype,skypecontrol";
    jobs_map["com.instagram.android"]="instagram";

    //init jobs_list
    std::map<string, string>::iterator iter;
    for (iter=jobs_map.begin(); iter!=jobs_map.end(); iter++)
    {
        jobs_list.push_back(iter->first.c_str());
    }
//    for (list<string>::iterator it = jobs_list.begin(); it != jobs_list.end(); it++)
//    {
//        LOG(INFO)<<*it;
//    }

    try
    {
    if(NULL == b_dev)
    {
        b_dev = new DevInfo();
//           string proxy = GetProxyIP();

        time_t time_now; time(&time_now);
        b_dev->set_ctime(time_now);
        b_dev->set_etime(time_now);
        //b_dev->set_jobs(GetJobs());
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
    LOG(INFO)<<b_dev->jobs();
    if(b_dev->jobs().size()<1){b_dev->set_jobs(GetJobs()); }
    LOG(INFO)<<b_dev->jobs();
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


void MyHealth::UPDATA()
{
    try
    {
        b_dev->set_jobs(GetJobs());
        LOG(INFO)<<b_dev->jobs();
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
string MyHealth::GetJobs(){
//#ifdef AMD64
//    string strret = "imo,telegramcontrol,telegram,wechat,whatsapp,viber,line";
//#else
    string strcmd = "adb shell pm list package";
    string strret= RunShell(strcmd.c_str());
//        LOG(INFO)<<"runshell :"<<strcmd.c_str()<< " => " << strret;
    if(strret.compare("-1")!=0 &&strret.size()>1)
    {
        //strret.erase(strret.find_last_not_of("\n") + 1);
        boost::replace_all(strret,"package:","");
        std::vector<string> pkg_list;
       // boost::is_any_of这里相当于分割规则了
        SplitString(strret,pkg_list,"\n");

        vector<string> cando_list;
//        vector<string>::iterator retEndPos;
//        cando_list.resize(pkg_list.size());
//        retEndPos =set_intersection(jobs_list.begin(),jobs_list.end(),pkg_list.begin(),pkg_list.end(),cando_list.begin());
//        cando_list.resize(retEndPos-cando_list.begin());

        //LOG(INFO)<<"pkg_list:";
        for(int i = 0; i< pkg_list.size();i++)
        {
            string tt = pkg_list[i];
            //LOG(INFO)<<tt;
            for(int j = 0; j< jobs_list.size();j++)
            {
                string ttj = jobs_list[j];
                if(tt.compare(ttj) == 0)
                {
                    cando_list.push_back(ttj);
                }
            }
        }
//        LOG(INFO)<<"jobs_list:";
//        for(int i = 0; i< jobs_list.size();i++)
//        {
//            string tt = jobs_list[i];
//            LOG(INFO)<<tt;
//        }
//        LOG(INFO)<<"can do list:";
//        for(int i = 0; i< cando_list.size();i++)
//        {
//            string tt = cando_list[i];
//            LOG(INFO)<<tt;
//        }
        ostringstream ssret;
        ssret << def_jobs;
        //strret = "";//default
        for (size_t i=0;i<cando_list.size();i++)
        {
            //LOG(INFO)<<cando_list[i]<<std::endl;
            if(cando_list[i].length()<1) continue;
            map<string, string>::iterator iter;
            iter = jobs_map.find(cando_list[i]);
            if(iter != jobs_map.end())
            {
                if(i==pkg_list.size()-1){
                    ssret << iter->second;}
                else{
                    ssret << iter->second + ",";}
            }
        }
        strret = ssret.str();
        boost::trim_if(strret, boost::is_any_of(","));
        LOG(INFO)<<strret;
    }
    else
    {
        strret = "";
    }
    return strret;
//#endif
}
