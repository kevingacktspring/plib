//
// Created by gackt on 2/6/20.
//

#ifndef PLIB_APPLOOPSTARTER_H
#define PLIB_APPLOOPSTARTER_H

#include "ElectionLogic.h"
#include "AppendEntryLogic.h"
#include "ClusterConfigTemp.h"

enum class Type {
    unknown = 0,
    request_vote_request = 1,
    request_vote_response = 2,
    append_entries_request = 3,
    append_entries_response = 4,
    client_request = 5
};

class AppLoopStarter {

public:
    AppLoopStarter(int nodeid);

    virtual ~AppLoopStarter();

    void run();

    void initialize();

private:
    int nodeid;
    int node_position;

    /**
     * log store
     */
    SegmentLog *segmentLog = new SegmentLog();

    /**
     * local machine state
     */
    PersistState *persistState = new PersistState(segmentLog);

    /**
     * cluster config
     */
    VolatileState **cluster_config =
            static_cast<VolatileState **>(malloc(cluster_size * sizeof(VolatileState *)));

    /**
     * initialize node server
     */
    ProtocalServerEpoll *protocalServerEpoll;
    std::thread protocal_server_thread;

    /**
     * initialize client for peer nodes
     */
    ProtocalClientTCP **clusterTcpClients =
            static_cast<ProtocalClientTCP **> (malloc(cluster_size * sizeof(ProtocalClientTCP *)));

    /**
     * election logic
     */
    ElectionLogic electionLogic();

    /**
     * append entry logic
     */
    AppendEntryLogic appendEntryLogic();
};

#endif //PLIB_APPLOOPSTARTER_H
