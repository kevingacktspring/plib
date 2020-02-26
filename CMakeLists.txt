cmake_minimum_required(VERSION 3.12)
project(plib)

set(CMAKE_CXX_STANDARD 14)

set(PROTOCAL_DATA
        DataProtocalSerializer.h
        StateLocalData.h
        InstallSnapshot.h
        RequestVoteCommand.h
        AppendLogCommand.h)

set(PERSIST_LOG
        SegmentLog.h
        SegmentLog.cpp)

set(PROTOCAL_SERVER_EPOLL
        ProtocalServerEpoll.cpp
        ProtocalServerEpoll.h)

set(PROTOCAL_SERVER_SELECT
        ProtocalServerSelect.cpp
        ProtocalServerSelect.h)

set(PROTOCAL_CLIENT_TCP
        ProtocalClientTCP.cpp
        ProtocalClientTCP.h)

set(PROTOCAL_CLIENT_UDP
        ProtocalClientUDP.cpp
        ProtocalClientUDP.h)

set(APP_STARTER
        AppLoopStarter.h
        AppLoopStarter.cpp)

set(RAFT_LOGIC
        ElectionLogic.h
        ElectionLogic.cpp
        AppendEntryLogic.h
        AppendEntryLogic.cpp)

add_executable(starter AppLoopStarter.cpp
        ${APP_STARTER}
        ${PROTOCAL_SERVER_EPOLL}
        ${PROTOCAL_SERVER_SELECT}
        ${PROTOCAL_CLIENT_TCP}
        ${PROTOCAL_CLIENT_UDP}
        ${PROTOCAL_DATA}
        ${PERSIST_LOG}
        ${RAFT_LOGIC})

add_executable(plib_server main.cpp
        ${PROTOCAL_SERVER_EPOLL}
        ${PROTOCAL_SERVER_SELECT}
        ${PROTOCAL_CLIENT_TCP}
        ${PROTOCAL_CLIENT_UDP}
        ${PROTOCAL_DATA}
        ${PERSIST_LOG}
        ${RAFT_LOGIC})
