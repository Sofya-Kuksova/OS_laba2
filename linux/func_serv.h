#ifndef FUNC_SERV_H
#define FUNC_SERV_H

#include <string>
#include <iostream>
#include <fcntl.h>      // для open()
#include <sys/mman.h>   // для mmap() и munmap()
#include <sys/stat.h>   // для ftruncate()
#include <unistd.h>     // для close() и unlink()
#include <cstring>      // для strncpy()

// Константы для имени файла и его размера
const char* const FILENAME = "shared_memory.txt";
const size_t FILESIZE = 1024;  // размер файла в байтах

// Функция для отображения файла в память
bool mapFile();

// Функция для записи данных в отображённую область
void writeData();

// Функция для отмены отображения (unmap) и удаления файла
void cleanup();

#endif
