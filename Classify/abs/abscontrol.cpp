#include "abscontrol.h"
#include "bin2ascii.h"
#include "jsoncpp/json/json.h"
#include <algorithm>

#include <curl/curl.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glog/logging.h>
#include "comm.h"
#include "boost/algorithm/string/replace.hpp"
#include <boost/regex.hpp>
#include<sys/stat.h>
using namespace std;


//struct myprint
//{
//    void operator()(string str) {cout<<str<<endl;}
//};
void absControl::body2files()
{
    string strbody;
    try
    {
    //base64decode
        strbody = b64_decode(this->body);
    }
    catch(exception &de)
    {
        LOG(ERROR)<<"b64_decode :"<<de.what();
        return;
    }

    Json::Value jbody;
    Json::Reader jread;
    if(!jread.parse(strbody,jbody)||jbody.isNull() ||!jbody.isObject()) {return;}

    string args=jbody["args"].asString();
    Json::Value::Members mem = jbody.getMemberNames();
    for (auto iter = mem.begin(); iter != mem.end(); iter++)
    {
        std::string strkey = *iter;
        std::size_t found = strkey.rfind("Path")!=string::npos?strkey.rfind("Path"):(strkey.rfind("path"));

        if (found==std::string::npos) continue;
        //have *Path field
        Json::Value PathValue = jbody[*iter];
        if(PathValue.isString())
        {
            //only one filepath
            string pathvalue=jbody[*iter].asString();
            if(pathvalue.empty()) continue;
            vector<string> tmp = path2files(pathvalue);
            if(tmp.size()<1) continue;
            //files.resize(files.size()+tmp.size());
            files.insert(files.end(),tmp.begin(),tmp.end());
            sort(files.begin(), files.end());
            auto iter = unique(files.begin(), files.end());
            files.erase(iter, files.end());
        }
        else if(!PathValue.isNull() && PathValue.isArray())
        {//have mult filepath
            for(int ind =0;ind<PathValue.size();ind++)
            {
                Json::Value PathValueEm = PathValue[ind];
                if(PathValueEm.isNull() || !PathValueEm.isString())continue;

                string pathvalue=PathValueEm.asString();
                if(pathvalue.empty()) continue;
                vector<string> tmp =path2files(pathvalue);
                if(tmp.size()<1) continue;
                files.resize(files.size()+tmp.size());
                files.insert(files.end(),tmp.begin(),tmp.end());

                sort(files.begin(), files.end());
                auto iter = unique(files.begin(), files.end());
                files.erase(iter, files.end());
            }
        }
    }
//    for_each(files.begin(),files.end(),myprint());
}
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    try
    {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
    }
    catch(...)
    {
        return 0;
    }
}



/*
               Function:   libcurl connection initialization  download  file
             Parameters:   (const char* url, const char outfilename[FILENAME_MAX])
                    url:   要下载文件的url地址
            outfilename:   下载文件指定的文件名
*/
int DOWNLOAD_FILE(const char* url, const char outfilename[FILENAME_MAX]){
    try
    {
    //    cout<<url<<endl;
     //   if(url.find("None")!=string::npos) return -1;
    CURL *curl;
    FILE *fp;
    CURLcode res;
    /*   调用curl_global_init()初始化libcurl  */
    res = curl_global_init(CURL_GLOBAL_ALL);
    if (CURLE_OK != res)
    {
        printf("init libcurl failed.");
        curl_global_cleanup();
        return -1;
    }
    /*  调用curl_easy_init()函数得到 easy interface型指针  */
    curl = curl_easy_init();
    if (curl) {

        fp =fopen(outfilename,"wb");

        /*  调用curl_easy_setopt()设置传输选项 */
        res = curl_easy_setopt(curl, CURLOPT_URL, url);
        if (res != CURLE_OK)
        {
            fclose(fp);
            curl_easy_cleanup(curl);
            return -1;
        }
        /*  根据curl_easy_setopt()设置的传输选项，实现回调函数以完成用户特定任务  */
        res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        if (res != CURLE_OK)
        {
            fclose(fp);
            curl_easy_cleanup(curl);
            return -1;
        }
        /*  根据curl_easy_setopt()设置的传输选项，实现回调函数以完成用户特定任务  */
        res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        if (res != CURLE_OK)
        {
            fclose(fp);
            curl_easy_cleanup(curl);
            return -1;
        }

        res = curl_easy_perform(curl);                               // 调用curl_easy_perform()函数完成传输任务
        if(fp!=NULL) fclose(fp);
        /* Check for errors */
        if(res != CURLE_OK){
            fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            LOG(ERROR)<<url;
            return -1;
        }

        /* always cleanup */
        curl_easy_cleanup(curl);                                     // 调用curl_easy_cleanup()释放内存

    }
    curl_global_cleanup();
    return 0;
    }
    catch(...)
    {
        LOG(ERROR)<<url<<endl;
        return 1;
    }

}
/*
 * mode 0   array to mulit
 *
 * mode 1   array to onelist
 */
void absControl::ProcessArray(Json::Value jbody,string tname,int mode)
{
    if(jbody.size()<1) return;
    if(mode==0){
        for(int ind =0;ind<jbody.size();ind++)
        {
            Json::Value jones = jbody[ind];
            if(jones.isNull() || !jones.isObject()) continue;
            Json::Value jelement;
            jelement["type"]=tname;
            jelement["tag"]= dt;
            jelement["body"]=jones;
            Json::FastWriter jwrite;
            string strcontacts = jwrite.write(jelement);
            rst.putjson(strcontacts);
        }
    }
    else if(mode==1)
    {
        for(int ind =0;ind<jbody.size();ind++)
        {
            Json::Value jones = jbody[ind];
            if(jones.isNull() || !jones.isObject()) continue;
            jbody[ind]=jones;
        }
        Json::Value jelement;
        jelement["type"]=tname;
        jelement["tag"]= dt;
        jelement["body"]=jbody;
        Json::FastWriter jwrite;
        string strcontacts = jwrite.write(jelement);
        rst.putjson(strcontacts);
    }
}
string Multiple2Array(string& body)
{
//        LOG(INFO)<<body;
        boost::algorithm::replace_all(body,"}\n{","},{");
        body = "["+body+"]";
    return body;
}

void absControl::ProcessFile(string file,string filename,string fileext)
{
    if((fileext.rfind("json")==string::npos && fileext.rfind("txt")==string::npos)) return;
    if(fileext.rfind("json") != string::npos)
    {
        string filebody =ReadLocalFile(file);
        Json::Value jfile;
        Json::Reader jread;
        Json::Value jarray;

        try
        {
            if(filebody.rfind("}\n{")!=string::npos) filebody = Multiple2Array(filebody);

            if(!jread.parse(filebody,jfile)||jfile.isNull()){throw exception();}
            else if(jfile.isArray())
            {
                jarray = jfile;
            }
            else if(jfile.isObject())
            {
                jarray.append(jfile);
            }


        }
        catch(exception &e)
        {
            LOG(ERROR)<<e.what()<<"parse error"<<file<<">>"<<filebody;
            return;
        }
        try
        {
            if(filename.rfind("dialog")!=string::npos)
            {
                ProcessDialog(jarray);
            }else if(filename.rfind("channel")!=string::npos)
            {
                ProcessChannel(jarray);
            }else if(filename.rfind("contacts")!=string::npos ||filename.rfind("contact")!=string::npos)
            {
                ProcessContacts(jarray);
            }
            else// if(filename.rfind("message")!=string::npos)
            {
                ProcessMessage(jarray);
            }
        }
        catch(exception &proe)
        {
            LOG(ERROR)<<proe.what();
            return;
        }

    }


}
void absControl::work()
{



    LOG(INFO)<<"WORK file:"<<files.size();
    for(int fileite =0;fileite< files.size();fileite++)
    {

        string file = files[fileite];
        //LOG(INFO)<<"WORK file:"<<fileite<<" name:"<<file;
        struct stat s;
        if (stat(file.c_str(),&s)!=0 || (s.st_mode & S_IFDIR)!=0){ continue ;}
        string filename;
        int findpos =file.rfind('/');if(findpos!=string::npos) filename = file.substr(findpos);
        filename = filename.size()>1?filename.substr(1):"";
        if(filename.size()<1) return;
        string fileext = getfileext(filename);
        if(fileext.empty()){return;}

        ProcessFile(file,filename,fileext);
//        cout <<file<<endl;


     }

}
string absControl::getkey(string data,string key)
{
    string keystr = key+"=(.*?), ";
    return getkey4reg(data,keystr);

}
string absControl::getkey4reg(string data,string regex)
{
    string keystr = regex;
    boost::regex reg(keystr);
    boost::smatch m;
    if (boost::regex_search(data,m,reg)) {
    if(m["val"].matched)
    {
        string ret(m["val"].str());
        return ret;
    }
    if (m[1].matched)
    {   string ret1(m[1].str());
        return ret1;
    }
    }


    return "";

}

void absControl::ProcessContacts(Json::Value jbody)
{
    //ProcessArray(jbody,"contacts",0);
    if(jbody.size()<1) return;
    for(int ind =0;ind<jbody.size();ind++)
    {
        Json::Value jones = jbody[ind];
        if(jones.isNull() || !jones.isObject()) continue;
        //filteravatar(jones);
        jbody[ind]=jones;

    }
    Json::Value jelement;
    jelement["type"]="contacts";
    jelement["tag"]= dt;
    jelement["body"]=jbody;
    Json::FastWriter jwrite;
    string strcontacts = jwrite.write(jelement);
    rst.putjson(strcontacts);
}

void absControl::ProcessMessage(Json::Value jbody)
{
    ProcessArray(jbody,"message",0);
//    if(jbody.size()<1) return;
//    for(int ind =0;ind<jbody.size();ind++)
//    {
//        Json::Value jones = jbody[ind];
//        if(jones.isNull() || !jones.isObject())return;
//        filtermessage(jones);

//        Json::Value jelement;
//        jelement["type"]="message";
//        jelement["tag"]= dt;
//        jelement["body"]=jones;
//        Json::FastWriter jwrite;
//        string strones = jwrite.write(jelement);
//        rst.putjson(strones);

//    }


}
void absControl::ProcessDialog(Json::Value jbody)
{
    for(int ind =0;ind<jbody.size();ind++)
    {
        Json::Value jones = jbody[ind];
        if(jones.isNull() || !jones.isObject()) continue;
        filterdialog(jones);
        jbody[ind]=jones;

    }
    Json::Value jelement;
    jelement["type"]="dialog";
    jelement["tag"]= dt;
    jelement["body"]=jbody;
    Json::FastWriter jwrite;
    string strcontacts = jwrite.write(jelement);
    rst.putjson(strcontacts);
}
void absControl::ProcessChannel(Json::Value jbody)
{
    for(int ind =0;ind<jbody.size();ind++)
    {
        Json::Value jones = jbody[ind];
        if(jones.isNull() || !jones.isObject()) continue;
        filterchannel(jones);
        jbody[ind]=jones;

    }
    Json::Value jelement;
    jelement["type"]="channel";
    jelement["tag"]= dt;
    jelement["body"]=jbody;
    Json::FastWriter jwrite;
    string strcontacts = jwrite.write(jelement);
    rst.putjson(strcontacts);
}
vector<string> path2files(string path)
{
    //is not exist
    struct stat s;

    if (stat(path.c_str(),&s)==0){
      if(s.st_mode & S_IFDIR){
        return getFiles(path);

      }else if (s.st_mode & S_IFREG){
          return {path};
      }
    }
    return vector<string>{};
}
