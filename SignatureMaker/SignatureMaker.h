#pragma once
#include "ProgressController.h"
#include "HashMaker.h"
#include <vector>
#include <thread>

class SignatureMaker
{
public:
    SignatureMaker(
        const std::string& input,
        const std::string& output,
        size_t blockSize,
        size_t threadsCount
    );
    void Run();

private:
    void CreateThreads();
    void JoinThreads();

private:
    ProgressController m_controller;
    size_t m_threadsCount = 0;
    std::vector<HashMaker> m_makers;
    std::vector<std::thread> m_makerThreads;
};
