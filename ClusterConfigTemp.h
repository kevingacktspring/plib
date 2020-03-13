//
// Created by gackt on 3/13/20.
//

#ifndef PLIB_CLUSTERCONFIGTEMP_H
#define PLIB_CLUSTERCONFIGTEMP_H

#include <stdint.h>
#include <malloc.h>
#include "StateLocalData.h"

static const uint8_t cluster_size = 5;
static const uint16_t base_port = 9090;
static const char *local_ip = "127.0.0.1\0";

static void loadClusterConfig(VolatileState **cluster_config) {
    VolatileState *resource;
    for (uint8_t i = 0; i < cluster_size; ++i) {
        resource = (VolatileState *) (malloc(sizeof(VolatileState)));
        uint8_t nodeid = i + 1;
        memcpy((void *) &(resource->nodeid), &nodeid, sizeof(uint8_t));
        uint16_t curr_port = base_port + i + 1;
        memcpy((void *) &(resource->servPort), &curr_port, sizeof(uint16_t));
        resource->servInetAddr = local_ip;
        *(cluster_config + i) = resource;
    }
}


#endif //PLIB_CLUSTERCONFIGTEMP_H
