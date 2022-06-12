#ifndef WIENERNODER314_CITY_HPP
#define WIENERNODER314_CITY_HPP

#include <fstream>

#include "Pathfinder.hpp"

class City : Pathfinder {
public:
    City();

private:
    int getInput();
    void createLine(string lineStr);
    void connectSwitchingOptions();
    string extractLineName(string lineStr);
    int extractCost(string strArrayEntry);
    void searchLoop();

};


#endif //WIENERNODER314_CITY_HPP
