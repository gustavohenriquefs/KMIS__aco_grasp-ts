#pragma once

// Macros comuns usados em todo o projeto
#define sz(v) ((int)v.size())
#define get_current_time() std::chrono::high_resolution_clock::now()
#define TIME_DIFF(start, end) std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
