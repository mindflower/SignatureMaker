#pragma once
#include "FileBlockMapper.h"
#include <optional>
#include <mutex>
#include <condition_variable>

class ProgressController
{
public:
    ProgressController(
        const std::string& inputFilename,
        const std::string& outputFilename,
        size_t inputBlockSize,
        size_t outputBlockSize,
        size_t threadCount
    );
    std::optional<size_t> GetNextBlockId();
    std::pair<uint8_t*, size_t> GetInputBlockById(size_t id) const;
    uint8_t* GetOutputBlockById(size_t id) const;
    void WaitUntilNextBlockReady();
    void ConsoleLog(const std::string& message);

private:
    struct BlockIdLimits
    {
        size_t lower = 0;
        size_t upper = 0;
    };

private:
    FileBlockMapper m_inputMapper;
    FileBlockMapper m_outputMapper;
    size_t m_inputBlocksCount = 0;
    size_t m_threadCount = 0;
    std::atomic_size_t m_currentBlockId = 0;
    std::atomic_size_t m_waitingThreadCount = 0;
    BlockIdLimits m_blockIdLimits;
    std::mutex m_waitMutex;
    std::condition_variable m_waitCv;
    std::mutex m_consoleMutex;
};
