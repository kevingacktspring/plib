//
// Created by gackt on 2/6/20.
//

#include <iostream>
#include <thread>
#include <signal.h>
#include <sys/un.h>

#include "ProtocalClientTCP.h"
#include "ProtocalClientUDP.h"
#include "ProtocalServerEpoll.h"
#include "ProtocalServerSelect.h"

#include "ClusterConfigTemp.h"
#include "FdUtils.h"

#include "AppLoopStarter.h"
#include "RequestVoteCommand.h"

AppLoopStarter::AppLoopStarter(int nodeid) : nodeid(nodeid) {

    /**
     * cache my node position
     */
    node_position = nodeid - 1;

    /**
     * initialize cluster in conf
     */
    loadClusterConfig(cluster_config);
    //debug
    for (int j = 0; j < cluster_size; ++j) {
        std::cout << "Cluster config -> " << std::endl
                  << " node: " << (*(cluster_config + j))->nodeid
                  << " address: " << (*(cluster_config + j))->servInetAddr
                  << " port: " << (*(cluster_config + j))->servPort << std::endl;
    }
}

/**
 *
 */
AppLoopStarter::~AppLoopStarter() {
    /**
     * shut-down epoll server
     */
    protocalServerEpoll->closeEpollServer();
    protocal_server_thread.join();

    /**
     * free protocalServerEpoll instance
     */
    delete (protocalServerEpoll);

    /**
     * free tcp client and cluster-nodes
     */
    for (int j = 0; j < cluster_size; ++j) {
        // tcp clients
        if (j != node_position) {
            free(clusterTcpClients + j);
        }
        // cluster nodes
        free((void *) (*(cluster_config + j))->servInetAddr);
        free(cluster_config + j);
    }
}

void AppLoopStarter::initialize() {
    /**
     * Initialize node server
     */
    protocalServerEpoll = new ProtocalServerEpoll(
            *(cluster_config + node_position), cluster_config, cluster_size);

    /**
     * Initialize client for peer nodes
     */
    for (int k = 0; k < cluster_size; ++k) {
        // set npt for my instance
        if ((*(cluster_config + k))->nodeid == nodeid) {
            *(clusterTcpClients + k) == nullptr;
            continue;
        }
        // create other tcp clients
        ProtocalClientTCP *protocalClientTcp = new ProtocalClientTCP(cluster_config[k]);
        *(clusterTcpClients + k) = protocalClientTcp;

        // deligate tcp client to epoll server
        protocalServerEpoll->registerClient(protocalClientTcp);
    }

    /**
     * run epoll server
     */
    protocal_server_thread = std::thread(&ProtocalServerEpoll::initService, protocalServerEpoll);
}

/**
 * business loop
 * @return
 */
void AppLoopStarter::run() {
    int round = 500;
    while (round > 0) {
        if (nodeid == 1) {
            ProtocalClientTCP *node2 = clusterTcpClients[1];
            int ret = send(node2->connfd, "Vote from node 1\n\0", 18, MSG_DONTWAIT | MSG_NOSIGNAL);
            if (ret < 0) {
                print_socket_err_reason(node2->connfd);
                node2->doConnect();
            }
        } else if (nodeid == 2) {
            ProtocalClientTCP *node1 = clusterTcpClients[0];
            int ret = send(node1->connfd, "Vote from node 2\n\0", 18, MSG_DONTWAIT | MSG_NOSIGNAL);
            if (ret < 0) {
                print_socket_err_reason(node1->connfd);
                node1->doConnect();
            }
        }
        --round;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    /*
    uint64_t round = 1;
    while(round < 500) {
        if (nodeid == 1) {
            ProtocalClientTCP *node2 = clusterTcpClients[1];

            ReqeustVoteReq reqeustVoteReq {round, 1, 3, 3};
            DataPacket dataPacket;
            compressReqeustVoteReq(std::ref(reqeustVoteReq), std::ref(dataPacket));

            int ret = send(node2->connfd, dataPacket.body, dataPacket.length, MSG_DONTWAIT | MSG_NOSIGNAL);
            if (ret < 0) {
                print_socket_err_reason(node2->connfd);
                node2->doConnect();
            }
        } else if (nodeid == 2) {
            ProtocalClientTCP *node1 = clusterTcpClients[0];

            ReqeustVoteReq reqeustVoteReq {round, 2, 5, 5};
            DataPacket dataPacket;
            compressReqeustVoteReq(std::ref(reqeustVoteReq), std::ref(dataPacket));

            int ret = send(node1->connfd, dataPacket.body, dataPacket.length, MSG_DONTWAIT | MSG_NOSIGNAL);
            if (ret < 0) {
                print_socket_err_reason(node1->connfd);
                node1->doConnect();
            }
        }
        --round;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    */
}

int main(int argc, char *argv[]) {
    std::cout << "App start entry here" << std::endl;

    /**
     * Catch Signal Handler SIGPIPE
     */
    signal(SIGPIPE, signal_callback_handler);

    /**
     * set max resource limit for procedure
     */
    set_resource_limit(MAXEPOLLSIZE);

    /**
     * get my id from argv[1] AppLoopStarter 1(2 ...)
     * argc = 2; 1.AppLoopStarter 2.myid
     */
    int nodeid = atoi(argv[1]);
    std::cout << "my node-id: " << nodeid << std::endl;
    if (nodeid < 1) {
        fprintf(stderr, "Check given node-id, must greater than Zero.");
        exit(-1);
    }

    /**
     * start business loop
     */
    AppLoopStarter appLoopStarter(nodeid);
    appLoopStarter.initialize();

    std::this_thread::sleep_for(std::chrono::seconds(3));

    appLoopStarter.run();

    return 0;
}
