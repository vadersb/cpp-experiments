//
// Created by Alexander on 30.09.2021.
//

#pragma once

#include <cassert>

namespace st::memory
{

	class MemoryPoolSettings final
	{
	public:

		static constexpr int MaxBucketsCount = 256;

		struct BucketDefinition
		{
		public:

			BucketDefinition() : m_ItemSize(0), m_FirstPageItemsCount(0), m_ExtraPageItemsCount(0), m_PreWarmFirstPage(false)
			{

			}

			BucketDefinition(int itemSize, int firstPageItemsCount, int pageItemsCount, bool preWarmFirstPage = true) :
					m_ItemSize(itemSize),
					m_FirstPageItemsCount(firstPageItemsCount),
					m_ExtraPageItemsCount(pageItemsCount),
					m_PreWarmFirstPage(preWarmFirstPage)
			{
				assert(m_ItemSize > 0);
				assert(m_FirstPageItemsCount > 0);
				assert(m_ExtraPageItemsCount > 0);
			}


			int m_ItemSize;
			int m_FirstPageItemsCount;
			int m_ExtraPageItemsCount;
			bool m_PreWarmFirstPage;
		};

		MemoryPoolSettings();

		[[nodiscard]] int GetBucketsCount() const;
		[[nodiscard]] const BucketDefinition& GetBucketDefinition(int index) const;

		void AddBucketDefinition(int itemSize, int firstPageItemsCount, int extraPageItemsCount, bool preWarmFirstPage);

	private:

		int m_BucketsCount;

		BucketDefinition m_BucketDefinitions[MaxBucketsCount];

	};

	MemoryPoolSettings GetDefaultMemoryPoolSettings(bool isThreadSafe);

}