//
// Created by Alexander Samarin on 06.08.2021.
//

#include <iostream>
#include "memory_pool_bucket.h"

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

		auto pResult = m_Items.top();
		m_Items.pop();

		return pResult;
	}


	void MemoryPoolBucket::Deallocate(void* p)
	{
		//todo check that the address is within pages

		m_Items.push(p);
	}


	void MemoryPoolBucket::AddPage()
	{
		void* pNewPage = std::malloc(m_SizePerPage);

		//adding new items
		for (int i = 0; i < m_ItemsPerPage; i++)
		{
			auto pItem = static_cast<char*>(pNewPage) + i * m_ItemSize;
			m_Items.push(pItem);
		}

		//adding page to pages
		m_Pages.push_back(pNewPage);
	}


	int MemoryPoolBucket::GetTotalItemsCount()
	{
		return (int)m_Pages.size() * m_ItemsPerPage;
	}


	int MemoryPoolBucket::GetFreeItemsCount()
	{
		return (int)m_Items.size();
	}

}
