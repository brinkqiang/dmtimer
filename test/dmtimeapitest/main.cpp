
#include "gtest.h"
#include "dmutil.h"
#include "dmtimermodule.h"
#include <chrono>

static inline uint32_t GetTickCount32_CLOCK_REALTIME_COARSE() {
#ifdef WIN32
    return ::GetTickCount();
#elif __APPLE__
    struct timespec ts = { 0 };
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#else
    struct timespec ts = { 0 };
    clock_gettime(CLOCK_REALTIME_COARSE, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#endif
}

static inline uint32_t GetTickCount32_CLOCK_REALTIME() {
#ifdef WIN32
    return ::GetTickCount();
#elif __APPLE__
    struct timespec ts = { 0 };
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#else
    struct timespec ts = { 0 };
    clock_gettime(CLOCK_REALTIME, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#endif
}

static inline uint32_t GetTickCount32_CLOCK_MONOTONIC() {
#ifdef WIN32
    return ::GetTickCount();
#elif __APPLE__
    struct timespec ts = { 0 };
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#else
    struct timespec ts = { 0 };
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#endif
}

static inline uint32_t GetTickCount32_gettimeofday() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

static inline uint32_t GetTickCount32_chrono_system_clock() {
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}
static inline uint32_t GetTickCount32_chrono_steady_clock() {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}

static inline uint32_t GetTickCount32_high_resolution_clock() {
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}

#define TIME_TEST_COUNT (10 * 1000 * 1000)

#ifndef WIN32
TEST(clock_realtime_coarse, clock_realtime_coarse)
{
    uint64_t qwCount = 0;
    for (int i=0; i < TIME_TEST_COUNT; ++i)
    {
        qwCount += GetTickCount32_CLOCK_REALTIME_COARSE();
    }
    std::cout << qwCount << std::endl;
}

TEST(clock_realtime, clock_realtime)
{
    uint64_t qwCount = 0;
    for (int i = 0; i < TIME_TEST_COUNT; ++i)
    {
        qwCount += GetTickCount32_CLOCK_REALTIME();
    }
    std::cout << qwCount << std::endl;
}

TEST(clock_monotonic, clock_monotonic)
{
    uint64_t qwCount = 0;
    for (int i = 0; i < TIME_TEST_COUNT; ++i)
    {
        qwCount += GetTickCount32_CLOCK_MONOTONIC();
    }
    std::cout << qwCount << std::endl;
}
#else
TEST(GetTickCount, GetTickCount)
{
    uint64_t qwCount = 0;
    for (int i = 0; i < TIME_TEST_COUNT; ++i)
    {
        qwCount += GetTickCount32();
    }
    std::cout << qwCount << std::endl;
}
#endif

TEST(gettimeofday, gettimeofday)
{
    uint64_t qwCount = 0;
    for (int i = 0; i < TIME_TEST_COUNT; ++i)
    {
        qwCount += GetTickCount32_gettimeofday();
    }
    std::cout << qwCount << std::endl;
}

TEST(chrono_system_clock, chrono_system_clock)
{
    uint64_t qwCount = 0;
    for (int i = 0; i < TIME_TEST_COUNT; ++i)
    {
        qwCount += GetTickCount32_chrono_system_clock();
    }
    std::cout << qwCount << std::endl;
}

TEST(chrono_steady_clock, chrono_steady_clock)
{
    uint64_t qwCount = 0;
    for (int i = 0; i < TIME_TEST_COUNT; ++i)
    {
        qwCount += GetTickCount32_chrono_steady_clock();
    }
    std::cout << qwCount << std::endl;
}

TEST(high_resolution_clock, high_resolution_clock)
{
    uint64_t qwCount = 0;
    for (int i = 0; i < TIME_TEST_COUNT; ++i)
    {
        qwCount += GetTickCount32_high_resolution_clock();
    }
    std::cout << qwCount << std::endl;
}