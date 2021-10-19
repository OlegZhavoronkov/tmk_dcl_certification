//------------------------------------------------------------------------------
//  Created     : 06.08.21
//  Author      : Victor Kats
//  Description : Import Spinnnaker-based capturer into project (see TMKDCL-380)
//------------------------------------------------------------------------------

#include "visitors.h"${REQUIRED_LIBS_QUALIFIED}
#include "log.h"

void JsonLoader::push(std::string name)
{
    boost::property_tree::ptree* a;
    if (stack.empty())
    {
        try
        {
            a = &json.get_child(name);
        }
        catch(std::exception &e)
        {
            Log(Log::Warning) << e.what() << ". Use default params";
            boost::property_tree::ptree node;
            json.add_child(name, node);
            a = &json.get_child(name);
        }
    }
    else
    {
        try
        {
            a = &(*stack.top()).get_child(name);
        }
        catch(std::exception &e)
        {
            Log(Log::Warning) << e.what() << ". Use default params";
            boost::property_tree::ptree node;
            (*stack.top()).add_child(name, node);
            a = &(*stack.top()).get_child(name);
        }
    }
    push(a);
}

void JsonLoader::push(boost::property_tree::ptree *tree)
{
    stack.push(tree);
}

void JsonLoader::pop()
{
    stack.pop();
}

JsonLoader::JsonLoader(boost::property_tree::ptree json)
{
    this->json = json;
}

JsonLoader::JsonLoader(std::string filename)
{

    read_json(filename, json);
}


template<>
void JsonLoader::visit<Property>(std::vector<Property>& value, std::string name)
{
    push(name);
    {
        for (boost::property_tree::ptree::value_type &it : *stack.top())
        {
            if (it.first[0] == '!')
                continue;
            Property prop;
            prop.name = it.first;
            push(&it.second);
            prop.accept(this);
            pop();
            value.push_back(prop);
        }
    }
    pop();
}

template<>
void JsonLoader::visit<CameraConfig>(std::vector<CameraConfig>& value, std::string name)
{
    push(name);
    {
        for (boost::property_tree::ptree::value_type &it : *stack.top())
        {
            if (it.first[0] == '!')
                continue;
            CameraConfig prop;
            prop.name = it.first;
            push(&it.second);
            prop.accept(this);
            pop();
            value.push_back(prop);
        }
    }
    pop();
}
