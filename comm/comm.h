#ifndef COMM_H
#define COMM_H

#include <string>
#include <vector>

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
#endif // COMM_H
