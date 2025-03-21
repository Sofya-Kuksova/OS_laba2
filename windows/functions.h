#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include <windows.h>
#include <iostream>
#include <string>
#include <cstdint>
#include <iomanip>

void GetSystemInformation();

void GetMemoryStatus();

void QueryMemoryState(uintptr_t address);

void SepAutomaticMemoryAllocation();

void SepManualMemoryAllocation(uintptr_t address);

void ComAutomaticMemoryAllocation();

void ComManualMemoryAllocation(uintptr_t address);

void WriteToMemoryAddress(uintptr_t address, const std::string &data);

void SetMemoryProtection(uintptr_t address);

void FreeAllocatedMemory(LPVOID reservedRegion);

#endif 
