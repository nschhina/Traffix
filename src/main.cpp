#include <string>
#include <cstdio>
#include <ctime>
#include "GUIDriver.h"

using namespace std;

int main(int argc, char *argv[]) {
    srand(time(NULL));
    GUIDriver *gd = new GUIDriver(argc, argv, 20, ":/data/diagonalGridDemo.txt", 1);
    // GUIDriver *gd = new GUIDriver(argc, argv, 20, ":/data/gridDemo.txt", 1);
    gd->run();
    return 0;
}
