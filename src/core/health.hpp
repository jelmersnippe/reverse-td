#pragma once

struct Health {
    int max;
    int current;

    Health(int start) { max = start, current = start; }
};
