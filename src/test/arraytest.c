#include    <stdint.h>
#include    <stdio.h>

typedef char page[4];

int main(void) {
    char area[16];
    void *ptr = (void *) area;
    page *pages = ptr;
    printf("%p %p %llu\n", ptr, pages[3], ((uintptr_t) pages[3]) - ((uintptr_t) ptr));
    return 0;
};
