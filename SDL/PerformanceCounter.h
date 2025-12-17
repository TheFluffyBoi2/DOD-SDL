#pragma once
#include <chrono>
#include <iostream>

class PerformanceCounter
{
public:
	std::string name;
	std::chrono::high_resolution_clock::time_point start;
	long long aggregated_time;
	int count;

	PerformanceCounter(std::string n)
		: name{n}, aggregated_time{ 0 }, count{ 0 } {}

	void Start();
	void End();
	void Reset();
	void PrintAverage();
};

