
#ifndef BASIC_CONFIG_H
#define BASIC_CONFIG_H


typedef int int32;
typedef long long int64;

//基本avp数据类型
enum BASIC_AVP_DATA_TYPE {
    UNDEFINE_TYPE = -1,
    OCTSTRING = 0,
    INT32,
    INT64,
    UINT32,
    UINT64,
    FLOAT32,
    FLOAT64,
    
    GROUPED,//MUST BE LAST
};

//扩展avp数据类型
enum DEVRIVED_AVP_DATA_TYPE {
    ADDRESS = GROUPED + 1,//扩展自OctetString
    TIME,//扩展自OctetString
    UTF8STRING,//扩展自OctetString
    DIAMETERID,//扩展自OctetString
    DIAMETERURI,
    ENUMERATED,//扩展自Integer32
    IP_FILTER_RULE,//扩展自OctetString
    QOS_FILTER_RULE,//扩展自OctetString
};

//如果上面avp数据类型位置有改动，请在相应的位置修改名称
const char *const AVP_DATA_TYPE_STRING[] = 
{
    "UNDEFINE_TYPE",
    "OctetString",
    "Integer32",
    "Integer64",
    "Unsigned32",
    "Unsigned64",
    "Float32",
    "Float64",
    "Grouped",
    "Address",
    "Time",
    "UTF8String",
    "DiameterIdentity",
    "DiameterURI",
    "Enumerated",
    "IPFilterRule",
    "QoSFilterRule",
};

#define INT_LEN sizeof(int32)
#define FLOAT_LEN sizeof(float);
#define BYTE_LEN 1
#endif
