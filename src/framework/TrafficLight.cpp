#include "TrafficLight.h"

int TrafficLight::counter = 0; // counter starts at 0

/**
 * Initializes a traffic light with a default state of RED.
 * @param type the type of turn this traffic light controls
 */
TrafficLight::TrafficLight(int type) {
    this->id = counter++; // assigns an id and increments the counter
    this->state = type == RIGHT ? GREEN : RED; // default state
    this->type = type;
}

/**
 * Deconstructs the traffic light.
 */
TrafficLight::~TrafficLight() {}

/**
 * Returns the unique ID of this traffic light.
 */
int TrafficLight::getID() const { return id; }

/**
 * Returns the current state of this traffic light.
 * @return 0 if red, 1 if green, 2 if yellow
 */
int TrafficLight::getState() const { return state; }

/**
 * Changes the current state of the traffic light.
 * @param state the new state of the traffic light (0 for red, 1 for green, 2 for yellow)
 */
void TrafficLight::setState(int state) {
    this->state = state;
}

/**
 * Returns the type of turn that this traffic light controls.
 * @return 0 if left turn, 1 if straight, 2 if right turn
 */
int TrafficLight::getType() const { return type; }

/**
 * Compares this TrafficLight to TrafficLight t by comparing their ids.
 *
 * @param  t the other traffic light
 * @return true if and only if this traffic light equals the other, false otherwise
 */
bool TrafficLight::operator == (const TrafficLight &t) const { return id == t.id; }

/**
 * Compares this TrafficLight to TrafficLight t by comparing their ids.
 *
 * @param  t the other traffic light
 * @return true if and only if this traffic light does not equals the other, false otherwise
 */
bool TrafficLight::operator != (const TrafficLight &t) const { return id != t.id; }
