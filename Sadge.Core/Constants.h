#pragma once

#include <stdint.h>

constexpr static auto KILOBYTES_TO_BYTES = 1024;

constexpr static double  T_RATE = (1 << 22);

constexpr static double  M_RATE = T_RATE / 4;
constexpr static double  M_RATE_4 = M_RATE / 4;

constexpr static uint8_t DEFAULT_READ = 0xFF;