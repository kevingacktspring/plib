//
// Created by gackt on 1/11/19.
//

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <bitset>

using namespace std;

struct dataSample {
    float a;
    int b;
    char c[8];
};

void sample (){
    struct dataSample temp, test, case3, case5;

    unsigned char *datas = static_cast<unsigned char *>(malloc(sizeof(dataSample)));
    //char *datas = static_cast<char *>(malloc(sizeof(dataSample)));

    //case1
    temp.a = 0.1f;
    temp.b = 26;

    char *cdata = "Test";
    strcpy(temp.c, cdata);

    memcpy((void *) &test, (void *) &temp, sizeof(struct dataSample));
    printf("%3.1f %d %s\n", test.a, test.b, test.c);

    //case2
    memcpy(datas, (void *) &temp, sizeof(struct dataSample));

    printf("Size of structure: %d \n", sizeof(dataSample));
    printf("Size of float: %d \n", sizeof(float));
    printf("Size of int: %d \n", sizeof(int));
    printf("Size of char: %d \n", sizeof(char));

    for (int i = 0; i < sizeof(dataSample); ++i) {
        printf("%02X", datas[i]);
    }
    printf("\n\n");
    for (int i = 0; i < sizeof(dataSample); ++i) {
        printf("%X", datas[i]);
    }
    printf("\n\n");


    //case3
    memcpy(&case3, datas, sizeof(struct dataSample));
    printf("case3 recovered a is: %f \n", case3.a);
    printf("case3 recovered b is: %d \n", case3.b);
    printf("case3 recovered c is: %s \n", case3.c);

    //case5
    string hexdata = "CDCCCC3D1A0000005465737400000000A08A151CFE7F0000";
    string case5str;
    int len = hexdata.length();
    for (int i = 0; i < len; i += 2) {
        string bytes = hexdata.substr(i, 2);
        char chr = (char) (int) strtol(bytes.c_str(), nullptr, 16);
        case5str.push_back(chr);
    }
    const char *case5data = case5str.c_str();


    memcpy(&case5, case5data, sizeof(struct dataSample));
    printf("case5 recovered a is: %f \n", case5.a);
    printf("case5 recovered b is: %d \n", case5.b);
    printf("case5 recovered c is: %s \n", case5.c);
    //printf("case5 recovered d is: %s \n", case5.d->data());


    //String Obj memmove
    string orgdata("this is a long string with a lot of words , I will move this to a new place");
    printf("original location %p \n", &orgdata);
    long lt = orgdata.length();

    void * newplace = malloc(lt);

    free(datas);
}
