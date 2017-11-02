#include <utility>
#include <cmath>
#include <unordered_set>
#include <assert.h>
#include "Simulation.h"
#include "controller/PretimedController.h"
#include "misc/pair_hash.h"

using namespace std;

/**
 * Initializes a new Simulation given a controller.
 * @param controller the controller that will handle the traffic
 */
Simulation::Simulation(Controller *controller) {
    this->controller = controller;
    currentTime = 0.0;
}

/**
 * Deconstructs the Simulation.
 */
Simulation::~Simulation() {}

/**
 * Performs the next iteration in the simulation.
 * @param timeElapsed the time elasped since the last iteration
 */
void Simulation::nextIteration(double timeElapsed) {
    currentTime += timeElapsed;
    controller->runEvents(currentTime);
    WeightedDigraph *G = controller->getGraph();
    unordered_set<int> done;
    unordered_set<pair<int, int>, pair_hash<int, int>> toDelete;
    // // PRE CHECK
    // for (pair<int, RoadSegment*> r : G->getRoadSegments()) {
    //     for (pair<int, Car*> c : r.second->getCars()) {
    //         assert(c.second != nullptr);
    //         if (c.second->hasNextRoad()) assert(c.second->peekNextRoad()->incoming.count(c.first));
    //     }
    // }
    for (pair<int, RoadSegment*> r : G->getRoadSegments()) {
        Point2D dest = r.second->getDestination()->getLocation();
        if (r.second->countCarsInQueue() > 0 && r.second->getLatestTime() + REACTION_TIME >= currentTime) {
            Car *c = r.second->getNextCar();
            r.second->removeNextCar(currentTime);
            done.insert(c->getID());
            if (!c->hasNextRoad() || (r.second->getDestination()->getLightBetween(r.first, c->peekNextRoad()->getID())->getState() == GREEN
                    && c->peekNextRoad()->getCapacity() - c->peekNextRoad()->getFlow() >= 1)) {
                c->setLocation(dest);
                assert(r.second->removeCar(c) && "car not on road");
                if (c->hasNextRoad()) {
                    RoadSegment *rp = c->getNextRoad();
                    assert(rp->addCar(c) && "car was already on road");
                } else {
                    delete c;
                }
            }
        }
        toDelete.clear();
        for (pair<int, Car*> c : r.second->getCars()) {
            if (done.count(c.first) == 0 && !r.second->isStopped(c.first)) {
                done.insert(c.first);
                double dx = (timeElapsed * c.second->getCurrentSpeed()) * cos(c.second->getCurrentLocation().angleTo(dest));
                double dy = (timeElapsed * c.second->getCurrentSpeed()) * sin(c.second->getCurrentLocation().angleTo(dest));
                Point2D newLoc(c.second->getCurrentLocation().x + dx, c.second->getCurrentLocation().y + dy);
                c.second->setLocation(newLoc);
                double maxDist = timeElapsed * r.second->getSpeedLimit() * 0.51;
                if (c.second->getDestination().distanceTo(newLoc) <= maxDist) {
                    toDelete.insert(make_pair(c.first, 1));
                } else if (r.second->countCarsInQueue() > 0 && r.second->getLastCar()->getCurrentLocation().distanceTo(newLoc) <= maxDist) {
                    assert(r.second->stop(c.first));
                } else if (dest.distanceTo(newLoc) <= maxDist) {
                    if (!c.second->hasNextRoad() || c.second->peekNextRoad()->getCapacity() - c.second->peekNextRoad()->getFlow() >= 1)
                        toDelete.insert(make_pair(c.first, 2));
                }
            }
        }
        for (pair<int, int> c : toDelete) {
            Car *car = r.second->getCar(c.first);
            RoadSegment *cur = car->getCurrentRoad();
            assert(r.second->removeCar(car) && "car not on road");
            if (c.second == 1) {
                delete car;
            } else if (car->hasNextRoad()) {
                if (car->peekNextRoad()->getCapacity() - car->peekNextRoad()->getFlow() < 1) continue;
                RoadSegment *rp = car->getNextRoad();
                assert(rp->addCar(car) && "car was already on road");
                car->setLocation(dest);
            } else {
                delete car;
            }
        }
    }
    // POST CHECK
    // for (pair<int, RoadSegment*> r : G->getRoadSegments()) {
    //     for (pair<int, Car*> c : r.second->getCars()) {
    //         assert(c.second != nullptr);
    //         if (c.second->hasNextRoad()) assert(c.second->peekNextRoad()->incoming.count(c.first));
    //     }
    // }
}
