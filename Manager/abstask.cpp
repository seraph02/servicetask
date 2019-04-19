#include "abstask.h"
#include<vector>
#include<boost/algorithm/string.hpp>
#include "comm.h"
using std::vector;
string absTask::GetKey()
{
    if(t_task.key_size()<1) return "";
    int keyid = t_task.info_size()!=0?t_task.progress()/t_task.info_size():0;
    if(keyid>t_task.info_size()) keyid-=1;
    string tmp = t_task.key(keyid);
    boost::trim_left(tmp);//去掉字符串左边空格
    boost::trim_right(tmp);//去掉字符串右边空格
    return tmp;
}
string absTask::GetInfo()
{
    if(t_task.info_size()<1) return "";
    int infoid = t_task.info_size()!=0?t_task.progress()%t_task.info_size():0;
    return t_task.info(infoid);
}
string absTask::GetAppName(string strinfo)
{
    vector<string> strsubarray =vector<string>();
    SplitString(strinfo.c_str(),strsubarray,"::");
    return strsubarray.size()>1?strsubarray[0].c_str():strinfo.c_str();
}
int absTask::GetTaskTol()
{
    return t_task.key().size()>0?t_task.key_size() * t_task.info_size():t_task.info_size();;
}
int absTask::progress()
{
    return t_task.progress();
}
