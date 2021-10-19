//------------------------------------------------------------------------------
//  Created     : 06.08.21
//  Author      : Victor Kats
//  Description : Import Spinnnaker-based capturer into project (see TMKDCL-380)
//------------------------------------------------------------------------------

#pragma once
#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>
#include <vector>
#include <sstream>
#include <functional>
#include "visitors.h"
#include "global_sc.h"

#ifdef WIN32
#include <stdint.h>
#include <stdio.h>

#define u_int64_t uint64_t

#endif
class Visitor;

template<class T>
T fromString(std::string s)
{
    T res;
    std::istringstream is(s);
    is >> res;
    return res;
}

template<class T>
struct PropertyBase
{
    T value, minValue, maxValue, increment;
};

class Capturer_EXPORT  Property : public PropertyBase<std::string>
{
public:
    std::string name;

    std::string type;

    bool edit;

    template<class T>
    void setSdkRestrictions(T min, T max, T inc, bool hasInc)
    {
        if (minValue == "sdk") minValue = std::to_string(min);
        if (maxValue == "sdk") maxValue = std::to_string(max);
        if (increment == "sdk")
        {
            if (!hasInc) inc = (max - min) / 10;
            increment = std::to_string(inc);
        }
    }

    template<class T>
    PropertyBase<T> cast()
    {
        return PropertyBase<T>{fromString<T>(value), fromString<T>(minValue), fromString<T>(maxValue), fromString<T>(increment)};
    }

    template<class T>
    void accept(T *visitor)
    {
        visitor->visit(type, "type", std::string("none"));
        visitor->visit(value, "value", std::string("none"));
        visitor->visit(edit, "edit", false);
        visitor->visit(minValue, "minValue", std::string("sdk"));
        visitor->visit(maxValue, "maxValue", std::string("sdk"));
        visitor->visit(increment, "increment", std::string("sdk"));
    }
};

class Capturer_EXPORT CameraConfig
{
public:
    std::string name;

    std::string sdk;
    u_int64_t serial;

    std::vector<Property> properties;

    template<class T>
    void accept(T *visitor)
    {
        visitor->visit(serial, "serial", (u_int64_t)0);
        visitor->visit(sdk, "SDK", std::string("none"));
        visitor->visit(properties, "Properties");
    }
};

class Capturer_EXPORT TriggerConfig
{
public:
    double expectedFrequency;

    bool useSoftWareTrigger;
    u_int64_t serial;
    u_int64_t serial1;
    u_int64_t serial2;
    std::string sdk;

    bool useHardWareTrigger;
    std::string deviceName;


    template<class T>
    void accept(T *visitor)
    {
        visitor->visit(expectedFrequency, "expectedFrequency", 1.0);
        visitor->visit(useSoftWareTrigger, "useSoftWareTrigger", true);
        visitor->visit(serial, "serial", (u_int64_t)0);
#if 0
        visitor->visit(serial1, "serial1", (u_int64_t)0);
        visitor->visit(serial2, "serial2", (u_int64_t)0);
#endif
        visitor->visit(sdk, "sdk", std::string("none"));
        visitor->visit(useHardWareTrigger, "useHardWareTrigger", false);
        visitor->visit(deviceName, "deviceName", std::string("none"));
    }
};

class Capturer_EXPORT ConfigParser
{
public:
    static std::vector<CameraConfig> getCamerasList(std::string filename);
    static std::vector<TriggerConfig> getTriggerList(std::string filename);

};

#endif // CONFIGPARSER_H
