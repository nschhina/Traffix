#include "StageThreeController.h"

using namespace std;

/**
 * Initializes the StageThreeController given a Weighted Directed Graph.
 * @param G the Weighted Directed Graph that the controller will control
 */
StageThreeController::StageThreeController(WeightedDigraph *G) : Controller(G) {}

/**
 * Deconstructs the StageThreeController.
 */
StageThreeController::~StageThreeController() {}

/**
 * Adds an event with a specified intersection ID at a specified time.
 * The event is when the traffic signal is scheduled to be cycled.
 * @param time the specified time the event will occur
 * @param id the intersection ID of the event
 */
void StageThreeController::addEvent(double time, int id) {
    events.push(make_pair(time, id));
}

/**
 * Returns true if the current time is at or after the time the next event is scheduled to occur.
 * Returns false otherwise.
 * @param currentTime the current time
 */
bool StageThreeController::checkNextEvent(double currentTime) const {
    return currentTime >= events.top().first;
}

/**
 * Runs the events that are before or at the current time that have not yet been run and adds the
 * next events to the event queue.
 * @param currentTime the current time
 */
void StageThreeController::runEvents(double currentTime) {
    while (!events.empty() && events.top().first <= currentTime) {
        Intersection *n = G->getIntersection(events.top().second);
        bool prevLeft = n->leftTurnSignalOn();
        n->cycle();
        if (n->leftTurnSignalOn()) events.push(make_pair(currentTime + 10.0, n->getID())); // 10 seconds for left signals
        else events.push(make_pair(currentTime + 30.0 - (10.0 * prevLeft), n->getID())); // 20 seconds if there was just a left signal, 30 otherwise
    }
    // add road segments to traffic light, get current cycle number in intersection, if green, check if net flow is less than 2 times the opposite flow
    // if so, then cycle the lights
}
