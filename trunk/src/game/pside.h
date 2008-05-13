#ifndef PSIDE_H
#define PSIDE_H

enum PSIDE {
    PS_UNDEFINED = 0, PS_GOOD = 0x1, PS_BAD = 0x2,
    PS_NEUTRAL = 0x4, PS_SPECIAL = 0x8, PS_MULTI = 0x10
};

#endif //PSIDE_H
