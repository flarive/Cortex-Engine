#pragma once

#include <windows.h>

// Auto select Nvidia or AMD GPU instead of builtin intel GPU
#define FORCE_FASTEST_GPU

#ifdef FORCE_FASTEST_GPU
extern "C" {
    __declspec(dllexport) uint32_t NvOptimusEnablement = 1; // NVidia
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1; // AMD
}
#else
extern "C" {
    __declspec(dllexport) uint32_t NvOptimusEnablement = 0;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 0;
}
#endif