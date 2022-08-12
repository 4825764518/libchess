#pragma once

#include <cassert>

#ifdef _MSC_VER
#define CHESSLIB_MSVC 1
#include <intrin.h>
#else
// TODO
#define CHESSLIB_GCC 1
#endif

#include <cstdint>

namespace chess {
namespace platform {
inline int popcount(uint64_t data) {
#if CHESSLIB_MSVC
  return static_cast<int>(__popcnt64(data));
#elif CHESSLIB_GCC
  return __builtin_popcountll(data);
#else
#error Unsupported compiler
#endif
}

inline int find_first_set(uint64_t data) {
#if CHESSLIB_MSVC
  unsigned long result = 0;
  _BitScanForward64(&result, data);
  return result;
#elif CHESSLIB_GCC
  return __builtin_ffsll(data);
#else
#error Unsupported compiler
#endif
}

inline int find_last_set(uint64_t data) {
#if CHESSLIB_MSVC
  unsigned long result = 0;
  _BitScanReverse64(&result, data);
  return result;
#elif CHESSLIB_GCC
#error TODO: gcc find_last_set
#else
#error Unsupported compiler
#endif
}

inline int count_leading_zeroes(uint64_t data) {
#if CHESSLIB_MSVC
  constexpr auto num_bits = sizeof(data) * 8;
  unsigned long scan_result = 0;
  if (_BitScanReverse64(&scan_result, data)) {
    return (num_bits - 1) - scan_result;
  } else {
    return num_bits;
  }
#elif CHESSLIB_GCC
  return __builtin_clzll(data);
#else
#error Unsupported compiler
#endif
}
} // namespace platform
} // namespace chess