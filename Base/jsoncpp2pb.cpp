
#include "jsoncpp2pb.h"
#include "bin2ascii.h"
#include <sstream>

typedef Json::Writer JsonWriter;
typedef Json::Reader JsonReader;
typedef Json::Value  JsonValue;

void FormatToJson(Json::Value& value, const ::google::protobuf::Message& msg);
void FormatRepeatedField(Json::Value& value, const ::google::protobuf::Message& msg, const google::protobuf::FieldDescriptor *field, const ::google::protobuf::Reflection *reflection)
{
        if (NULL == field || NULL == reflection)
        {
                FormatToJson(value, msg);
        }

        for (int i = 0; i < reflection->FieldSize(msg, field); ++i)
        {
                Json::Value tmp_value;
                switch (field->type())
                {
                case FieldDescriptor::TYPE_MESSAGE:
                        {
                                const Message& tmp_msg = reflection->GetRepeatedMessage(msg, field, i);
                                if (0 != tmp_msg.ByteSize())
                                {
                                        FormatToJson(tmp_value, tmp_msg);
                                }
                        }
                        break;
                case FieldDescriptor::TYPE_INT32:
                        tmp_value = reflection->GetRepeatedInt32(msg, field, i);
                        break;
                case FieldDescriptor::TYPE_UINT32:
                        tmp_value = reflection->GetRepeatedUInt32(msg, field, i);
                        break;
                case FieldDescriptor::TYPE_INT64:
                        {
//                                static char int64str[25];
//                                memset(int64str, 0, sizeof(int64str));
//                                snprintf(int64str, sizeof(int64str), "%lld", (long long)reflection->GetRepeatedInt64(msg, field, i));
//                                tmp_value = int64str;
                                  tmp_value = (long long)reflection->GetRepeatedInt64(msg, field, i);
                        }
                        break;
                case FieldDescriptor::TYPE_UINT64:
                        {
//                                static char uint64str[25];
//                                memset(uint64str, 0, sizeof(uint64str));
//                                snprintf(uint64str, sizeof(uint64str), "%llu", (unsigned long long)reflection->GetRepeatedUInt64(msg, field, i));
//                                tmp_value = uint64str;
                                  tmp_value = (unsigned long long)reflection->GetRepeatedUInt64(msg, field, i);
                        }
                        break;
                case FieldDescriptor::TYPE_STRING:
                case FieldDescriptor::TYPE_BYTES:
                        tmp_value = reflection->GetRepeatedString(msg, field, i);
                        break;
                default:
                        break;
                }
                value.append(tmp_value);
        }
}
void FormatToJson(Json::Value& value, const ::google::protobuf::Message& msg)
{
    const Descriptor* descriptor = msg.GetDescriptor();
    const Reflection* reflection = msg.GetReflection();

        const int count = descriptor->field_count();

    for (int i = 0; i < count; ++i)
    {
        const FieldDescriptor* field = descriptor->field(i);

        if (field->is_repeated())
        {
            if (reflection->FieldSize(msg, field) > 0)
            {
                FormatRepeatedField(value[field->name()], msg, field, reflection);
            }
            continue;
        }

        if (!reflection->HasField(msg, field))
        {
            continue;
        }

        switch (field->type())
        {
        case FieldDescriptor::TYPE_MESSAGE:
            {
                const Message& tmp_msg = reflection->GetMessage(msg, field);
                if (0 != tmp_msg.ByteSize())
                {
                    FormatToJson(value[field->name()], tmp_msg);
                }
            }
            break;
        case FieldDescriptor::TYPE_INT32:
            value[field->name()] = reflection->GetInt32(msg, field);
            break;
        case FieldDescriptor::TYPE_UINT32:
            value[field->name()] = reflection->GetUInt32(msg, field);
            break;
        case FieldDescriptor::TYPE_INT64:
            {
//                static char int64str[25];
//                memset(int64str, 0, sizeof(int64str));
//                snprintf(int64str, sizeof(int64str), "%lld", (long long)reflection->GetInt64(msg, field));
//                value[field->name()] = int64str;
                  value[field->name()] = (long long)reflection->GetInt64(msg, field);
            }
            break;
        case FieldDescriptor::TYPE_UINT64:
            {
//                static char uint64str[25];
//                memset(uint64str, 0, sizeof(uint64str));
//                snprintf(uint64str, sizeof(uint64str), "%llu", (unsigned long long)reflection->GetUInt64(msg, field));
//                value[field->name()] = uint64str;
                  value[field->name()] = (unsigned long long)reflection->GetUInt64(msg, field);
            }
            break;
        case FieldDescriptor::TYPE_STRING:
        case FieldDescriptor::TYPE_BYTES:
            {
                value[field->name()] = reflection->GetString(msg, field);
            }
            break;
        default:
            break;
        }
    }
}






string pb2json( const ::google::protobuf::Message& msg)
{

        Json::Value value;
        const Descriptor* descriptor = msg.GetDescriptor();
        const Reflection* reflection = msg.GetReflection();

        const int count = descriptor->field_count();

        for (int i = 0; i < count; ++i)
        {
                const FieldDescriptor* field = descriptor->field(i);

                if (field->is_repeated())
                {
                        if (reflection->FieldSize(msg, field) > 0)
                        {
                                FormatRepeatedField(value[field->name()], msg, field, reflection);
                        }
                        continue;
                }

                if (!reflection->HasField(msg, field))
                {
                        continue;
                }

                switch (field->type())
                {
                case FieldDescriptor::TYPE_MESSAGE:
                        {
                                const Message& tmp_msg = reflection->GetMessage(msg, field);
                                if (0 != tmp_msg.ByteSize())
                                {
                                        FormatToJson(value[field->name()], tmp_msg);
                                }
                        }
                        break;
                case FieldDescriptor::TYPE_INT32:
                        value[field->name()] = reflection->GetInt32(msg, field);
                        break;
                case FieldDescriptor::TYPE_UINT32:
                        value[field->name()] = reflection->GetUInt32(msg, field);
                        break;
                case FieldDescriptor::TYPE_INT64:
                        {
//                                static char int64str[25];
//                                memset(int64str, 0, sizeof(int64str));
//                                snprintf(int64str, sizeof(int64str), "%lld", (long long)reflection->GetInt64(msg, field));
//                                value[field->name()] = int64str;
                                  value[field->name()] = (long long)reflection->GetInt64(msg, field);
                        }
                        break;
                case FieldDescriptor::TYPE_UINT64:
                        {
//                                static char uint64str[25];
//                                memset(uint64str, 0, sizeof(uint64str));
//                                snprintf(uint64str, sizeof(uint64str), "%llu", (unsigned long long)reflection->GetUInt64(msg, field));
//                                value[field->name()] = uint64str;
                                  value[field->name()] = (unsigned long long)reflection->GetUInt64(msg, field);
                        }
                        break;
                case FieldDescriptor::TYPE_STRING:
                case FieldDescriptor::TYPE_BYTES:
                        {
                                value[field->name()] = reflection->GetString(msg, field);
                        }
                        break;
                case FieldDescriptor::TYPE_ENUM:
                        {
                                int intvalue = reflection->GetEnum(msg, field)->number();
                                value[field->name()] = intvalue;
                        }
                default:
                        break;
                }
        }

        Json::StyledWriter styled_writer;
        return styled_writer.write(value);
}

static void _json2field(Message &msg, const FieldDescriptor *field, Json::Value& jf);
static void _json2pb(Message& msg, Json::Value& root)
{
    const Descriptor *d = msg.GetDescriptor();
    const Reflection *ref = msg.GetReflection();
    if (!d || !ref) throw j2pb_error("No descriptor or reflection");


    JsonValue::Members mem = root.getMemberNames();
    for (auto iter = mem.begin(); iter != mem.end(); iter++)
    {
        const std::string &name = *iter;
        Json::Value jf = root[name];

        const FieldDescriptor *field = d->FindFieldByName(name);
        if (!field)
            field = ref->FindKnownExtensionByName(name);
            //field = d->file()->FindExtensionByName(name);

        if (!field) throw j2pb_error("Unknown field: " + std::string(name));


        if (field->is_repeated()) {
            if (!jf.isArray())
                throw j2pb_error(field, "Not array");
            for (int j = 0; j < jf.size(); j++)
//                FormatRepeatedField(jf[j],msg,field,ref);
                _json2field(msg, field, jf[j]);
        } else
//            FormatRepeatedField(jf,msg,field,ref);
              _json2field(msg, field, jf);
    }
}

void json2pb(Message &msg, string buf)
{
    Json::Value root;
    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;
    JSONCPP_STRING errs;
    Json::CharReader* reader = builder.newCharReader();
    bool bol = reader->parse(buf.c_str(), buf.c_str() + buf.size(), &root, &errs);

    if (!bol)
        throw j2pb_error(std::string("Load failed: ") + errs);

    if (!root.isObject())
        throw j2pb_error("Malformed JSON: not an object");

    _json2pb(msg, root);
    delete reader;
}
static void _json2field(Message &msg, const FieldDescriptor *field, Json::Value& jf)
{
    const Reflection *ref = msg.GetReflection();
    const bool repeated = field->is_repeated();

    switch (field->cpp_type())
    {
#define _SET_OR_ADD(sfunc, afunc, value)			\
        do {						\
            if (repeated)				\
                ref->afunc(&msg, field, value);	\
            else					\
                ref->sfunc(&msg, field, value);	\
        } while (0)
        case FieldDescriptor::CPPTYPE_STRING: {
            if (!jf.isString())
                throw j2pb_error(field, "Not a string");
            const char * value = jf.asCString();
            if(field->type() == FieldDescriptor::TYPE_BYTES)
                _SET_OR_ADD(SetString, AddString, b64_decode(value));
            else
                _SET_OR_ADD(SetString, AddString, value);
            break;
        }
        case FieldDescriptor::CPPTYPE_MESSAGE: {
            Message *mf = (repeated)?
                ref->AddMessage(&msg, field):
                ref->MutableMessage(&msg, field);
            _json2pb(*mf, jf);
            break;
        }
        case FieldDescriptor::CPPTYPE_ENUM: {
            const EnumDescriptor *ed = field->enum_type();
            const EnumValueDescriptor *ev = 0;
            if (jf.isIntegral()) {
                ev = ed->FindValueByNumber(jf.asInt());
            } else if (jf.isString()) {
                ev = ed->FindValueByName(jf.asCString());
            } else
                throw j2pb_error(field, "Not an integer or string");
            if (!ev)
                throw j2pb_error(field, "Enum value not found");
            _SET_OR_ADD(SetEnum, AddEnum, ev);
            break;
        }
        case FieldDescriptor::CPPTYPE_INT32:{
            _SET_OR_ADD(SetInt32, AddInt32, jf.asInt());
            break;
        }
        case FieldDescriptor::CPPTYPE_INT64:{
            int64 tmp_value;
            std::stringstream istream(jf.asString());
            istream>>tmp_value;
//            _SET_OR_ADD(SetInt64, AddInt64, tmp_value);
              _SET_OR_ADD(SetInt64,AddInt64,jf.asInt64());
            break;
        }
        default:
            break;
    }


}


//搜索json得到指定key的值,只支持stringvalue
//如果strcontent不为空，使用strcontent解析,如果为空直接使用v
string GetJsonStringValue(string strContent, string& strKey, Json::Value v)
{
    if (!strContent.empty())
    {
        Json::Reader reader;
        if (!reader.parse(strContent, v))
        {
            return string("");
        }
    }

    Json::Value::Members mem = v.getMemberNames();
    Json::Value::Members::iterator it;
    //遍历所有key广度搜索
    for (it = mem.begin(); it != mem.end(); it++)
    {
        if (*it == strKey)
        {
            if (v[*it].type() == Json::stringValue)
            {
                return v[*it].asString();
            }
            return string("");
        }
    }

    //如果未找到，再深度搜索
    for (it = mem.begin(); it != mem.end(); it++)
    {
        Json::ValueType type = v[*it].type();
        if (type == Json::objectValue)
        {
            string strvalue = GetJsonStringValue(string(""), strKey, v[*it]);
            if (!strvalue.empty())
            {
                return strvalue;
            }
        }
        else if (type == Json::arrayValue)
        {
            for (int i = 0; i < v[*it].size(); i++)
            {
                //如果在递归的过程中已找到，则返回
                string strvalue = GetJsonStringValue(string(""), strKey, v[*it][i]);
                if (!strvalue.empty())
                {
                    return strvalue;
                }
            }
        }
    }

    return string("");
}
