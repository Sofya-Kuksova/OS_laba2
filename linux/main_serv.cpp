#include "func_serv.h"

using namespace std;

int main(){
    int choice;
    do {
        cout << "\n=== Server Menu ===\n";
        cout << "<1> Map file into memory\n"; 
        cout << "<2> Write data\n";
        cout << "<3> Exit (unmap and delete file)\n";
        cout << "[0] Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                if (!mapFile()) {
                    std::cerr << "Failed to map file." << std::endl;
                }
                break;
            case 2:
                writeData();
                break;
            case 3:
                cleanup();
                break;
            case 0:
                cout << "Exiting server application." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 0);
    return 0;
}
