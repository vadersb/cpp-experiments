//
// Created by Alexander on 24.09.2021.
//

#include <vector>
#include <future>
#include <thread>
#include <iostream>
#include <set>

const int NumberOfTasks = 5000;

std::atomic_int g_Counter = 0;

std::set<std::thread::id> g_UsedThreads;
std::mutex g_Mutex;

void RegisterThreadID(std::thread::id threadID)
{
	std::lock_guard<std::mutex> guard(g_Mutex);
	g_UsedThreads.insert(threadID);
}


int SomeLongTask(char displayChar, int inputValue, int sleepTime)
{
	//std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));

	const int NumberOfIterations = 10000000;

	int result = 0;

	for (int i = 0; i < NumberOfIterations; i++)
	{
		result += i % 123456;

		if (result > NumberOfIterations)
		{
			if (result % 2 == 0)
			{
				result /= 100;
			}
			else
			{
				result /= 27;
			}
		}
	}

	auto threadID = std::this_thread::get_id();

	std::cout << displayChar << " " << threadID << "\n";
	g_Counter++;
	RegisterThreadID(threadID);
	return result;
}


void AsyncStressTest()
{
	auto timeStart = std::chrono::steady_clock::now();

	std::vector<std::future<int>> futures;

	bool LaunchAsync = true;

	long int totalResult = 0;

	if (LaunchAsync == true)
	{
		for (int i = 0; i < NumberOfTasks; i++)
		{
			int sleepTime = (i * 5) % 73;

			futures.emplace_back(std::async(std::launch::async, SomeLongTask,'a' + i % 150, i, sleepTime));
		}

		while(g_Counter < NumberOfTasks)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			std::cout << "\n";
		}

		for (int i = 0; i < NumberOfTasks; i++)
		{
			totalResult += futures[i].get();
		}
	}
	else
	{
		for (int i = 0; i < NumberOfTasks; i++)
		{
			int sleepTime = (i * 5) % 73;
			totalResult += SomeLongTask('a' + i % 150, i, sleepTime);
		}
	}

	auto timeEnd = std::chrono::steady_clock::now();

	auto duration = timeEnd - timeStart;
	auto durationInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);

	std::cout << "\n\nASYNC TEST DONE. Total result: " << totalResult << "\n" << "Duration: " << durationInMilliseconds.count() << "\n";
	std::cout << "Used threads amount :" << g_UsedThreads.size() << "\n";
}


