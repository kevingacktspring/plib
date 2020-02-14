//
// Created by gackt on 2/6/20.
//

#ifndef PLIB_DATAPROTOCALSERIALIZER_H
#define PLIB_DATAPROTOCALSERIALIZER_H

#include <stdint.h>

struct __attribute__((__packed__)) DataPacket {
    uint64_t length;
    char* body;
};

/**
 * TODO, serialization, compress-decompress
 */

#endif //PLIB_DATAPROTOCALSERIALIZER_H
