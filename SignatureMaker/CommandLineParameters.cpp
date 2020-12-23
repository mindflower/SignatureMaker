#include "CommandLineParameters.h"
#include <boost/program_options.hpp>

using namespace boost::program_options;
using namespace std;

const string CommandLineParameters::INPUT = "input"s;
const string CommandLineParameters::OUTPUT = "output"s;
const string CommandLineParameters::BLOCK_SIZE = "block"s;
const string CommandLineParameters::THREAD_COUNT = "threads"s;

CommandLineParameters::CommandLineParameters(int argc, const char* argv[])
{
    options_description desc{"Options"};
    desc.add_options()
        (INPUT.c_str(), value<string>(), "Input file")
        (OUTPUT.c_str(), value<string>()->default_value("out.bin"), "Output file")
        (BLOCK_SIZE.c_str(), value<size_t>()->default_value(1048576), "Block size")
        (THREAD_COUNT.c_str(), value<size_t>()->default_value(8), "Threads count")
        ;
    store(parse_command_line(argc, argv, desc), m_varialbesMap);
    notify(m_varialbesMap);
}
