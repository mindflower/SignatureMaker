#include "HashMaker.h"
#include <xxhash.hpp>
#include <boost\exception\diagnostic_information.hpp>

using namespace std;

size_t HashMaker::BlockSize()
{
    return sizeof(xxh::hash_t<64>);
}

HashMaker::HashMaker(ProgressController& controller) :
    m_controller(controller)
{
}

void HashMaker::Run() noexcept
{
    auto& controller = m_controller.get();
    try
    {
        while (true)
        {
            const auto oCurrentBlockId = controller.GetNextBlockId();
            if (oCurrentBlockId.has_value())
            {
                m_currentBlockId = oCurrentBlockId.value();
                auto [pInput, inputSize] = controller.GetInputBlockById(m_currentBlockId);
                auto pOutput = controller.GetOutputBlockById(m_currentBlockId);
                const xxh::hash_t<64> hash = xxh::xxhash<64>(pInput, inputSize);
                memcpy(pOutput, &hash, sizeof(xxh::hash_t<64>));
            }
            else
            {
                return;
            }
        }
    }
    catch (...)
    {
        controller.ConsoleLog(
            "Block " + to_string(m_currentBlockId) + " processing error: " +
            boost::current_exception_diagnostic_information()
        );
    }
}
