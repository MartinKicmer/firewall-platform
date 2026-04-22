//
// Created by andrew on 4/22/26.
//

#pragma once
#include "DTOS.h"
#include <semaphore>
class LoggingBuffer {
public:
    LoggingBuffer() = default;
    void write(const CombinedLogRecord& record);
    CombinedLogRecord read();
private:
    static constexpr size_t BUFFER_SIZE = 1024;
    CombinedLogRecord buffer[BUFFER_SIZE];
    int writeIndex = 0;
    int readIndex = 0;
    std::counting_semaphore<BUFFER_SIZE> free_slots{BUFFER_SIZE};
    std::counting_semaphore<BUFFER_SIZE> full_slots{0};
};

