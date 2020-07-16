#include "telegramcontrol.h"
#include "comm.h"
#include "appconf.h"
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include "glog/logging.h"
#include "Base64Encoder.h"



void TELEGRAMControl::ProcessFile(string file, string filename, string fileext)
{
    Json::Value jarray;
    try
    {
        LOG(INFO)<<file;
        if(fileext.rfind("session") != string::npos)
        {
            Json::Value jfile;
            string strcmd="";
            ostringstream ocmd;
            ocmd << "base64 "<<file<<" |tr '\n' '@' > "<<filename<<".b64";
            strcmd = ocmd.str();
            RunShell(strcmd.c_str());
            string fbase64 = ReadLocalFile(filename+".b64");
            ostringstream omcmd;
            omcmd<<"rm "<<filename<<".b64";
            strcmd = omcmd.str();
            RunShell(strcmd.c_str());
            jfile["phone"]=app->getkey();
            jfile["fname"]=filename;
            jfile["fbase64"]=fbase64;
            jarray.append(jfile);
            ProcessSession(jarray);
        }
        else if(fileext.rfind("txt") != string::npos)
        {
            string abspath="";
            string strkey=app->getkey();
            if(file.find(app->getkey())!=string::npos)
            {
                abspath=file.substr(file.find(app->getkey()));
                vector<string> tp;
                SplitString(abspath,tp,"/");
                //LOG(INFO)<<tp.size();
                if(tp.size()>2) return;
                //LOG(INFO)<<file;

            }

            Json::Value jarray;
            try
            {
                int linenum = getFiletotals(file);

                if(filename.rfind("contacts")!=string::npos ||filename.rfind("contact")!=string::npos)
                {
                    for(int i=0;i<linenum;i++)
                    {

                        Json::Value jfile;


                        string bodystr=ReadLocalFile(file,i+1);

                        string other=bodystr;
                        //LOG(INFO)<<bodystr;
                        string strcmd="";
                        stringstream ocmd;
                        Base64Encoder b64;
                        string strbase64 = b64.encode(bodystr);
                        boost::replace_all(strbase64,"\n","");
                        //LOG(INFO)<<strbase64;
                        ofstream ofs;
                        ofs.open("bs64_py.txt", ofstream::out | ofstream::binary);//"datareslut"
                        if (ofs)
                        {
                            ofs<<strbase64;
                            ofs.close();
                        }

                        //ocmd<<"python3 telec2j.py "<<strbase64;
                        ocmd<<"python3 telec2j.py bs64_py.txt f";
                        strcmd=ocmd.str();
                        string pyrst = RunShell(strcmd.c_str());
                        try{
                            Json::Reader jread;
                            if(!jread.parse(pyrst,jfile)||jfile.isNull()){throw exception();}
                            //LOG(INFO)<<pyrst;
                            //LOG(INFO)<<jfile["id"];
                        }
                        catch(exception &e)
                        {

                        }

                        //jfile["other"]=other;
                        jarray.append(jfile);

                    }
                    ProcessContacts(jarray);
                }
                else if(filename.rfind("group_user")!=string::npos)
                {
                    for(int i=0;i<linenum;i++)
                    {

                        Json::Value jfile;
                        string bodystr=ReadLocalFile(file,i+1);
                        if((i+1)%100==0)
                            LOG(INFO)<<file<<"("<<i+1<<")";
                        string other=bodystr;
                        string strcmd="";
                        stringstream ocmd;
                        Base64Encoder b64;
                        string strbase64 = b64.encode(bodystr);

                        boost::replace_all(strbase64,"\n","");
                        //LOG(INFO)<<bodystr;
                        //LOG(INFO)<<strbase64;
                        ofstream ofs;
                        ofs.open("bs64_py.txt", ofstream::out | ofstream::binary);//"datareslut"
                        if (ofs)
                        {
                            ofs<<strbase64;
                            ofs.close();
                        }

                        //ocmd<<"python3 telec2j.py "<<strbase64;
                        ocmd<<"python3 telec2j.py bs64_py.txt f";
                        strcmd=ocmd.str();
                        string pyrst = RunShell(strcmd.c_str());
                        try{
                            Json::Reader jread;
                            if(!jread.parse(pyrst,jfile)||jfile.isNull()){throw exception();}
                            //LOG(INFO)<<pyrst;
                            //LOG(INFO)<<jfile["id"];
                        }
                        catch(exception &e)
                        {

                            LOG(INFO)<<pyrst;
                        }
                        string mid=jfile["id"].asString();
                        string fname=filename;
                        boost::replace_all(fname,fileext,"");
                        boost::replace_all(fname,"group_user","");
                        jfile["g_id"] = fname;
                        if(!mid.empty()) jfile["docid"]=fname+mid;
                        //jfile["other"]=other;
                        jarray.append(jfile);
                    }
                    ProcessArray(jarray,"groupuser",0);

                }
                else// if(filename.rfind("message")!=string::npos)
                {
                    for(int i=0;i<linenum;i++)
                    {
                        Json::Value jfile;
                        string bodystr=ReadLocalFile(file,i+1);
                        string other=bodystr;
                        string strcmd="";
                        stringstream ocmd;
                        Base64Encoder b64;
                        string strbase64 = b64.encode(bodystr);

                        boost::replace_all(strbase64,"\n","");
                        ofstream ofs;
                        ofs.open("bs64_py.txt", ofstream::out | ofstream::binary);//"datareslut"
                        if (ofs)
                        {
                            ofs<<strbase64;
                            ofs.close();
                        }

                        //ocmd<<"python3 telec2j.py "<<strbase64;
                        ocmd<<"python3 telec2j.py bs64_py.txt f";
                        strcmd=ocmd.str();
                        string pyrst = RunShell(strcmd.c_str());
                        try{
                            Json::Reader jread;
                            if(!jread.parse(pyrst,jfile)||jfile.isNull()){throw exception();}
                            //LOG(INFO)<<pyrst;
                            //LOG(INFO)<<jfile["id"];
                        }
                        catch(exception &e)
                        {

                            LOG(ERROR)<<pyrst<<strcmd;
                        }


                        string type="";
                        string url="";
                        Json::Value attachments=jfile["media"];
                        if(!attachments.isNull())
                        {
                                //LOG(INFO)<<attachments;
                            stringstream opath;
                            string strkey=file;
                            boost::replace_all(strkey,"/"+filename,"");
                            //strkey=strkey.substr(strkey.find_last_of("/")+1);
                            string channelid=filename;
                            boost::replace_all(channelid,"_message.txt","");
                                if(!attachments["classname"].isNull())
                                {
                                    type=attachments["classname"].asString();
                                    if(type.find("MediaPhoto")!=string::npos)
                                    {
                                        opath<<strkey<<"/"<<channelid<<"/"<<attachments["photo"]["id"]<<".jpg";
                                        string strphotopath=opath.str();
                                        jfile["media"]["filepath"]=strphotopath;
                                    }
                                    else if(type.find("MediaWebPage")!=string::npos)
                                    {
//                                        type=attachments["classname"].asString();
//                                        LOG(INFO)<<type;
                                    }
                                    else if(type.find("MediaDocument")!=string::npos)
                                    {
                                        type=attachments["classname"].asString();
                                        //LOG(INFO)<<type;
                                        //LOG(INFO)<<attachments;
                                        string mime_type=attachments["document"]["mime_type"].asString();
                                        Json::Value jarray=attachments["document"]["attributes"];
                                        Json::Value jsonfileame;
                                        for(int aid=0;aid<jarray.size();aid++)
                                        {
                                            jsonfileame=jarray[aid]["file_name"];
                                            if(!jsonfileame.isNull()) break;

                                        }

                                        string strfilename="";
                                        if(!jsonfileame.isNull()) strfilename=jsonfileame.asString();
                                        string attext=mime_type;
                                        attext=attext.substr(attext.find("/")+1);
                                        //LOG(INFO)<<attext;
                                        if(attext.find("zip")!=string::npos){
                                           //LOG(INFO)<<attext;
                                        }
                                        opath<<strkey<<"/"<<channelid<<"/";
                                        if(!strfilename.empty())
                                        {
                                            //LOG(INFO)<<strfilename;
                                            opath<<strfilename;
                                        }
                                        else
                                            opath<<attachments["document"]["id"]<<"."<<attext;
                                        string strfilepath=opath.str();
                                        jfile["media"]["filepath"]=strfilepath;

                                    }
                                }



//                            if(attachments.find("ImageAttachment")!=string::npos)
//                            {
//                                url=getkey(attachments,"preview_url");
//                            }
//                            else
//                                url=getkey(attachments,"url");
//                            boost::replace_all(url,"'","");
//                            type=attachments.substr(0,attachments.find("("));

//                            if(!url.empty()&&attachments.find("ShareAttachment")==string::npos){

//                            //down avatar file
//                            string path_dir = APPCONF::getInstance()->refpath +"app/fbm/"+app->getkey()+"/"+ type +"/";
//                            checkdir(path_dir);
//                            string fileext=getfileext(url);
//                            string path= path_dir + filename4time(fileext);
//                            try
//                            {
//                                //cout<<url<<endl;

//                                if(DOWNLOAD_FILE(url.c_str(),path.c_str())==0)
//                                {
//                                    jfile["localurl"]=(GetCurrPath()+path).c_str();
//                                }
//                            }
//                            catch(...)

//                            {

//                            }
//                            }
                        }
                        else
                        {
                            type="message";
                        }
                        string mid=jfile["id"].asString();
                        string fname=filename;
                        boost::replace_all(fname,fileext,"");
                        boost::replace_all(fname,"message","");
                        if(!mid.empty())
                            jfile["docid"]=fname+mid;
//                        else
//                        {
//                            cout<<"aa";
//                        }
                        jfile["type"]=type;
                        //jfile["other"]=other;
                        jarray.append(jfile);

                    }
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
    catch(exception &e)
    {

    }

}
void TELEGRAMControl::ProcessSession(Json::Value jbody)
{
    if(jbody.size()<1) return;
    for(int ind =0;ind<jbody.size();ind++)
    {
        Json::Value jones = jbody[ind];
        if(jones.isNull() || !jones.isObject()) continue;
        jbody[ind]=jones;

        Json::Value jelement;
        jelement["type"]="session";
        jelement["tag"]= dt;
        jelement["body"]=jbody;
        Json::FastWriter jwrite;
        string strsession = jwrite.write(jelement);
        rst.putjson(strsession);
    }
}
