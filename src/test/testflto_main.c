#include    <stdio.h>
#include    "testflto_func.h"

int main(void) {
    printf("g=%d\n", get_g());
    set_g(1);
    printf("g=%d\n", get_g());
    return 0;
}
