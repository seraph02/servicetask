#ifndef EXPCLASSIFY_H
#define EXPCLASSIFY_H
#include "string"
#include "jsoncpp/json/json.h"
using std::string;
class expclassify
{
private:
    string dcfexename ="dataclassify";
public:
    expclassify();
    void Go(Json::Value args);
    bool PUSHRemoteDataCF( string info,string taskid,string strkey,string indices ,string resultjson);
    bool POSTTaskResult(string indices,string strpostdata);

};

#endif // EXPCLASSIFY_H
