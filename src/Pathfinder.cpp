#include "globals.h"
#include "Pathfinder.hpp"

// firstLines: ["U1", "U4", "1", "2"] (all lines from input-Station-start )
// lastLines: ["U1", "U2", "U4", "1"] (all lines from input-Station-end)
// check in double-for-loop, if there is a direct connection (U1->U1, U4->U4, 1->1) = isOneLiner
// yes: noLineSwitching()
// no: withLineSwitching()
// end: print result and delete all results afterwards
void Pathfinder::searchPath(string* first, string* last) {
    vector<string> firstLines = SwitchingStations.at(*first);
    vector<string> lastLines = SwitchingStations.at(*last);

    bool isOneLiner = false;

    for (int i = 0; i < firstLines.size(); ++i) {
        for (int j = 0; j < lastLines.size(); ++j) {
            if (firstLines.at(i) == lastLines.at(j)) {
                isOneLiner = true;
                noLineSwitching(first, last, &firstLines.at(i));
            }
        }
    }
    if (!isOneLiner) {
        withLineSwitching(first, last, firstLines, lastLines);
    } else {
        cout << "direct connection found!\n";
    }
    printResult();
    results = vector<TravellingOption*>();
}

// go through all Stations on one line and check whether it's the first or last station
// the first time its true, it switches [isTraveling] to [true]. the second time, to [false]
//
//    false   |      true       |   false
// s - s - s - f - s - s - s - l - s - s - s
// s - s - s - l - s - s - s - f - s - s - s
//
// while true: add this Stations name and cost to travelingStations and travelingCosts
// end: add new TravellingOption to results-vector
void Pathfinder::noLineSwitching(string* first, string* last, string* line) {
    vector<Station*> travelingLine = LinesByName.at(*line)->Stations;
    bool isTraveling = false;
    TravellingOption* newTravellingOption = new TravellingOption(LinesByName.at(*line), first, last);

    for (int i = 0; i < travelingLine.size(); ++i) {
        if (travelingLine.at(i)->name == *first || travelingLine.at(i)->name == *last) {
            if (!isTraveling) { isTraveling = true; }
            else { break; }
        }
        if (isTraveling) {
            newTravellingOption->travelingCosts.push_back(travelingLine.at(i)->cost);
            newTravellingOption->travelingStations.push_back(travelingLine.at(i)->name);
        }
    }
    results.push_back(newTravellingOption);
}

// recursive-multi-state-function by Domain314
// deepness sets how deep it has to search for connections, in infinite search-tree. it starts for instance with 2:
// if deepness not 0, it calls itself in if-statement (return true/false), but with deepness-1
// if deepness is 0, it adds the line it got to, to travelledLines-vector
// and if it got to the searched line (*U3 == *U3), it adds a string with the line-name to switchingOptions-vector, at the end of it and return true
// the "return true" ends in the if statement (from deepness != 0) -> it adds "<" + line-Name at witch it is on this level, in the search-tree. and returns also true
// => each "true" at the deepest point (deepness == 0), triggers all predecessors if-statements, which add a piece to the connection-strings, in switching-Options:
// ["U3<U2<U4"], ["U3<6<71"], [...], ...
// "U3" from deepness == 0, "<U2" from deepness == 1, "<U4" from deepness == 2
bool Pathfinder::recLineSearch(vector<string>* travelledLines, vector<string>* switchingOptions, vector<string>* actualLines, string searchLine, int deepness) {
    bool isCon = false;
    for (int i = 0; i < actualLines->size(); ++i) {
        if (deepness == 0) {
            travelledLines->push_back(actualLines->at(i));
            if (LinesByName.at(actualLines->at(i)) == LinesByName.at(searchLine)) {
                switchingOptions->push_back(searchLine);
                isCon = true;
            }
        } else {
            if (recLineSearch(travelledLines, switchingOptions, &LinesByName.at(actualLines->at(i))->MapLines, searchLine, deepness-1)) {
                switchingOptions->at(switchingOptions->size()-1) += "<" + actualLines->at(i);
                return true;
            }
        }
    }
    return isCon;
}

// initialize search for each Line at destination-Station
void Pathfinder::initiateSearch(vector<string>* travelledLines, vector<string>* switchingOptions, vector<string>* firstLines, vector<string>* lastLines, int iterations) {
    for (int j = 0; j < lastLines->size(); ++j) {
        if (recLineSearch(travelledLines, switchingOptions, firstLines, lastLines->at(j), iterations)) {
            cout << "\nfound connection to " << lastLines->at(j);
        }
    }
}

// search for destination Station of this line
string Pathfinder::getLastStation(string* line, vector<Station*>* stations) {
    string _line = *line;
    for (int i = 0; i < stations->size(); ++i) {
        vector<string>* switchingOptions = &SwitchingStations.at(stations->at(i)->name);
        if (std::find(switchingOptions->begin(), switchingOptions->end(), *line) != switchingOptions->end()) {
            return stations->at(i)->name;
        }
    }
    return "-";
}

// create new TravellingOption and check its pos in switchingArray => isLastLine
// foreach station in this line, check if it's the destination station or first station.
// same, like noLineSwitching, if first/last station => add name and costs to travellingStations- and travellingCosts-vector
// additionally: if destination station, set travellingStation->end to its name.
// after loop: if first station is not travelingStations[0], then it was added backwards => reverse both, travelingStations and travelingCosts
// foreach cost in travelingCosts, add up, to sumCost
// if !isLastLine => create recursively new TravellingOption and set it as this->next
// end: return this
TravellingOption* Pathfinder::calcTravellingOption(int pos, string* first, string* last, Line* actualLine, vector<string>* switchingArray) {
    bool isTravelling = false;
    bool isLastLine = pos == switchingArray->size()-1;
    string _switchingLine = isLastLine ? *last : getLastStation(&switchingArray->at(pos+1), &actualLine->Stations);

    TravellingOption* newTravellingOption = new TravellingOption (actualLine, first);

    for (int i = 0; i < actualLine->Stations.size(); ++i) {
        string actualStationName = actualLine->Stations.at(i)->name;
        bool isSwitchingStation = actualStationName == _switchingLine;

        if (actualLine->Stations.at(i)->name == *first || actualLine->Stations.at(i)->name == _switchingLine) {
            if (isSwitchingStation) {
                newTravellingOption->end = &actualLine->Stations.at(i)->name;
            }

            if (!isTravelling) { isTravelling = true; }
            else {
                newTravellingOption->travelingCosts.push_back(actualLine->Stations.at(i)->cost);
                newTravellingOption->travelingStations.push_back(actualLine->Stations.at(i)->name);
                break;
            }
        }

        if (isTravelling) {
            newTravellingOption->travelingCosts.push_back(actualLine->Stations.at(i)->cost);
            newTravellingOption->travelingStations.push_back(actualLine->Stations.at(i)->name);
        }
    }

    if (*first != newTravellingOption->travelingStations.at(0)) {
        reverse(newTravellingOption->travelingCosts.begin(), newTravellingOption->travelingCosts.end());
        reverse(newTravellingOption->travelingStations.begin(), newTravellingOption->travelingStations.end());
    }

    for (int i = 0; i < newTravellingOption->travelingCosts.size()-1; ++i) {
        newTravellingOption->sumCost += newTravellingOption->travelingCosts.at(i);
    }

    if (pos != switchingArray->size()-1) {
        newTravellingOption->next = calcTravellingOption(pos+1, newTravellingOption->end, last, LinesByName.at(switchingArray->at(pos+1)), switchingArray);
    }
    return newTravellingOption;

}

// add up all costs of 2 TravellingOptions and all their ->nexts
// return compare both sums
bool Pathfinder::compareTravellingOptions(TravellingOption* first, TravellingOption* second) {
    int sumFirst = 0, sumSecond = 0, amountFirst = 0, amountSecond = 0;
    TravellingOption* _a = first;
    TravellingOption*_b = second;
    while (true) {
       sumFirst += _a->sumCost;
       amountFirst++;
       if (_a->next == nullptr) { break; }
       else { _a = _a->next; }
    }
    sumFirst += 5*amountFirst;
    while (true) {
        sumSecond += _b->sumCost;
        amountSecond++;
        if (_b->next == nullptr) { break; }
        else { _b = _b->next; }
    }
    sumSecond += 5*amountSecond;

    return sumFirst < sumSecond;
}

// add all costs of a TravellingOption and return it
int Pathfinder::calcTravellingCosts(TravellingOption* travellingOption) {
    int sum = 0, amount = 0;
    TravellingOption* _a = travellingOption;
    while (true) {
        sum += _a->sumCost;
        amount++;
        if (_a->next == nullptr) { break; }
        else { _a = _a->next; }
    }
    sum += 5*amount;
    return sum;
}

// create foreach switchingOption ("U1<U2<U4"), a chain of TravellingOptions
// first split switchingOption in lines, using "<" as seperator
// reverse output, if it was found from destination-perspective
// add all lines to a string and check if this found way is already in linesCalculated. yes: ignore, no: add it, initialize recursive TravellingOption-Creation
// add newly created TravellingOption-chain to results-vector.
// end: sort results, based on their costs sum.
void Pathfinder::createTravellingOption(string* first, string* last, vector<string>* switchingOptions, vector<string>* firstLines) {
    vector<string> linesCalculated;
    for (int i = 0; i < switchingOptions->size(); ++i) {
        stringstream ss(switchingOptions->at(i));
        string temp;
        vector<string> switchingArray;

        while (getline(ss, temp, '<')) {
            switchingArray.push_back(temp);
        };
        if (find(firstLines->begin(), firstLines->end(), switchingArray.at(0)) == firstLines->end()) {
            std::reverse(switchingArray.begin(), switchingArray.end());
        }

        string connectionExtracted;
        for (int j = 0; j < switchingArray.size(); ++j) {
            connectionExtracted += switchingArray.at(j);
        }

        if (find(linesCalculated.begin(), linesCalculated.end(), connectionExtracted) == linesCalculated.end()) {
            linesCalculated.push_back(connectionExtracted);
            TravellingOption* newTrallingOption = calcTravellingOption(0, first, last, LinesByName.at(switchingArray.at(0)), &switchingArray);
            results.push_back(newTrallingOption);
        }
    }
    for (int i = 0; i < results.size(); ++i) {
        results.at(i)->sumCost = calcTravellingCosts(results.at(i));
    }
    sort(results.begin(), results.end(), compareTravellingOptions);
}

// begin with deepness 1 (1x line switching/"Umsteigen")
// while no results found (and extraIteration >= 0), begin recursive-multi-state-function, to search for line-connections
// if one or more connections were found, reduce extraIteration by 1
// => One additional loop-iteration to get super-sure, that more line switching won't get a better result
// end: create TravellingOptions based on the results
void Pathfinder::withLineSwitching(string* first, string* last, vector<string> firstLines, vector<string> lastLines) {
    cout << "no direct connection found!\n";

    vector<string> travelledLines, switchingOptions;
    int deepness = 1;
    int extraIterations = 1;
    while (switchingOptions.empty() || extraIterations >= 0) {
        initiateSearch(&travelledLines, &switchingOptions, &firstLines, &lastLines, deepness);
        initiateSearch(&travelledLines, &switchingOptions, &lastLines, &firstLines, deepness);
        deepness++;

        if (!switchingOptions.empty()) {
            --extraIterations;
        }
    }

    createTravellingOption(first, last, &switchingOptions, &firstLines);
}

// print best result first and if results > 1: print other results after "Alternatives:" too
void Pathfinder::printResult() {
    cout << "\n\nBest Option:\n";
    printTravellingOption(results.at(0), 0);
    if (results.size() > 1) {
        cout << "Alternatives:\n";
        for (int i = 1; i < results.size(); ++i) {
            printTravellingOption(results.at(i), 0);
        }
    }
}

// print all details from TravellingOption.
// if it has a ->next, print it recursively too
// add all costs again and print result at the end
// return costs for recursive predecessor's costs addition
int Pathfinder::printTravellingOption(TravellingOption* travellingOption, int pos) {
    int sumCost = 0;
    cout << WHITE << "\nLine: " << printLineColour(travellingOption->line->line) << "\n";
    string start = *travellingOption->start;
    string arrayStart = travellingOption->travelingStations.at(0);
    if (travellingOption->travelingStations.at(0) == *travellingOption->start) {
        for (int i = 0; i < travellingOption->travelingStations.size()-1; ++i) {
            sumCost += travellingOption->travelingCosts.at(i);
            cout << travellingOption->travelingStations.at(i) << "\n -> " << travellingOption->travelingCosts.at(i) << "\n";
        }
        cout << *travellingOption->end;
    } else {
        cout << *travellingOption->start;
        for (int i = travellingOption->travelingStations.size()-1; i >= 0; --i) {
            sumCost += travellingOption->travelingCosts.at(i);
            cout << "\n -> " << travellingOption->travelingCosts.at(i) << "\n" << travellingOption->travelingStations.at(i);
        }
    }
    if (travellingOption->next != nullptr) {
        sumCost += 5;
        cout << CYAN << "\n-- Switching Line: +5 --" << RESET;
        sumCost += printTravellingOption(travellingOption->next, pos+1);
    }
    if (pos == 0) {
        cout << "\n------------------------\n" << RED <<"cost sum: " << sumCost << RESET << "\n\n";
    }
    return sumCost;
}

// print each U-Bahn-Line in its "right" colour :)
string Pathfinder::printLineColour(string line) {
    if (line[0] == 'U') {
        switch (line[1]) {
            case '1': return BOLDRED + line + RESET; break;
            case '2': return BOLDMAGENTA + line + RESET; break;
            case '3': return BOLDYELLOW + line + RESET; break;
            case '4': return BOLDGREEN + line + RESET; break;
            case '6': return BOLDRED + line + RESET; break;
            default: return line + RESET; break;
        }
    }
    else return line + RESET;
}