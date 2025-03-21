#include "functions.h"

using namespace std;

string getBinaryMask(DWORD mask) { // преобразование числа в маску активного процессора
    string result;
    for (int i = sizeof(DWORD)*8 - 1; i >= 0; --i) {
        result.push_back((mask & (1 << i)) ? '1' : '0');
    }
    size_t pos = result.find_first_not_of('0');
    return (pos != string::npos) ? result.substr(pos) : "0";
}

// Получение размера региона по allocation granularity (обычно 64КБ)
SIZE_T GetDefaultRegionSize() {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwAllocationGranularity;
}

// получение информации о вычислительной системе
void GetSystemInformation() {
    SYSTEM_INFO si; //  структуру для хранения системных сведений.
    GetSystemInfo(&si);

    cout << "System Information:" << endl;
    string architecture;
    switch(si.wProcessorArchitecture) {  // тип Архитектуры процессора
        case PROCESSOR_ARCHITECTURE_INTEL:
            architecture = "x86 (32-bit)";
            break;
        case PROCESSOR_ARCHITECTURE_AMD64:
            architecture = "x64 (64-bit)";
            break;
        case PROCESSOR_ARCHITECTURE_ARM:
            architecture = "ARM";
            break;
        case PROCESSOR_ARCHITECTURE_ARM64:
            architecture = "ARM64";
            break;
        default:
            architecture = "Unknown";
            break;
    }
    cout << "  Processor Architecture: " << architecture << endl;
    cout << "  Page Size: " << si.dwPageSize << " bytes" << endl; // Размер страницы памяти в байтах (наименьшая единица выделения пямяти) Знание этого помогает в управлении памятью (например, выделение буфера должно быть кратно размеру страницы)
    cout << "  Minimum Application Address: " << si.lpMinimumApplicationAddress << endl; // Наименьший адрес памяти, который может использовать приложение.
    cout << "  Maximum Application Address: " << si.lpMaximumApplicationAddress << endl; //  Самый высокий адрес памяти, который может использовать приложение
    cout << "  Active Processor Mask: " << getBinaryMask(si.dwActiveProcessorMask) << endl; // Битовая маска, представляющая активные процессоры (0 -n 1 - y) Используется для привязки потоков (назначения процессов конкретным ядрам)
    cout << "  Number of Processors: " << si.dwNumberOfProcessors << endl; // общее количество логических процессоров (ядер или потоков)
    string processorType;
    switch(si.dwProcessorType) {  // Тип процессора (семейство) - устаревшее значение
        case PROCESSOR_INTEL_386:
            processorType = "Intel 386";
            break;
        case PROCESSOR_INTEL_486:
            processorType = "Intel 486";
            break;
        case PROCESSOR_INTEL_PENTIUM:
            processorType = "Intel Pentium";
            break;
        // For modern systems, use a custom mapping:
        case 8664:
            processorType = "x64 (AMD or Intel 64-bit)";
            break;
        default:
            processorType = "Unknown Processor Type";
            break;
    }
    cout << "  Processor Type: " << processorType << endl;
    cout << "  Allocation Granularity: " << si.dwAllocationGranularity << " bytes" << endl; // минимальный размер блока памяти, который операционная система Windows использует при резервировании или выделении виртуальной памяти через VirtualAlloc
    cout << "  Processor Level: " << si.wProcessorLevel << " (Processor family indicator)" << endl; // уровень процессора (поколение) в основном сохраняется для совместимости
    WORD revision = si.wProcessorRevision;
    BYTE model = revision >> 8;       // high-order byte
    BYTE stepping = revision & 0xFF;    // low-order byte

    cout << "  Processor Revision: " << "Model: " << (int)model 
        << ", Stepping: " << (int)stepping << endl; // Старший байт - версия процессора (семейство, конструкция), Байт младший - шаговый параметр (номер (версия) незначительной модификация или обновление в рамках той же модели процессора)
}

// определение статуса виртуальной памяти
void GetMemoryStatus() {
    MEMORYSTATUS memStatus; 
    GlobalMemoryStatus(&memStatus);

    cout << "Memory Status:" << endl;
    cout << "  Memory Load: " << memStatus.dwMemoryLoad << "%" << endl; // Процент используемой физической памяти
    cout << "  Total Physical Memory: " << memStatus.dwTotalPhys / (1024 * 1024) << " MB" << endl; // общий объем физической оперативной памяти (в байтах)
    cout << "  Available Physical Memory: " << memStatus.dwAvailPhys / (1024 * 1024) << " MB" << endl; // объем свободной оперативной памяти, доступной для приложений
    cout << "  Total Virtual Memory: " << memStatus.dwTotalVirtual / (1024 * 1024) << " MB" << endl; // общий объем виртуальной памяти (объединяющее оперативную память и пространство, предоставляемое файлом подкачки - вытеснение на диск допустим)
    cout << "  Available Virtual Memory: " << memStatus.dwAvailVirtual / (1024 * 1024) << " MB" << endl; // объем свободной виртуальной памяти, доступной приложению
}

// вывод информации о состоянии области памяти по заданному адресу
void QueryMemoryState(uintptr_t address) { // беззнаковое целое
    LPCVOID lpAddress = reinterpret_cast<LPCVOID>(address); // константный указатель на любой тип
    MEMORY_BASIC_INFORMATION mbi;
    SIZE_T result = VirtualQuery(lpAddress, &mbi, sizeof(mbi)); // Работает с отдельными страницами внутри региона
    
    if (result == 0) {
        cout << "VirtualQuery error. Error code: " << GetLastError() << endl;
        return;
    }
    
    cout << "\nMemory information for address " << hex << address << ":\n";
    cout << "  Base address: " << mbi.BaseAddress << endl; //  начало непрерывного региона виртуальной памяти
    cout << "  Allocation address: " << mbi.AllocationBase << endl; // точка, с которой началась общая операция выделения памяти
    cout << "  Region size: " << dec << mbi.RegionSize << " bytes" << endl; // размер области памяти, начинающейся с базового адреса
    
    // состоянии памяти
    cout << "  State: ";
    if (mbi.State == MEM_COMMIT) //  память реально выделена (готова к использованию для хранения данных)
        cout << "MEM_COMMIT";
    else if (mbi.State == MEM_RESERVE) // память зарезервирована, но не выделена (гарантирует его доступность в будущем)
        cout << "MEM_RESERVE"; 
    else if (mbi.State == MEM_FREE) // память свободна
        cout << "MEM_FREE";
    else
        cout << "Unknown";
    cout << endl;
    
    // защита, которая была задана при первоначальном выделении памяти
    cout << "  Allocation protection (AllocationProtect): ";
    switch (mbi.AllocationProtect) {
        case PAGE_EXECUTE:          cout << "PAGE_EXECUTE"; break; // Разрешено выполнение кода, но чтение и запись запрещены
        case PAGE_EXECUTE_READ:     cout << "PAGE_EXECUTE_READ"; break; // Разрешено выполнение кода и чтение данных, но запись запрещена
        case PAGE_EXECUTE_READWRITE:cout << "PAGE_EXECUTE_READWRITE"; break; // Разрешено выполнение кода, чтение и запись
        case PAGE_EXECUTE_WRITECOPY:cout << "PAGE_EXECUTE_WRITECOPY"; break; // Позволяет выполнять код и производить копирование при записи (разделять исходную страницу м/у несколькими процессами)
        case PAGE_NOACCESS:         cout << "PAGE_NOACCESS"; break; // Доступ к области памяти полностью запрещён
        case PAGE_READONLY:         cout << "PAGE_READONLY"; break; // Разрешено только чтение
        case PAGE_READWRITE:        cout << "PAGE_READWRITE"; break; // Разрешены и чтение, и запись
        case PAGE_WRITECOPY:        cout << "PAGE_WRITECOPY"; break; // Разрешено чтение, а запись выполняется по принципу "копировать при записи"
        default:                    cout << "Unknown"; break;
    }
    cout << endl;
    
    // текущая защита региона памяти (может изменится при импользовании VirtualProtect)
    cout << "  Current protection (Protect): ";
    switch (mbi.Protect) {
        case PAGE_EXECUTE:          cout << "PAGE_EXECUTE"; break;
        case PAGE_EXECUTE_READ:     cout << "PAGE_EXECUTE_READ"; break;
        case PAGE_EXECUTE_READWRITE:cout << "PAGE_EXECUTE_READWRITE"; break;
        case PAGE_EXECUTE_WRITECOPY:cout << "PAGE_EXECUTE_WRITECOPY"; break;
        case PAGE_NOACCESS:         cout << "PAGE_NOACCESS"; break;
        case PAGE_READONLY:         cout << "PAGE_READONLY"; break;
        case PAGE_READWRITE:        cout << "PAGE_READWRITE"; break;
        case PAGE_WRITECOPY:        cout << "PAGE_WRITECOPY"; break;
        default:                    cout << "Unknown"; break;
    }
    cout << endl;
    
    // тип региона памяти ( использованеи в виртуальной памяти)
    cout << "  Region type: ";
    switch (mbi.Type) {
        case MEM_IMAGE:   cout << "MEM_IMAGE"; break; // регион, связанный с образом (файлом) исполняемого модуля (содержит исполняемый файл)
        case MEM_MAPPED:  cout << "MEM_MAPPED"; break; // регион, отображённый в память (память связана с файлом, но не является исполняемым модулем)
        case MEM_PRIVATE: cout << "MEM_PRIVATE"; break; // частный регион, не связанный с файлом
        default:          cout << "Unknown"; break;
    }
    cout << endl;
}

// Автоматическое резервирование и выделение памяти с последующим запросом на освобождение
void SepAutomaticMemoryAllocation() {
    SIZE_T regionSize = GetDefaultRegionSize();
    
    // Резервирование региона памяти. NULL - система сама выберет адрес.
    LPVOID reservedRegion = VirtualAlloc(NULL, regionSize, MEM_RESERVE, PAGE_READWRITE);
    if (reservedRegion == NULL) {
        cout << "Error reserving memory. Error code: " << GetLastError() << endl;
        return;
    }
    cout << "Memory region reserved at address: " << reservedRegion << endl;

    // Коммит (выделение физической памяти) для ранее зарезервированного региона
    LPVOID committedRegion = VirtualAlloc(reservedRegion, regionSize, MEM_COMMIT, PAGE_READWRITE);
    if (committedRegion == NULL) {
        cout << "Error committing memory. Error code: " << GetLastError() << endl;
        VirtualFree(reservedRegion, 0, MEM_RELEASE);
        return;
    }
    cout << "Memory committed successfully." << endl;

    // Запрос на освобождение памяти
    string symb;
    cout << "Do you want to free the allocated memory? (y/n) ";
    cin >> symb;
    if (symb == "y" || symb == "Y") {
        FreeAllocatedMemory(reservedRegion);
    } else {
        cout << "Memory not freed. Make sure to free it later to avoid memory leaks!" << endl;
    }
}

// Ручное резервирование и коммит памяти с указанием начального адреса
void SepManualMemoryAllocation(uintptr_t address) {
    SIZE_T regionSize = GetDefaultRegionSize();

    // Резервирование памяти по заданному адресу
    LPVOID reservedRegion = VirtualAlloc(reinterpret_cast<LPVOID>(address), regionSize, MEM_RESERVE, PAGE_READWRITE);
    if (reservedRegion == NULL) {
        cout << "Error reserving memory at specified address. Error code: " << GetLastError() << endl;
        return;
    }
    cout << "Memory region reserved at address: " << reservedRegion << endl;

    // Коммит памяти в ранее зарезервированном регионе
    LPVOID committedRegion = VirtualAlloc(reservedRegion, regionSize, MEM_COMMIT, PAGE_READWRITE);
    if (committedRegion == NULL) {
        cout << "Error committing memory. Error code: " << GetLastError() << endl;
        VirtualFree(reservedRegion, 0, MEM_RELEASE);
        return;
    }
    cout << "Memory committed successfully." << endl;

    // Запрос на освобождение памяти
    string symb;
    cout << "Do you want to free the allocated memory? (y/n) ";
    cin >> symb;
    if (symb == "y" || symb == "Y") {
        FreeAllocatedMemory(reservedRegion);
    } else {
        cout << "Memory not freed. Make sure to free it later to avoid memory leaks!" << endl;
    }
}

// Одновременное резервирование и коммит памяти (Common Automatic)
void ComAutomaticMemoryAllocation() {
    SIZE_T regionSize = GetDefaultRegionSize();

    // Одновременное резервирование и коммит памяти
    LPVOID allocatedMemory = VirtualAlloc(NULL, regionSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (allocatedMemory == NULL) {
        cout << "Error reserving memory. Error code: "  << GetLastError() << endl;
        return;
    }
    cout << "Memory region reserved and committed at address: " << allocatedMemory << endl;

    // Запрос на освобождение памяти
    string symb;
    cout << "Do you want to free the allocated memory? (y/n) ";
    cin >> symb;
    if (symb == "y" || symb == "Y") {
        FreeAllocatedMemory(allocatedMemory);
    } else {
        cout << "Memory not freed. Make sure to free it later to avoid memory leaks!" << endl;
    }
}

// Одновременное резервирование и коммит памяти с указанием начального адреса (Common Manual)
void ComManualMemoryAllocation(uintptr_t address) {
    SIZE_T regionSize = GetDefaultRegionSize();

    // Резервирование памяти по заданному адресу
    LPVOID allocatedMemory = VirtualAlloc(reinterpret_cast<LPVOID>(address), regionSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (allocatedMemory == NULL) {
        cout << "Error reserving memory at specified address. Error code: " << GetLastError() << endl;
        return;
    }
    cout << "Memory region reserved and committed at address: " << allocatedMemory << endl;

    // Запрос на освобождение памяти
    string symb;
    cout << "Do you want to free the allocated memory? (y/n) ";
    cin >> symb;
    if (symb == "y" || symb == "Y") {
        FreeAllocatedMemory(allocatedMemory);
    } else {
        cout << "Memory not freed. Make sure to free it later to avoid memory leaks!" << endl;
    }
}

// Запись данных в память по указанному адресу
void WriteToMemoryAddress(uintptr_t address, const string &data) {
    // Рассчитываем требуемый размер (включая завершающий нулевой символ)
    size_t requiredSize = data.length() + 1;

    // Приводим введённый адрес к указателю на char
    char* ptr = reinterpret_cast<char*>(address);

    // Проверяем доступность памяти для записи
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(reinterpret_cast<LPCVOID>(address), &mbi, sizeof(mbi)) == 0) {
        cout << "Error querying memory state. Error code: " << GetLastError() << endl;
        return;
    }

   // Проверяем, доступна ли память для записи
if (!(mbi.Protect & PAGE_READWRITE) && !(mbi.Protect & PAGE_EXECUTE_READWRITE)) {
    cout << "Error: Memory at address " << reinterpret_cast<void*>(address)
         << " is not writable.\nCurrent protection flags: ";

    switch (mbi.Protect) {
        case PAGE_NOACCESS: cout << "PAGE_NOACCESS (No access)"; break;
        case PAGE_READONLY: cout << "PAGE_READONLY (Read-only)"; break;
        case PAGE_READWRITE: cout << "PAGE_READWRITE (Read/Write)"; break;
        case PAGE_WRITECOPY: cout << "PAGE_WRITECOPY (Write-copy)"; break;
        case PAGE_EXECUTE: cout << "PAGE_EXECUTE (Execute-only)"; break;
        case PAGE_EXECUTE_READ: cout << "PAGE_EXECUTE_READ (Execute/Read)"; break;
        case PAGE_EXECUTE_READWRITE: cout << "PAGE_EXECUTE_READWRITE (Execute/Read/Write)"; break;
        case PAGE_EXECUTE_WRITECOPY: cout << "PAGE_EXECUTE_WRITECOPY (Execute/Write-copy)"; break;
        default: cout << "Unknown protection code: " << mbi.Protect; break;
    }
    
    cout << "\nMemory cannot be modified. Operation aborted." << endl;
    return;
}


    // Пытаемся записать данные в память
    if (ptr != nullptr) {
        copy(data.begin(), data.end(), ptr);
        ptr[data.length()] = '\0'; // Добавляем завершающий нулевой символ
    } else {
        cout << "Error: Null pointer. Cannot write to memory." << endl;
        return;
    }

    // Выводим записанные данные
    cout << "Data stored in memory at address " << reinterpret_cast<void*>(address) << ": " << ptr << endl;

    // Запрос на освобождение памяти
    string symb;
    cout << "Do you want to free the used memory? (y/n) ";
    cin >> symb;
    if (symb == "y") {
        // Приводим адрес к типу LPVOID и освобождаем память
        LPVOID memPtr = reinterpret_cast<LPVOID>(address);
        FreeAllocatedMemory(memPtr);
    } else {
        cout << "Memory not freed. Make sure to free it later to avoid memory leaks!" << endl;
    }
}

// Установка и проверка защиты региона памяти без выделения памяти
void SetMemoryProtection(uintptr_t address) {
    // Получаем regionSize равным allocation granularity
    SIZE_T regionSize = GetDefaultRegionSize();

    // Проверка текущей защиты памяти
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(reinterpret_cast<LPCVOID>(address), &mbi, sizeof(mbi)) == 0) {
        cout << "Error querying memory state. Error code: " << GetLastError() << endl;
        return;
    }
    cout << "Current protection before change: ";
    switch (mbi.Protect) {
        case PAGE_NOACCESS: cout << "PAGE_NOACCESS"; break;
        case PAGE_READONLY: cout << "PAGE_READONLY"; break;
        case PAGE_READWRITE: cout << "PAGE_READWRITE"; break;
        case PAGE_EXECUTE: cout << "PAGE_EXECUTE"; break;
        case PAGE_EXECUTE_READ: cout << "PAGE_EXECUTE_READ"; break;
        case PAGE_EXECUTE_READWRITE: cout << "PAGE_EXECUTE_READWRITE"; break;
        default: cout << "Unknown"; break;
    }
    cout << endl;
    
    // Выбор типа защиты
    cout << "1. PAGE_NOACCESS (access denied)" << endl;
    cout << "2. PAGE_READONLY (read-only)" << endl;
    cout << "3. PAGE_READWRITE (read and write)" << endl;
    cout << "4. PAGE_EXECUTE (execute code)" << endl;
    cout << "5. PAGE_EXECUTE_READ (execute and read)" << endl;
    cout << "6. PAGE_EXECUTE_READWRITE (execute, read, and write)" << endl;
    cout << "Choose the type of protection to set: ";
    
    int choice;
    cin >> choice;
    
    DWORD protection;
    switch (choice) {
        case 1: protection = PAGE_NOACCESS; break;
        case 2: protection = PAGE_READONLY; break;
        case 3: protection = PAGE_READWRITE; break;
        case 4: protection = PAGE_EXECUTE; break;
        case 5: protection = PAGE_EXECUTE_READ; break;
        case 6: protection = PAGE_EXECUTE_READWRITE; break;
        default:
            cout << "Error: Invalid choice." << endl;
            return;
    }
    
    // Установка защиты памяти
    DWORD oldProtect;
    if (VirtualProtect(reinterpret_cast<LPVOID>(address), regionSize, protection, &oldProtect)) {
        cout << "Protection successfully set." << endl;
    } else {
        cout << "Error setting protection. Error code: " << GetLastError() << endl;
        return;
    }
    
    // Запрос на вывод информации о состоянии памяти по этому адресу
    string symb;
    cout << "Do you want to see memory state information? (y/n) ";
    cin >> symb;
    if (symb == "y" || symb == "Y") {
        QueryMemoryState(address);
    }
    
    // Запрос на освобождение памяти
    cout << "Do you want to free the used memory? (y/n) ";
    cin >> symb;
    if (symb == "y" || symb == "Y") {
        LPVOID memPtr = reinterpret_cast<LPVOID>(address);
        FreeAllocatedMemory(memPtr);
    } else {
        cout << "Memory not freed. Make sure to free it later to avoid memory leaks!" << endl;
    }
}

// Функция для освобождения выделенной памяти (регион, базовый адресс вводим)
void FreeAllocatedMemory(LPVOID reservedRegion) {
    if (VirtualFree(reservedRegion, 0, MEM_RELEASE)) {
        cout << "Memory freed successfully." << endl;
    } else {
        cout << "Error freeing memory. Error code: " << GetLastError() << endl;
    }
}

