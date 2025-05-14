#include <iostream>
#include <map>
#include <vector>
#include <iomanip>
#include <sstream>
#include <algorithm>

using namespace std;

enum Operation {
    ENQUEUE, DISTRIBUTE, STOP, UNKNOWN
};

struct Ticket {
    string number;
    int minutes;
};

vector<Ticket> ticketsQueue;
unsigned int windowsCount = 0;
int ticketCounter = 0;

// создание талона в очередь
string generateTicketNumber(int minutes) {
    ostringstream oss;
    oss << "T" << setw(3) << setfill('0') << ticketCounter++;
    return oss.str();
}

Operation stringToOperation(const string& operStr) {
    if (operStr == "ENQUEUE") return ENQUEUE;
    if (operStr == "DISTRIBUTE") return DISTRIBUTE;
    if (operStr == "STOP") return STOP;
    return UNKNOWN;
}

void enqueueOperation(int minutes) {
    string ticketNum = generateTicketNumber(minutes);
    ticketsQueue.push_back({ticketNum, minutes});
    cout << ticketNum << endl;
}

void distributeTickets() {
    if (windowsCount == 0 || ticketsQueue.empty()) {
        cout << "Нет окон или посетителей для распределения" << endl;
        return;
    }

    vector<int> windowsTime(windowsCount, 0);
    vector<vector<string>> windowsTickets(windowsCount);

    for (const auto& ticket : ticketsQueue) { // перебор минимальных по времени окон
        auto minIt = min_element(windowsTime.begin(), windowsTime.end());
        int windowIndex = distance(windowsTime.begin(), minIt);
        
        windowsTime[windowIndex] += ticket.minutes;
        windowsTickets[windowIndex].push_back(ticket.number);
    }

    for (int i = 0; i < windowsCount; ++i) { // вывод распределенных талонов
        cout << "Окно " << i+1 << " (" << windowsTime[i] << " минут): ";
        for (size_t j = 0; j < windowsTickets[i].size(); ++j) {
            cout << windowsTickets[i][j];
            if (j != windowsTickets[i].size() - 1) cout << ", ";
        }
        cout << endl;
    }

    ticketsQueue.clear();
}

int main() {
    cout << "Введите кол-во окон" << endl;
    cin >> windowsCount;
    cin.ignore();
    bool continueLoop = 1;

    while (continueLoop) {
        cout << "<<< ";
        string input;
        getline(cin, input);

        istringstream iss(input);
        string command;
        iss >> command;

        Operation oper = stringToOperation(command);

        switch (oper) {
            case ENQUEUE: {
                int minutes;
                if (iss >> minutes) {
                    enqueueOperation(minutes);
                } else {
                    cout << "Ошибка: не указано время приема" << endl;
                }
                break;
            }
            case DISTRIBUTE:
                distributeTickets();
                break;
            case STOP:
                continueLoop = 0;
                break;
            case UNKNOWN:
                cout << "Ошибка: неизвестная команда" << endl;
                break;
        }
    }
}