#include "SignatureMaker.h"

using namespace std;

SignatureMaker::SignatureMaker(
    const string& input,
    const string& output,
    size_t blockSize,
    size_t threadsCount
) :
    m_controller(input, output, blockSize, HashMaker::BlockSize(), threadsCount),
    m_threadsCount(threadsCount)
{
    m_makers.reserve(m_threadsCount);
    m_makerThreads.reserve(m_threadsCount);
}

void SignatureMaker::Run()
{
    CreateThreads();
    JoinThreads();
}

void SignatureMaker::CreateThreads()
{
    for (size_t i = 0; i < m_threadsCount; ++i)
    {
        m_makers.emplace_back(m_controller);
    }

    for (auto& maker : m_makers)
    {
        m_makerThreads.emplace_back(&HashMaker::Run, &maker);
    }
}

void SignatureMaker::JoinThreads()
{
    for (auto& makerThread : m_makerThreads)
    {
        if (makerThread.joinable())
        {
            makerThread.join();
        }
    }
}
