#include <stdio.h>
#include "gc_detect_leaks.h"
int main() {
    GC_find_leak = 1;
    char *a;
    while(1){
        a = (char*)malloc(100);  //t1.c line7
        //free(a);
        CHECK_LEAKS();
    }
}
