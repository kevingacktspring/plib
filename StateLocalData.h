//
// Created by gackt on 2/1/20.
//

#ifndef PLIB_STATELOCALDATA_H
#define PLIB_STATELOCALDATA_H

#include <stdint.h>
#include "SegmentLog.h"

enum class Role {
    unknow      = 0,
    candidate   = 1,
    leader      = 2
};

/**
 * Persist state in all servers
 */
struct __attribute__ ((__packed__)) PersistState {
    uint64_t currentTerm;
    uint64_t voteFor;
    SegmentLog *logs;

    uint64_t lastAppliedIndex;

    Role  nodeRole = Role::candidate;
    uint64_t electionTimeNano = 0;

    std::atomic<uint64_t> leaderId{0};
    std::atomic<uint64_t> commitIndex{0};
    std::atomic<uint64_t> lastApplied{0};

    PersistState(SegmentLog *logs) : logs(logs) {}
};

/**
 * Volatile state on all servers
 */
struct __attribute__ ((__packed__)) VolatileState {
    const uint16_t nodeid;
    const char * servInetAddr;
    const uint16_t servPort;

    bool responedVote = false;
    bool grantedVote = false;

    uint64_t matchIndex = 0;
    uint64_t nextIndex = 0;

    uint64_t lastRequestTimeNano;
    uint64_t nextRequestTimeNano;

    /**
     * Leader use this record to ensure leadership
     */
    uint64_t lastResponseTimeNano;
};

#endif //PLIB_STATELOCALDATA_H
