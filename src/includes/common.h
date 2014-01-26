#ifndef COMMON_H
#define COMMON_H

enum AVP_ERROR
{
    ERROR_SUCCESS = 0,
    ERROR_NULL_POINTER,
    ERROR_ILLEGAL_PARA,
    ERROR_ILLEGAL_DATA,
    ERROR_OPEN_FILE,
    ERROR_CONFIG,
    ERROR_UNDEFINE_TYPE,
    ERROR_PARASE_CONFIG,
};


static void StringTrim(std::string &str, const char ignore = '\0')
{
    if (str.empty())
    {
        return;
    }
    size_t endPos = str.length();
    int posBegin = (int)endPos;
    int posEnd = -1;

    for (size_t i = 0; i<str.length(); i++)
    {
        char ch = str[i];
        if (ch != '\r' && ch != '\n' && ch != ' ' && ch != '\t' && ch != ignore)
        {
            posBegin = (int)i;
            break;
        }
    }
    for (size_t i = endPos; i> 0; i--)
    {
        char ch = str[i-1];
        if (ch != '\r' && ch != '\n' && ch != ' ' && ch != '\t' && ch != ignore)
        {
            posEnd = (int)i-1;
            break;
        }
    }
    if (posBegin <= posEnd)
    {
        str = str.substr(posBegin, posEnd-posBegin+1);
    }
    else
    {
        str.clear();
    }
}
#endif
