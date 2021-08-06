//
// Created by Alexander Samarin on 06.08.2021.
//

#pragma once

#include <vector>
#include <stack>


namespace st::memory
{
	class MemoryPoolBucket
	{
	public:

		explicit MemoryPoolBucket(int itemSize, int itemsPerPage, bool preWarm);

		~MemoryPoolBucket();

		void* Allocate();
		void Deallocate(void* p);

		int GetTotalItemsCount();
		int GetFreeItemsCount();

	private:

		void AddPage();


		int m_ItemSize;
		int m_ItemsPerPage;
		int m_SizePerPage;

		std::vector<void*> m_Pages;
		std::stack<void*> m_Items;
	};
}

