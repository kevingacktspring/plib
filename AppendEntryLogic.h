//
// Created by gackt on 2/6/20.
//

#ifndef PLIB_APPENDENTRYLOGIC_H
#define PLIB_APPENDENTRYLOGIC_H

#include "ProtocalClientTCP.h"
#include "RequestVoteCommand.h"
#include "StateLocalData.h"

class AppendEntryLogic {
public:
    AppendEntryLogic(VolatileState *localConfig, VolatileState **clusterConfig, PersistState *persistState,
                     ProtocalClientTCP **clusterTcpClients);

    virtual ~AppendEntryLogic() = default;

    void appendEntries();

    void handleAppendEntriesRequest();

    void handleAppendEntriesResponse();

    void getLogEntries(const uint64_t start_index, LogEntries *logEntries);

private:
    uint64_t  max_entry_bytes = 4000000;
    uint16_t  max_entry_batch = 2000;

    VolatileState *local_config;

    VolatileState **cluster_config;

    PersistState *persistState;

    /**
     * Tcp Clients in cluster
     */
    ProtocalClientTCP **clusterTcpClients;
};


#endif //PLIB_APPENDENTRYLOGIC_H
