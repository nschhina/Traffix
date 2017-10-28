#ifndef ROADSEGMENT_H_
#define ROADSEGMENT_H_

#include <queue>
#include <unordered_set>
#include <unordered_map>
#include "Forward.h"
#include "Intersection.h"
#include "Car.h"

struct RoadSegment {
private:
    static const double EPS;
    static int counter; // number of road segments that have been created
    int id; // each road segment has a unique id number
    Intersection *source; // the source intersection
    Intersection *destination; // the destination intersection
    double length; // the length of the road segment
    double speedLimit; // the speed limit of the road segment
    int flow; // the current amount of traffic on the road segment
    int capacity; // the maximum number of vehicles on the road segment
    std::unordered_map<int, Car*> cars; // the cars on this road segement
    std::queue<int> waiting; // the queue of cars waiting on this intersection
    double latestTime; // the latest time a car left the waiting queue
    std::unordered_set<int> inQueue; // the IDs of the cars in the queue

    void addFlow(int value);
    void subtractFlow(int value);

public:
    RoadSegment(Intersection *source, Intersection *destination, double speedLimit, int capacity);
    ~RoadSegment();
    int getID() const;
    Intersection *getSource() const;
    Intersection *getDestination() const;
    double getLength() const;
    double getSpeedLimit() const;
    double getExpectedTime() const;
    int getFlow() const;
    int getCapacity() const;
    bool addCar(Car *c);
    bool removeCar(Car *c);
    bool carOnRoad(Car *c);
    bool stop(int id);
    Car *getNextCar();
    void removeNextCar(double currentTime);
    Car *getLastCar();
    int countCarsInQueue() const;
    bool isStopped(int id) const;
    double getLatestTime() const;
    Car *getCar(int id);
    const std::unordered_map<int, Car*> &getCars() const;
    bool operator == (const RoadSegment &r) const;
    bool operator != (const RoadSegment &r) const;
    static bool lengthOrderLt(const RoadSegment &r, const RoadSegment &s);
    static bool lengthOrderLe(const RoadSegment &r, const RoadSegment &s);
    static bool lengthOrderGt(const RoadSegment &r, const RoadSegment &s);
    static bool lengthOrderGe(const RoadSegment &r, const RoadSegment &s);
    static bool speedLimitOrderLt(const RoadSegment &r, const RoadSegment &s);
    static bool speedLimitOrderLe(const RoadSegment &r, const RoadSegment &s);
    static bool speedLimitOrderGt(const RoadSegment &r, const RoadSegment &s);
    static bool speedLimitOrderGe(const RoadSegment &r, const RoadSegment &s);
    static bool expectedTimeOrderLt(const RoadSegment &r, const RoadSegment &s);
    static bool expectedTimeOrderLe(const RoadSegment &r, const RoadSegment &s);
    static bool expectedTimeOrderGt(const RoadSegment &r, const RoadSegment &s);
    static bool expectedTimeOrderGe(const RoadSegment &r, const RoadSegment &s);
};

#endif
