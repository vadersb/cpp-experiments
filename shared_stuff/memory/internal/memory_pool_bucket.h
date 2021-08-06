//
// Created by Alexander Samarin on 06.08.2021.
//

#pragma once

namespace st::memory
{
	class MemoryPoolBucket
	{
	public:

		explicit MemoryPoolBucket(int itemSize);

		~MemoryPoolBucket();

		void* Allocate();
		void Deallocate(void* p);

	private:

		int m_ItemSize;

	};
}

