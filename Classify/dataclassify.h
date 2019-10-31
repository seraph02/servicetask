#ifndef DATACLASSIFY_H
#define DATACLASSIFY_H

#include <string>
#include<vector>
#include<map>

using std::string;
using std::vector;
using std::map;
class DataClassify
{
public:
    DataClassify();
    void Go(string result,string appname,string key);
private:
    map<string,string> appmap;
    string getmapping(string appname);

};

#endif // DATACLASSIFY_H
