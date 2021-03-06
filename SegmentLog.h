//
// Created by gackt on 2/1/20.
//

#ifndef PLIB_SEGMENTLOG_H
#define PLIB_SEGMENTLOG_H

#include <stdint.h>
#include <unordered_map>
#include <vector>
#include <cstring>
#include <atomic>
#include "DataProtocalSerializer.h"

/**
 * Structure
 */
struct __attribute__ ((__packed__)) LogEntry {
    uint16_t    key_size;
    char *      key;
    uint16_t    val_size;
    char *      value;
    LogEntry *  next;
    LogEntry *  previous;
};

struct __attribute__ ((__packed__)) LogEntries {
    uint16_t    length;  // length of log-entry array
    uint16_t    mem_size;  // size of LogEntries memory cost
    LogEntry *  start;
};

struct __attribute__ ((__packed__)) Meta {
    uint64_t term;
};

struct __attribute__ ((__packed__)) Entry {
    Meta        meta;
    LogEntry *  content;
};

static void compressLogEntries(const LogEntries &logEntries, DataPacket &dataProtocal) {
    dataProtocal.length = sizeof(logEntries.length) + sizeof(logEntries.mem_size) + logEntries.mem_size;
    dataProtocal.body = static_cast<char *>(malloc(dataProtocal.length));
    memcpy(&dataProtocal.body, &logEntries.length, sizeof(logEntries.length));
    memcpy(&dataProtocal.body + sizeof(logEntries.length), &logEntries.mem_size, sizeof(logEntries.mem_size));
    memcpy(&dataProtocal.body + sizeof(logEntries.length) + sizeof(logEntries.mem_size), &logEntries.start,
           logEntries.mem_size);
}

static void decompressLogEntries(const DataPacket &dataProtocal, LogEntries &logEntries) {
    memcpy(&logEntries.length, &dataProtocal.body, sizeof(logEntries.length));
    memcpy(&logEntries.mem_size, &dataProtocal.body + sizeof(logEntries.length), sizeof(logEntries.mem_size));
    memcpy(&logEntries.start, &dataProtocal.body + sizeof(logEntries.length) + sizeof(logEntries.mem_size),
           logEntries.mem_size);
}

class SegmentLog {
public:
    SegmentLog();

    virtual ~SegmentLog() = default;

    std::unordered_map<std::string, Entry> log_map;
    std::vector<Entry> log_vector;

    std::atomic<uint64_t> first_index{0};
    std::atomic<uint64_t> last_index{0};
};


#endif //PLIB_SEGMENTLOG_H
