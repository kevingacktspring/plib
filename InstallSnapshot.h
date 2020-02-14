//
// Created by gackt on 2/1/20.
//

#ifndef PLIB_INSTALLSNAPSHOT_H
#define PLIB_INSTALLSNAPSHOT_H

#include <stdint.h>

struct __attribute__((__packed__)) InstallSnapshotReq {
    uint64_t term;
    uint16_t candidateId;
    uint64_t lastLogIndex;
    uint64_t lastLogTerm;
};

struct __attribute__((__packed__)) InstallSnapshotResp {
    uint64_t term;
    uint16_t candidateId;
    bool voteGrated;
};

#endif //PLIB_INSTALLSNAPSHOT_H
