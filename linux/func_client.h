#ifndef FUNC_CLIENT_H
#define FUNC_CLIENT_H

#include <iostream>
#include <sys/select.h>  // Для select()
#include <fcntl.h>      // для open()
#include <sys/mman.h>   // для mmap() и munmap()
#include <sys/stat.h>   // для работы с правами доступа
#include <unistd.h>     // для close()
#include <cstring>      // для работы со строками

// Определяем константы: имя файла и его размер
const char* const FILENAME = "shared_memory.txt";
const size_t FILESIZE = 1024;  // размер файла в байтах

// Функция для отображения файла в память
bool mapFileClient();

// Функция для чтения данных из отображённой области
void readData();

// Функция для отмены отображения файла (unmap)
void cleanupClient();

#endif 
