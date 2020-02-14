cmake_minimum_required(VERSION 3.13)
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

set(PROTOCAL_SERVER
        ProtocalServerUDP.cpp
        ProtocalServerUDP.h)

set(PROTOCAL_CLIENT
        ProtocalClientUDP.cpp
        ProtocalClientUDP.h)

set(RAFT_LOGIC
        AppLoopStarter.h
        AppLoopStarter.cpp
        ElectionLogic.h
        ElectionLogic.cpp
        AppendEntryLogic.h
        AppendEntryLogic.cpp)

add_executable(plib_server main.cpp
        ${PROTOCAL_SERVER}
        ${PROTOCAL_CLIENT}
        ${PROTOCAL_DATA}
        ${PERSIST_LOG}
        ${RAFT_LOGIC})
