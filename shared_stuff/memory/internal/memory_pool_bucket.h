//
// Created by Alexander Samarin on 06.08.2021.
//

#pragma once

#include <vector>


namespace st::memory
{
	class MemoryPoolBucket
	{
	public:

		explicit MemoryPoolBucket(int itemSize, int itemsPerPage, bool preWarm);

		~MemoryPoolBucket();

		void* Allocate();
		void Deallocate(void* p);

		[[nodiscard]] int GetTotalItemsCount() const;
		[[nodiscard]] int GetFreeItemsCount() const;
		[[nodiscard]] int GetTotalMemoryUsed() const;

	private:

		void AddPage();

		bool CheckIfAddressIsWithinPages (void* p) const;


		int m_ItemSize;
		int m_ItemsPerPage;
		int m_SizePerPage;

		std::vector<void*> m_Pages;
		std::vector<void*> m_Items;
	};

	//todo new bucket for new memory pool
	//later: *potential* performance increase to have m_CachedItems along with m_Items for data locality
}

