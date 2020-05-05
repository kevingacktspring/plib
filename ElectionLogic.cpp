//
// Created by gackt on 2/6/20.
//

#include "ElectionLogic.h"

void ElectionLogic::requestVote(){

}

void ElectionLogic::handleRequestVoteRequest(const ReqeustVoteReq &voteReq, ReqeustVoteResp &voteResp){

}

void ElectionLogic::handleReqeustVoteResponse(const ReqeustVoteResp &voteResp){

}

void ElectionLogic::becomeLeader(){

}

void ElectionLogic::electionTimeOut(){

}

void ElectionLogic::leaderShipTimeout(){

}

ElectionLogic::ElectionLogic(VolatileState *localConfig, VolatileState **clusterConfig, PersistState *persistState,
                             ProtocalClientTCP **clusterTcpClients) : local_config(localConfig),
                                                                      cluster_config(clusterConfig),
                                                                      persistState(persistState),
                                                                      clusterTcpClients(clusterTcpClients) {}

