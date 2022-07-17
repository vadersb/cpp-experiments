//
// Created by Alexander on 22.09.2021.
//

#include "main.h"
#include "spdlog/spdlog.h"

int main()
{
	spdlog::info("Threading tasks experiment:");

	//todo
	//* parallel tasks
	//* sequential tasks sharing common memory buffer

	//backends
	//   worker thread
	//   async
	//   enkiTS https://github.com/dougbinks/enkiTS
	//   https://github.com/cameron314/concurrentqueue
#ifdef USELIB_ENKITS
	Test_EnkiTS();
#endif
}