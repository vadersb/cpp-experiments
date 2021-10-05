//
// Created by Alexander on 11.09.2021.
//

#include "delegate.h"
#include <iostream>
#include <memory_pool.h>
#include "delegate_types.h"

int main()
{
	//init
	st::memory::MemoryPoolSingleThreaded::Init();
	st::memory::MemoryPoolMultiThreaded::Init();

	DelegateTypesTest();
	DelegatePoolableDebugTest();
	DelegateEqualityTest();

	//cleanup
	st::memory::MemoryPoolMultiThreaded::Release();
	st::memory::MemoryPoolSingleThreaded::Release();

	return 0;
}

