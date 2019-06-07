#include "absapp.h"
absresult* absAPP::run(string body)
{
    if(NULL == control) return NULL;
    this->control->set(body);//set body
    this->control->body2files();//process body 2 files
    this->control->work();//process files and dataclassify
    return this->control->produce();//return jsonlist
}
