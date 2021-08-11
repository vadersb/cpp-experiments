//
// Created by Alexander Samarin on 07.08.2021.
//

#include "memory_reference_counted.h"
#include <cassert>

namespace st::memory
{
	ReferenceCounted::ReferenceCounted() :
	m_ReferenceCount(0),
	m_WeakReferenceCount(0)
	{

	}


	void ReferenceCounted::ReferenceCountStart()
	{
		assert(m_ReferenceCount == 0);
		assert(m_WeakReferenceCount == 0);

		m_ReferenceCount = 1;
	}


	void ReferenceCounted::ReferenceCountIncrease()
	{
		assert(m_ReferenceCount > 0);
		assert(m_WeakReferenceCount >= 0);

		m_ReferenceCount++;
	}


	void ReferenceCounted::ReferenceCountDecrease()
	{
		assert(m_ReferenceCount > 0);
		assert(m_WeakReferenceCount >= 0);

		m_ReferenceCount--;

		if (m_ReferenceCount == 0)
		{
			OnNoReferenceCountingOwnersLeft();

			if (m_WeakReferenceCount == 0)
			{
				delete this;
			}
		}
	}


	void ReferenceCounted::WeakReferenceCountIncrease()
	{
		assert(m_ReferenceCount > 0);
		assert(m_WeakReferenceCount >= 0);

		m_WeakReferenceCount++;
	}


	void ReferenceCounted::WeakReferenceCountDecrease()
	{
		assert(m_ReferenceCount >= 0);
		assert(m_WeakReferenceCount > 0);

		m_WeakReferenceCount--;

		if (m_WeakReferenceCount == 0)
		{
			if (m_ReferenceCount == 0)
			{
				delete this;
			}
		}
	}




}

