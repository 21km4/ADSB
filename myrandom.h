#pragma once
#ifndef _WIN32
#include <sys/random.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifndef _WIN32
unsigned int myrand() {
    unsigned char buf[4];
    ssize_t size = getrandom(buf, 4, GRND_NONBLOCK);
    assert(size != -1);

    unsigned int res = 0;
    for(int i = 0; i < 4; i++) {
        unsigned int tmp = 1;
        for(int j = 0; j < i; j++) tmp *= 256;
        tmp *= (unsigned int)buf[i];
        res += tmp;
    }
    return res;
}
#else
unsigned int myrand() {
    return (rand() | (rand() << 16)); 
}
#endif

// [l, r)
int randint(int l, int r) {
    assert(l < r);
    return (myrand() % (r - l) + l);
}
