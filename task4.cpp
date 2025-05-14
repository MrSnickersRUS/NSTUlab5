#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <algorithm>
#include <vector>

using namespace std;

enum Operation {
    CLASS, NEXT, VIEW, UNKNOWN
};

const map<string, Operation> operations = {
    {"CLASS", CLASS}, 
    {"NEXT", NEXT}, 
    {"VIEW", VIEW}
};

map<int, set<string>> schedule;
int currentMonthDays = 31;

void classOperation(const map<string, string>& command) {
    int day = stoi(command.at("day"));
    if (day < 1 || day > 31) {
        cerr << "Ошибка: день должен быть от 1 до 31" << endl;
        return;
    }
    schedule[day].insert(command.at("class"));
}

void nextOperation() {
    map<int, set<string>> newSchedule;
    int newMonthDays;
    
    // количество дней в следующем месяце
    if (currentMonthDays == 31) {
        newMonthDays = 30;
    } else {
        newMonthDays = 31;
    }
    
    // занятия в новый месяц
    for (const auto& [day, classes] : schedule) {
        int newDay = min(day, newMonthDays);
        newSchedule[newDay].insert(classes.begin(), classes.end());
    }
    
    schedule = move(newSchedule);
    currentMonthDays = newMonthDays;
}

void viewOperation(int day) {
    if (day < 1 || day > currentMonthDays) {
        cout << "In " << day << " day We are free!" << endl;
        return;
    }
    
    if (schedule.count(day)) {
        cout << "In " << day << " day " << schedule[day].size() 
             << " classes in university:" << endl;
        bool first = true;
        for (const auto& subject : schedule[day]) {
            if (!first) cout << ", ";
            cout << subject;
            first = false;
        }
        cout << endl;
    } else {
        cout << "In " << day << " day We are free!" << endl;
    }
}

bool msgChecker(const map<string, string>& command) {
    if (command.at("oper") == "NEXT") return true;
    
    if (operations.count(command.at("oper")) == 0) {
        cerr << "Неизвестная команда" << endl;
        return false;
    }
    
    try {
        int day = stoi(command.at("day"));
        if (day <= 0 || day > 31) {
            cerr << "День должен быть числом от 1 до 31" << endl;
            return false;
        }
    } catch (...) {
        cerr << "Ошибка: некорректный формат дня" << endl;
        return false;
    }
    
    return true;
}

map<string, string> inputToMap(const string& input) {
    map<string, string> inputArgs;
    istringstream iss(input);
    vector<string> tokens;
    string token;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    if (tokens.empty()) {
        cerr << "Ошибка: пустая команда" << endl;
        return inputArgs;
    }
    
    inputArgs["oper"] = tokens[0];
    
    if (tokens[0] == "CLASS") {
        if (tokens.size() >= 2) inputArgs["day"] = tokens[1];
        if (tokens.size() >= 3) inputArgs["class"] = tokens[2];
    } else if (tokens[0] == "VIEW") {
        if (tokens.size() >= 2) inputArgs["day"] = tokens[1];
    }
    
    return inputArgs;
}

Operation stringToOper(const string& oper) {
    auto it = operations.find(oper);
    if (it != operations.end()) {
        return it->second;
    }
    return UNKNOWN;
}

int main() {
    int numLoops;
    cin >> numLoops;
    cin.ignore();
    
    for (int i = 0; i < numLoops; i++) {
        string input;
        getline(cin, input);
        
        map<string, string> command = inputToMap(input);
        if (command.empty()) continue;
        
        if (msgChecker(command)) {
            Operation oper = stringToOper(command.at("oper"));
            switch (oper) {
                case CLASS:
                    classOperation(command);
                    break;
                case NEXT:
                    nextOperation();
                    break;
                case VIEW:
                    viewOperation(stoi(command.at("day")));
                    break;
                default:
                    cout << "Неизвестная команда" << endl;
                    break;
            }
        }
    }
    
    return 0;
}