#include <chrono>
#include <cmath>
#include <assert.h>
#include <QString>
#include <QFile>
#include <QResource>
#include <QIODevice>
#include <QTextStream>
#include "GUIDriver.h"
#include "controller/PretimedController.h"

using namespace std;

/**
 * Initializes a new GUIDriver.
 * @param argc should be the argument from main
 * @param argv should be the array of arguments from main
 * @param iterationsPerSecond the number of iterations to be executed in the simulator per second
 * @param fileName the file to load the city
 */
GUIDriver::GUIDriver(int argc, char *argv[], double iterationsPerSecond, string fileName) {
    assert(iterationsPerSecond > 0.0 && "iterationsPerSecond must be a positive value");
    this->iterationsPerSecond = iterationsPerSecond;
    iterationLength = 1.0 / iterationsPerSecond;
    QResource common(QString::fromStdString(fileName));
    QFile file(common.absoluteFilePath());
    assert(file.exists() && "unable to open file");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);
    G = new WeightedDigraph();
    controller = new PretimedController(G);
    sim = new Simulation(controller);
    int cntIntersections, cntRoadSegments, cntConnections, cntCars;
    in >> cntIntersections >> cntRoadSegments >> cntConnections >> cntCars >> carsPerSecond;
    Intersection *intersections[cntIntersections];
    for (int i = 0; i < cntIntersections; i++) {
        double x, y;
        in >> x >> y;
        intersections[i] = new Intersection(x, y);
    }
    for (int i = 0; i < cntRoadSegments; i++) {
        int A, B;
        double speedLimit;
        int capacity;
        in >> A >> B >> speedLimit >> capacity;
        assert(G->addRoadSegment(new RoadSegment(intersections[A], intersections[B], speedLimit, capacity)));
    }
    for (int i = 0; i < cntConnections; i++) {
        int intxn, from, to, type;
        in >> intxn >> from >> to >> type;
        intersections[intxn]->connect(from, to, type);
    }
    for (int i = 0; i < cntCars; i++) {
        Car *c = getRandomCar(G);
        c->setSpeed(c->getCurrentRoad()->getSpeedLimit());
    }
    for (int i = 0; i < cntIntersections; i++) {
        intersections[i]->cycle();
    }
    file.close();
    app = new QApplication(argc, argv);
    gui = new GUI(G);
    eventLoop = new QEventLoop(gui);
    gui->show();
}

/**
 * Deconstructs the GUIDriver and the associated simulation.
 */
GUIDriver::~GUIDriver() {
    delete sim;
    delete gui;
    delete eventLoop;
    delete app;
}

/**
 * Runs the simulation in the console.
 */
void GUIDriver::run() {
    bool exit = false;
    auto start = chrono::high_resolution_clock::now();
    auto lastCarSpawn = chrono::high_resolution_clock::now();
    while (!exit) {
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start;
        while (elapsed.count() < iterationLength) { // waits for an iteration
            end = chrono::high_resolution_clock::now();
            elapsed = end - start;
        }
        start = end;
        sim->nextIteration(iterationLength);
        chrono::duration<double> timeSinceLastCar = end - lastCarSpawn;
        if (timeSinceLastCar.count() >= 1.0 / ((double) carsPerSecond)) {
            for (int i = 0; i < (int) floor(timeSinceLastCar.count() * ((double) carsPerSecond)); i++) {
                Car *c = getRandomCar(G);
                c->setSpeed(c->getCurrentRoad()->getSpeedLimit());
            }
            lastCarSpawn = end;
        }
        draw();
    }
}

/**
 * Displays the city layout to the GUI.
 */
void GUIDriver::draw() {
    gui->drawComponents();
    app->processEvents(eventLoop->AllEvents);
}
