#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>


void change(char *source) {
          source[0] = 'a';
          printf("%s\n", source);
}

int main() {

char *a = "ABC";
change(a);

return 0;
} 

