//
// Created by gackt on 2/6/20.
//

#include <iostream>
#include <thread>
#include "ProtocalClientTCP.h"
#include "ProtocalClientUDP.h"
#include "ProtocalServerEpoll.h"
#include "ProtocalServerSelect.h"

#include "AppLoopStarter.h"

int main() {
    std::cout<< "App client entry here" << std::endl;

    /**
     * set max resource limit for procedure
     */
    /*struct rlimit rt;
    rt.rlim_max = rt.rlim_cur = MAXEPOLLSIZE;
    if (setrlimit(RLIMIT_NOFILE, &rt) == -1) {
        perror("setrlimit error");
        return -1;
    }*/

    ProtocalClientTCP clientTcp("127.0.0.1\0", 9090);

    int i = 0;
    while (true) {
        std::string tmp("vote now, my id is " + std::to_string(i));
        clientTcp.handle(tmp);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        ++i;
    }

}
