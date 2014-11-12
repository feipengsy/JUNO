#ifndef BASEIOSTREAM_H
#define BASEIOSTREAM_H

#include <string>

class BaseIOStream {

public:
    BaseIOStream(const std::string& name) : m_name(name) {}
    virtual ~BaseIOStream() {}

    const std::string& objName() { return m_name; }

private:
    std::string m_name;

};

#endif
