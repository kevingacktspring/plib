//
// Created by gackt on 1/28/20.
//

#ifndef PLIB_APPENDLOGCOMMAND_H
#define PLIB_APPENDLOGCOMMAND_H

#include <stdint.h>

struct __attribute__((__packed__)) appendLogReq {
    uint64_t term;
    uint16_t leaderId;
    uint64_t prevLogTerm;

    uint64_t leaderCommit;  // leader's commitIndex
};

struct __attribute__((__packed__)) appendLogResp {
    uint64_t term;
    bool success;
};

#endif //PLIB_APPENDLOGCOMMAND_H
