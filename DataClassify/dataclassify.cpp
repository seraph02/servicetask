#include "dataclassify.h"
#include "absapp.h"
#include "reflex.h"
#include "app/app.h"
#include "glog/logging.h"
DataClassify::DataClassify()
{
    this->appmap["fbmcontrol"]="FBM";
    this->appmap["imocontrol"]="IMO";
    this->appmap["voxercontrol"]="VOXER";
    this->appmap["telegramcontrol"]="TELEGRAM";
}

void DataClassify::Go(string result,string appname,string key)
{
    string strret = result;
    string apptype =appname;
    string classtype = appmap[appname];
    absAPP* ptrObj=(absAPP*)ClassFactory::getInstance().getClassByName(classtype);
    absresult* rst;
    if(ptrObj!=NULL)
    {
        ptrObj->setkey(key);
        rst = ptrObj->run(strret);
        vector<string> filelist = rst->getfile();
        vector<string> jsonlist = rst->getjson();
        for (auto val : filelist)
        {
            LOG(INFO) << "file " << val << endl;
        }

        if(jsonlist.size()<1){delete ptrObj;return;}
//        stringstream ss;
        cout<<"[";
        for(vector<string>::iterator it = jsonlist.begin();it!=jsonlist.end();)
//        for (string val : jsonlist)
        {
            cout<<*it;
            ++it;
            if(it!=jsonlist.end()) cout<<",";
//            LOG(INFO) << "json " << *it << endl;
        }
        cout<<"]";

//        LOG(INFO) << "json " << *it << endl;



        delete ptrObj;
    }
}
