#define GC_DEBUG
#include "gc.h"

//code must be compiled with -DGC_DETECT_MEM_LEAK to check memory leak with Boehm GC

#ifdef GC_DETECT_MEM_LEAK
int GC_find_leak = 1;
void* operator new(size_t size, const char * s, int i) {
    return GC_debug_malloc(size,s,i);
}
void* operator new[](size_t size,  const char * s, int i) {
    return GC_debug_malloc(size,s,i);
}
void operator delete(void* p) {
    if (p) GC_debug_free(p);
}
void  operator delete[](void* p) {
    if (p) GC_debug_free(p);
}
#   define new new(__FILE__, __LINE__)
#   define malloc(n) GC_MALLOC(n)
#   define calloc(m,n) GC_MALLOC((m)*(n))
#   define free(p) GC_FREE(p)
#   define realloc(p,n) GC_REALLOC((p),(n))
#   define CHECK_LEAKS() GC_gcollect()
#else
int GC_find_leak = 0;
#   define CHECK_LEAKS()
#endif

/*
void* operator new(size_t size) {
    return GC_MALLOC(size);
}
void* operator new[](size_t size) {
    return GC_MALLOC(size);
}
void operator delete(void* p) {
    if (p) GC_FREE(p);
}
void  operator delete[](void* p) {
    if (p) GC_FREE(p);
}
*/
