//
// Created by gackt on 2/1/20.
//

#ifndef PLIB_STATELOCALDATA_H
#define PLIB_STATELOCALDATA_H

#include <stdint.h>
#include "SegmentLog.h"

/**
 * Persist state in all servers
 */
struct __attribute__ ((__packed__)) PersistState{
    uint64_t currentTerm;
    uint64_t lastAppliedIndex;
    uint64_t segmentInitIndex;
    uint16_t voteFor;
    SegmentLog logs;
};

/**
 * Volatile state on all servers
 */
struct __attribute__ ((__packed__)) VolatileState{
    uint16_t clusterId;
    char * cname;
    uint16_t port;

    bool responedVote;
    bool grantedVote;

    uint64_t matchIndex;
    uint64_t nextIndex;

    uint64_t lastRequestTimeNano;
    uint64_t nextRequestTimeNano;

    /**
     * Leader use this record to ensure leadership
     */
    uint64_t lastResponseTimeNano;
};

#endif //PLIB_STATELOCALDATA_H
