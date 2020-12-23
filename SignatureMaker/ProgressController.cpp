#include "ProgressController.h"
#include <iostream>

using namespace std;

ProgressController::ProgressController(
    const string& inputFilename,
    const string& outputFilename,
    const size_t inputBlockSize,
    const size_t outputBlockSize,
    const size_t threadCount
) :
    m_inputMapper(inputFilename, inputBlockSize),
    m_outputMapper(outputFilename, outputBlockSize),
    m_threadCount(threadCount)
{
    m_blockIdLimits.upper = m_inputMapper.MapForRead();
    m_inputBlocksCount = m_inputMapper.GetBlocksCount();
    m_outputMapper.MapForReadWrite(m_inputBlocksCount * outputBlockSize);
}

optional<size_t> ProgressController::GetNextBlockId()
{
    const auto currentBlockId = m_currentBlockId++;
    if (currentBlockId < m_inputBlocksCount)
    {
        if (currentBlockId >= m_blockIdLimits.upper)
        {
            WaitUntilNextBlockReady();
        }
        return currentBlockId;
    }
    else
    {
        return nullopt;
    }
}

pair<uint8_t*, size_t> ProgressController::GetInputBlockById(const size_t id) const
{
    const auto blockSize = m_inputMapper.GetBlockSize();
    const auto pageSize = m_inputMapper.GetPageSize();
    const auto blockOffset = blockSize * id;
    const auto pageOffset = blockOffset % pageSize;
    const auto ptrOffset = (blockOffset / pageSize) + pageOffset;
    if (ptrOffset > m_inputMapper.GetMapSize())
    {
        throw runtime_error("Input file offset out of bounds");
    }
    else
    {
        return {m_inputMapper.GetMapPtr() + ptrOffset, blockSize};
    }
}

uint8_t* ProgressController::GetOutputBlockById(const size_t id) const
{
    const auto blockSize = m_outputMapper.GetBlockSize();
    const auto offset = id * blockSize;
    if (offset > m_outputMapper.GetMapSize())
    {
        throw runtime_error("Output file offset out of bounds");
    }
    else
    {
        return m_outputMapper.GetMapPtr() + offset;
    }
}

void ProgressController::WaitUntilNextBlockReady()
{
    if (++m_waitingThreadCount == m_threadCount)
    {
        m_blockIdLimits.lower = m_blockIdLimits.upper;
        m_blockIdLimits.upper += m_inputMapper.Remap(m_blockIdLimits.upper);
        m_waitingThreadCount = 0;
        m_waitCv.notify_all();
        return;
    }
    unique_lock<mutex> lock(m_waitMutex);
    m_waitCv.wait(lock);
}

void ProgressController::ConsoleLog(const string& message)
{
    lock_guard<mutex> guard(m_consoleMutex);
    cout << "(Thread: " << this_thread::get_id() << ") " << message << endl;
}
