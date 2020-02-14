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

static feed_back *load_bin_data(char *binptr) {

    size_t required_size = sizeof(fas_info) + sizeof(account) + sizeof(owner) + sizeof(ledger);

    feed_back *info = static_cast<feed_back *>(malloc(sizeof(feed_back)));
    memset(info, 0, sizeof(feed_back));

    //First 8 bytes are size_flag
    long data_size;
    memset(&data_size, 0, sizeof(long));
    memcpy(&data_size, binptr, sizeof(long));
    info->size = required_size;

    //Next 8 bytes are original ptr offset
    long offset;
    memset(&offset, 0, sizeof(void *));
    memcpy(&offset, binptr + sizeof(long), sizeof(void *));

    //Rest bytes are data content
    char *data_address = static_cast<char *>(malloc(required_size));
    memset(data_address, 0, required_size);
    memcpy(data_address, binptr + sizeof(long) + sizeof(void *), required_size);
    info->address = data_address;

    //test get raw data type ledger, contains no ptr
    ledger *ledger1 = static_cast<ledger *>(malloc(sizeof(ledger)));
    memset(ledger1, 0, sizeof(ledger));

    void * ledger1_address = data_address + sizeof(fas_info) + sizeof(account) + sizeof(owner);
    printf("ledger1 address: %p \n", ledger1_address);
    memcpy(ledger1, ledger1_address, sizeof(ledger));

    printf("Case1 get raw data by offset, no ptr \n");
    printf("transactionId: %d \n", ledger1->transaction_id);
    printf("amount: %d \n", ledger1->amount);
    printf("\n");

    free(ledger1);

    //test offset calculate
    fas_info *fas1 = static_cast<fas_info *>(malloc(sizeof(fas_info)));
    memset(fas1, 0, sizeof(fas_info));
    memcpy(fas1, data_address, sizeof(fas_info));
/**
    account *account1 = static_cast<account *>(malloc(sizeof(account)));
    memset(account1, 0, sizeof(account));
    memcpy(account1, data_address + sizeof(fas_info), sizeof(account));
    free(account1);

    owner *owner1 = static_cast<owner *>(malloc(sizeof(owner)));
    memset(owner1, 0, sizeof(owner));
    memcpy(owner1, data_address + sizeof(fas_info) + sizeof(account), sizeof(owner));
    free(owner1);

    ledger *ledger3 = static_cast<ledger *>(malloc(sizeof(ledger)));
    memset(ledger3, 0, sizeof(ledger));
    memcpy(ledger3, data_address + sizeof(fas_info) + sizeof(account) + sizeof(owner), sizeof(ledger));
    free(ledger3);
**/

    long o_ledger_address_long = reinterpret_cast<long>(fas1->ledgerptr);
    char *o_ledger_address = reinterpret_cast<char *>(o_ledger_address_long);
    char *offset_address = reinterpret_cast<char *>(offset);

    //get offset value by ptrdiff_t
    ptrdiff_t offset_diff = o_ledger_address - offset_address;

    void *n_ledger_address = data_address + offset_diff;

    ledger *ledger2 = static_cast<ledger *>(malloc(sizeof(ledger)));
    memset(ledger2, 0, sizeof(ledger));
    memcpy(ledger2, n_ledger_address, sizeof(ledger));

    printf("Case2 get data by ptr offset of original data clause \n");
    printf("transactionId: %d \n", ledger2->transaction_id);
    printf("amount: %d \n", ledger2->amount);
    printf("\n");

    free(ledger2);
    free(fas1);

    //finally
    //free(info);

    return info;
}
