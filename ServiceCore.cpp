#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "conf.h"
#include "manager_core.h"

#include <fcntl.h>

#define DEFAULT_FILE "/tmp/ServiceCore.pid"

int main(int argc, char* argv[])
{
    int fd = -1;
    char buf[32];

    fd = open(DEFAULT_FILE, O_WRONLY | O_CREAT, 0666);
    if (fd < 0) {
    perror("Fail to open");
    exit(1);
    }

    struct flock lock;
    bzero(&lock, sizeof(lock));

    if (fcntl(fd, F_GETLK, &lock) < 0) {
    perror("Fail to fcntl F_GETLK");
    exit(1);
    }

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;

    if (fcntl(fd, F_SETLK, &lock) < 0) {
    perror("Fail to fcntl F_SETLK");
    exit(1);
    }

    pid_t pid = getpid();
    int len = snprintf(buf, 32, "%d\n", (int)pid);

    write(fd, buf, len); //Write pid to the file


    string configfile;
    for(int i=0;i<argc;i++)
    {
        if(strcmp(argv[i],"-h")==0 )
        {
            cout<<"\n\t-h\t\t\thelp \n"<<
                  "\t-D or -d\t\tdaemon\n"<<
                  "\t-C [PATH] or -c [PATH]\tconfig file path\n"<<endl;
            return 0;
        }
        if(strcmp(argv[i],"-D")==0 || strcmp(argv[i],"-d")==0)
        {
            if(daemon(0,0) == -1)
                exit(EXIT_FAILURE);

        }
        if((strcmp(argv[i],"-c")==0 ||strcmp(argv[i],"-C")==0)&&(i+1 <argc))
        {
            configfile=argv[i+1];
//            cout<<configfile<<endl;
        }
    }
//    configfile="/home/ubuntu/source/servicecore/servicecore.conf";

    SCPROTO::ConfInfo* conf = Manager_conf::getInstance();
    if(configfile.size()<1)
    {
      configfile ="servicecore.conf";
    }
    if(access(configfile.c_str(),0)==-1)
    {

    }
    else
    {
        map<string,string> m_mapConfig;
        ReadConfig(configfile.c_str(),m_mapConfig);
        map<string, string>::const_iterator mite = m_mapConfig.begin();
        for (; mite != m_mapConfig.end(); ++mite) {
            if(mite->first.compare("NETDISKPATH")==0)
            {
                conf->set_netdiskpath(mite->second);
            }else if(mite->first.compare("POCPATH")==0)
            {
                conf->set_pocpath(mite->second);
            }else if(mite->first.compare("ESHOST")==0)
            {
                conf->set_eshost(mite->second);
            }else if(mite->first.compare("ESPORT")==0)
            {
                conf->set_esport(mite->second);
            }else if(mite->first.compare("LOGPATH")==0)
            {
                conf->set_logpath(mite->second);
            }
            else if(mite->first.compare("TASKTMPPATH")==0)
            {
                conf->set_tasktmppath(mite->second);
            }

        }
    }


    if(conf->tasktmppath().size()<1)  conf->set_tasktmppath("tasks/");
    if(conf->logpath().size()<1) conf->set_logpath("log/");
    if(conf->pocpath().size()<1) conf->set_pocpath("POC/");
    if(conf->eshost().size()<1) conf->set_eshost("localhost");
    if(conf->esport().size()<1) conf->set_esport("9200");
    if(conf->netdiskpath().size()<1) conf->set_netdiskpath("netdisk/");





    //全局资源初始化，放在主线程

    google::InitGoogleLogging((const char *)argv[0]);

    string logpath = conf->logpath();
    if(access(logpath.c_str(),0)==-1)
    {
          string command = "mkdir -p "+logpath;
          system(command.c_str());
    }
      string taskpath = conf->tasktmppath();
      if(access(taskpath.c_str(),0)==-1)
      {
            string command = "mkdir -p "+taskpath;
            system(command.c_str());
      }
      if(access(taskpath.c_str(),0)==-1 ||access(logpath.c_str(),0)==-1)
      {
          LOG(ERROR)<<taskpath<<" or "<<logpath<<" create error";
          return 0;
      }


    google::SetStderrLogging(google::GLOG_INFO);       // 设置glog的输出级别，这里的含义是输出INFO级别以上的信息

    // 设置INFO级别以上的信息log文件的路径和前缀名
    google::SetLogDestination(google::GLOG_INFO, (logpath+"INFO_").c_str());

    // 设置WARNING级别以上的信息log文件的路径和前缀名
    //google::SetLogDestination(google::GLOG_WARNING, (logpath+"WARNING_").c_str());

    // 设置ERROR级别以上的信息log文件的路径和前缀名
    //google::SetLogDestination(google::GLOG_ERROR, (logpath+"ERROR_").c_str());

    FLAGS_colorlogtostderr = true;                     // 开启终端颜色区分

    LOG(INFO) << "Found " << google::COUNTER <<endl;



//    LOG(INFO) << str ;                // << " cookies";
//    LOG(WARNING) << "warning test";    // 会输出一个Warning日志
//    LOG(ERROR) << "error test";        // 会输出一个Error日志
    signal(SIGPIPE, SIG_IGN);

    try
    {
        Manager_Core::getInstance()->Init();

        while(1){}


    }
    catch (exception& e)
    {
        LOG(ERROR) << e.what() << endl;
        cout<<e.what()<<endl;
    }



    google::ShutdownGoogleLogging();                   // 全局关闭glog

    return EXIT_SUCCESS;

}

