//
// Created by gackt on 1/12/19.
//
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <bitset>
#include <cstring>

#include "structure_fixed_size.h"
using namespace std;

static feed_back * compress_data(fas_info * fasptr){

    size_t required_size = sizeof(fas_info) + sizeof(account) + sizeof(owner) + sizeof(ledger);

    void * applied_area = malloc(required_size);

    if (applied_area == nullptr){
        perror("Now Memory Avaliable...");
        exit(1);
    }

    //Calculate each address
    void * account_cp_address = (char*)applied_area + sizeof(fas_info);
    void * owner_cp_address = (char*)account_cp_address + sizeof(account);
    void * ledger_cp_address = (char*)owner_cp_address + sizeof(owner);

    //Move is from the last to fist And change the ptr
    memcpy(ledger_cp_address, fasptr->ledgerptr, sizeof(ledger));
    fasptr->ledgerptr = static_cast<ledger *>(ledger_cp_address); //change ptr to copy address

    memcpy(owner_cp_address, fasptr->accountptr->ownerptr, sizeof(owner));
    fasptr->accountptr->ownerptr = static_cast<owner *>(owner_cp_address);//change ptr to copy address

    memcpy(account_cp_address, fasptr->accountptr, sizeof(account));
    fasptr->accountptr = static_cast<account *>(account_cp_address);//change ptr to copy address

    memcpy(applied_area, fasptr, sizeof(fas_info));

    feed_back *results = static_cast<feed_back *>(malloc(sizeof(feed_back)));
    results->address = applied_area;
    results->size = required_size;

    return results;
}

static void free_compressed (feed_back* ptr) {
    free(ptr->address);
    free(ptr);
};
