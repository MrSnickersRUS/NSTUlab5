using System;
using System.Collections.Generic;
using System.Linq;

class Warehouse {
    private const int MAXCAPACITY = 6000;
    private const int MAXZONES = 3;
    private const int MAXSTACKS = 20;
    private const int MAXSECTIONS = 5;
    private const int MAXSHELFS = 2;
    private const int MAXCELL = 10;

    enum Operation { STOP, ADD, REMOVE, INFO, UNKNOWN }

    private static readonly Dictionary<string, Operation> operations = new Dictionary<string, Operation> {
        {"STOP", Operation.STOP},
        {"ADD", Operation.ADD},
        {"REMOVE", Operation.REMOVE},
        {"INFO", Operation.INFO}
    };

    private static Dictionary<string, Dictionary<string, int>> storage = new Dictionary<string, Dictionary<string, int>>();
    private static int totalItems = 0;

    static string NumToLiteral(int a) {
        switch (a) {
            case 1: return "А";
            case 2: return "Б";
            case 3: return "В";
            default: return "";
        }
    }

    static void StorageGenerator() {
        for (int zone = 1; zone <= MAXZONES; zone++) {
            for (int stack = 1; stack <= MAXSTACKS; stack++) {
                for (int section = 1; section <= MAXSECTIONS; section++) {
                    for (int shelf = 1; shelf <= MAXSHELFS; shelf++) {
                        string cell = NumToLiteral(zone) + stack.ToString() + section.ToString() + shelf.ToString();
                        storage[cell] = new Dictionary<string, int>();
                    }
                }
            }
        }
    }

    static int GetCellTotal(string cell) {
        int total = 0;
        if (storage.ContainsKey(cell)) {
            foreach (var item in storage[cell]) {
                total += item.Value;
            }
        }
        return total;
    }

    static int GetZoneTotal(string zonePrefix) {
        int total = 0;
        foreach (var cell in storage) {
            if (cell.Key.StartsWith(zonePrefix)) {
                foreach (var item in cell.Value) {
                    total += item.Value;
                }
            }
        }
        return total;
    }

    static void AddOperation(Dictionary<string, string> command) {
        string cell = command["adress"];
        string itemName = command["name"];
        int quantity = int.Parse(command["num"]);
        
        int currentCellTotal = GetCellTotal(cell);
        
        if (currentCellTotal + quantity > MAXCELL) {
            Console.WriteLine($"Ошибка: в ячейке {cell} недостаточно места (максимум {MAXCELL} единиц)");
            return;
        }
        
        if (totalItems + quantity > MAXCAPACITY) {
            Console.WriteLine($"Ошибка: на складе недостаточно места (максимум {MAXCAPACITY} единиц)");
            return;
        }
        
        if (!storage[cell].ContainsKey(itemName)) {
            storage[cell][itemName] = 0;
        }
        storage[cell][itemName] += quantity;
        totalItems += quantity;
        Console.WriteLine($"Добавлено {quantity} единиц товара \"{itemName}\" в ячейку {cell}");
    }

    static void RemoveOperation(Dictionary<string, string> command) {
        string cell = command["adress"];
        string itemName = command["name"];
        int quantity = int.Parse(command["num"]);
        
        if (!storage[cell].ContainsKey(itemName) || storage[cell][itemName] < quantity) {
            Console.WriteLine($"Ошибка: в ячейке {cell} недостаточно товара \"{itemName}\" для списания");
            return;
        }
        
        storage[cell][itemName] -= quantity;
        totalItems -= quantity;
        
        if (storage[cell][itemName] == 0) {
            storage[cell].Remove(itemName);
        }
        
        Console.WriteLine($"Списано {quantity} единиц товара \"{itemName}\" из ячейки {cell}");
    }

    static void InfoOperation() {
        double totalPercent = (double)totalItems / MAXCAPACITY * 100;
        Console.WriteLine($"Общая загруженность склада: {totalPercent:F2}%");
        
        string[] zones = { "А", "Б", "В" };
        foreach (string zone in zones) {
            int zoneTotal = GetZoneTotal(zone);
            int zoneCells = MAXSTACKS * MAXSECTIONS * MAXSHELFS;
            int zoneCapacity = zoneCells * MAXCELL;
            double zonePercent = (double)zoneTotal / zoneCapacity * 100;
            Console.WriteLine($"Зона {zone} загружена на {zonePercent:F2}%");
        }
        
        Console.WriteLine("\nСодержимое ячеек:");
        foreach (var cell in storage) {
            if (cell.Value.Count > 0) {
                Console.Write($"{cell.Key}: ");
                foreach (var item in cell.Value) {
                    Console.Write($"{item.Key} - {item.Value} шт.; ");
                }
                Console.WriteLine();
            }
        }
        
        Console.WriteLine("\nПустые ячейки:");
        int emptyCount = 0;
        foreach (var cell in storage) {
            if (cell.Value.Count == 0) {
                Console.Write($"{cell.Key}, ");
                emptyCount++;
            }
        }
        Console.WriteLine();
    }

    static bool MsgChecker(Dictionary<string, string> command) {
        if (!command.ContainsKey("oper")) {
            Console.WriteLine("Не указана операция");
            return false;
        }
        
        if (command["oper"] == "INFO" || command["oper"] == "STOP") return true;
        
        if (!operations.ContainsKey(command["oper"])) {
            Console.WriteLine("Неизвестная команда");
            return false;
        }
        
        if (!command.ContainsKey("num")) {
            Console.WriteLine("Не указано количество");
            return false;
        }
        
        if (!int.TryParse(command["num"], out int quantity) || quantity <= 0 || quantity > 10) {
            Console.WriteLine("Количество должно быть от 1 до 10");
            return false;
        }

        if (!command.ContainsKey("adress")) {
            Console.WriteLine("Не указан адрес ячейки");
            return false;
        }
        
        string cell = command["adress"];
        if (!storage.ContainsKey(cell)) {
            Console.WriteLine($"Ячейка с адресом {cell} не существует");
            return false;
        }
        
        return true;
    }

    static Dictionary<string, string> InputToMap(string input) {
        Dictionary<string, string> inputArgs = new Dictionary<string, string>();
        string[] parts = input.Split(new[] {' '}, StringSplitOptions.RemoveEmptyEntries);
        
        if (parts.Length == 0) {
            Console.WriteLine("Не введена команда");
            return inputArgs;
        }
        
        inputArgs["oper"] = parts[0];
        
        if (inputArgs["oper"] == "INFO" || inputArgs["oper"] == "STOP") return inputArgs;
        
        if (parts.Length > 1) inputArgs["name"] = parts[1];
        if (parts.Length > 2) inputArgs["num"] = parts[2];
        if (parts.Length > 3) inputArgs["adress"] = parts[3];
        
        return inputArgs;
    }

    static Operation StringToOper(string oper) {
        if (operations.ContainsKey(oper))
            return operations[oper];
        return Operation.UNKNOWN;
    }

    static void Main() {
        StorageGenerator();
        bool continueLoop = true;

        while (continueLoop) {
            Console.Write("Введите команду: ");
            string msg = Console.ReadLine();
            
            Dictionary<string, string> command = InputToMap(msg);
            if (command.Count == 0) continue;
            
            if (MsgChecker(command)) {
                Operation oper = StringToOper(command["oper"]);
                switch (oper) {
                    case Operation.ADD:
                        AddOperation(command);
                        break;
                    case Operation.REMOVE:
                        RemoveOperation(command);
                        break;
                    case Operation.INFO:
                        InfoOperation();
                        break;
                    case Operation.STOP:
                        continueLoop = false;
                        break;
                    default:
                        Console.WriteLine("Неизвестная команда");
                        break;
                }           
            }
        }
    }
}