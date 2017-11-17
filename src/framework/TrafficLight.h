#ifndef TRAFFICLIGHT_H_
#define TRAFFICLIGHT_H_

#include "Forward.h"

// light colours
#define RED 0
#define GREEN 1
#define YELLOW 2

// turning at intersection
#define LEFT 0
#define STRAIGHT 1
#define RIGHT 2

struct TrafficLight {
private:
    static int counter; // number of traffic lights that have been created
    int id; // each traffic light has a unique id number
    int state; // current state of the traffic light
    int type; // the type of turning this traffic light controls

public:
    TrafficLight(int type);
    ~TrafficLight();
    int getID() const;
    int getState() const;
    void setState(int state);
    int getType() const;
    bool operator == (const TrafficLight &t) const;
    bool operator != (const TrafficLight &t) const;
};

#endif
