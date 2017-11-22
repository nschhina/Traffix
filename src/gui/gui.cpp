#include <cmath>
#include <assert.h>
#include <QBrush>
#include <QPainter>
#include <ui_GUI.h>
#include "gui.h"
#include "ui_gui.h"

using namespace std;

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 480
#define SCALE_FACTOR 1
#define EPS 1e-9
#define ROAD_WIDTH 5
#define ROAD_SEPARATION 5
#define INTERSECTION_RADIUS 10
#define CAR_RADIUS 1
#define LABEL_DIST 20
#define PI 3.14159265358979323846
#define LABEL_WIDTH 60
#define LABEL_HEIGHT 60
#define EFF_LABEL_WIDTH 40
#define EFF_LABEL_HEIGHT 10

struct color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

// colour constants
const color COLOR_BLACK = {0, 0, 0};
const color COLOR_WHITE = {255, 255, 255};
const color COLOR_RED = {255, 0, 0};
const color COLOR_YELLOW = {255, 255, 0};
const color COLOR_GREEN = {0, 255, 0};
const color COLOR_BLUE = {0, 0, 255};

/**
 * Constructs a
 * @param G The WeightedDigraph to be drawn to the screen
 * @param parent The parent Qt GUI componenet. SHould be a null pointer in all cases.
 */
GUI::GUI(WeightedDigraph *G, QWidget *parent) : QMainWindow(parent), ui(new Ui::GUI), image(SCREEN_WIDTH, SCREEN_HEIGHT, QImage::Format::Format_ARGB32) {
    ui->setupUi(this);
    this->graph = G;
    this->parent = parent;
    efficiencyLabel = new QLabel(this);
    drawComponents();
}

/**
 * Deconstructs the GUI object and the QLabels for each RoadSegment.s
 */
GUI::~GUI() {
    delete efficiencyLabel;
    for (pair<int, QLabel*> p : labels) {
        delete p.second;
    }
    delete ui;
}

void GUI::renderImage() {
     ui->picture->setPixmap(QPixmap::fromImage(image));
}
/**
 * Draws the Intersections, Road Segments, and Cars to the Window
 */
void GUI::drawComponents() {
    char effLabelBuffer[7];
    string effLabelStr;
    sprintf(effLabelBuffer,"%.2f%%",graph->getEfficiency() * 100.0);
    effLabelStr = string(effLabelBuffer);
    efficiencyLabel->setText(QString::fromStdString(effLabelStr));
    efficiencyLabel->setGeometry(SCREEN_WIDTH - EFF_LABEL_WIDTH, SCREEN_HEIGHT, EFF_LABEL_WIDTH, EFF_LABEL_HEIGHT);

    //efficiencyLabel = QLabel(QString::fromStdString(s))
    QPainter painter(&image);
    QPen *pen;
    //QBrush brush;
    image.fill(QColor(COLOR_WHITE.r,COLOR_WHITE.g,COLOR_WHITE.b));
    RoadSegment *road;
    double theta;
    double adjX;
    double adjY;
    double labelPosX;
    double labelPosY;
    double percentage;
    Car *car;

    // draws circles to represent each intersection
    for(pair<int, Intersection*> p: graph->getIntersections()) {
        Intersection *intersection = p.second;
        QPoint location(intersection->getLocation().x * SCALE_FACTOR, intersection->getLocation().y * SCALE_FACTOR);
        painter.setPen(QPen(QColor(COLOR_BLACK.r, COLOR_BLACK.g, COLOR_BLACK.b))); // border colour
        painter.setBrush(QBrush(QColor(COLOR_BLACK.r, COLOR_BLACK.g, COLOR_BLACK.b))); // fill colour
        painter.drawEllipse(location, INTERSECTION_RADIUS, INTERSECTION_RADIUS);
    }

    char buffer[25];
    string s;

    // draws Lines to represent each road
    for(pair<int, RoadSegment*> p: graph->getRoadSegments()) {
        road = p.second;
        Point2D tempSource = road->getSource()->getLocation();
        Point2D tempDest = road->getDestination()->getLocation();
        theta = tempSource.angleTo(tempDest); // (-PI, PI]
        labelPosX = LABEL_DIST * cos(theta + PI / 2) + ((tempSource.x + tempDest.x) / 2) - (LABEL_WIDTH / 2);
        labelPosY = LABEL_DIST * sin(theta + PI / 2) + ((tempSource.y + tempDest.y) / 2) - (LABEL_HEIGHT / 2);
        adjX = (theta >= 0.0 ? -1.0 : 1.0) * ROAD_SEPARATION * abs(sin(theta));
        adjY = (abs(theta) >= PI / 2 ? -1.0 : 1.0) * ROAD_SEPARATION * abs(cos(theta));
        QPoint source(tempSource.x * SCALE_FACTOR + adjX, tempSource.y * SCALE_FACTOR + adjY);
        QPoint destination(tempDest.x * SCALE_FACTOR + adjX, tempDest.y * SCALE_FACTOR + adjY);

        // colours roads based on flow:capacity ratio
        percentage = (double) road->getFlow() / (double) road->getCapacity();
        if (abs(percentage) <= EPS) pen = new QPen(QColor(COLOR_BLACK.r, COLOR_BLACK.g, COLOR_BLACK.b));
        else if (percentage <= 0.33) pen = new QPen(QColor(COLOR_GREEN.r, COLOR_GREEN.g, COLOR_GREEN.b));
        else if (percentage <= 0.67) pen = new QPen(QColor(COLOR_YELLOW.r, COLOR_YELLOW.g, COLOR_YELLOW.b));
        else pen = new QPen(QColor(COLOR_RED.r, COLOR_RED.g, COLOR_RED.b));

        pen->setWidth(ROAD_WIDTH);
        painter.setPen(*pen);
        painter.drawLine(source, destination);

        // draws blue dots to represent cars
        for (pair<int, Car*> c : road->getCars()) {
            car = c.second;
            QPoint location(car->getCurrentLocation().x * SCALE_FACTOR + adjX, car->getCurrentLocation().y * SCALE_FACTOR + adjY);
            painter.setPen(QPen(QColor(COLOR_BLUE.r, COLOR_BLUE.g, COLOR_BLUE.b))); // border colour is changed here
            painter.setBrush(QBrush(QColor(COLOR_BLUE.r, COLOR_BLUE.g, COLOR_BLUE.b))); // fill colour is changed here
            painter.drawEllipse(location, CAR_RADIUS, CAR_RADIUS); // change the constant to change the radius, DO NOT change this value here
        }
        if (!labels.count(p.first)) labels[p.first] = new QLabel(this);
        labels[p.first]->setAlignment(Qt::AlignCenter);
        sprintf(buffer,"%.1f\n%d / %d",road->getSpeedLimit(), road->getFlow(), road->getCapacity());
        s = string(buffer);
        labels[p.first]->setText(QString::fromStdString(s));
        labels[p.first]->setGeometry(SCALE_FACTOR * labelPosX, SCALE_FACTOR * labelPosY, LABEL_WIDTH, LABEL_HEIGHT);
        delete pen;
    }
    painter.end();
    ui->picture->setPixmap(QPixmap::fromImage(image)); // paints components to the ui
}
