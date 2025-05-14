#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <iomanip>

using namespace std;

#define MAXCAPACITY 6000
#define MAXZONES 3
#define MAXSTACKS 20
#define MAXSECTIONS 5
#define MAXSHELFS 2
#define MAXCELL 10

enum Operation {
    STOP, ADD, REMOVE, INFO, UNKNOWN
};

const map<string, Operation> operations = {
    {"STOP", STOP}, 
    {"ADD", ADD}, 
    {"REMOVE", REMOVE}, 
    {"INFO", INFO}
};

map<string, map<string, int>> storage;
int totalItems = 0;

string numToLiteral(int a) {
    switch (a) {
        case 1: return "А";
        case 2: return "Б";
        case 3: return "В";
        default: return "";
    }
}

void storageGenerator() {
    for (int zone = 1; zone <= MAXZONES; zone++) {
        for (int stack = 1; stack <= MAXSTACKS; stack++) {
            for (int section = 1; section <= MAXSECTIONS; section++) {
                for (int shelf = 1; shelf <= MAXSHELFS; shelf++) {
                    string cell = numToLiteral(zone) + to_string(stack) + to_string(section) + to_string(shelf);
                    storage[cell] = map<string, int>();
                }
            }
        }
    }
}

int getCellTotal(const string& cell) {
    int total = 0;
    if (storage.find(cell) != storage.end()) {
        for (const auto& item : storage.at(cell)) {
            total += item.second;
        }
    }
    return total;
}

int getZoneTotal(const string& zone_prefix) {
    int total = 0;
    for (const auto& cell : storage) {
        if (cell.first.rfind(zone_prefix, 0) == 0) {
            for (const auto& item : cell.second) {
                total += item.second;
            }
        }
    }
    return total;
}

void addOperation(const map<string, string>& command) {
    string cell = command.at("adress");
    string itemName = command.at("name");
    int quantity = stoi(command.at("num"));
    
    int currentCellTotal = getCellTotal(cell);
    
    if (currentCellTotal + quantity > MAXCELL) {
        cout << "Ошибка: в ячейке " << cell << " недостаточно места (максимум " << MAXCELL << " единиц)" << endl;
        return;
    }
    
    if (totalItems + quantity > MAXCAPACITY) {
        cout << "Ошибка: на складе недостаточно места (максимум " << MAXCAPACITY << " единиц)" << endl;
        return;
    }
    
    storage[cell][itemName] += quantity;
    totalItems += quantity;
    cout << "Добавлено " << quantity << " единиц товара \"" << itemName << "\" в ячейку " << cell << endl;
}

void removeOperation(const map<string, string>& command) {
    string cell = command.at("adress");
    string itemName = command.at("name");
    int quantity = stoi(command.at("num"));
    
    if (storage[cell].count(itemName) == 0 || storage[cell][itemName] < quantity) {
        cout << "Ошибка: в ячейке " << cell << " недостаточно товара \"" << itemName << "\" для списания" << endl;
        return;
    }
    
    storage[cell][itemName] -= quantity;
    totalItems -= quantity;
    
    if (storage[cell][itemName] == 0) {
        storage[cell].erase(itemName);
    }
    
    cout << "Списано " << quantity << " единиц товара \"" << itemName << "\" из ячейки " << cell << endl;
}

void infoOperation() {
    double totalPercent = (static_cast<double>(totalItems) / MAXCAPACITY) * 100;
    cout << "Общая загруженность склада: " << fixed << setprecision(2) << totalPercent << "%" << endl;
    
    const vector<string> zones = {"А", "Б", "В"};
    for (const string& zone : zones) {
        int zoneTotal = getZoneTotal(zone);
        int zoneCells = MAXSTACKS * MAXSECTIONS * MAXSHELFS;
        int zoneCapacity = zoneCells * MAXCELL;
        double zonePercent = (static_cast<double>(zoneTotal) / zoneCapacity) * 100;
        cout << "Зона " << zone << " загружена на " << fixed << setprecision(2) << zonePercent << "%" << endl;
    }
    
    cout << "\nСодержимое ячеек:" << endl;
    for (const auto& cell : storage) {
        if (!cell.second.empty()) {
            cout << cell.first << ": ";
            for (const auto& item : cell.second) {
                cout << item.first << " - " << item.second << " шт.; ";
            }
            cout << endl;
        }
    }
    
    cout << "\nПустые ячейки:" << endl;
    int emptyCount = 0;
    for (const auto& cell : storage) {
        if (cell.second.empty()) {
            cout << cell.first << ", ";
            emptyCount++;
        }
    }
    cout << endl;
}

bool msgChecker(const map<string, string>& command) {
    if (command.find("oper") == command.end()) {
        cerr << "Не указана операция" << endl;
        return false;
    }
    
    if (command.at("oper") == "INFO" || command.at("oper") == "STOP") return true;
    
    if (operations.count(command.at("oper")) == 0) {
        cerr << "Неизвестная команда" << endl; 
        return false;
    }
    
    if (command.find("num") == command.end()) {
        cerr << "Не указано количество" << endl;
        return false;
    }
    
    int quantity = stoi(command.at("num"));
    if (quantity <= 0 || quantity > 10) {
        cerr << "Количество должно быть от 1 до 10" << endl;
        return false;
    }

    if (command.find("adress") == command.end()) {
        cerr << "Не указан адрес ячейки" << endl;
        return false;
    }
    
    string cell = command.at("adress");
    if (storage.count(cell) == 0) {
        cerr << "Ячейка с адресом " << cell << " не существует" << endl;
        return false;
    }
    
    return true;
}

map<string, string> inputToMap(const string& input) {
    map<string, string> inputArgs;
    istringstream iss(input);
    string val;
    
    if (iss >> val) inputArgs["oper"] = val;
    else {
        cerr << "Не введена команда" << endl;
        return inputArgs;
    }
    
    if (inputArgs["oper"] == "INFO" || inputArgs["oper"] == "STOP") return inputArgs;
    
    if (iss >> val) inputArgs["name"] = val;
    if (iss >> val) inputArgs["num"] = val;
    if (iss >> val) inputArgs["adress"] = val;
    
    return inputArgs;
}

Operation stringToOper(const string& oper) {
    auto itOper = operations.find(oper);
    if (itOper != operations.end()) return itOper->second;
    return UNKNOWN;
}

int main() {
    storageGenerator();
    bool continueLoop = true;

    while (continueLoop) {
        cout << "Введите команду: ";
        string msg;
        getline(cin, msg);
        
        map<string, string> command = inputToMap(msg);
        if (command.empty()) continue;
        
        if (msgChecker(command)) {
            Operation oper = stringToOper(command.at("oper"));
            switch (oper) {
                case ADD:
                    addOperation(command);
                    break;
                case REMOVE:
                    removeOperation(command);
                    break;
                case INFO:
                    infoOperation();
                    break;
                case STOP:
                    continueLoop = false;
                    break;
                default:
                    cout << "Неизвестная команда" << endl;
                    break;
            }           
        }
    }
    
    return 0;
}