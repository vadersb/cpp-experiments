//
// Created by Alexander on 29.09.2021.
//

#include "main.h"
#include <iostream>
#include "memory_pool.h"
#include "spdlog/spdlog.h"

void BenchmarkRun(int runNumber);
void SimpleTemplateTest();

enum class AllocType
{
	Std,
	PoolSingleThreaded,
	PoolMultiThreaded
};


int main()
{
	SimpleTemplateTest();

	st::memory::MemoryPoolSingleThreaded::Init();
	st::memory::MemoryPoolMultiThreaded::Init();

	const int BenchmarkRuns = 10;

	for (int i = 0; i < BenchmarkRuns; i++)
	{
		BenchmarkRun(i);
	}

	st::memory::MemoryPoolMultiThreaded::Release();
	st::memory::MemoryPoolSingleThreaded::Release();

	std::getchar();

	return 0;
}


void SimpleTemplateTest()
{
	SetGlobalValue<true>(10);
	SetGlobalValue<false>(-5);

	SetTrueGlobalValue(20);

	std::cout << "[flag:true]  " << GetGlobalValue<true>() << "\n";
	std::cout << "[flag:false] " << GetGlobalValue<false>() << "\n";


}

template<typename T, AllocType allocType> T* Allocate()
{
	if constexpr(allocType == AllocType::Std)
	{
		return new T();
	}
	else
	{
		if constexpr(allocType == AllocType::PoolSingleThreaded)
		{
			return st::memory::MemoryPoolSingleThreaded::Allocate<T>();
		}
		else
		{
			return st::memory::MemoryPoolMultiThreaded::Allocate<T>();
		}
	}
}


template<typename T, AllocType allocType> void Deallocate(T* p)
{
	if constexpr(allocType == AllocType::Std)
	{
		delete p;
	}
	else
	{
		if constexpr(allocType == AllocType::PoolSingleThreaded)
		{
			st::memory::MemoryPoolSingleThreaded::Deallocate(p, sizeof(T));
		}
		else
		{
			st::memory::MemoryPoolMultiThreaded::Deallocate(p, sizeof(T));
		}
	}
}


template<AllocType allocType, typename T> void AllocateSomeObjects(std::vector<T*>& container, int amountToAllocate)
{
	for (int i = 0; i < amountToAllocate; i++)
	{
		auto p = Allocate<T, allocType>();
		container.push_back(p);
	}
}

template<AllocType allocType, typename T> void DeallocateAllObjects(std::vector<T*>& container)
{
	auto size = container.size();

	for (int i = 0; i < size; i++)
	{
		auto p = container[i];
		Deallocate<T, allocType>(p);
	}

	container.clear();
}



template<AllocType allocType> void Benchmark(int iterations)
{
	using smallObject = std::aligned_storage<8>::type;
	using mediumObject = std::aligned_storage<64>::type;
	using largeObject = std::aligned_storage<1024+512+256>::type;

	std::vector<smallObject*> smallPointers;
	smallPointers.reserve(iterations);

	std::vector<mediumObject*> mediumPointers;
	mediumPointers.reserve(iterations);

	std::vector<largeObject*> largePointers;
	largePointers.reserve(iterations);

	for (int i = 0; i < iterations; i++)
	{
		//-----
		//allocating
		int amount = (i % 100) + 2;

		if (i % 10 == 0)
		{
			//large objects
			AllocateSomeObjects<allocType>(largePointers, amount);
		}
		else
		{
			if (i % 4 == 0)
			{
				//medium objects
				AllocateSomeObjects<allocType>(mediumPointers, amount);
			}
			else
			{
				//small objects
				AllocateSomeObjects<allocType>(smallPointers, amount);
			}
		}

		//-----
		//deallocating
		if (i % 96 == 0)
		{
			//large objects
			DeallocateAllObjects<allocType>(largePointers);
		}

		if (i % 179 == 0)
		{
			//medium objects
			DeallocateAllObjects<allocType>(mediumPointers);
		}

		if (i % 512 == 0)
		{
			//small objects
			DeallocateAllObjects<allocType>(smallPointers);
		}
	}

	//-----
	//cleanup

	//large objects
	DeallocateAllObjects<allocType>(largePointers);

	//medium objects
	DeallocateAllObjects<allocType>(mediumPointers);

	//small objects
	DeallocateAllObjects<allocType>(smallPointers);
}


template<typename TimePoint>
auto GetDurationInMicroseconds(TimePoint from, TimePoint to)
{
	auto duration = to - from;
	return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
}


void BenchmarkRun(int runNumber)
{
	spdlog::info("BENCHMARK RUN {}", runNumber + 1);

	const int BenchmarkIterationsCount = 500000;

	//std
	{
		auto timeStart = std::chrono::high_resolution_clock::now();
		Benchmark<AllocType::Std>(BenchmarkIterationsCount);
		auto timeEnd = std::chrono::high_resolution_clock::now();
		spdlog::info("              std time: {}", GetDurationInMicroseconds(timeStart, timeEnd));
	}

	//memory pool single threaded
	{
		auto timeStart = std::chrono::high_resolution_clock::now();
		Benchmark<AllocType::PoolSingleThreaded>(BenchmarkIterationsCount);
		auto timeEnd = std::chrono::high_resolution_clock::now();
		spdlog::info("   memory pool ST time: {}", GetDurationInMicroseconds(timeStart, timeEnd));
	}

	//memory pool multithreaded
	{
		auto timeStart = std::chrono::high_resolution_clock::now();
		Benchmark<AllocType::PoolMultiThreaded>(BenchmarkIterationsCount);
		auto timeEnd = std::chrono::high_resolution_clock::now();
		spdlog::info("   memory pool MT time: {}", GetDurationInMicroseconds(timeStart, timeEnd));
	}

	spdlog::info(" ");

}











