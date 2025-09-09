#include "Cell.h"

Cell::Cell(char type)
{
    this->type = type;
    this->population = 0;
    this->pollution = 0;
    this->hasPower = false;
}

// returns true if passed coordinates are out of bounds
bool outOfBounds(int r, int c, int MAX_ROW, int MAX_COL)
{
    return (r < 0 || r >= MAX_ROW || c < 0 || c >= MAX_COL);
}

// prompts for config file, reads in region and updates gameboard
void initializeRegion(vector<vector<Cell>> &gameboard)
{
    int timeLimit, refreshRate;         // will store maximum number of rounds for the simulation and refresh rate
    string configFile, regionFile;      // will store file names for simulation initialization
    string tempString;                  // used for parsing data in file stream
    std::cout << "Enter name of configuration file: ";
    std::cin >> configFile;
    ifstream fin;
    try
    {
        fin.open(configFile);           // open configuration file
        if(fin.fail())
            throw ios_base::failure("Error opening configuration file");
    }
    catch(const exception& e)
    {
        std::cout << e.what() << endl;
        exit(EXIT_FAILURE);
    }
    getline(fin, tempString, ':');                  // go until semi colon, store in temp string
    getline(fin, regionFile);                       // store region file name
    std::cout << "Region file: " << regionFile << endl;
    getline(fin, tempString, ':');                  // go until semi colon, store in temp string
    fin >> timeLimit;                               // store time limit
    getline(fin, tempString, ':');                  // go until semi colon, store in temp string
    fin >> refreshRate;                             // store refresh rate
    fin.close();
    try
    {
        fin.open(regionFile);                       // open region file
        if(fin.fail())
            throw ios_base::failure("Error opening region file");
    }
    catch(const exception& e)
    {
        std::cout << e.what() << endl;
        exit(EXIT_FAILURE);
    }
    int row = 0;
    while(!fin.eof())
    {
        getline(fin, tempString);                   // get line from region file
        gameboard.push_back(vector<Cell>());        // add new row to gameboard vector
        for(char& c : tempString)                   // parse new line char by char
        {
            if(c != ',')                            // ignore commas
            {
                gameboard[row].push_back(Cell(c));  // add char to gameboard in correct coords
            }
        }
        row++;                                      // increment to next column
    }
    // initialize power member booleans
    for(int r=0; r<gameboard.size(); r++)
    {
        for(int c=0; c<gameboard[r].size(); c++)
        {
            checkPower(gameboard, r, c);
        }
    }
    std::cout << "Initializing Region..." << endl;
    printRegion(gameboard, 0, 0);
    beginGrowth(gameboard, timeLimit, refreshRate);   
}

// prints current state of regio
void printRegion(vector<vector<Cell>> gameboard, int workers, int goods)
{
    for(auto& row:gameboard)            // iterate over gameboard
    {
        for(Cell& c : row)
        {
            std::cout << c.type;      // print type of Cell
            if(c.population > 0)
            {
                std::cout << c.population;
            }
            if(c.pollution > 0)
            {
                std::cout << "(" << c.pollution << ")";
            }
            std::cout << '\t';
        }
        std::cout << endl;
    }
}

// checks if current cell are connected to power; updates hasPower member boolean
void checkPower(vector<vector<Cell>> &gameboard, int row, int col)
{
    // iterate over adjacent cells
    for(int r = row-1; r <= row+1; r++){
        for(int c = col-1; c <= col+1; c++){
            // check if current coords are valid
            if(outOfBounds(r, c, gameboard.size()-1, gameboard[row].size()-1))
                continue;
            // check if current cell is power
            if(gameboard[r][c].type == POWERLINE || gameboard[r][c].type == POWERLINE_ROAD)
            {
                gameboard[row][col].hasPower = true;    // assign member power boolean to true
                return;     // exit function
            }
        }
    }
    gameboard[row][col].hasPower = false;
}

// begins process of region growth
void beginGrowth(vector<vector<Cell>> &gameboard, int timeLimit, int refreshRate)
{
    int totalPopulation = 0;
    int industrialPopulation = 0;
    int totalPrevPopulation = 0;
    int currCellPop;
    int round = 0;
    int workers = 0, goods = 3;     // simulation begins with several goods to allow for equal chance of commercial vs industrial growth
    srand(time(NULL));
    int mayor = -1;     // simulation begins without influence of a mayor
    int electionFreq = 1;
    std::cout << "Enter frequency of elections (elections occur every n rounds): " << endl;
    try{
        std::cin >> electionFreq;
        if(std::cin.fail())
        {
            throw ios_base::failure("Invalid input type");
        }
    }catch(exception & e)
    {
        std::cout << e.what();
        exit(EXIT_FAILURE);
    }
    do{
        std::cout << "ROUND " << round+1 << endl;
        for(int r=0; r<gameboard.size(); r++)
        {
            for(int c=0; c<gameboard[r].size(); c++)
            {
                currCellPop = gameboard[r][c].population;
                if(gameboard[r][c].type == INDUSTRIAL && mayor == PROGRESSIVE)    // check if commercial cell can grow instead
                {
                    growCellPriority(gameboard, r, c, workers, goods, mayor);
                }
                else if(gameboard[r][c].type == COMMERCIAL && mayor == CONSERVATIVE)    // checl if industrial cell can grow instead
                {
                    growCellPriority(gameboard, r, c, workers, goods, mayor);
                }
                else
                {
                    growCell(gameboard, r, c, workers, goods);
                }
                if(gameboard[r][c].population > currCellPop)
                {
                    totalPopulation++;
                    if(gameboard[r][c].type == INDUSTRIAL)
                    {
                        industrialPopulation++;
                    }
                }
            }
        }
        if(round % refreshRate == 0)
        {
            printRegion(gameboard, workers, goods);
        }
        if(round % electionFreq == 0)
        {
            do
            {
                mayor = election(gameboard, mayor, industrialPopulation, totalPopulation);
            } while (mayor == -2);
        }
        std::cout << "WORKERS: " << workers << endl;
        std::cout << "GOODS:   " << goods << endl;
        std::cout << "MAYOR: ";
        switch(mayor)
        {
            case CONSERVATIVE:
                std::cout << "Conservative" << endl;
                break;
            case PROGRESSIVE:
                std::cout << "Progressive" << endl;
                break;
        }
        std::cout << endl << endl;
        round++;
        if(totalPopulation == totalPrevPopulation)    // if there is no difference between rounds
        {
            break;
        }
        totalPrevPopulation = totalPopulation;
    }while(round < timeLimit);

    std::cout << "GROWTH PERIOD ENDED. BEGINNING POLLUTION SPREAD" << endl << endl;

    beginPollution(gameboard);
    printRegion(gameboard, workers, goods);

    finalOutput(gameboard);
}

// is called from main; grows passed cell, if possible
void growCell(vector<vector<Cell>> &gameboard, int row, int col, int &workers, int &goods)
{
    int numAdjacentCells = 0;     // stores number of adjacent cells that meet the required population
    int reqAdjacentPop;     // stores the required population of adjacent cells for growth
    int reqNumAdjacentCells;    // stores the required number of adjacent cells for growth
    int maxPop;     // stores the maximum population of the cell type
    int pop = gameboard[row][col].population;   // stores population of current cell
    char cellType = gameboard[row][col].type;   // stores type of cell
    switch(cellType)    // check exit conditions
    {
        case COMMERCIAL:
            if(goods < 1)   // if not enough goods
                return;     // no cell growth
            if(workers < 2) // if not enough workers
                return;     // no cell growth
            maxPop = 2;
            break;
        case INDUSTRIAL:
            if(workers < 2) // if not enough workers
                return;     // no cell growth
            maxPop = 3;
            break;
        case RESIDENTIAL:
            maxPop = 5;
            break;
        default:
            return;
    }
    // adjacent cells for growth reqs
    reqAdjacentPop = pop;
    reqNumAdjacentCells = reqAdjacentPop*2;

    if(pop == 0)    // special case of 0; check if cell has power
    {
        reqAdjacentPop = 1;
        reqNumAdjacentCells = 1;
        if(gameboard[row][col].hasPower)    // if cell has power, ignore cell growth reqs
        {
            gameboard[row][col].population++;
            if(cellType == INDUSTRIAL || cellType == COMMERCIAL)
                workers--;
            if(cellType == COMMERCIAL)
                goods--;
            return;
        }
    }

    if(pop >= maxPop)   // if population is at maximum
        return;     // no cell growth

    // iterate over adjacent cells
    for(int r = row-1; r <= row+1; r++){
        for(int c = col-1; c <= col+1; c++){
            // check if current coords are valid
            if(outOfBounds(r, c, gameboard.size()-1, gameboard[row].size()-1))
                continue;
            // check adjacent cells for enough population
            else if(gameboard[r][c].population >= reqAdjacentPop)
            {
                numAdjacentCells++;   // increment adjacent cell count
            }
        }
    }
    if(numAdjacentCells < reqNumAdjacentCells)     // not enough adjacent cells with pop req
    {
        return;     // no cell growth
    }
    gameboard[row][col].population++;   // increment population
    if(cellType == INDUSTRIAL)
    {
        workers --;     // assign 1 worker to job
        goods++;        // produce 1 good
    }
    if(cellType == COMMERCIAL)
    {
        workers--;      // assign 1 worker to job
        goods--;        // assign 1 good to job
    }
    if(cellType == RESIDENTIAL)
        workers ++;     // increase workers
}

// is called when there are only 2 workers during growth function
void growCellPriority(vector<vector<Cell>> &gameboard, int row, int col, int &workers, int &goods, int mayor)
{
    vector<pair<char,pair<int,int>>> coords;     // stores coordinates of each population cell type
    bool firstIteration = true;

    char priorityType;      // initialize priority type based on mayor
    if(mayor == PROGRESSIVE)
    {
        priorityType = COMMERCIAL;
    }
    else if(mayor == CONSERVATIVE)
    {
        priorityType = INDUSTRIAL;
    }

    for(int r = row; r < gameboard.size(); r++)     // iterate over remaining cells
    {
        for(int c = 0; c < gameboard[r].size(); c++)
        {
            if(firstIteration)
            {
                c = col;
                firstIteration = false;
            }
            if(gameboard[r][c].type == COMMERCIAL || gameboard[r][c].type == INDUSTRIAL)      // add worker cells to vector
            {
                coords.push_back(make_pair(gameboard[r][c].type,make_pair(r,c)));
            }
        }
    }
    // check for priority type cell; if exists, grow cell
    for(auto & i : coords)
    {
        if(i.first == priorityType && workers >= 1)
        {
            growCell(gameboard, i.second.first, i.second.second, workers, goods);
            return;
        }
    }
    // check for cell with max population
    int maxPop = 0;
    vector<pair<int,int>> maxPopCoords;
    for(auto & i : coords)
    {
        int currPop = gameboard[i.second.first][i.second.second].population;
        if(currPop > maxPop)
        {
            maxPop = currPop;
            maxPopCoords.erase(maxPopCoords.begin(), maxPopCoords.end());
            maxPopCoords.push_back(make_pair(i.second.first, i.second.second));
        }
        else if(currPop == maxPop)
        {
            maxPopCoords.push_back(make_pair(i.second.first, i.second.second));
        }
    }
    if(maxPopCoords.size() == 1)
    {
        growCell(gameboard, maxPopCoords[0].first, maxPopCoords[0].second, workers, goods);
        return;
    }
    // check for biggest total adjacent population
    maxPop = 0;
    vector<int> maxPopIdxs;
    for(int i=0; i<maxPopCoords.size(); i++)
    {
        int currPop = getAdjacentPopulation(gameboard, maxPopCoords[i].first, maxPopCoords[i].second);
        if(currPop > maxPop)
        {
            maxPop = currPop;
            maxPopIdxs.erase(maxPopIdxs.begin(), maxPopIdxs.end());
            maxPopIdxs.push_back(i);
        }
        else if(currPop == maxPop)
        {
            maxPopIdxs.push_back(i);
        }
    }
    if(maxPopIdxs.size() == 1)
    {
        growCell(gameboard, maxPopCoords[maxPopIdxs[0]].first, maxPopCoords[maxPopIdxs[0]].second, workers, goods);
        return;
    }
    // check for smallest row (Y value)
    vector<int> minRowIdxs;
    int minRow = maxPopCoords[maxPopIdxs[0]].first;
    for(int & i : maxPopIdxs)
    {
        int currRow = maxPopCoords[i].first;
        if(currRow < minRow)
        {
            minRow = currRow;
            minRowIdxs.erase(minRowIdxs.begin(), minRowIdxs.end());
            minRowIdxs.push_back(i);
        }
        else if(currRow == minRow)
        {
            minRowIdxs.push_back(i);
        }
    }
    if(minRowIdxs.size() == 1)
    {
        growCell(gameboard, maxPopCoords[minRowIdxs[0]].first, maxPopCoords[minRowIdxs[0]].second, workers, goods);
        return;
    }
    // check for smallest col (X value)
    vector<int> minColIdxs;
    int minCol = maxPopCoords[minRowIdxs[0]].second;
    for(int & i : maxPopIdxs)
    {
        int currCol = maxPopCoords[i].second;
        if(currCol < minCol)
        {
            minCol = currCol;
            minColIdxs.erase(minColIdxs.begin(), minColIdxs.end());
            minColIdxs.push_back(i);
        }
        else if(currCol == minCol)
        {
            minColIdxs.push_back(i);
        }
    }
    growCell(gameboard, maxPopCoords[minColIdxs[0]].first, maxPopCoords[minColIdxs[0]].second, workers, goods);
}

// returns total population of adjacent cells
int getAdjacentPopulation(vector<vector<Cell>> gameboard, int row, int col)
{
    int result = 0;
    // iterate over adjacent cells
    for(int r = row-1; r <= row+1; r++){
        for(int c = col-1; c <= col+1; c++){
            // check if current coords are valid
            if(outOfBounds(r, c, gameboard.size()-1, gameboard[row].size()-1))
                continue;
            // check if current cell is power
            result += gameboard[r][c].population;
        }
    }
    return result;
}

// begins process of pollution spread
void beginPollution(vector<vector<Cell>> &gameboard)
{
    for(int r=0; r<gameboard.size(); r++)
    {
        for(int c=0; c<gameboard[r].size(); c++)
        {
            pollutionSpreadCell(gameboard, r, c);
        }
    }
}

// helper function for pollution spread
int distance(int thisRow, int thisCol, int thatRow, int thatCol)
{
    if(thisCol == thatCol) //same column
		return abs(thisRow - thatRow);
	if(thisRow == thatRow) //same row
		return abs(thisCol - thatCol);
	if(abs(thisRow - thatRow) == 1 && abs(thisCol - thatCol) == 1)  //diagonal case
		return 2;
	return 3; //cells are too far apart
}

// spreads pollution from passed cell, if possible
void pollutionSpreadCell(vector<vector<Cell>> &gameboard, int row, int col)
{
    // check if current cell is industrial
    if(gameboard[row][col].type != INDUSTRIAL)
        return;
    // iterate over adjacent cells
    for(int r = row-2; r <= row+2; r++)
    {
        for(int c = col-2; c <= col+2; c++)
        {  
            // check if current index is out of bounds
            if(outOfBounds(r, c, gameboard.size()-1, gameboard[r].size()-1))
                continue;
            // check if pollution can spread to current cell
            if(gameboard[row][col].population <= distance(r,c,row,col))
                continue;
            gameboard[r][c].pollution += (gameboard[row][col].population - distance(r,c,row,col));  // add pollution proportional to distance
        }
    }
}

int election(vector<vector<Cell>> gameboard, int mayor, int industrialPopulation, int totalPopulation)
{
    float pollutionRatio = 1.0 * industrialPopulation / (totalPopulation-industrialPopulation); // the higher this ratio, the higher the likelihood of a progressive vote
    int progressiveVotes = 0, conservativeVotes = 0;
    for(int r=0; r<gameboard.size(); r++)
    {
        for(int c=0; c<gameboard[r].size(); c++)
        {
            srand(time(NULL));
            float pollutionRatioComparator = ((float)(rand()) / (float)(RAND_MAX));
            if(gameboard[r][c].type == INDUSTRIAL)
            {
                for(int i=0; i<gameboard[r][c].population; i++)
                {
                    int randNum = rand() % 3;
                    if(randNum == 0)
                    {
                        progressiveVotes++;
                    }
                    else
                    {
                        if(pollutionRatioComparator <= pollutionRatio)
                        {
                            progressiveVotes++;
                        }
                        else
                        {
                            conservativeVotes++;
                        }
                    }
                }
            }
            else if(gameboard[r][c].type == COMMERCIAL)
            {
                for(int i=0; i<gameboard[r][c].population; i++)
                {
                    int randNum = rand() % 3;
                    if(randNum == 0)
                    {
                        if(pollutionRatioComparator <= pollutionRatio)
                        {
                            progressiveVotes++;
                        }
                        else
                        {
                            conservativeVotes++;
                        }
                    }
                    else
                    {
                        progressiveVotes++;
                    }
                }
            }
        }
    }
    std::cout << "PROGRESSIVE VOTES: " << progressiveVotes << endl;
    std::cout << "CONSERVATIVE VOTES: " << conservativeVotes << endl;
    if(progressiveVotes == conservativeVotes && progressiveVotes == 0)
    {
        std::cout << "No votes. Election postponed" << endl;
        return mayor;
    }
    else if(progressiveVotes == conservativeVotes && progressiveVotes != 0)
    {
        std::cout << "Tie. Call for another election" << endl;
        return -2;  // tie. call another election
    }
    else if(progressiveVotes > conservativeVotes)
    {
        std::cout << "Progressive mayor elected" << endl;
        return 1;   // progressive mayor
    }
    std::cout << "Conservative mayor elected" << endl;
    return 0;   // conservative mayor

}

// outputs the final region state; prompts user for zone analysis
void finalOutput(vector<vector<Cell>> gameboard)
{
    int residentialPop = 0, industrialPop = 0, commercialPop = 0, pollution = 0;
    bool running = true;
    char choice;
    int rowMin = 0, rowMax = gameboard.size()-1;
    int colMin = 0, colMax = gameboard[0].size()-1;
    do{
        for(int r=rowMin; r<rowMax; r++)
        {
            for(int c=colMin; c<colMax; c++)
            {
                pollution += gameboard[r][c].pollution;
                if(gameboard[r][c].type == RESIDENTIAL)
                    residentialPop += gameboard[r][c].population;
                if(gameboard[r][c].type == INDUSTRIAL)
                    industrialPop += gameboard[r][c].population;
                if(gameboard[r][c].type == COMMERCIAL)
                    commercialPop += gameboard[r][c].population;
            }
        }
        std::cout << "RESIDENTIAL POPULATION: - " << residentialPop << endl;
        std::cout << "INDUSTRIAL POPULATION: -- " << industrialPop << endl;
        std::cout << "COMMERCIAL POPULATION: -- " << commercialPop << endl;
        std::cout << "POLLUTION: -------------- " << pollution << endl;
        bool prompting;
        do{
            prompting = false;
            std::cout << endl << "Analyze a specific region? (y/n): ";
            std::cin >> choice;
            if(choice == 'n')
            {
                exit(EXIT_SUCCESS);
            }
            if(choice != 'y')
            {
                std::cout << "Error, invalid input." << endl;
                prompting = true;
                continue;
            }
            residentialPop = 0;
            industrialPop = 0;
            commercialPop = 0;
            pollution = 0;
            std::cout << "Enter lower bound for ROW: ";
            std::cin >> rowMin;
            if(rowMin < 0 || rowMin >= gameboard.size())
            {
                std::cout << "Error. Out of bounds." << endl;
                prompting = true;
                continue;
            }
            std::cout << "Enter upper bound for ROW: ";
            std::cin >> rowMax;
            if(rowMax < 0 || rowMax >= gameboard.size())
            {
                std::cout << "Error. Out of bounds." << endl;
                prompting = true;
                continue;
            }
            std::cout << "Enter lower bound for COL: ";
            std::cin >> colMin;
            if(colMin < 0 || colMin >= gameboard[0].size())
            {
                std::cout << "Error. Out of bounds." << endl;
                prompting = true;
                continue;
            }
            std::cout << "Enter upper bound for COL: ";
            std::cin >> colMax;
            if(colMax < 0 || colMax >= gameboard[0].size())
            {
                std::cout << "Error. Out of bounds." << endl;
                prompting = true;
                continue;
            }
            std::cout << endl;
        }while(prompting);
    }while(running);
}