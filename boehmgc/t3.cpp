#include <stdio.h>
#include "gc_detect_leaks.h"
class Foo{};
int main() {
    Foo *f;
    while(1){
        f =new Foo;  //t3.cpp line7
//        delete f;
        CHECK_LEAKS();
    }
}
