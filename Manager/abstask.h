#ifndef ABSTASK_H
#define ABSTASK_H
#include<string>
#include"comm.h"
using std::string;
#include "taskinfo.pb.h"
using namespace SCPROTO;
class absTask
{
public:
    virtual void gettask()=0;
    virtual void run()=0;
    virtual void after()=0;
    void setpath4task(string path)
    {
        taskpath=path.length()>1&&(path[path.size()-1]=='/'|path[path.size()-1]=='\\')?path:path+'/';
    }
    string getfilename4task()
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
    void showtask();

protected:
    string taskpath;
    int step;
protected:

public:
    TaskInfo t_task;
    string GetInfo();
    string GetKey();
    static string GetAppName(string strinfo);
    int version=0;
    int GetTaskTol();
    int progress();
    void NextKey();
    void NextInfo();
    void setstep(int s){step=s;}
    int getstep(){int s=step;step=0;return s;}
};

#endif // ABSTASK_H
