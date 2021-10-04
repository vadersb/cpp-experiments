//
// Created by Alexander Samarin on 06.08.2021.
//


#include <cstdlib>
#include "memory_pool_bucket.h"
#include <cassert>
#include "spdlog/spdlog.h"

namespace st::memory
{

	MemoryPoolBucket::MemoryPoolBucket() :
			m_ItemSize(0),
			m_FirstPageItemsCount(0),
			m_ExtraPageItemsCount(0),
			m_Pages(),
			m_Items()
	{

	}

	MemoryPoolBucket::~MemoryPoolBucket()
	{
		if (GetFreeItemsCount() != GetTotalItemsCount())
		{
			int unreleasedCount = GetTotalItemsCount() - GetFreeItemsCount();
			spdlog::error("Memory Pool: unreleased pointers ({} in total) for bucket with item size {}", unreleasedCount, m_ItemSize);
			return;
		}

		for (auto pPage : m_Pages)
		{
			std::free(pPage);
		}
	}


	void MemoryPoolBucket::Setup(const MemoryPoolSettings::BucketDefinition &bucketDefinition)
	{
		m_ItemSize = bucketDefinition.m_ItemSize;
		m_FirstPageItemsCount = bucketDefinition.m_FirstPageItemsCount;
		m_ExtraPageItemsCount = bucketDefinition.m_ExtraPageItemsCount;

		assert(m_ItemSize > 0);
		assert(m_FirstPageItemsCount > 0);
		assert(m_ExtraPageItemsCount > 0);

		assert(m_Pages.size() == 0);
		assert(m_Items.size() == 0);

		//validating item size
		if (m_ItemSize < sizeof(std::max_align_t))
		{
			if (m_ItemSize != 2 && m_ItemSize != 4 && m_ItemSize != 8)
			{
				spdlog::warn("Memory pool: inefficient bucket item size: {}", m_ItemSize);
			}
		}

		m_Pages.reserve(32);
		m_Items.reserve(m_FirstPageItemsCount + m_ExtraPageItemsCount);

		if (bucketDefinition.m_PreWarmFirstPage)
		{
			AddPage();
		}
	}


	void* MemoryPoolBucket::Allocate()
	{
		if (m_Items.empty())
		{
			AddPage();
		}

		assert(m_Items.empty() == false);

		auto pResult = m_Items.back();
		m_Items.pop_back();

		return pResult;
	}


	void MemoryPoolBucket::Deallocate(void* p)
	{
		//check that the address is within pages
#ifdef MEMORY_POOL_CHECK_ADDRESS_BOUNDS
		assert(CheckIfAddressIsWithinPages(p) == true);
#endif

		//check that the address is not already in m_Items
#ifdef MEMORY_POOL_CHECK_ADDRESS_BOUNDS
		auto searchResult = std::find(std::begin(m_Items), std::end(m_Items), p);
		assert(searchResult == std::end(m_Items));
#endif

		m_Items.push_back(p);

		assert(GetFreeItemsCount() <= GetTotalItemsCount());
	}


	void MemoryPoolBucket::AddPage()
	{
		int itemsCount;
		bool pageIsExtra;

		if (m_Pages.empty())
		{
			itemsCount = m_FirstPageItemsCount;
			pageIsExtra = false;
		}
		else
		{
			itemsCount = m_ExtraPageItemsCount;
			pageIsExtra = true;
		}

		int pageSize = itemsCount * m_ItemSize;

		void* pNewPage = std::malloc(pageSize);

		//adding new items
		for (int i = 0; i < itemsCount; i++)
		{
			auto pItem = static_cast<char*>(pNewPage) + i * m_ItemSize;
			m_Items.push_back(pItem);
		}

		//adding page to pages
		m_Pages.push_back(pNewPage);

		if (pageIsExtra)
		{
			spdlog::info("Memory pool: adding extra page for item size [{}], new pages count: [{}].", m_ItemSize, m_Pages.size());
		}
	}


	bool MemoryPoolBucket::CheckIfAddressIsWithinPages(void* p) const
	{
		auto pagesCount = m_Pages.size();

		for (int i = 0; i < pagesCount; i++)
		{
			int pageSize = GetPageSize(i == 0);

			auto pPage = m_Pages[i];

			if (p >= pPage && p < static_cast<char*>(pPage) + pageSize)
			{
				return true;
			}
		}

		return false;
	}


	int MemoryPoolBucket::GetTotalItemsCount() const
	{
		int result = 0;

		auto pagesCount = m_Pages.size();

		if (pagesCount > 0)
		{
			result += m_FirstPageItemsCount;
		}

		if (pagesCount > 1)
		{
			result += (pagesCount - 1) * m_ExtraPageItemsCount;
		}

		return result;
	}


	int MemoryPoolBucket::GetFreeItemsCount() const
	{
		return (int)m_Items.size();
	}


	int MemoryPoolBucket::GetTotalMemoryUsed() const
	{
		return GetTotalItemsCount() * m_ItemSize;
	}


	int MemoryPoolBucket::GetAlignment([[maybe_unused]] int itemSize)
	{
		return sizeof(std::max_align_t);
	}


}
