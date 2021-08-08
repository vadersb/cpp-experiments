//
// Created by Alexander Samarin on 06.08.2021.
//

#include <iostream>
#include "memory_pool_bucket.h"
#include <cassert>

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
	}


	int MemoryPoolBucket::GetTotalItemsCount() const
	{
		return (int)m_Pages.size() * m_ItemsPerPage;
	}


	int MemoryPoolBucket::GetFreeItemsCount() const
	{
		return (int)m_Items.size();
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

}
