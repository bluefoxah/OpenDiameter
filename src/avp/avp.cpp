#include <arpa/inet.h>
#include <string.h>

#include "avp.h"
#include "log/log4z.h"
#include "includes/common.h"

using namespace bluefoxah;

CAVPHead::CAVPHead()
{
    m_iAvpCode  = -1;
    m_cAvpFlags = -1;
    m_iAvpHeadLen   = 0;
    m_iVenderId    = -1;
    m_iDataType = UNDEFINE_TYPE;
}

CAVPHead::CAVPHead(const int32 iCode, const char cFlag, const int32 iID)
{
    m_iAvpCode = iCode;
    m_cAvpFlags = cFlag;
    
    //根据是否有venderId来设置
    if (cFlag&(0x80))
    {
        m_iAvpHeadLen = 12;
    }
    else
    {
        m_iAvpHeadLen = 8;
    }
    m_iVenderId = iID;
}

CAVPHead::CAVPHead(const CAVPHead& oriAvpHead)
{
    m_iAvpCode = oriAvpHead.m_iAvpCode;
    m_cAvpFlags = oriAvpHead.m_cAvpFlags;
    m_iAvpHeadLen = oriAvpHead.m_iAvpHeadLen;
    m_iVenderId = oriAvpHead.m_iVenderId;
    m_iDataType = oriAvpHead.m_iDataType;   
}
#include <arpa/inet.h>

CAVPHead::~CAVPHead()
{
    
}

int32 CAVPHead::init(const char* pConfigFile)
{
    
    static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    static bool bInit = false;
    
    pthread_mutex_lock(&lock);
    if(bInit)
    {
        pthread_mutex_unlock(&lock);
        return ERROR_SUCCESS;
    }
    
    if(parseConfig(pConfigFile) != ERROR_SUCCESS)
    {
        LOGE("parase config error. [FileName:" << pConfigFile << "]");
        return ERROR_PARASE_CONFIG;
    }
    
}

int32 CAVPHead::parseConfig(const char* pConfigFile)
{
    FILE* pFile = fopen(pConfigFile, "r");
    if(NULL == pFile)
    {
        LOGE("open config file error. [FileName:" << pConfigFile << "]");
        return ERROR_OPEN_FILE;
    }
    
    int32 iCurLineNum = 0;
    char szBuf[512];
    string strLine, strValue, strType;
    size_t begin, end;
    SAvpKey sAvpKey;
    SAvpValue sAvpValue;
    int32 iType = -1;
    
    do
    {
        if(NULL == fgets(szBuf, 511, pFile))
        {
            break;
        }
        ++iCurLineNum;
        strLine = szBuf;
        StringTrim(strLine);
        
        if (strLine.empty())
        {
            continue;
        }
        
        if (*(strLine.begin()) == '#')
        {
            continue;
        }
        
        begin = strLine.find("<AVP>");
        if(begin == strLine.npos)
        {
            LOGE("config file error. [Line:" << iCurLineNum<<"]");
            return ERROR_CONFIG;
        }
        end = strLine.find("</AVP>");
        if(end == strLine.npos)
        {
            LOGE("config file error. [Line:" << iCurLineNum<<"]");
            return ERROR_CONFIG;
        }
        
        strValue = strLine.substr(begin + 5, end - begin - 5);
        
        //解析avp name
        begin = strLine.find("<name>");
        if(begin == strLine.npos)
        {
            LOGE("config file error[<name>]. [Line:" << iCurLineNum<<"]");
            return ERROR_CONFIG;
        }
        end = strLine.find("</name>");
        if(end == strLine.npos)
        {
            LOGE("config file error[</name>]. [Line:" << iCurLineNum<<"]");
            return ERROR_CONFIG;
        }
        sAvpValue.m_strAvpName = strLine.substr(begin + 6, end - begin - 6);
        
        //解析avp code
        begin = strLine.find("<code>");
        if(begin == strLine.npos)
        {
            LOGE("config file error[<code>]. [Line:" << iCurLineNum<<"]");
            return ERROR_CONFIG;
        }
        end = strLine.find("</code>");
        if(end == strLine.npos)
        {
            LOGE("config file error[</code>]. [Line:" << iCurLineNum<<"]");
            return ERROR_CONFIG;
        }
        sAvpKey.m_iCode = atol(strLine.substr(begin + 6 , end - begin -6).c_str());
        
        //解析vender id
        begin = strLine.find("<vender>");
        if(begin == strLine.npos)
        {
            sAvpKey.m_iVenderId = -1;
        }
        else
        {
            end = strLine.find("</vender>");
            if(end == strLine.npos)
            {
                LOGE("config file error[</vender>]. [Line:" << iCurLineNum<<"]");
                return ERROR_CONFIG;
            }
            sAvpKey.m_iVenderId = atol(strLine.substr(begin + 8, end - begin - 8).c_str());
        }
        
        //解析avp type
        begin = strLine.find("<type>");
        if(begin == strLine.npos)
        {
            LOGE("config file error[<type>]. [Line:" << iCurLineNum<<"]");
            return ERROR_CONFIG;
        }
        end = strLine.find("</type>");
        if(end == strLine.npos)
        {
            LOGE("config file error[</type>]. [Line:" << iCurLineNum<<"]");
            return ERROR_CONFIG;
        }
        strType = strLine.substr(begin + 6, end - begin - 6);
        iType = -1;
        
        if(strcmp(AVP_DATA_TYPE_STRING[OCTSTRING], strType.c_str()) == 0)
        {
            iType = OCTSTRING;
        }
        else if(strcmp(AVP_DATA_TYPE_STRING[INT32], strType.c_str()) == 0)
        {
            iType = INT32;
        }
        else if(strcmp(AVP_DATA_TYPE_STRING[INT64], strType.c_str()) == 0)
        {
            iType = INT64;
        }
        else if(strcmp(AVP_DATA_TYPE_STRING[UINT32], strType.c_str()) == 0)
        {
            iType = UINT32;
        }
        else if(strcmp(AVP_DATA_TYPE_STRING[UINT64], strType.c_str()) == 0)
        {
            iType = UINT64;
        }
        else if(strcmp(AVP_DATA_TYPE_STRING[FLOAT32], strType.c_str()) == 0)
        {
            iType = FLOAT32;
        }
        else if(strcmp(AVP_DATA_TYPE_STRING[FLOAT64], strType.c_str()) == 0)
        {
            iType = FLOAT64;
        }
        else if(strcmp(AVP_DATA_TYPE_STRING[GROUPED], strType.c_str()) == 0)
        {
            iType = GROUPED;
        }
        else if(strcmp(AVP_DATA_TYPE_STRING[ADDRESS], strType.c_str()) == 0)
        {
            iType = ADDRESS;
        }
        else if(strcmp(AVP_DATA_TYPE_STRING[TIME], strType.c_str()) == 0)
        {
            iType = TIME;
        }
        else if(strcmp(AVP_DATA_TYPE_STRING[UTF8STRING], strType.c_str()) == 0)
        {
            iType = UTF8STRING;
        }
        else if(strcmp(AVP_DATA_TYPE_STRING[DIAMETERID], strType.c_str()) == 0)
        {
            iType = DIAMETERID;
        }
        else if(strcmp(AVP_DATA_TYPE_STRING[DIAMETERURI], strType.c_str()) == 0)
        {
            iType = DIAMETERURI;
        }
        else if(strcmp(AVP_DATA_TYPE_STRING[ENUMERATED], strType.c_str()) == 0)
        {
            iType = ENUMERATED;
        }
        else if(strcmp(AVP_DATA_TYPE_STRING[IP_FILTER_RULE], strType.c_str()) == 0)
        {
            iType = IP_FILTER_RULE;
        }
        else if(strcmp(AVP_DATA_TYPE_STRING[QOS_FILTER_RULE], strType.c_str()) == 0)
        {
            iType = QOS_FILTER_RULE;
        }
        else
        {
            LOGW("undefine type. [Line:" << iCurLineNum<<"]");
            iType = UNDEFINE_TYPE;
        }
        sAvpValue.m_iAvpType = iType;
        
        m_hmAvp.insert(pair<SAvpKey, SAvpValue>(sAvpKey, sAvpValue));
        
    }while(1);
    
    fclose(pFile);
    
    return ERROR_SUCCESS;
}

CAVPHead&  CAVPHead::operator=(const CAVPHead& oriAvpHead)
{
    if(this == &oriAvpHead)
    {
        return *this;
    }
    
    m_iAvpCode = oriAvpHead.m_iAvpCode;
    m_cAvpFlags = oriAvpHead.m_cAvpFlags;
    m_iAvpHeadLen = oriAvpHead.m_iAvpHeadLen;
    m_iVenderId = oriAvpHead.m_iVenderId;
    m_iDataType = oriAvpHead.m_iDataType;
    
    return *this;
}

void CAVPHead::setAvpCode(const int32 iAvpCode)
{
    m_iAvpCode = iAvpCode;
}

void CAVPHead::setAvpFlags(const char cAvpCode)
{
    m_cAvpFlags = cAvpCode;
}

void CAVPHead::setVenderId(const int32 iVenderId)
{
    m_iVenderId = iVenderId;
}

void CAVPHead::setDataType(const int32 iDataType)
{
    m_iDataType = iDataType;
}

int32 CAVPHead::getAvpCode()
{
    return m_iAvpCode;
}

char CAVPHead::getAvpFlags()
{
    return m_cAvpFlags;
}

int32 CAVPHead::getAvpHeadLen()
{
    return m_iAvpHeadLen;
}

int32 CAVPHead::getVenderId()
{
    return m_iVenderId;
}

int32 CAVPHead::getDataType()
{
    return m_iDataType;
}

int32 CAVPHead::getAvpCode() const
{
    return m_iAvpCode;
}

char CAVPHead::getAvpFlags() const
{
    return m_cAvpFlags;
}

int32 CAVPHead::getAvpHeadLen() const
{
    return m_iAvpHeadLen;
}

int32 CAVPHead::getVenderId() const
{
    return m_iVenderId;
}

int32 CAVPHead::getDataType() const
{
    return m_iDataType;
}

bool CAVPHead::isGroup()
{
    return m_iDataType == GROUPED;
}

int32 CAVPHead::decode(const char* pData, const int32 iLen)
{
    if(NULL == pData || iLen < 8)
    {
        LOGE("illegal parameter");
        return ERROR_ILLEGAL_PARA;
    }
    
    const char* pTmp = pData;
    //解析avp code
    m_iAvpCode = ntohl(*(int32*)pTmp);
    pTmp += 4;
    
    //解析 avp flags
    m_cAvpFlags = *pTmp;
    //pTmp += 1;
    
    //解析AVP 长度
    m_iAvpLen = ntohl(*(int32*)pTmp)&(0xFFFFFF);
    
    //解析vender id
    if(m_cAvpFlags & 0x80)
    {
        m_iAvpHeadLen = 12;
        pTmp += 4;
        m_iVenderId = ntohl(*(int32*)pTmp);
        if(m_iAvpLen < 12)
        {
            LOGE("illegal avp len");
            return ERROR_ILLEGAL_DATA;
        }
    }
    else
    {
        m_iAvpHeadLen = 8;
        m_iVenderId = -1;
    }
    
    m_iDataType = queryAvpType(m_iAvpCode, m_iVenderId);
    
    return ERROR_SUCCESS;
}

int32 CAVPHead::encode(char* pData, const int32 iLen, const int32 iDataLen)
{
    if(pData == NULL || iLen < 8)
    {
        LOGE("illegal parameter");
        return ERROR_ILLEGAL_PARA;
    }
    
    char* pTmp = pData;
    *(int32*)pTmp = htonl(m_iAvpCode);
    pTmp += 4;
    
    int32 iTotalLen = iDataLen + 8;
    if(m_cAvpFlags & 0x80)
    {
        iTotalLen += 4;
        
        *(int32*) pTmp = htonl(iTotalLen) || (m_cAvpFlags << 24);
        pTmp += 4;
        *(int32*) pTmp = htonl(m_iVenderId);
    }
    else
    {
        *(int32*) pTmp = htonl(iTotalLen) || (m_cAvpFlags << 24);
    }
}

void CAVPHead::dump(string& strHead)
{
    char szBuf[1024];
    int32 iVFlag = m_cAvpFlags & 0x80;
    int32 iMFlag = m_cAvpFlags & 0x40;
    int32 iPFlag = m_cAvpFlags & 0x20;
    
    string strAvpName = queryAvpName(m_iAvpCode, m_iVenderId);
    
    if(iVFlag)
    {
        snprintf(szBuf, 1024, "AVP_NAME:[%s] AVP_CODE[%d] VMP_FLAG[%d%d%d] VENDER_ID[%d] DATA_TYPE[%s]", strAvpName.c_str(), m_iAvpCode, iVFlag, 
iMFlag, iPFlag, m_iVenderId, AVP_DATA_TYPE_STRING[m_iDataType]);
    }
    else
    {
        snprintf(szBuf, 1024, "AVP_NAME:[%s] AVP_CODE[%d] VMP_FLAG[%d%d%d] DATA_TYPE[%s]", strAvpName.c_str(), m_iAvpCode, iVFlag, iMFlag, iPFlag, 
AVP_DATA_TYPE_STRING[m_iDataType]);
    }
    
    strHead = szBuf;
}

int32 CAVPHead::queryAvpType(const int32 iCode, const int32 iVenderId)
{
    hash_map<SAvpKey, SAvpValue, hash_avp, equal_avp>::iterator it;
    if((it = m_hmAvp.find(SAvpKey(iCode,iVenderId))) == m_hmAvp.end())
    {
        return UNDEFINE_TYPE;
    }
    else
    {
        return it->second.m_iAvpType;
    }
}

string CAVPHead::queryAvpName(const int32 iCode, const int32 iVenderId)
{
    hash_map<SAvpKey, SAvpValue, hash_avp, equal_avp>::iterator it;
    if((it = m_hmAvp.find(SAvpKey(iCode,iVenderId))) == m_hmAvp.end())
    {
        return string(AVP_DATA_TYPE_STRING[UNDEFINE_TYPE]);
    }
    else
    {
        return it->second.m_strAvpName;
    }
}
CAVPData::CAVPData()
{
    m_pData = NULL;
    m_iLen = 0;
}

CAVPData::CAVPData(const char* pData, const int32 iLen)
{
   m_pData = pData;
   m_iLen = iLen;
}

CAVPData::CAVPData(const CAVPData& oriAvpData)
{
    m_pData = oriAvpData.m_pData;
    m_iLen = oriAvpData.m_iLen;
}

CAVPData& CAVPData::operator=(const CAVPData& oriAvpData)
{
    if(this == &oriAvpData)
    {
        return *this;
    }
    m_pData = oriAvpData.m_pData;
    m_iLen = oriAvpData.m_iLen;
    
    return *this;
}

CAVPData::~CAVPData()
{
    
}

int32 CAVPData::getLen()
{
    return m_iLen;
}

int32 CAVPData::getLen() const
{
    return m_iLen;
}

const char* CAVPData::getData()
{
    return m_pData;
}

const char* CAVPData::getData() const
{
    return m_pData;
}

int32 CAVPData::decode()
{

}

int32 CAVPData::decode(const char* pData, const int32 iLen)
{

}

int32 CAVPData::encode()
{

}

int32 CAVPData::encode(char* pData, const int32 iLen)
{

}







