#ifndef JSONCPP2PB_H
#define JSONCPP2PB_H
#include<stdio.h>

#include <jsoncpp/json/json.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

using namespace ::google::protobuf;
void json2pb(Message &msg, string buf);
string pb2json(const ::google::protobuf::Message& msg);
void serialize_message(const google::protobuf::Message& message, std::string* serialized_string);
string GetJsonStringValue(string strContent, string& strKey, Json::Value v);

class j2pb_error : public std::exception {
    std::string _error;
public:
    j2pb_error(const std::string &e) : _error(e) {}
    j2pb_error(const FieldDescriptor *field, const std::string &e) : _error(field->name() + ": " + e) {}

    virtual ~j2pb_error() throw() {};

    virtual const char *what() const throw () { return _error.c_str(); };
};
#define JSON_ERROR_TEXT_LENGTH    160
#define JSON_ERROR_SOURCE_LENGTH   80
#define JSON_STRICT         0x2
typedef struct json_error_t {
    int line;
    int column;
    int position;
    char source[JSON_ERROR_SOURCE_LENGTH];
    char text[JSON_ERROR_TEXT_LENGTH];
} json_error_t;
#endif // JSONCPP2PB_H

