//
// Created by Alexander on 11.08.2021.
//

#pragma once

#include "memory_reference_counted.h"
#include "memory_rcptr.h"
#include "utils_cast.h"

namespace st::memory
{
	template<typename T> class wptr
	{
	public:

		template<typename U> friend class wptr;
		template<typename U> friend class rcptr;

		static_assert(std::is_base_of_v<ReferenceCounted, T>);

		//CONSTRUCTORS

		//default
		wptr() : m_Pointer(nullptr)
		{

		}

		//naked pointers
		explicit wptr(T* p) : m_Pointer(p)
		{
			if (m_Pointer != nullptr)
			{
				if (m_Pointer->IsOutOfScope())
				{
					m_Pointer = nullptr;
				}
			}

			IncreaseRefCount();
		}

		template<typename U> explicit wptr(U* ptr)
		{
			m_Pointer = st::utils::CheckedDynamicCastUpDown<U, T>(ptr);

			if (m_Pointer != nullptr)
			{
				if (m_Pointer->IsOutOfScope())
				{
					m_Pointer = nullptr;
				}
			}

			IncreaseRefCount();
		}


		//COPY CONSTRUCTORS

		//wptr
		wptr(const wptr& pointerToCopyFrom)
		{
			if (pointerToCopyFrom.m_Pointer == nullptr || pointerToCopyFrom.m_Pointer->IsOutOfScope())
			{
				m_Pointer = nullptr;
			}
			else
			{
				m_Pointer = pointerToCopyFrom.m_Pointer;
				IncreaseRefCount();
			}
		}

		template<typename U> explicit wptr(const wptr<U>& pointerToCopyFrom)
		{
			if (pointerToCopyFrom.m_Pointer == nullptr || pointerToCopyFrom.m_Pointer->IsOutOfScope())
			{
				m_Pointer = nullptr;
			}
			else
			{
				m_Pointer = st::utils::CheckedDynamicCastUpDown<U, T>(pointerToCopyFrom.m_Pointer);
				IncreaseRefCount();
			}
		}

		//rcptr
		explicit wptr(const rcptr<T>& strongPointer) : m_Pointer(strongPointer.m_Pointer)
		{
			IncreaseRefCount();
		}


		template<typename U> explicit wptr(const rcptr<U>& strongPointer)
		{
			m_Pointer = st::utils::CheckedDynamicCastUpDown<U, T>(strongPointer.m_Pointer);

			IncreaseRefCount();
		}


		//MOVE CONSTRUCTORS

		//wptr
		//todo


		//rcptr
		//todo


		//DESTRUCTOR
		~wptr()
		{
			DecreaseRefCountAndReset();
		}

		//COPY ASSIGNMENT


		//MOVE ASSIGNMENT


		//REFRESH
		//returns true if contains valid pointer
		bool Refresh()
		{
			ResetIfExpired();
			return ContainsValidPointer();
		}

		//RESET
		void Reset()
		{
			DecreaseRefCountAndReset();
		}

		//SWAP
		void Swap(wptr& pointerToSwapWith)
		{
			ResetIfExpired();
			pointerToSwapWith.ResetIfExpired();
			std::swap(m_Pointer, pointerToSwapWith.m_Pointer);
		}

		//LOCK
		[[nodiscard]] rcptr<T> Lock() const noexcept
		{
			if (ContainsValidPointer())
			{
				return rcptr<T>(*this);
			}
			else
			{
				return rcptr<T>();
			}
		}

		template<typename U> rcptr<U> Lock() const noexcept
		{
			if (ContainsValidPointer())
			{
				return rcptr<U>(*this);
			}
			else
			{
				return rcptr<U>();
			}
		}


		//QUERIES
		[[nodiscard]] bool ContainsValidPointer() const
		{
			if (m_Pointer == nullptr)
			{
				return false;
			}
			else
			{
				return !m_Pointer->IsOutOfScope();
			}
		}

		explicit operator bool() const noexcept
		{
			return ContainsValidPointer();
		}

		[[nodiscard]] int GetUseCount() const
		{
			if (m_Pointer != nullptr)
			{
				return m_Pointer->GetReferenceCount();
			}
			else
			{
				return 0;
			}
		}

		[[nodiscard]] bool IsExpired() const
		{
			return GetUseCount() == 0;
		}

	private:


		inline void ResetIfExpired()
		{
			if (m_Pointer != nullptr)
			{
				if (m_Pointer->IsOutOfScope())
				{
					m_Pointer->WeakReferenceCountDecrease();
					m_Pointer = nullptr;
				}
			}
		}

		inline void IncreaseRefCount()
		{
			if (m_Pointer != nullptr)
			{
				m_Pointer->WeakReferenceCountIncrease();
			}
		}

		inline void DecreaseRefCountAndReset()
		{
			if (m_Pointer != nullptr)
			{
				m_Pointer->WeakReferenceCountDecrease();
				m_Pointer = nullptr;
			}
		}



		T* m_Pointer;

	};



}
