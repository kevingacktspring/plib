//
// Created by gackt on 1/28/20.
//

#ifndef PLIB_APPENDLOGCOMMAND_H
#define PLIB_APPENDLOGCOMMAND_H

#include <stdint.h>
#include "DataProtocalSerializer.h"
#include "SegmentLog.h"

struct __attribute__((__packed__)) AppendLogReq {
    uint64_t term;
    uint16_t leaderId;
    uint64_t prevLogTerm;
    uint64_t prevLogIndex;
    uint64_t leaderCommitIndex;  // leader's commitIndex
    LogEntries *logEntries;
};

struct __attribute__((__packed__)) AppendLogResp {
    uint64_t term;
    bool success;
};

static void compressAppendLogReq(const AppendLogReq &appendLogReq, DataPacket &dataPacket) {
    /**
     * calculate size
     */
    uint64_t basic_size = sizeof(AppendLogReq::term) + sizeof(AppendLogReq::leaderId)
                          + sizeof(AppendLogReq::prevLogTerm) + sizeof(AppendLogReq::prevLogIndex)
                          + sizeof(AppendLogReq::leaderCommitIndex);

    uint64_t full_size = basic_size
                         + sizeof(appendLogReq.logEntries->length)  // length
                         + sizeof(appendLogReq.logEntries->mem_size)  // memory size
                         + appendLogReq.logEntries->mem_size;
    dataPacket.length = full_size;
    dataPacket.body = static_cast<char *>(malloc(full_size));
    /**
     * apply copy
     */
    uint64_t tmp_size = basic_size;
    memcpy(dataPacket.body, &appendLogReq, tmp_size);
    memcpy(dataPacket.body + tmp_size, &(appendLogReq.logEntries->length), sizeof(LogEntries::length));
    tmp_size += sizeof(LogEntries::length);
    memcpy(dataPacket.body + tmp_size, &(appendLogReq.logEntries->mem_size), sizeof(LogEntries::mem_size));
    tmp_size += sizeof(LogEntries::mem_size);
    /**
     * entry body
     */
    auto tmp_entry = appendLogReq.logEntries->start;
    while (tmp_entry != nullptr) {
        // key-size
        memcpy(dataPacket.body + tmp_size, &(tmp_entry->key_size), sizeof(LogEntry::key_size));
        tmp_size += sizeof(LogEntry::key_size);
        // key
        memcpy(dataPacket.body + tmp_size, tmp_entry->key, tmp_entry->key_size);
        tmp_size += tmp_entry->key_size;
        // value-size
        memcpy(dataPacket.body + tmp_size, &(tmp_entry->val_size), sizeof(LogEntry::val_size));
        tmp_size += sizeof(LogEntry::val_size);
        // value
        memcpy(dataPacket.body + tmp_size, tmp_entry->value, tmp_entry->val_size);
        tmp_size += tmp_entry->val_size;
        // next ptr
        tmp_entry = tmp_entry->next;
    }
}

/**
 * Memory structure
 * -AppendLogReq
 * -- term
 * -- leaderId
 * -- prevLogTerm
 * -- prevLogIndex
 * -- Logentries->length
 * -- Logentries->memsize
 * -- Logentry.1->key_size
 * -- Logentry.1->key
 * -- Logentry.1->val_size
 * -- Logentry.1->value
 * -- Logentry.2->key_size
 * ...
 *
 * @param dataPacket
 * @param appendLogReq
 */
static void decompressAppendLogReq(const DataPacket &dataPacket, AppendLogReq &appendLogReq) {
    /**
     * calculate size
     */
    uint64_t tmp_size = sizeof(AppendLogReq::term) + sizeof(AppendLogReq::leaderId)
                        + sizeof(AppendLogReq::prevLogTerm) + sizeof(AppendLogReq::prevLogIndex)
                        + sizeof(AppendLogReq::leaderCommitIndex);
    memcpy(&appendLogReq, dataPacket.body, tmp_size);
    /**
     * log-entry-s
     */
    LogEntries *logEntries = static_cast<LogEntries *>(malloc(sizeof(LogEntries)));
    appendLogReq.logEntries = logEntries;

    memcpy(logEntries, dataPacket.body + tmp_size, sizeof(LogEntries::length));
    tmp_size += sizeof(LogEntries::length);
    memcpy(logEntries + sizeof(LogEntries::length), dataPacket.body + tmp_size, sizeof(LogEntries::mem_size));
    tmp_size += sizeof(LogEntries::mem_size);

    if (logEntries->length <= 0) {
        return;
    }

    /**
     * deal first
     */
    uint16_t tmp_length = appendLogReq.logEntries->length;
    LogEntry *tmp_entry = static_cast<LogEntry *>(malloc(sizeof(LogEntry)));
    appendLogReq.logEntries->start = tmp_entry;
    do {
        // key-size
        memcpy(&(tmp_entry->key_size), dataPacket.body + tmp_size, sizeof(LogEntry::key_size));
        tmp_size += sizeof(LogEntry::key_size);
        // key
        tmp_entry->key = static_cast<char *>(malloc(tmp_entry->key_size));
        memcpy(tmp_entry->key, dataPacket.body + tmp_size, tmp_entry->key_size);
        tmp_size += tmp_entry->key_size;
        // value-size
        memcpy(&(tmp_entry->val_size), dataPacket.body + tmp_size, sizeof(LogEntry::val_size));
        tmp_size += sizeof(LogEntry::val_size);
        // value
        tmp_entry->value = static_cast<char *>(malloc(tmp_entry->val_size));
        memcpy(tmp_entry->value, dataPacket.body + tmp_size, tmp_entry->val_size);
        tmp_size += tmp_entry->val_size;
        //
        tmp_length -= 1;

        //check not last
        if (tmp_length >= 1) {
            LogEntry *previous = tmp_entry;
            tmp_entry = static_cast<LogEntry *>(malloc(sizeof(LogEntry)));
            previous->next = tmp_entry;
            tmp_entry->previous = previous;
        }
    } while (tmp_length >= 1);
}

inline static void compressAppendLogResp(const AppendLogResp &appendLogResp, DataPacket &dataPacket) {
    dataPacket.length = sizeof(AppendLogResp);
    dataPacket.body = static_cast<char *>(malloc(sizeof(AppendLogResp)));
    memcpy(dataPacket.body, &appendLogResp, sizeof(AppendLogResp));
}

inline static void decompressAppendLogResp(const DataPacket &dataPacket, AppendLogResp &appendLogResp) {
    memcpy(&appendLogResp, dataPacket.body, sizeof(AppendLogResp));
}

#endif //PLIB_APPENDLOGCOMMAND_H
