#include "CommandLineParameters.h"
#include "SignatureMaker.h"
#include <boost/exception/diagnostic_information.hpp>
#include <iostream>
#include <chrono>

int main(int argc, const char *argv[])
{
    try
    {
        CommandLineParameters parameters{argc, argv};

        std::cout << "Start signature making..." << std::endl;
        const auto start = std::chrono::high_resolution_clock::now();

        SignatureMaker maker{
            parameters.Get<std::string>(CommandLineParameters::INPUT),
            parameters.Get<std::string>(CommandLineParameters::OUTPUT),
            parameters.Get<size_t>(CommandLineParameters::BLOCK_SIZE),
            parameters.Get<size_t>(CommandLineParameters::THREAD_COUNT)
        };
        maker.Run();

        const auto end = std::chrono::high_resolution_clock::now();
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() % 1000;
        const auto sec = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
        std::cout << "Time passed: " << sec << ':' << ms << " sec" << std::endl;
    }
    catch (...)
    {
        std::cout << "Exception occured: " << boost::current_exception_diagnostic_information();
    }
}
