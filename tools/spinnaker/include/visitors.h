//------------------------------------------------------------------------------
//  Created     : 06.08.21
//  Author      : Victor Kats
//  Description : Import Spinnnaker-based capturer into project (see TMKDCL-380)
//------------------------------------------------------------------------------

#ifndef VISITORS_H
#define VISITORS_H
#include <string>
#include <fstream>
#include <stack>
#include <typeinfo>
#include "configparser.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "global_sc.h"
#include "log.h"

class CameraConfig;
class Property;

class Capturer_EXPORT JsonLoader
{
private:
    void push(std::string name);
    void push(boost::property_tree::ptree* tree);
    void pop();
    boost::property_tree::ptree json;
    std::stack<boost::property_tree::ptree*> stack;
public:
    JsonLoader(boost::property_tree::ptree json);
    JsonLoader(std::string filename);

    template<class T>
    void visit(T& value, std::string name)
    {
        push(name);
        value.accept(this);
        pop();
    }

    template<typename T>
    void visit(T& value, std::string name, T _default)
    {
        value = (*stack.top()).get(name, _default);
    }

    template<class T>
    void visit(std::vector<T>& value, std::string name, void * _array)
    {
        try
        {
            for (auto& item : stack.top()->get_child(name))
                value.push_back(item.second.get_value<T>());
        }
        catch(std::exception &e)
        {
            Log(Log::Warning) << e.what() << ". Generic array is not loaded";
            value.push_back(0);
        }
    }

    template<class T>
    void visit(std::vector<T>& value, std::string name)
    {
        push(name);
        {
            for (boost::property_tree::ptree::value_type &it : *stack.top())
            {
                if (it.first[0] == '!')
                    continue;
                T prop;
                push(&it.second);
                prop.accept(this);
                pop();
                value.push_back(prop);
            }
        }
        pop();
    }
};

template<>
void JsonLoader::visit<Property>(std::vector<Property>& value, std::string name);

template<>
void JsonLoader::visit<CameraConfig>(std::vector<CameraConfig>& value, std::string name);



#endif // VISITORS_H
