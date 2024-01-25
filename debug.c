#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

// Exercise 5: this function is supposed to return a value that doubles "i"
int double_num(int i) {
    return 2*i;
}


int main(void)
{
    // Exercise 5: fix a syntax error
    int num = 10;
    int doub_num = double_num(num);
    printf("double a number (%d) is (%d)\n", num, doub_num);
    // check if the returned value is what we want
    assert(num + num == doub_num);
    printf("Pass!\n");

    return 0;
}

