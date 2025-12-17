#include "PerformanceCounter.h"

void PerformanceCounter::Start() {
	start = std::chrono::high_resolution_clock::now();
}

void PerformanceCounter::End() {
	auto end = std::chrono::high_resolution_clock::now();
	long long ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

	aggregated_time += ns;
	count++;
}

void PerformanceCounter::Reset() {
	aggregated_time = 0;
	count = 0;
}

void PerformanceCounter::PrintAverage() {
	if (count >= 10) {
		std::cout << "Function " << name << " average: " << aggregated_time / count << '\n';
	}
}