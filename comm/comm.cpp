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
    char buff[1024];
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
     std::string contents(buffer.str());
    return contents;

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
        system(cmd.c_str());
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
