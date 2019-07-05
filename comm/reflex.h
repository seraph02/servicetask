#ifndef REFLEX_H
#define REFLEX_H
#include<string>
#include<map>
using namespace std;
typedef void* (*PTRCreateObject)(void);

//工厂类的定义
class ClassFactory{
private:
    map<string, PTRCreateObject> m_classMap ;
    ClassFactory(){} //构造函数私有化

public:
    void* getClassByName(string className);
    void registClass(string name, PTRCreateObject method) ;
    static ClassFactory& getInstance() ;
};


//注册动作类
class RegisterAction{
public:
    RegisterAction(string className,PTRCreateObject ptrCreateFn){
        ClassFactory::getInstance().registClass(className,ptrCreateFn);
    }
};

#define REGISTER(className)                                             \
    className* objectCreator##className(){                              \
        return new className;                                           \
    }                                                                   \
    RegisterAction g_creatorRegister##className(                        \
        #className,(PTRCreateObject)objectCreator##className)


#endif // REFLEX_H
