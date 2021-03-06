#ifndef COMM_H
#define COMM_H

#include <string>
#include <vector>
#include <chrono>
#include <curl/curl.h>

using std::string;
using std::vector;
 string RunShell(const char*);
 string GetCurrPath();
 bool WriteLocalFile(string filename,string jsonbody);
 string ReadLocalFile(string filename);
 void SplitString(const string& s,std::vector<string> &, const string& c);
//static void Binarycout(int n);
 void trim(char *str);
void checkdir(string);
string filename4time(string);
string getfileext(string filename);
void b64_decode2file(string b64str,string dstfile);
std::tm gettm(int64_t timestamp);
string gettimenow();
int getFiletotals(string filename);
string ReadLocalFile(string filename,int linenum);
vector<string> getFiles(string cate_dir);
string GetLocalIP();
void printtime();
string ReadLine(string filename,int line);
CURLcode HttpGet(const std::string & strUrl, std::string & strResponse,int nTimeout);
#endif // COMM_H
