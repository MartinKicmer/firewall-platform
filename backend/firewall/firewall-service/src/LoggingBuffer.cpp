//
// Created by andrew on 4/22/26.
//

#include "../headers/LoggingBuffer.h"

void LoggingBuffer::write(const CombinedLogRecord& record) {
    if (free_slots.try_acquire()) {
        this->buffer[this->writeIndex] = record;
        this->writeIndex = (this->writeIndex + 1) % BUFFER_SIZE;
        full_slots.release();
    }
}

CombinedLogRecord LoggingBuffer::read() {
    full_slots.acquire();
    CombinedLogRecord record = this->buffer[this->readIndex];
    this->readIndex = (this->readIndex + 1) % BUFFER_SIZE;
    free_slots.release();
    return record;
}
