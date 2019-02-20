#pragma once

struct a6_future;

struct a6_promise {
    struct a6_future *future;
};
