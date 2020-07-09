#ifndef ABSRESULT_H
#define ABSRESULT_H
#include <vector>
#include<string>
#include <fstream>
#include <boost/algorithm/string.hpp>
using namespace std;
//absresult
class absresult
{

public:
    absresult()
    {
        writelocalstart();
    }
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
        //if(vjson.size()<10) vjson.push_back(json);
        putlocalfile(json);
    }
    ~absresult()
    {
        writelocalend();
    }
private:
    ofstream ofs;
    vector<string> vfile;
    vector<string> vjson;
    void writelocalstart()
    {
        ofs.open("dataresult", ofstream::out | ofstream::binary);//"datareslut"
        if (ofs)
        {
            ofs<<"\n";
        }

    }
    void writelocalend()
    {
        if(ofs)
        {

            ofs.seekp(-1,std::ios::end);
            ofs<<"\n";
            ofs.close();
        }
    }
    void putlocalfile(string json)
    {
        boost::replace_all(json,"\n","");

                if (ofs)
                {
                    ofs<<json<<"\n";
                }

    }
};

#endif // ABSRESULT_H
