#include "func_client.h"

using namespace std;

int main() {
    int choice;
    do {
        cout << "\n=== Client Menu ===\n";
        cout << "<1> Map file into memory\n";
        cout << "<2> Read data\n";
        cout << "<3> Exit (unmap file)\n";
        cout << "[0] Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                if (!mapFileClient()) {
                    std::cerr << "Failed to map file." << std::endl;
                }
                break;
            case 2:
                readData();
                break;
            case 3:
                cleanupClient();
                break;
            case 0:
                cout << "Exiting client application." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 0);
    return 0;
}
