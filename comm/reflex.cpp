#include "reflex.h"

//工厂类的实现

//@brief:获取工厂类的单个实例对象
ClassFactory& ClassFactory::getInstance(){
    static ClassFactory sLo_factory;
    return sLo_factory ;
}

//@brief:通过类名称字符串获取类的实例
void* ClassFactory::getClassByName(string className){
    map<string, PTRCreateObject>::const_iterator iter;
    iter = m_classMap.find(className) ;
    if ( iter == m_classMap.end() )
        return NULL ;
    else
        return iter->second() ;
}

//@brief:将给定的类名称字符串和对应的创建类对象的函数保存到map中
void ClassFactory::registClass(string name, PTRCreateObject method){
    m_classMap.insert(pair<string, PTRCreateObject>(name, method)) ;
}
