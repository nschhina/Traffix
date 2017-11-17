#ifndef CAR_H_
#define CAR_H_

#include <vector>
#include "Forward.h"
#include "Point2D.h"
#include "RoadSegment.h"
#include "Intersection.h"
#include "WeightedDigraph.h"
#include "DijkstraDirectedSP.h"

struct RoadSegment; // forward declaration
struct Intersection; // foward declaration

struct Car {
private:
    static int counter; // number of cars that have been created
    int id; // each car has a unique id number
    double currentSpeed; // the car's curent speed
    Point2D currentLocation; // the car's current location
    RoadSegment *currentRoad; // the road the car is currently on
    Point2D source; // the x y location of the source
    Point2D destination; // the x y location of the destination
    std::vector<RoadSegment*> sourceRoads; // possible roads the that lead directly out from the source
    std::vector<RoadSegment*> destinationRoads; // possible roads that lead directly into the destination
    std::vector<int> sourceIntersections; // IDs of possible source intersections
    std::vector<int> destinationIntersections; // IDs of possible destination intersections
    std::vector<double> initialTime; // the initial time to reach of the possible source intersections
    std::vector<double> excessTime; // the extra time to reach the destination from the possible destination interesctions
    DijkstraDirectedSP *path; // the path that the car will take
    int pathIndex; // the current index on the path that the car is on

public:
    Car(Point2D &source, Point2D &destination, std::vector<RoadSegment*> &sourceRoads, std::vector<RoadSegment*> &destinationRoads, WeightedDigraph *G);
    ~Car();
    int getID() const;
    double getCurrentSpeed() const;
    void setSpeed(double speed);
    RoadSegment *getCurrentRoad() const;
    bool hasNextRoad() const;
    RoadSegment *getNextRoad() const;
    void setRoad(RoadSegment *road);
    Point2D getCurrentLocation() const;
    void setLocation(Point2D &location);
    Point2D getSource() const;
    Point2D getDestination() const;
};

RoadSegment *getRandomRoadSegment(WeightedDigraph *G);
Point2D getRandomLocation(RoadSegment *r);
Car *getRandomCar(WeightedDigraph *G);

#endif
