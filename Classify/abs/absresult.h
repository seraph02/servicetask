#ifndef ABSRESULT_H
#define ABSRESULT_H
#include <vector>
#include<string>
using namespace std;
//absresult
class absresult
{
public:
    vector<string> getfile()
    {
        return vfile;
    }
    vector<string> getjson()
    {
        return vjson;
    }
    void putfile(string file)
    {
        vfile.push_back(file);
    }
    void putjson(string json)
    {
        vjson.push_back(json);
    }
private:
    vector<string> vfile;
    vector<string> vjson;
};

#endif // ABSRESULT_H
