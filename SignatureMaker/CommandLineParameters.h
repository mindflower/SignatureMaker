#pragma once
#include <boost/program_options/variables_map.hpp>

class CommandLineParameters
{
public:
    static const std::string INPUT;
    static const std::string OUTPUT;
    static const std::string BLOCK_SIZE;
    static const std::string THREAD_COUNT;

public:
    CommandLineParameters(int argc, const char* argv[]);

    template<class T>
    T Get(const std::string& parameter) const
    {
        if (m_varialbesMap.count(parameter))
        {
            return m_varialbesMap[parameter].as<T>();
        }
        else
        {
            throw std::runtime_error("Parameter missing: " + parameter);
        }
    }

private:
    boost::program_options::variables_map m_varialbesMap;
};
