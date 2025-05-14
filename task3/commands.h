#ifndef COMMANDS_H
#define COMMANDS_H

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;

enum class Operation {
    CREATE_PLANE,
    PLANES_FOR_TOWN,
    TOWNS_FOR_PLANE,
    PLANES,
    UNKNOWN,
    STOP
};

map<string, vector<string>> planesData;
map<string, vector<string>> townsData;

// 
Operation parseCommand(const string& commandStr) {
    if (commandStr == "CREATE_PLANE") return Operation::CREATE_PLANE;
    if (commandStr == "PLANES_FOR_TOWN") return Operation::PLANES_FOR_TOWN;
    if (commandStr == "TOWNS_FOR_PLANE") return Operation::TOWNS_FOR_PLANE;
    if (commandStr == "PLANES") return Operation::PLANES;
    if (commandStr == "STOP") return Operation::STOP;
    return Operation::UNKNOWN;
}

// создание самолета
void createPlane(const vector<string>& args) {
    if (args.size() < 2) {
        cout << "Ошибка: недостаточно аргументов" << endl;
        return;
    }
    
    const string& planeName = args[0];
    planesData[planeName] = vector<string>(args.begin() + 1, args.end());
    
    for (const auto& town : planesData[planeName]) {
        townsData[town].push_back(planeName);
    }
    
    cout << "Самолет " << planeName << " добавлен" << endl;
}

// самолеты проходящие через город
void planesForTown(const vector<string>& args) {
    if (args.empty()) {
        cout << "Ошибка: укажите город" << endl;
        return;
    }
    
    const string& town = args[0];
    if (townsData.count(town)) {
        cout << "Самолеты через " << town << ": ";
        for (const auto& plane : townsData[town]) {
            cout << plane << " ";
        }
        cout << endl;
    } else {
        cout << "Город " << town << " не найден" << endl;
    }
}

// города через которые проходит самолет
void townsForPlane(const vector<string>& args) {
    if (args.empty()) {
        cout << "Ошибка: укажите самолет" << endl;
        return;
    }
    
    const string& plane = args[0];
    if (planesData.count(plane)) {
        cout << "Маршрут " << plane << ": ";
        for (const auto& town : planesData[plane]) {
            cout << town << " (";
            for (const auto& otherPlane : townsData[town]) {
                if (otherPlane != plane) {
                    cout << otherPlane << " ";
                }
            }
            cout << ") ";
        }
        cout << endl;
    } else {
        cout << "Самолет " << plane << " не найден" << endl;
    }
}

// вывести все самолеты
void showAllPlanes() {
    for (const auto& [plane, towns] : planesData) {
        cout << plane << ": ";
        for (const auto& town : towns) {
            cout << town << " ";
        }
        cout << endl;
    }
}

#endif