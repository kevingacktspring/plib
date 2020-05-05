//
// Created by gackt on 2/6/20.
//

#ifndef PLIB_ELECTIONLOGIC_H
#define PLIB_ELECTIONLOGIC_H

#include "ProtocalClientTCP.h"
#include "RequestVoteCommand.h"
#include "StateLocalData.h"

class ElectionLogic {

public:
    ElectionLogic(VolatileState *localConfig, VolatileState **clusterConfig, PersistState *persistState,
                  ProtocalClientTCP **clusterTcpClients);

    virtual ~ElectionLogic() = default;

    void requestVote();

    void handleRequestVoteRequest(const ReqeustVoteReq &voteReq, ReqeustVoteResp &voteResp);

    void handleReqeustVoteResponse(const ReqeustVoteResp &voteResp);

    void increaseCommitIndex();

    void becomeLeader();

    void electionTimeOut();

    void leaderShipTimeout();

    void stepDown(uint64_t newTerm);

    uint64_t stepDown(uint64_t index) const {
        if (index == 0) {
            return 0;
        }
        return persistState->logs->log_vector[index - persistState->logs->first_index].meta.term;
    }

private:
    VolatileState *local_config;

    VolatileState **cluster_config;

    PersistState *persistState;

    /**
     * Tcp Clients in cluster
     */
    ProtocalClientTCP **clusterTcpClients;

};


#endif //PLIB_ELECTIONLOGIC_H
