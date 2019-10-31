#include "comm.h"
#include <unistd.h> //exec systemctl
#include <bitset>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include<string.h>
#include "Base64Decoder.h"
#include <glog/logging.h>

#include<sys/stat.h>
#include <dirent.h>
#include <algorithm>
using std::stringstream;
using std::ofstream;
//void Binarycout(int n)
//{
// for (int i=31;i>=0;i--)
// {
// std::cout<<((n>>i)&1);
// }
// std::cout<<std::endl;
//}

void trim(char *str)
{
    if (NULL == str || 0 == strlen(str))
    {
        return;
    }
    char *p1 = str;
    char *p2 = str+strlen(str)-1;
    while (p2 > str && *p2 == ' ')
    {
        p2--;
    }
    p2++;
    *p2 = 0;
    while (*p1  && *p1 == ' ')
    {
        p1++;
    }
    strncpy(str ,p1 ,p2-p1+1);
}

std::string RunShell(const char* cmd)
{
//    LOG(INFO) <<"runshell-->"<< cmd << endl;
     std::stringstream buffer;

     try
     {

    FILE *fstream = NULL;
    char buff[2048];
    memset(buff, 0, sizeof(buff));

    if(NULL == (fstream = popen(cmd,"r")))
    {
        fprintf(stderr,"execute command failed: %s",strerror(errno));
        return "";
    }
    while(NULL != fgets(buff, sizeof(buff), fstream))
    {
        buffer<<buff;
    }
    pclose(fstream);

     }
     catch(...)
     {

     }

    return buffer.str();

}
bool WriteLocalFile(std::string filename,std::string jsonbody)
{
//    LOG(INFO)<<"WriteLocalFile :"<<filename;
    bool ret=false;

    FILE * fp = fopen(filename.c_str(), "wb");
    if(fp==NULL) return false;
    fwrite(jsonbody.c_str(), 1, jsonbody.size(), fp);
    fclose(fp);

    ret = true;
    return ret;
}
string GetCurrPath()
 {
    int maxsize =254;
    char current_absolute_path[maxsize];

    //获取当前程序绝对路径
    int cnt = readlink("/proc/self/exe", current_absolute_path, maxsize);
    if (cnt < 0 || cnt >= maxsize)
    {
        printf("***Error***\n");
        exit(-1);
    }
    string ret(current_absolute_path);
    int pos = ret.rfind('/');
    if(pos!=string::npos) ret = ret.substr(0,pos);
    return ret+"/";
}

string ReadLocalFile(string filename)
{
//    LOG(INFO)<<"ReadLocalFile :"<<filename;
//    char buf[8192];
    std::ifstream t(filename);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string contents(buffer.str());
    t.close();

    return contents;
}
int getFiletotals(string filename)
{
    std::stringstream buffer;
    std::stringstream cmdbuf;
    cmdbuf << "awk '{print NR}' "<<filename<<" | tail -n1";
    string cmdstr = cmdbuf.str();
    string bufferstr = RunShell(cmdstr.c_str());
    if(bufferstr.empty()) return 0;
    buffer << bufferstr;
    std::string contents(buffer.str());
    return atoi(contents.c_str());
}
string ReadLocalFile(string filename,int linenum)
{
//    LOG(INFO)<<"ReadLocalFile :"<<filename;
//    char buf[8192];
//    std::ifstream t(filename);
//    std::stringstream buffer;
//    buffer << t.rdbuf();
//    std::string contents(buffer.str());
//    t.close();

    std::stringstream buffer;
    std::stringstream cmdbuf;
    cmdbuf << "sed -n "<<linenum<<",1p "<<filename;
    string cmdstr = cmdbuf.str();
    string bufferstr = RunShell(cmdstr.c_str());
    if(bufferstr.empty()) return "";
    buffer << bufferstr;
    std::string contents(buffer.str());
    return contents;
}
void SplitString(const string& s,vector<string> &v, const string& c)
{
    string::size_type pos1, pos2;

    pos2 = s.find(c);
    pos1 = 0;
    while(string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2-pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if(pos1 != s.length())
        v.push_back(s.substr(pos1));


}
void checkdir(string dir)
{
    if(access(dir.c_str(),0)!=0)
    {
        string cmd = "mkdir -p " + dir;
        RunShell(cmd.c_str());
    }
}
string filename4time(string ext)
{
    srand((unsigned int)time(0));
    stringstream ss;
    time_t timep;time(&timep);
    int num = rand()%100;
    ss<< timep <<num<< ((ext[0]=='.')?"":".") << ext;
    return ss.str();
}
string getfileext(string filename)
{
     int findpos;
     findpos = filename.rfind('?');
     if(findpos!=string::npos) filename = filename.substr(0,findpos);
     findpos=filename.rfind('.');
     if(findpos!=string::npos) filename = filename.substr(findpos);
    return filename;
}
void b64_decode2file(string b64str,string dstfile)
{
    Base64Decoder decoder;
    ofstream ofs(dstfile, ofstream::out | ofstream::binary);
    if (ofs)
    {
        int numberOfBytesDecoded;
        char decodedBuffer[b64str.length()];

            numberOfBytesDecoded = decoder.decode(b64str.c_str(), b64str.length(), decodedBuffer);
            ofs.write(decodedBuffer, numberOfBytesDecoded);

    }
    else
        LOG(ERROR) << "Cannot open file: " << dstfile ;


}
std::tm gettm(int64_t timestamp)
{
    int64_t milli = timestamp+ (int64_t)8*60*60*1000;//此处转化为东八区北京时间，如果是其它时区需要按需求修改
    auto mTime = std::chrono::milliseconds(milli);
    auto tp=std::chrono::time_point<std::chrono::system_clock,std::chrono::milliseconds>(mTime);
    auto tt = std::chrono::system_clock::to_time_t(tp);
//    printf("%4d年%02d月%02d日 %02d:%02d:%02d\n",now->tm_year+1900,now->tm_mon+1,now->tm_mday,now->tm_hour,now->tm_min,now->tm_sec);
   return *std::gmtime(&tt);
}
string gettimenow()
{
    time_t time_now; time(&time_now);
    stringstream out;
    out<<time_now;

    return  out.str();

}
/**
 * @function: 获取cate_dir目录下的所有文件名
 * @param: cate_dir - string类型
 * @result：vector<string>类型
*/
vector<string> getFiles(string cate_dir)
{
    vector<string> files;//存放文件名

    DIR *dir;
    struct dirent *ptr;
    char base[1000];

    if ((dir=opendir(cate_dir.c_str())) == NULL)
        {
        perror("Open dir error...");
                exit(1);
        }
    cate_dir = (cate_dir.length()>1&&(cate_dir[cate_dir.size()-1]=='/'|cate_dir[cate_dir.size()-1]=='\\')?cate_dir:cate_dir+'/');
    while ((ptr=readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
                continue;
        else if(ptr->d_type == 8)    ///file
            //printf("d_name:%s/%s\n",basePath,ptr->d_name);
            files.push_back(cate_dir + ptr->d_name);
        else if(ptr->d_type == 10)    ///link file
            //printf("d_name:%s/%s\n",basePath,ptr->d_name);
            continue;
        else if(ptr->d_type == 4)    ///dir
        {
            vector<string> tmp = getFiles(cate_dir+ptr->d_name);
            files.insert(files.end(), tmp.begin(), tmp.end());

        }
    }
    closedir(dir);


    //排序，按从小到大排序
    sort(files.begin(), files.end());
    return files;
}
string GetLocalIP()
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
