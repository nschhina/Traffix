#ifndef BASICCONTROLLER_H_
#define BASICCONTROLLER_H_

#include "../framework/Framework.h"
#include "Controller.h"

#define MIN_TIME 5
#define MAX_TIME 50

struct BasicController : public Controller {
public:
    BasicController(WeightedDigraph *G);
    ~BasicController();
    void addEvent(double time, int id);
    bool checkNextEvent(double currentTime) const;
    void runEvents(double currentTime);
};

#endif
