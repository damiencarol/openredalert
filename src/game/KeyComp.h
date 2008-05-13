#ifndef KEYCOMP_H
#define KEYCOMP_H

class FibHeapEntry;

/** Friend class which compares ActionEvents priority */
class KeyComp {
public:
    bool operator()(FibHeapEntry * x, FibHeapEntry * y) ;
};

#endif //KEYCOMP_H
