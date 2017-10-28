#include <string>
#include <cstdio>
#include <ctime>
#include "ConsoleDriver.h"

using namespace std;

int main() {
    srand(time(NULL));
    ConsoleDriver *cd = new ConsoleDriver(1, "data/fixedIntxnRoadsRandCarDemo.txt");
    cd->run();
}
