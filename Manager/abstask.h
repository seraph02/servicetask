#ifndef ABSTASK_H
#define ABSTASK_H
#include<string>
using std::string;
#include "taskinfo.pb.h"
using namespace SCPROTO;
class absTask
{
public:
    virtual void gettask()=0;
    virtual void run()=0;
    virtual void after()=0;
    void setfilename4task(string file){filename4task=file;}
    string getfilename4task()
    {
        return filename4task;
    }

protected:
    string filename4task;

protected:

public:
    TaskInfo t_task;
    string GetInfo();
    string GetKey();
    static string GetAppName(string strinfo);
    int GetTaskTol();
    int progress();
};

#endif // ABSTASK_H
