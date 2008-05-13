#ifndef BQTIMER_H
#define BQTIMER_H

#include "ActionEvent.h"

class BQueue;

class BQTimer : public ActionEvent
{
public:
    BQTimer(BQueue* queue, BQTimer** backref);
    ~BQTimer();
    
    void run();
    void Reshedule();
	void destroy (void);

private:
    BQueue* queue;
    bool scheduled;
    BQTimer** backref;
	bool Destroy;
};

#endif //BQTIMER_H
