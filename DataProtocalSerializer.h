//
// Created by gackt on 2/6/20.
//

#ifndef PLIB_DATAPROTOCALSERIALIZER_H
#define PLIB_DATAPROTOCALSERIALIZER_H

#include <stdint.h>

enum Type {
    unknown = 0,
    request_vote_request = 1,
    request_vote_response = 2,
    append_entries_request = 3,
    append_entries_response = 4,
    client_request = 5
};

struct __attribute__((__packed__)) DataPacket {
    uint8_t msgtype;
    uint64_t length;
    char *body;
};

struct __attribute__((__packed__)) MessagePacket {
    uint64_t length;
    char *body;
};

static void compressDataPacket(const DataPacket &dataPacket, MessagePacket &messagePacket) {
    messagePacket.length = sizeof(DataPacket::msgtype) + sizeof(DataPacket::length) + dataPacket.length;
    messagePacket.body = static_cast<char *>(malloc(messagePacket.length));
    memcpy(messagePacket.body, &dataPacket.msgtype, sizeof(DataPacket::msgtype));
    memcpy(messagePacket.body + sizeof(DataPacket::msgtype), &dataPacket.length, sizeof(DataPacket::length));
    memcpy(messagePacket.body + sizeof(DataPacket::msgtype) + sizeof(DataPacket::length),
           dataPacket.body, dataPacket.length);
    free(dataPacket.body);
}

static void deCompressDataPacket(const MessagePacket &messagePacket, DataPacket &dataPacket) {
    bzero(&dataPacket, sizeof(DataPacket));
    memcpy(&dataPacket.msgtype, messagePacket.body, sizeof(DataPacket::msgtype));
    memcpy(&dataPacket.length, messagePacket.body + sizeof(DataPacket::msgtype), sizeof(DataPacket::length));

    uint64_t body_size = messagePacket.length - sizeof(DataPacket::msgtype) - sizeof(DataPacket::length);
    dataPacket.body = static_cast<char *>(malloc(body_size));
    memcpy(dataPacket.body, messagePacket.body + sizeof(DataPacket::msgtype) + sizeof(DataPacket::length), body_size);
}


/**
 * TODO, serialization, compress-decompress
 */

#endif //PLIB_DATAPROTOCALSERIALIZER_H
