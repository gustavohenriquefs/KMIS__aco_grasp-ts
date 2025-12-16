#pragma once

#include <iostream>
#include <ostream>
#include <chrono>
using TimePoint = std::chrono::steady_clock::time_point;
#include "io_perf.cpp"

// Macros comuns usados em todo o projeto
#define sz(v) ((int)v.size())
#define get_current_time() std::chrono::steady_clock::now()
#define TIME_DIFF(start, end) std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()

#ifdef DEBUG
using std::cerr;
using std::cout;
using std::endl;
#else
#define cout null_stream
#define cerr null_stream
#define endl "\n"
#endif