//
// Created by gackt on 2/6/20.
//

#ifndef PLIB_ELECTIONLOGIC_H
#define PLIB_ELECTIONLOGIC_H


class ElectionLogic {

    void requestVote();

    void handleRequestVoteRequest();

    void handleReqeustVoteResponse();

    void becomeLeader();

    void electionTimeOut();

    void leaderShipTimeout();

    void stepDown();

};


#endif //PLIB_ELECTIONLOGIC_H
