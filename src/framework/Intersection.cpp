#include <assert.h>
#include "Intersection.h"

using namespace std;

int Intersection::counter = 0; // counter starts at 0

/**
 * Initializes a new intersection given an x, y coordianate in the 2-D cartesian plane.
 * @param x the x-coordinate
 * @param y the y-coordinate
 */
Intersection::Intersection(double x, double y) {
    this->location = Point2D(x, y);
    id = Intersection::counter++; // assigns an id and increments the counter
    currentCycleNumber = 0;
    numberOfCycles = 0;
    leftTurn = -1;
}

/**
 * Initializes a new intersection given a point in the 2-D cartesian plane.
 * @param location the location of the intersection as a point
 */
Intersection::Intersection(Point2D &location) {
    this->location = Point2D(location.x, location.y);
    this->id = counter++; // assigns an id and increments the counter
    currentCycleNumber = 0;
    numberOfCycles = 0;
    leftTurn = -1;
}

/**
 * Deconstructs the Intersection
 */
Intersection::~Intersection() {}

/**
 * Returns the unique ID of this intersection.
 */
int Intersection::getID() const { return id; }

/**
 * Adds a RoadSegment to the intersection
 * @return false if the road segment is already in the intersection, true otherwise
 */
bool Intersection::add(RoadSegment *r) {
    if (inboundRoads.count(r->getID()) || outboundRoads.count(r->getID()) ) return false;
    if (r->getDestination()->getID() == this->id) {
        inboundRoads[r->getID()] = r;
        inboundIntersections[r->getSource()->getID()] = r->getSource();
        roadFrom[r->getSource()->getID()] = r;
    }
    if (r->getSource()->getID() == this->id) {
        outboundRoads[r->getID()] = r;
        outboundIntersections[r->getDestination()->getID()] = r->getDestination();
        roadTo[r->getDestination()->getID()] = r;
    }
    if (r->getDestination()->getID() != this->id && r->getSource()->getID() != this->id) {
        assert(false && "this road segment does not start or end at this intersection");
    }
    return true;
}

/**
 * Removes a RoadSegment from the intersection
 * @return false if the road segment was not in the intersection, true otherwise
 */
bool Intersection::remove(RoadSegment *r) {
    if (inboundRoads.count(r->getID()) == 0 && outboundRoads.count(r->getID()) == 0) return false;
    if (inboundRoads.count(r->getID())) {
        inboundRoads.erase(r->getID());
        inboundIntersections.erase(r->getSource()->getID());
        roadFrom.erase(r->getSource()->getID());
        for (int to : adjacentOut[r->getID()]) { // removing all paths that lead out from the road segment
            // removing the linked traffic lights
            int lightID = lights[make_pair(r->getID(), to)]->getID();
            for (int link : linksLeft[lightID]) {
                linksStraight[link].erase(lightID);
            }
            linksLeft.erase(lightID);
            for (int link : linksStraight[lightID]) {
                linksStraight[link].erase(lightID);
            }
            linksStraight.erase(lightID);
            for (int link : linksRight[lightID]) {
                linksStraight[link].erase(lightID);
            }
            linksRight.erase(lightID);
            // removing the traffic light
            lightFromID.erase(lightID);
            int cycleNum = cycleNumber[lightID];
            cycleToLight[cycleNum].erase(lightID);
            if (cycleToLight[cycleNum].size() == 0) {
                for (int light : cycleToLight.back()) {
                    cycleToLight[cycleNum].insert(light);
                    cycleNumber[light] = cycleNum;
                }
                cycleToLight[cycleNum].erase(lightID);
                numberOfCycles--;
            }
            cycleNumber.erase(lightID);
            delete lights[make_pair(r->getID(), to)];
            lights.erase(make_pair(r->getID(), to));
            adjacentIn[to].erase(r->getID());
        }
        adjacentOut.erase(r->getID());
    }
    if (outboundRoads.count(r->getID())) { // removing all paths that lead into the road segment
        outboundRoads.erase(r->getID());
        outboundIntersections.erase(r->getDestination()->getID());
        roadTo.erase(r->getDestination()->getID());
        for (int from : adjacentIn[r->getID()]) {
            // removing the linked traffic lights
            int lightID = lights[make_pair(from, r->getID())]->getID();
            for (int link : linksLeft[lightID]) {
                linksStraight[link].erase(lightID);
            }
            linksLeft.erase(lightID);
            for (int link : linksStraight[lightID]) {
                linksStraight[link].erase(lightID);
            }
            linksStraight.erase(lightID);
            for (int link : linksRight[lightID]) {
                linksStraight[link].erase(lightID);
            }
            linksRight.erase(lightID);
            // removing the traffic light
            lightFromID.erase(lightID);
            int cycleNum = cycleNumber[lightID];
            cycleToLight[cycleNum].erase(lightID);
            if (cycleToLight[cycleNum].size() == 0) {
                for (int light : cycleToLight.back()) {
                    cycleToLight[cycleNum].insert(light);
                    cycleNumber[light] = cycleNum;
                }
                cycleToLight[cycleNum].erase(lightID);
                numberOfCycles--;
            }
            cycleNumber.erase(lightID);
            delete lights[make_pair(from, r->getID())];
            lights.erase(make_pair(from, r->getID()));
            adjacentOut[from].erase(r->getID());
        }
        adjacentIn.erase(r->getID());
    }
    return true;
}

/**
 * Connects two roads with a traffic light with a specified type
 * @param from the ID of the source road segment
 * @param to the ID of the destination road segment
 * @param type the type of turn the traffic light controls (0 for left, 1 for straight, 2 for right)
 */
void Intersection::connect(int from, int to, int type) {
    assert(inboundRoads.count(from) && "no inbound road exists in the intersection");
    assert(outboundRoads.count(to) && "no outbound road exists in the intersection");
    adjacentOut[from].insert(to);
    adjacentIn[to].insert(from);
    TrafficLight *t = new TrafficLight(inboundRoads[from], outboundRoads[to], type);
    lights[make_pair(from, to)] = t;
    lightFromID[t->getID()] = t;
    if (t->getType() == STRAIGHT) {
        cycleNumber[t->getID()] = numberOfCycles;
        cycleToLight.push_back(unordered_set<int>());
        cycleToLight[numberOfCycles].insert(t->getID());
        numberOfCycles++;
    }
}

/**
 * Links two lights together so they can be synchronized.
 * Lights can only be linked if A and B are both straight types, A is straight, B is left, or A is straight, B is right
 * @param A the ID of one traffic light
 * @param B the ID of the other traffic light
 */
void Intersection::link(int A, int B) {
    int AType = lightFromID[A]->getType();
    assert(AType == STRAIGHT && "light A must be of type straight");
    int BType = lightFromID[B]->getType();
    if (BType == LEFT) {
        linksLeft[A].insert(B);
        linksStraight[B].insert(A);
    } else if (BType == STRAIGHT) {
        linksStraight[A].insert(B);
        linksStraight[B].insert(A);
        int minCycle = min(cycleNumber[A], cycleNumber[B]);
        int maxCycle = max(cycleNumber[A], cycleNumber[B]);
        cycleNumber[A] = cycleNumber[B] = minCycle;
        for (int light : cycleToLight[maxCycle]) {
            cycleToLight[minCycle].insert(light);
            cycleNumber[light] = minCycle;
        }
        for (int light : cycleToLight.back()) {
            cycleToLight[maxCycle].insert(light);
            cycleNumber[light] = maxCycle;
        }
        cycleToLight.pop_back();
    } else { // BTYPE == RIGHT
        linksRight[A].insert(B);
        linksStraight[B].insert(A);
    }
}

/**
 * Helper function for assign. Ensures that all linked lights get assigned the same cycle number.
 */
/*void Intersection::dfs(int light, int cur) {
    cycleNumber[light] = cur;
    for (int next : linksStraight[light]) {
        if (cycleNumber[next] == -1) dfs(next, cur);
    }
}*/

/**
 * Assigns a cycle number for a light.
 */
/*void Intersection::assign() {
    // resets everything to negative 1
    for (pair<int, TrafficLight*> light : lightFromID) {
        cycleNumber[light.first] = -1;
    }
    int cur = 0;
    for (pair<int, TrafficLight*> light : lightFromID) {
        if (cycleNumber[light.first] == -1 && light.second->getType() == STRAIGHT) {
            dfs(light.first, cur);
            cur++;
        }
    }
    numberOfCycles = cur;
}*/

/**
 * Cycles the traffic lights in the intersection.
 */
void Intersection::cycle() {
    unordered_set<int> greens;
    unordered_set<int> lefts;
    for (int light : cycleToLight[(currentCycleNumber + numberOfCycles - 1) % numberOfCycles]) {
        lightFromID[light]->setState(RED);
        for (int left : linksLeft[light]) {
            lightFromID[left]->setState(RED);
        }
    }
    for (int light : cycleToLight[currentCycleNumber]) {
        greens.insert(light);
        if (!leftTurn) {
            for (int left : linksLeft[light]) {
                lefts.insert(left);
            }
        }
    }
    if (lefts.size() == 0) {
        leftTurn = false;
        for (int light : greens) {
            lightFromID[light]->setState(GREEN);
        }
        currentCycleNumber = (currentCycleNumber + 1) % numberOfCycles;
    } else {
        leftTurn = true;
        for (int light : lefts) {
            lightFromID[light]->setState(GREEN);
        }
    }
}

/**
 * Returns the current cycle number in the intersection.
 */
int Intersection::getCurrentCycle() const { return currentCycleNumber; }

/**
 * Returns true if any left turn signal is on, false otherwise.
 */
bool Intersection::leftTurnSignalOn() const { return leftTurn; }

/**
 * Returns the number of outbound road segments in this intersection.
 */
int Intersection::outdegree() const { return outboundRoads.size(); }

/**
 * Returns the number of inbound road segments in this intersection.
 */
int Intersection::indegree() const { return inboundRoads.size(); }

/**
 * Returns an immutable reference to the road segments (and their IDs) coming into the interesection.
 */
const unordered_map<int, RoadSegment*> &Intersection::getInboundRoads() const { return inboundRoads; }

/**
 * Returns an immutable reference to the road segments (and their IDs) going out from the intersection.
 */
const unordered_map<int, RoadSegment*> &Intersection::getOutboundRoads() const { return outboundRoads; }

/**
 * Returns an immutable reference to the adjacent intersections (and their IDs) that have a road leading into this interesection.
 */
const unordered_map<int, Intersection*> &Intersection::getInboundIntersections() const { return inboundIntersections; }

/**
 * Returns an immutable reference to the adjacent intersections (and their IDs) that have a road leading out from this intersection.
 */
const unordered_map<int, Intersection*> &Intersection::getOutboundIntersections() const { return outboundIntersections; }

/**
 * Returns true if there is a traffic light between the two road segmetns given their IDs, false otherwise.
 */
bool Intersection::isConnected(int from, int to) {
    assert(inboundRoads.count(from) && outboundRoads.count(to) && "one of the roads is not in the intersection");
    pair<int, int> p = make_pair(from, to);  
    return lights.count(p) > 0;
}

/**
 * Returns a pointer to the traffic light between the two road segments given their IDs.
 */
TrafficLight *Intersection::getLightBetween(int from, int to) {
    assert(inboundRoads.count(from) && outboundRoads.count(to) && "one of the roads is not in the intersection");
    pair<int, int> p = make_pair(from, to);
    assert(lights.count(p) && "the is no light between the two roads");
    return lights[p];
}

/**
 * Returns a pointer to the road that leads from the intersection with the specified ID.
 */
RoadSegment *Intersection::getRoadFrom(int id) {
    assert(inboundIntersections.count(id) && "there is no road leading from this intersection");
    return roadFrom[id];
}

/**
 * Returns a pointer to the road that leads to the intersection with the specified ID.
 */
RoadSegment *Intersection::getRoadTo(int id) {
    assert(outboundIntersections.count(id) && "there is no road leading from this intersection");
    return roadTo[id];
}

/**
 * Returns the location of this interesection.
 */
Point2D Intersection::getLocation() const { return this->location; }

/**
 * Compares this Intersection to Intersection n by comparing their ids.
 *
 * @param  n the other intersection
 * @return true if and only if this intersection equals the other, false otherwise
 */
bool Intersection::operator == (const Intersection &n) const { return id == n.id; }

/**
 * Compares this Intersection to Intersection t by comparing their ids.
 *
 * @param  n the other intersection
 * @return true if and only if this intersection does not equals the other, false otherwise
 */
bool Intersection::operator != (const Intersection &n) const { return id != n.id; }
