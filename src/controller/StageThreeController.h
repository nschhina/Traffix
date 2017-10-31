#ifndef STAGETHREECONTROLLER_H_
#define STAGETHREECONTROLLER_H_

#include "../framework/Framework.h"
#include "Controller.h"

struct StageThreeController : public Controller {
    StageThreeController(WeightedDigraph *G);
    ~StageThreeController();
    void addEvent(double time, int id);
    bool checkNextEvent(double currentTime) const;
    void runEvents(double currentTime);
};

#endif
