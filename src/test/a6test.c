#include    <stdio.h>
#include    <core/swarm.h>


int main(void) {
    struct a6_swarm *swarm = a6_swarm_create(4);
    a6_swarm_launch(swarm);
    while (getchar() != '\n');
    a6_swarm_destroy(swarm);
    return 0;
}
