//
// Created by Alexander on 25.08.2022.
//

#pragma once

#include <cassert>
#include <type_traits>
#include "memory_settings.h"
#include "memory_reference_counted.h"
#include "utils_cast.h"

namespace st::memory
{
	template<typename T> class tptr final
	{
	public:

		//FRIENDS
		template<typename U> friend class rcptr;
		template<typename U> friend class wptr;

		static_assert(std::is_base_of_v<ReferenceCounted, T>);


		//DESTRUCTOR
		~tptr()
		{
			if (m_Pointer != nullptr)
			{
				m_Pointer->ReferenceCountDecrease();
				m_Pointer = nullptr;
			}
		}


		//implicit conversions
		operator T*() const
		{
			return m_Pointer;
		}

		operator T&() const
		{
			return *m_Pointer;
		}


	private:

		T* m_Pointer;

		//CONSTRUCTOR
		tptr(T* pointer, bool canBeNull) : m_Pointer(pointer)
		{
			if (m_Pointer != nullptr)
			{
				assert(m_Pointer->GetReferenceCount() >= 1);
				m_Pointer->ReferenceCountIncrease();
			}
			else
			{
				assert(canBeNull);
			}
		}

	};

}
