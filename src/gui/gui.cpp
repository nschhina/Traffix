#include <cmath>
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
#define ROAD_WIDTH 3
#define INTERSECTION_RADIUS 8 // the radius of the vertices of the graph

struct color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};
// colour constants for use in GUI
const color COLOR_BLACK = {0, 0, 0};
const color COLOR_WHITE = {255, 255, 255};
const color COLOR_RED = {255, 0, 0};
const color COLOR_YELLOW = {255, 255, 0};
const color COLOR_GREEN = {0, 255, 0};
const color COLOR_BLUE = {0, 0, 255};
WeightedDigraph *graph;
QImage image(SCREEN_WIDTH, SCREEN_HEIGHT, QImage::Format::Format_ARGB32);
/**
 * Constructs a
 * @param G The WeightedDigraph to be drawn to the screen
 * @param parent The parent Qt GUI componenet. SHould be a null pointer in all cases.
 */
GUI::GUI(WeightedDigraph *G, QWidget *parent) : QMainWindow(parent), ui(new Ui::GUI), image(800, 480, QImage::Format::Format_ARGB32) {
    ui->setupUi(this);
    this->graph = G;
    this->parent = parent;
    drawComponents();
}
/**
 * Deconstructs the GUI Object and the guis
 */
GUI::~GUI() {
    delete ui;
}
void GUI::renderImage() {
     ui->picture->setPixmap(QPixmap::fromImage(image));
}
/**
 * Draws the RoadSegments and Intersections to the Window
 */
void GUI::drawComponents() {
    QPainter painter(&image);
    QBrush brush;
    image.fill(QColor(COLOR_WHITE.r,COLOR_WHITE.g,COLOR_WHITE.b)); // background fill colour. can be changed as well
    // draws Ellipses to represent each intersection
    for(pair<int, Intersection*> p: graph->getIntersections()) {
        Intersection *intersection = p.second;
        QPoint location(intersection->getLocation().x * SCALE_FACTOR, intersection->getLocation().y * SCALE_FACTOR);
        painter.setPen(QPen(QColor(COLOR_BLACK.r, COLOR_BLACK.g, COLOR_BLACK.b))); // border colour is changed here
        painter.setBrush(QBrush(QColor(COLOR_BLACK.r, COLOR_BLACK.g, COLOR_BLACK.b))); // fill colour is changed here
        painter.drawEllipse(location, INTERSECTION_RADIUS, INTERSECTION_RADIUS); // change the constant to change the radius, DO NOT change this value here
    }
    // list of
    QList<QLabel*> speeds;
    for(int i = 0; i < graph->countIntersections(); i++) {
        speeds.insert(i, new QLabel(this));
    }
    char buffer[10];
    int labelIndex = 0;
    string s;
    // draws Lines to represent each road
    for(pair<int, RoadSegment*> p: graph->getRoadSegments()) {
        RoadSegment *road = p.second;
        Point2D tempSource = road->getSource()->getLocation();
        Point2D tempDest = road->getDestination()->getLocation();
        QPoint source(tempSource.x * SCALE_FACTOR, tempSource.y * SCALE_FACTOR);
        QPoint destination(tempDest.x * SCALE_FACTOR, tempDest.y * SCALE_FACTOR);
        if (source.rx() == destination.rx()) {
            int adj = source.ry() > destination.ry() ? 3 : -3;
            source.rx() += adj;
            destination.rx() += adj;
        } else if (source.ry() == destination.ry()) {
            int adj = source.rx() < destination.rx() ? 3 : -3;
            source.ry() += adj;
            destination.ry() += adj;
        }
        QPen *pen;
        double percentage = (double) road->getFlow() / (double) road->getCapacity();
        if (abs(percentage) <= EPS) pen = new QPen(QColor(COLOR_BLACK.r, COLOR_BLACK.g, COLOR_BLACK.b));
        else if (percentage <= 0.33) pen = new QPen(QColor(COLOR_GREEN.r, COLOR_GREEN.g, COLOR_GREEN.b));
        else if (percentage <= 0.67) pen = new QPen(QColor(COLOR_YELLOW.r, COLOR_YELLOW.g, COLOR_YELLOW.b));
        else pen = new QPen(QColor(COLOR_RED.r, COLOR_RED.g, COLOR_RED.b));
        pen->setWidth(ROAD_WIDTH);
        painter.setPen(*pen);
        painter.drawLine(source, destination);
        speeds.insert(labelIndex, new QLabel(this));
        //speeds.value(index)->setFrameStyle(QFrame::Panel | QFrame::Sunken);
        sprintf(buffer,"%.1f",road->getSpeedLimit());
        s = string(buffer);
        speeds.value(labelIndex)->setText(QString::fromStdString(s));
        //speeds.value(index)->setAlignment(Qt::AlignBottom | Qt::AlignRight);
        speeds.value(labelIndex)->setGeometry(SCALE_FACTOR * (tempSource.x + tempDest.x) / 2,SCALE_FACTOR * (tempSource.y + tempDest.y) / 2,50,10);
        labelIndex++;
    }
    painter.end();
    ui->picture->setPixmap(QPixmap::fromImage(image)); // paints components to the ui
}
