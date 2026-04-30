#pragma once
#include <stdint.h>
#include <string>

class SciterValue
{
    enum VALUE_TYPE
    {
        TYPE_UNDEFINED = 0,
        TYPE_BOOL = 1,
        TYPE_INT32_T = 2,
        TYPE_STRING = 3,
    };

public:
    SciterValue();
    SciterValue(bool Value);
    SciterValue(int32_t Value);
    SciterValue(std::string Value);
    ~SciterValue();

    bool isBool(void) const;
    bool isInt(void) const;
    bool isString(void) const;
    bool GetValueBool(void) const;
    int32_t GetValueInt(void) const;
    const std::string & GetValueStr(void) const;

private:
    VALUE_TYPE m_type;
    uint32_t m_valueInt;
    std::string m_valueStr;
};
