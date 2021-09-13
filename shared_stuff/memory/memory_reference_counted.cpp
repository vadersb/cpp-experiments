//
// Created by Alexander Samarin on 07.08.2021.
//

#include "memory_reference_counted.h"
#include <cassert>

namespace st::memory
{
	ReferenceCounted::ReferenceCounted() :
	m_ReferenceCount(1), //reference count is 1 because it may be changed in constructor of derived class and constructor will RefCountDecrease() which will lead to calling the destructor
	m_WeakReferenceCount(0)
	{

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
			m_WeakReferenceCount++; //to avoid calling 'delete this' twice if WeakReferenceCountDecrease() is called somewhere inside OnNoReferenceCountingOwnersLeft()

			OnNoReferenceCountingOwnersLeft();

			m_WeakReferenceCount--;

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

