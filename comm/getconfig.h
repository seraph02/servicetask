#ifndef _GET_CONFIG_H_
#define _GET_CONFIG_H_
#define COMMENT_CHAR '#'
#include <string>
#include <map>

using namespace std;

bool ReadConfig(const string & filename, map<string, string> & m);
void PrintConfig(const map<string, string> & m);

#endif
