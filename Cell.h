#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <random>
#include <iostream>

using namespace std;

enum Types {INDUSTRIAL = 'I', COMMERCIAL = 'C', RESIDENTIAL = 'R', POWERLINE = 'T', ROAD = '-', POWERLINE_ROAD = '#', POWERPLANT = 'P', NONE = ' '};
enum Mayors {CONSERVATIVE = 0, PROGRESSIVE = 1};

// struct will be used for all cells in the gameboard
struct Cell{
    char type;         // uses enum to translate
    int population;     // cell's population
    int pollution;      // cell's pollution (max of 9)
    bool hasPower;      // true if cell is connected to a power source
    Cell(char type);    // default constructor
};

// returns true if current coordinates are out of bounds
bool outOfBounds(int r, int c, int MAX_ROW, int MAX_COL);   

// prompts for config file, reads in region and updates gameboard
void initializeRegion(vector<vector<Cell>> &gameboard);

// prints current state of region
void printRegion(vector<vector<Cell>> gameboard, int workers, int goods);

// checks if current cell are connected to power; updates hasPower member boolean
void checkPower(vector<vector<Cell>> &gameboard, int row, int col);  

// begins process of region growth
void beginGrowth(vector<vector<Cell>> &gameboard, int timeLimit, int refreshRate);

// is called from main; grows passed cell, if possible
void growCell(vector<vector<Cell>> &gameboard, int row, int col, int &workers, int &goods);

// is called when there are only 2 workers during growth function
void growCellPriority(vector<vector<Cell>> &gameboard, int row, int col, int &workers, int &goods, int mayor);

// returns total population of adjacent cells
int getAdjacentPopulation(vector<vector<Cell>> gameboard, int row, int col);

// begins process of pollution spread
void beginPollution(vector<vector<Cell>> & gameboard);

// helper function for pollution spread
int distance(int thisRow, int thisCol, int thatRow, int thatCol);

// spreads pollution from passed cell, if possible
void pollutionSpreadCell(vector<vector<Cell>> &gameboard, int row, int col);

// is called as frequently as the refresh rate; elects mayor based on popular vote; returns 0 for conservative, 1 for progressive, -1 for tie
int election(vector<vector<Cell>> gameboard, int mayor, int industrialPopulation, int totalPopulation);

// outputs the final region state; prompts user for zone analysis
void finalOutput(vector<vector<Cell>> gameboard);