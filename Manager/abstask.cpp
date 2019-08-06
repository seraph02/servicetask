#include "abstask.h"
#include "comm.h"
#include<vector>
#include<boost/algorithm/string.hpp>
#include <glog/logging.h>
using std::vector;
string absTask::getfilename4task()
{
    if(t_task.id().empty())
    {
        std::vector<string> files = getFiles(taskpath);
        string taskfile="";
        for(string s : files)
        {
            if(getfileext(s).compare(".tsk")==0) taskfile=s;
            break;
        }
        if(taskfile.empty())return taskfile;
        return taskfile;
    }
    return taskpath+t_task.id()+".tsk";
}
void absTask::showtask()
{
    if(t_task.id().size()<1) return;
    LOG(INFO)<<t_task.id();
}
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
void absTask::NextKey()
{
    int progress = t_task.progress();
    int infosize = t_task.info_size();
    int intstep = infosize-progress%infosize;
    setstep(intstep);
}
void absTask::NextInfo()
{
//    int progress = t_task.progress();
    setstep(1);
}
