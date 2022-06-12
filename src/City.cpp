#include "City.hpp"

City::City() {
    while (true) {
        if (getInput() == 0) {
            break;
        }
    }
}

int City::getInput() {
    cout << "Drag&Drop Data to create City";
    string path;
//    std::cin >> path;

    ifstream inFile("/home/domain/Nextcloud/Documents/Technikum/2_Semester/ALGOS/WienerNoder/data.txt", ios::in);
//    ifstream inFile(path, ios::in);
    if( inFile.fail( ) ) {
        cerr << "Error: File not found." << path <<endl;
        return 1;
    } else {
        string lineStr;
        while (getline(inFile, lineStr))
        {
            createLine(lineStr);
        }
        connectSwitchingOptions();

        std::cout << "\ndone\n" << std::endl;
        searchLoop();
    }
    return 0;
}

void City::createLine(string lineStr) {
    string lineName = extractLineName(lineStr);
    stringstream ss(lineStr);
    string temp;
    vector<string> strArray;
    Line* newLine = new Line(lineName);

    while (getline(ss, temp, '\"')) {
        transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
        strArray.push_back(temp);
    };

    for (int i = 1; i < strArray.size(); i += 2) {
        Station* newStation;
        if (i == strArray.size()-1) {
            newStation = new Station(strArray[i], 1);
        } else {
            newStation = new Station(strArray[i], extractCost(strArray[i+1]));
        }
        newLine->Stations.push_back(newStation);
        if (SwitchingStations.contains(newStation->name)) {
            SwitchingStations.at(newStation->name).push_back(lineName);
        } else {
            vector<string> line = vector<string>();
            line.push_back(lineName);
            SwitchingStations.insert(pair<string, vector<string>>(newStation->name, line));
        }
    }

    LinesByName.insert(pair<string, Line*>(lineName, newLine));
    LinesByIndex.push_back(newLine);
}

int City::extractCost(string strArrayEntry) {
    return (int)strArrayEntry[1]-48;
}

string City::extractLineName(string lineStr) {
    stringstream ss;
    if (lineStr[1] == ':') ss << lineStr[0];
    if (lineStr[2] == ':') ss << lineStr[0] << lineStr[1];
    if (lineStr[3] == ':') ss << lineStr[0] << lineStr[1] << lineStr[2];
    return ss.str();
}

void City::searchLoop() {
    while (true) {
        string stations;
        cout << "\nEnter first and last Station to begin search. Separate with dash -\n[first] - [last]\nOr \"q\" to quit\n";
        getline(cin, stations);
        if (stations == "q") { break; }

        stringstream ss(stations);
        string temp;
        vector<string> searchArray;

        while (getline(ss, temp, '-')) {
            while(temp[temp.size()-1] == ' ') { temp.erase(temp.size()-1, 1); }
            while(temp[0] == ' ') { temp.erase(0, 1); }
            transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
            searchArray.push_back(temp);
        };
        if (searchArray.size() == 2) {
            if (SwitchingStations.contains(searchArray[0]) && SwitchingStations.contains(searchArray[1])) {
                searchPath(&searchArray[0], &searchArray[1]);
            } else {
                cout << "Stations not found\n\n";
            }
        } else {
            cout << "Please enter a valid format: \n[first] - [last]\n\"q\"\n";
        }

    }
}

void City::connectSwitchingOptions() {
    for (int i = 0; i < LinesByIndex.size(); ++i) {
        for (int j = 0; j < LinesByIndex.at(i)->Stations.size(); ++j) {
            for (int k = 0; k < SwitchingStations.at(LinesByIndex.at(i)->Stations.at(j)->name).size(); ++k) {
                if (!LinesByIndex.at(i)->SwitchingOptions.contains(SwitchingStations.at(LinesByIndex.at(i)->Stations.at(j)->name).at(k)) && SwitchingStations.at(LinesByIndex.at(i)->Stations.at(j)->name).at(k) != LinesByIndex.at(i)->line) {
                    LinesByIndex.at(i)->SwitchingOptions.insert(pair<string, Line*>(SwitchingStations.at(LinesByIndex.at(i)->Stations.at(j)->name).at(k), LinesByName.at(SwitchingStations.at(LinesByIndex.at(i)->Stations.at(j)->name).at(k))));
                    LinesByIndex.at(i)->MapLines.push_back(SwitchingStations.at(LinesByIndex.at(i)->Stations.at(j)->name).at(k));
                }
            }
        }
    }
}