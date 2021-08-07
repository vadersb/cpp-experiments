//
// Created by Alexander Samarin on 07.08.2021.
//

#include "memory_reference_counted.h"
#include <cassert>

namespace st::memory
{
	ReferenceCounted::ReferenceCounted() :
	m_ReferenceCount(0)
	{

	}


	void ReferenceCounted::ReferenceCountIncrease()
	{
		assert(m_ReferenceCount >= 0);

		m_ReferenceCount++;
	}


	void ReferenceCounted::ReferenceCountDecrease()
	{
		assert(m_ReferenceCount > 0);

		m_ReferenceCount--;

		if (m_ReferenceCount == 0)
		{
			delete this;
		}
	}


}

