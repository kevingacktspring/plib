//
// Created by gackt on 1/28/20.
//

#ifndef PLIB_REQUESTVOTECOMMAND_H
#define PLIB_REQUESTVOTECOMMAND_H

#include <stdint.h>
#include <cstddef>
#include <malloc.h>
#include <cstring>
#include "DataProtocalSerializer.h"

struct __attribute__((__packed__)) ReqeustVoteReq {
    uint64_t term;
    uint16_t candidateId;
    uint64_t lastLogIndex;
    uint64_t lastLogTerm;
};

struct __attribute__((__packed__)) ReqeustVoteResp {
    uint64_t term;
    uint16_t candidateId;
    bool voteGrated;
};

inline static void compressReqeustVoteReq(const ReqeustVoteReq &reqeustVoteReq, DataPacket &dataProtocal) {
    dataProtocal.length =  sizeof(ReqeustVoteReq);
    dataProtocal.body = static_cast<char *>(malloc(sizeof(ReqeustVoteReq)));
    memcpy(&dataProtocal.body, &reqeustVoteReq, sizeof(ReqeustVoteReq));
}

inline static void decompressReqeustVoteReq(const DataPacket &dataProtocal, ReqeustVoteReq &reqeustVoteReq) {
    memcpy(&reqeustVoteReq, &dataProtocal.body, sizeof(ReqeustVoteReq));
}

inline static void compressReqeustVoteResp(const ReqeustVoteResp &reqeustVoteResp, DataPacket &dataProtocal) {
    dataProtocal.length =  sizeof(ReqeustVoteResp);
    dataProtocal.body = static_cast<char *>(malloc(sizeof(ReqeustVoteResp)));
    memcpy(dataProtocal.body, &reqeustVoteResp, sizeof(ReqeustVoteResp));
}

inline static void decompressReqeustVoteResp(const DataPacket &dataProtocal, ReqeustVoteReq &reqeustVoteReq) {
    memcpy(&reqeustVoteReq, &dataProtocal.body, sizeof(ReqeustVoteReq));
}

#endif //PLIB_REQUESTVOTECOMMAND_H
