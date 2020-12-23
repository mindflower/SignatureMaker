#pragma once
#include "ProgressController.h"

class HashMaker
{
public:
    static size_t BlockSize();

public:
    HashMaker(ProgressController& controller);
    void Run() noexcept;

private:
    std::reference_wrapper<ProgressController> m_controller;
    size_t m_currentBlockId = 0;
};
