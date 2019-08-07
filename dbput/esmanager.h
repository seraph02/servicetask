#ifndef ESMANAGER_H
#define ESMANAGER_H
#include <elasticlient/client.h>
#include <elasticlient/bulk.h>
#include <string>
#include <vector>
using std::string;
using std::vector;
class esmanager
{
public:
    esmanager();
    int Go();

private:
    long bulkcount=0;
    string bulkindices="";
    vector<string> m_hosts ={"http://localhost:9200/"};
    void POSTBulkend(elasticlient::SameIndexBulkData& bulkdata);
    bool POSTBulkdata(elasticlient::SameIndexBulkData& bulkdata,string type,string docid,string data);
};

#endif // ESMANAGER_H
