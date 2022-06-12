#ifndef WIENERNODER314_PATHFINDER_HPP
#define WIENERNODER314_PATHFINDER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <unordered_map>

using namespace std;

typedef struct Station {
    std::string name;
    int cost;

    Station(string _name, int _cost) {
        name = _name;
        cost = _cost;
    }
}Station;

typedef struct Line {
    string line;
    vector<Station*> Stations;
    unordered_map<string, Line*> SwitchingOptions;
    vector<string> MapLines;

    Line(string _line) {
        line = _line;
    }
}Line;

typedef struct TravellingOption {
    Line* line;
    string* start;
    string* end;
    int sumCost = 0;
    TravellingOption* next = nullptr;

    vector<int> travelingCosts;
    vector<string> travelingStations;

    TravellingOption(Line* _line, string* _start) {
        line = _line;
        start = _start;
    }

    TravellingOption(Line* _line, string* _start, string* _end) {
        line = _line;
        start = _start;
        end = _end;
    }
}TravellingOption;

class Pathfinder {
public:
    unordered_map<string,Line*> LinesByName;
    vector<Line*> LinesByIndex;
    unordered_map<string, vector<string>> SwitchingStations;

    void searchPath(string* first, string* last);

private:
    vector<TravellingOption*> results;

    void noLineSwitching(string* first, string* last, string* line);
    void withLineSwitching(string* first, string* last, vector<string> firstLines, vector<string> lastLines);

    void initiateSearch(vector<string>* travelledLines, vector<string>* switchingOptions, vector<string>* firstLines, vector<string>* lastLines, int iterations);
    bool isConnected(Line* startLine, Line* endLine);
    bool recLineSearch(vector<string>* travelledLines, vector<string>* switchingOptions, vector<string>* actualLines, string searchLine, int deepness);

    void createTravellingOption(string* first, string* last, vector<string>* switchingOptions,vector<string>* firstLines);
    TravellingOption* calcTravellingOption(int pos, string* first, string* last, string* switchingLine, Line* line,vector<string>* switchingArray);

    static bool compareTravellingOptions(TravellingOption* first, TravellingOption* second);
    int calcTravellingCosts(TravellingOption* travellingOption);

    string getLastStation(string* line, vector<Station*>* stations);

    void printResult();
    int printTravellingOption(TravellingOption* travellingOption, int pos);
    string printLineColour(string line);
};


#endif //WIENERNODER314_PATHFINDER_HPP
