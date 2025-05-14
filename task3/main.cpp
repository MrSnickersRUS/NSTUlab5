#include "commands.h"
#include <sstream>
#include <vector>

using namespace std;

int main() {
    string input;
    bool continueLoop = 1;
    
    while (continueLoop) {
        getline(cin, input);
        
        istringstream iss(input);
        string commandStr;
        iss >> commandStr;
        
        vector<string> args;
        string arg;
        while (iss >> arg) {
            args.push_back(arg);
        }
        
        Operation cmd = parseCommand(commandStr);
        
        switch (cmd) {
            case Operation::CREATE_PLANE: createPlane(args); break;
            case Operation::PLANES_FOR_TOWN: planesForTown(args); break;
            case Operation::TOWNS_FOR_PLANE: townsForPlane(args); break;
            case Operation::PLANES: showAllPlanes(); break;
            case Operation::STOP: continueLoop = 0; break;
            case Operation::UNKNOWN: 
                cout << "Неизвестная команда" << endl;
                break;
        }
    }
    
    return 0;
}