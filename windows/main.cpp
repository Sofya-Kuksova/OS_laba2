#include "functions.h"
#include <regex>
#include <limits>
#include <iostream>
#include <string>

using namespace std;

// Функция для очистки потока ввода
void ClearInputStream() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}
    
// Функция для проверки формата адреса ("0x" + шестнадцатеричные цифры)
bool ValidateHexAddress(const string& addrStr) {
    regex pattern("^0x[0-9A-Fa-f]+$");
    return regex_match(addrStr, pattern);
}
    
// Функция для конвертации строки в uintptr_t
uintptr_t ConvertHexAddress(const string& addrStr) {
    return static_cast<uintptr_t>(stoul(addrStr, nullptr, 16));
}

void DisplayMenu() {
    cout << "\nMenu:\n";
    cout << "[1] Get System Information\n";
    cout << "[2] Get Memory Status\n";
    cout << "[3] Query Memory State\n";
    cout << "[4] Seperate Automatic memory allocation\n";
    cout << "[5] Seperate Manual memory allocation\n";
    cout << "[6] Common Automatic memory allocation\n";
    cout << "[7] Common Manual memory allocation\n";
    cout << "[8] Write to Memory by address\n";
    cout << "[9] Set Memory Protection\n";
    cout << "[10] Free Memory by address\n";
    cout << "[0] Exit\n";
    cout << "Enter your choice: ";
}

int main() {
    int choice = -1;

    do {
        // Зацикливаем ввод номера опции до корректного ввода числа
        bool validInput = false;
        while (!validInput) {
            DisplayMenu();
            cin >> choice;
            if (cin.fail()) {
                cout << "Invalid input! Please enter a valid number.\n";
                ClearInputStream();
            } else {
                validInput = true;
            }
        }
        
        // Обработка выбора пользователя
        switch (choice) {
            case 1:
                GetSystemInformation();
                break;
            case 2:
                GetMemoryStatus();
                break;
            case 3: {
                string addrStr;
                cout << "Note: the base address should be aligned to the page size.\n";
                cout << "Enter memory address in hexadecimal format (e.g., 0x1234ABCD): ";
                ClearInputStream();
                getline(cin, addrStr);
                if (!ValidateHexAddress(addrStr)) {
                    cout << "Invalid address format! Please use format 0x followed by hexadecimal digits.\n";
                    break;
                }
                uintptr_t address = ConvertHexAddress(addrStr);
                QueryMemoryState(address);
                break;
            }
            case 4: {
                SepAutomaticMemoryAllocation();
                break;
            }
            case 5: {
                string addrStr;
                cout << "Note: address alignment by allocation granularity: (64KB - 0x10000)\n";
                cout << "Enter memory address in hexadecimal format (e.g., 0x1234ABCD): ";
                ClearInputStream();
                getline(cin, addrStr);
                if (!ValidateHexAddress(addrStr)) {
                    cout << "Invalid address format! Please use format 0x followed by hexadecimal digits.\n";
                    break;
                }
                uintptr_t address = ConvertHexAddress(addrStr);
                SepManualMemoryAllocation(address);
                break;
            }
            case 6: { 
                ComAutomaticMemoryAllocation();
                break;
            }
            case 7: {
                string addrStr;
                cout << "Note: address alignment by allocation granularity: (64KB - 0x10000)\n";
                cout << "Enter memory address in hexadecimal format (e.g., 0x1234ABCD): ";
                ClearInputStream();
                getline(cin, addrStr);
                if (!ValidateHexAddress(addrStr)) {
                    cout << "Invalid address format! Please use format 0x followed by hexadecimal digits.\n";
                    break;
                }
                uintptr_t address = ConvertHexAddress(addrStr);
                ComManualMemoryAllocation(address);
                break;
            }
            case 8: {
                string addrStr;
                cout << "Note: address alignment by allocation granularity: (64KB - 0x10000)\n";
                cout << "Enter memory address in hexadecimal format (e.g., 0x1234ABCD): ";
                ClearInputStream();
                getline(cin, addrStr);
                if (!ValidateHexAddress(addrStr)) {
                    cout << "Invalid address format! Please use format 0x followed by hexadecimal digits.\n";
                    break;
                }
                uintptr_t address = ConvertHexAddress(addrStr);
                string data;
                cout << "Enter the string to write: ";
                getline(cin, data);
                if (data.empty()) {
                    cout << "Input string is empty. Nothing will be written.\n";
                    break;
                }
                WriteToMemoryAddress(address, data);
                break;
            }
            case 9: {
                string addrStr;
                cout << "Note: address alignment by allocation granularity: (64KB - 0x10000)\n";
                cout << "Enter memory address in hexadecimal format (e.g., 0x1234ABCD): ";
                ClearInputStream();
                getline(cin, addrStr);
                if (!ValidateHexAddress(addrStr)) {
                    cout << "Invalid address format! Please use format 0x followed by hexadecimal digits.\n";
                    break;
                }
                uintptr_t address = ConvertHexAddress(addrStr);
                SetMemoryProtection(address);
                break;
            }
            case 10: {
                string addrStr;
                cout << "Enter memory address in hexadecimal format (e.g., 0x1234ABCD): ";
                ClearInputStream();
                getline(cin, addrStr);
                if (!ValidateHexAddress(addrStr)) {
                    cout << "Invalid address format! Please use format 0x followed by hexadecimal digits.\n";
                    break;
                }
                uintptr_t address = ConvertHexAddress(addrStr);
                LPVOID ptr = reinterpret_cast<LPVOID>(address);
                FreeAllocatedMemory(ptr);
                break;
            }
            case 0:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice! Try again.\n";
        }
    } while (choice != 0);
    
    return 0;
}
