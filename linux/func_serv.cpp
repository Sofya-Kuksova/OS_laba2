#include "func_serv.h"

// Глобальные переменные для хранения дескриптора файла и указателя на область памяти, куда будет отображён файл
static int fd = -1; // ещё не открыт
static void* ptr = nullptr;

bool mapFile() {
    // Открытие или создание файла
    fd = open(FILENAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR); // файл открывается для чтения и записи, создание, права доступа — чтение и запись для владельца
    if (fd < 0) {
        std::cerr << "Error opening/creating file." << std::endl;
        return false;
    }
    // Установка размера файла (необходимо для корректного отображения)
    if (ftruncate(fd, FILESIZE) == -1) {
        std::cerr << "Error setting file size." << std::endl;
        close(fd);
        return false;
    }
    // Отображение файла в память
    ptr = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); // система сама выбирает адрес в памяти, размер отображаемой области, доступ на чтение и запись, изменения в памяти будут записаны в файл, дескриптор, смещение от начала файла
    if (ptr == MAP_FAILED) {
        std::cerr << "Error mapping file into memory." << std::endl;
        close(fd);
        return false;
    }
    std::cout << "File mapped successfully." << std::endl; // ptr указывает на область памяти, где находится содержимое файла
    return true;
}

void writeData() {
    if (!ptr) { // был ли файл отображён в память
        std::cerr << "Memory is not mapped. Please map the file first." << std::endl;
        return;
    }
    std::cout << "Enter data to write: ";
    std::string input;
    std::getline(std::cin >> std::ws, input); // убирает лишние пробельные символы перед вводом
    // Копирование введённых данных в отображённую область памяти
    snprintf((char*)ptr, FILESIZE, "%s", input.c_str());
    std::cout << "Data written to mapped memory." << std::endl; // MAP_SHARED->в файле тоже
}

void cleanup() {
    // Отмена отображения файла в память
    if (ptr && ptr != MAP_FAILED) { // файл отображён
        if (munmap(ptr, FILESIZE) == -1) {
            std::cerr << "Error unmapping memory." << std::endl;
        } else {
            std::cout << "Memory unmapped successfully." << std::endl;
        }
        ptr = nullptr;
    }
    // Закрытие файлового дескриптора
    if (fd != -1) {
        close(fd);
        fd = -1;
    }
    // Удаление файла
    if (unlink(FILENAME) == 0) { // удаляет файл
        std::cout << "File deleted successfully." << std::endl;
    } else {
        std::cerr << "Error deleting file." << std::endl;
    }
}