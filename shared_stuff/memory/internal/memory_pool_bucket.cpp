//
// Created by Alexander Samarin on 06.08.2021.
//

#include <iostream>
#include "memory_pool_bucket.h"
#include <cassert>
#include "spdlog/spdlog.h"

namespace st::memory
{
	MemoryPoolBucket::MemoryPoolBucket(int itemSize, int itemsPerPage, bool preWarm) :
	m_ItemSize(itemSize),
	m_ItemsPerPage(itemsPerPage),
	m_SizePerPage(m_ItemSize * m_ItemsPerPage),
	m_Pages(),
	m_Items()
	{
		if (preWarm)
		{
			AddPage();
		}
	}


	MemoryPoolBucket::~MemoryPoolBucket()
	{
		if (GetFreeItemsCount() != GetTotalItemsCount())
		{
			//todo log that some used items weren't freed
			std::cout << "Some items are not freed!" << std::endl;
		}

		for (auto pPage : m_Pages)
		{
			std::free(pPage);
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
#ifdef DEBUG
		assert(CheckIfAddressIsWithinPages(p) == true);
#endif

		//check that the address is not already in m_Items
#ifdef DEBUG
		auto searchResult = std::find(std::begin(m_Items), std::end(m_Items), p);
		assert(searchResult == std::end(m_Items));
#endif

		m_Items.push_back(p);

		assert(GetFreeItemsCount() <= GetTotalItemsCount());
	}


	void MemoryPoolBucket::AddPage()
	{
		void* pNewPage = std::malloc(m_SizePerPage);

		//adding new items
		for (int i = 0; i < m_ItemsPerPage; i++)
		{
			auto pItem = static_cast<char*>(pNewPage) + i * m_ItemSize;
			m_Items.push_back(pItem);
		}

		//adding page to pages
		m_Pages.push_back(pNewPage);

		spdlog::info("Adding new page for item size [{}], new pages count: [{}]", m_ItemSize, m_Pages.size());
	}


	int MemoryPoolBucket::GetTotalItemsCount() const
	{
		return (int)m_Pages.size() * m_ItemsPerPage;
	}


	int MemoryPoolBucket::GetFreeItemsCount() const
	{
		return (int)m_Items.size();
	}

	int MemoryPoolBucket::GetTotalMemoryUsed() const
	{
		return (int)m_Pages.size() * m_SizePerPage;
	}


	bool MemoryPoolBucket::CheckIfAddressIsWithinPages(void* p) const
	{
		for(auto pPage : m_Pages)
		{
			if (p >= pPage && p < static_cast<char*>(pPage) + m_SizePerPage)
			{
				return true;
			}
		}

		return false;
	}


	MemoryPoolBucketNew::MemoryPoolBucketNew() :
			m_ItemSize(0),
			m_FirstPageItemsCount(0),
			m_PageItemsCount(0),
			m_Pages(),
			m_Items()
	{

	}

	MemoryPoolBucketNew::~MemoryPoolBucketNew()
	{
		//todo
	}


	void MemoryPoolBucketNew::Setup(const MemoryPoolSettings::BucketDefinition &bucketDefinition)
	{
		m_ItemSize = bucketDefinition.m_ItemSize;
		m_FirstPageItemsCount = bucketDefinition.m_FirstPageItemsCount;
		m_PageItemsCount = bucketDefinition.m_PageItemsCount;

		assert(m_ItemSize > 0);
		assert(m_FirstPageItemsCount > 0);
		assert(m_PageItemsCount > 0);

		assert(m_Pages.size() == 0);
		assert(m_Items.size() == 0);

		if (bucketDefinition.m_PreWarmFirstPage)
		{
			AddPage();
		}
	}


	void* MemoryPoolBucketNew::Allocate()
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


	void MemoryPoolBucketNew::Deallocate(void* p)
	{
		//check that the address is within pages
#ifdef DEBUG
		assert(CheckIfAddressIsWithinPages(p) == true);
#endif

		//check that the address is not already in m_Items
#ifdef DEBUG
		auto searchResult = std::find(std::begin(m_Items), std::end(m_Items), p);
		assert(searchResult == std::end(m_Items));
#endif

		m_Items.push_back(p);

		assert(GetFreeItemsCount() <= GetTotalItemsCount());
	}


	void MemoryPoolBucketNew::AddPage()
	{
		//todo
	}


	bool MemoryPoolBucketNew::CheckIfAddressIsWithinPages(void* p) const
	{
		auto pagesCount = m_Pages.size();

		for (int i = 0; i < pagesCount; i++)
		{
			int pageSize;

			if (i == 0)
			{
				pageSize = m_FirstPageItemsCount * m_ItemSize;
			}
			else
			{
				pageSize = m_PageItemsCount * m_ItemSize;
			}

			auto pPage = m_Pages[i];

			if (p >= pPage && p < static_cast<char*>(pPage) + pageSize)
			{
				return true;
			}
		}

		return false;
	}


	int MemoryPoolBucketNew::GetTotalItemsCount() const
	{
		int result = 0;

		auto pagesCount = m_Pages.size();

		if (pagesCount > 0)
		{
			result += m_FirstPageItemsCount;
		}

		if (pagesCount > 1)
		{
			result += (pagesCount - 1) * m_PageItemsCount;
		}

		return result;
	}


	int MemoryPoolBucketNew::GetFreeItemsCount() const
	{
		return (int)m_Items.size();
	}


	int MemoryPoolBucketNew::GetTotalMemoryUsed() const
	{
		return GetTotalItemsCount() * m_ItemSize;
	}



}
