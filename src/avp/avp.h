
#ifndef AVP_H
#define AVP_H

#undef  __DEPRECATED

#ifdef __GNUC__
#include <ext/hash_map>
#else
#include <hash_map>
#endif

#include "includes/basic_config.h"


namespace std
{
using namespace __gnu_cxx;
}

using namespace std;

namespace bluefoxah {
    //AVP键值
    struct SAvpKey
    {
    public:
        int32 m_iCode;//AVP code
        int32 m_iVenderId;//制造商ID
        SAvpKey() :m_iCode(-1),m_iVenderId(-1)
        {
        }
        SAvpKey(const int32 iCode,const int32 iVenderId = -1)
        {
            m_iCode = iCode;
            m_iVenderId = iVenderId;
        }
    };

    struct hash_avp
    {
        size_t operator()(const SAvpKey& a)const
        {
            return size_t(a.m_iCode) + size_t(a.m_iVenderId);
        }
    };

    struct equal_avp
    {
        bool operator()(const SAvpKey& a, const SAvpKey& b)const
        {
            return (a.m_iCode == b.m_iCode)&&(a.m_iVenderId == b.m_iVenderId);
        }
    };

    //AVP信息
    struct SAvpValue
    {
    public:
        string m_strAvpName;// AVP name
        int32 m_iAvpType;//AVP数据类型
        char m_bFlag;//AVP VMP flag
    };



    /*          0               1               2                3
    *    0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7
    *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    *   |                           AVP Code                            |
    *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    *   |V M P r r r r r|                  AVP Length                   |
    *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    *   |                        Vendor-ID (opt)                        |
    *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    *   |    Data ...
    *   +-+-+-+-+-+-+-+-+
    */
    class CAVPHead {
    private:
        int32 m_iAvpCode;//avp Code
        int32 m_iAvpHeadLen;
        int32 m_iVenderId;
        int32 m_iDataType;
        int32 m_iAvpLen;
        hash_map<SAvpKey, SAvpValue, hash_avp, equal_avp> m_hmAvp;
        char m_cAvpFlags;//avp 标志位
    public:
        CAVPHead();
        CAVPHead(const int32 iCode, const char cFlag = '\0', const int32 iID = -1);
        CAVPHead(const CAVPHead& oriAvpHead);
        ~CAVPHead();
        CAVPHead& operator=(const CAVPHead& oriAvpHead);
        int32 init(const char* pConfigFile);
        int32 parseConfig(const char* pConfigFile);
        
        void setAvpCode(const int32 iAvpCode);
        void setAvpFlags(const char cAvpCode);
        void setVenderId(const int32 iVenderId);
        void setDataType(const int32 iDataType);
        void setAvpLen(const int32 iAvpLen);
        
        int32 getAvpCode();
        int32 getAvpCode() const;
        char getAvpFlags();
        char getAvpFlags() const;
        int32 getAvpHeadLen();
        int32 getAvpHeadLen() const;
        int32 getAvpLen();
        int32 getAvpLen() const;
        int32 getVenderId();
        int32 getVenderId() const;
        int32 getDataType();
        int32 getDataType() const;
        
        bool isGroup();
        
        int32 decode();
        int32 decode(const char* pData, const int32 iLen);
        int32 encode();
        int32 encode(char* pData, const int32 iLen, const int32 iDataLen);
        void dump(string& strHead);
        
        int32 queryAvpType(const int32 iCode, const int32 iVenderId);
        string queryAvpName(const int32 iCode, const int32 iVenderId);
    };

    class CAVPData {
    private:
        const char* m_pData;
        int32 m_iLen;
    public:
        CAVPData();
        CAVPData(const char* pData, const int32 iLen);
        CAVPData(const CAVPData& oriAvpData);
        CAVPData& operator=(const CAVPData& oriAvpData);
        ~CAVPData();
        
        
        int32 getLen();
        int32 getLen() const;
        const char* getData();
        const char* getData() const;
        
        int32 decode();
        int32 decode(const char* pData, const int32 iLen);
        int32 encode();
        int32 encode(char* pData, const int32 iLen);
        void dump(string& strData);
    };

    class CAvp {
    private:
        CAVPHead m_AvpHead;
        CAVPData m_AvpData;
    public:
        CAvp();
        CAvp(const char* pBuf, const int32 iLen);
        CAvp(const CAvp& cAvp);
        CAvp& operator=(const CAvp& cAvp);
        
        
        int32 decode();
        int32 decode(const char* pData, const int32 iLen);
        int32 encode();
        int32 encode(char* pData, const int32 iLen);
    };
}

#endif
