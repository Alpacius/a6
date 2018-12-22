static __thread int g = 0;

int get_g(void) {
    return g;
}

void set_g(int ival) {
    g = ival;
}
