#include "Cell.h"

int main()
{
    std::cout << "Welcome to SimCity Version 2.0, now with dynamic growth rates and mayoral elections!" << endl;
    vector<vector<Cell>> gameboard;     // will store the game state during the simulation
    initializeRegion(gameboard);
    return 0;
}