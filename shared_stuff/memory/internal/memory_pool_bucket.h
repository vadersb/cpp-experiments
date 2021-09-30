//
// Created by Alexander Samarin on 06.08.2021.
//

#pragma once

#include <vector>
#include "memory_pool_settings.h"


namespace st::memory
{

	class MemoryPoolBucket
	{
		//later: *potential* performance increase to have m_CachedItems along with m_Items for data locality

	public:

		MemoryPoolBucket();
		~MemoryPoolBucket();

		void Setup(const MemoryPoolSettings::BucketDefinition& bucketDefinition);

		[[nodiscard]] void* Allocate();
		void Deallocate(void* p);

		[[nodiscard]] int GetTotalItemsCount() const;
		[[nodiscard]] int GetFreeItemsCount() const;
		[[nodiscard]] int GetTotalMemoryUsed() const;
		[[nodiscard]] inline int GetItemSize() const
		{
			return m_ItemSize;
		}

	private:

		void AddPage();

		bool CheckIfAddressIsWithinPages (void* p) const;

		[[nodiscard]] inline int GetPageSize(bool isFirst) const
		{
			if (isFirst)
			{
				return m_FirstPageItemsCount * m_ItemSize;
			}
			else
			{
				return m_ExtraPageItemsCount * m_ItemSize;
			}
		}

		[[maybe_unused]]
		static int GetAlignment([[maybe_unused]] int itemSize);

		int m_ItemSize;
		int m_FirstPageItemsCount;
		int m_ExtraPageItemsCount;

		std::vector<void*> m_Pages;
		std::vector<void*> m_Items;
	};

}

