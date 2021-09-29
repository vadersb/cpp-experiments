//
// Created by Alexander on 29.09.2021.
//

#include "main.h"
#include <iostream>
#include "memory_pool.h"

void SimpleTemplateTest();


int main()
{
	SimpleTemplateTest();

//	st::memory::MemoryPoolSingleThreaded::Init();
//
//	st::memory::MemoryPoolSingleThreaded::Release();

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