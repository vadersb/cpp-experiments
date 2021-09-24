//
// Created by Alexander on 24.09.2021.
//

#include <iostream>
#include "spdlog/spdlog.h"
#include "main.h"
#include <thread>

int main()
{
	spdlog::info("Threading std stuff experiment.");

	auto numberOfThreads = std::thread::hardware_concurrency();

	spdlog::info("{} threads available.", numberOfThreads);

	AsyncStressTest();

	//todo test enkiTS
	//todo test taskflow

}