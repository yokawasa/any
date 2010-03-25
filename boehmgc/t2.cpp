#include <stdio.h>
#define GC_DEBUG
#include "gc_cpp.h"
#define CHECK_LEAKS() GC_gcollect()
class Foo: public gc{};
int main() {
    GC_find_leak = 1;
    Foo *f;
    while(1){
        f =new Foo;   //t2.cpp line10
//        delete f;
        CHECK_LEAKS();
    }
}
