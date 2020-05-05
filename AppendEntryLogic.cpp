//
// Created by gackt on 2/6/20.
//

#include "AppendEntryLogic.h"

AppendEntryLogic::AppendEntryLogic(VolatileState *localConfig, VolatileState **clusterConfig,
                                   PersistState *persistState, ProtocalClientTCP **clusterTcpClients) : local_config(
        localConfig), cluster_config(clusterConfig), persistState(persistState), clusterTcpClients(clusterTcpClients) {}

