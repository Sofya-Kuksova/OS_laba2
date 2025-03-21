#include "func_client.h"

// Глобальные переменные для хранения файлового дескриптора и указателя на отображённую память
static int fd = -1;
static void* ptr = nullptr;

bool mapFileClient() {
    // Открываем существующий файл (предполагается, что сервер его уже создал)
    fd = open(FILENAME, O_RDWR); // файл открывается для чтения и записи
    if (fd < 0) {
        std::cerr << "Error opening file. Make sure the server has created the file." << std::endl;
        return false;
    }
    
    // Настройка дескриптора для ожидания доступности ласти памяти для чтения (не факт, что сервер записал данные)
    fd_set read_fds;
    FD_ZERO(&read_fds); // очищает набор
    FD_SET(fd, &read_fds); // добавляет дескриптор fd в набор, чтобы отслеживать его состояние
    
    // Установка таймаута ожидания (например, 5 секунд)
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0; // милли
    
    int result = select(fd + 1, &read_fds, NULL, NULL, &timeout);  // проверяет, готов ли файл к чтению(проверка всех дескрипторо от 0 до fd+1, проверить дескрипторов на готовность для чтения, для записи и для исключительных условий: проверка этих наборов не производится, максимально допустимое время ожидания)
    if (result < 0) {
        std::cerr << "Select error: " << strerror(errno) << std::endl;
        close(fd);
        return false;
    } else if (result == 0) {
        std::cerr << "Timeout: File not ready for reading." << std::endl;
        close(fd);
        return false;
    }
    
    // Если select() возвращает значение > 0, значит файл готов для чтения
    ptr = mmap(NULL, FILESIZE, PROT_READ, MAP_SHARED, fd, 0); // Адрес выбирает система, Размер отображаемой области, Доступ только для чтения, Изменения видны другим процессамФайловый дескриптор, Смещение от начала файла
    if (ptr == MAP_FAILED) {
        std::cerr << "Error mapping file into memory." << std::endl;
        close(fd);
        return false;
    }
    std::cout << "File mapped successfully." << std::endl;
    return true;
}

void readData() {
    if (!ptr) { // был ли файл отображён
        std::cerr << "Memory is not mapped. Please map the file first." << std::endl;
        return;
    }
    // Читаем и выводим содержимое отображённой области
    std::cout << "\nReading data from mapped memory:" << std::endl;
    std::cout << static_cast<char*>(ptr) << std::endl;
}

void cleanupClient() {
    // Отмена отображения файла в память
    if (ptr && ptr != MAP_FAILED) {
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
}