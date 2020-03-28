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
    messagePacket.length = sizeof(uint8_t) + sizeof(uint64_t) + dataPacket.length;
    messagePacket.body = static_cast<char *>(malloc(messagePacket.length));
    memcpy(messagePacket.body, &dataPacket.msgtype, sizeof(uint8_t));
    memcpy(messagePacket.body + sizeof(uint8_t), &dataPacket.length, sizeof(uint64_t));
    memcpy(messagePacket.body + sizeof(uint8_t) + sizeof(uint64_t), dataPacket.body, dataPacket.length);
    free(dataPacket.body);
}

static void deCompressDataPacket(const MessagePacket &messagePacket, DataPacket &dataPacket) {
    bzero(&dataPacket, sizeof(DataPacket));
    memcpy(&dataPacket.msgtype, messagePacket.body, sizeof(uint8_t));
    memcpy(&dataPacket.length, messagePacket.body + sizeof(uint8_t), sizeof(uint64_t));

    uint64_t body_size = messagePacket.length - sizeof(uint8_t) - sizeof(uint64_t);
    dataPacket.body = static_cast<char *>(malloc(body_size));
    memcpy(dataPacket.body, messagePacket.body + sizeof(uint8_t) + sizeof(uint64_t), body_size);
}


/**
 * TODO, serialization, compress-decompress
 */

#endif //PLIB_DATAPROTOCALSERIALIZER_H
