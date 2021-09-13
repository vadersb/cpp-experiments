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
	st::memory::MemoryPoolInit(true);

	DelegateTypesTest();


	//cleanup
	st::memory::MemoryPoolRelease();

	return 0;
}

