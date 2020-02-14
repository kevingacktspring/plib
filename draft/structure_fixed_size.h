
#ifndef COMPRESSEDSTRUCTURE_STRUCTURE_FIXED_SIZE_H
#define COMPRESSEDSTRUCTURE_STRUCTURE_FIXED_SIZE_H

struct feed_back{
    long size;
    void* address;
};

struct fas_info{
    long order_id;
    char date_stamp[20];
    struct account *accountptr;
    struct ledger *ledgerptr;
};

struct account{
    int account_id;
    long balance;
    struct owner *ownerptr;
};

struct owner{
    char name[20];
    char identity[20];
};

struct ledger{
    long transaction_id;
    long amount;
};

#endif
