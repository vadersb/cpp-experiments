//
// Created by Alexander on 11.08.2021.
//

#pragma once

#include "memory_rcptr.h"
#include "memory_reference_counted.h"
#include "utils_cast.h"

namespace st::memory
{
	template<typename T> class rcptr;
	template<typename T> class wptr;

	template<typename T> class wptr final
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
		//considered unnecessary


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
		wptr(wptr&& pointerToMoveFrom) noexcept : m_Pointer(pointerToMoveFrom.m_Pointer)
		{
			pointerToMoveFrom.m_Pointer = nullptr;
		}

		template<typename U> explicit wptr(wptr<U>&& pointerToMoveFrom) noexcept
		{
			m_Pointer = st::utils::CheckedDynamicCastUpDown<U, T>(pointerToMoveFrom.m_Pointer);
			pointerToMoveFrom.m_Pointer = nullptr;
		}



		//DESTRUCTOR
		~wptr()
		{
			DecreaseRefCountAndReset();
		}

		//COPY ASSIGNMENT

		//wptr
		wptr& operator=(const wptr& otherPtr)
		{
			if (this == &otherPtr) return *this;
			if (m_Pointer == otherPtr.m_Pointer) return *this;

			DecreaseRefCountAndReset();
			m_Pointer = otherPtr.m_Pointer;
			IncreaseRefCount();

			return *this;
		}

		template<typename U> wptr<T>& operator=(const wptr<U>& otherPtr)
		{
			if (this == &otherPtr) return *this;
			if (m_Pointer == otherPtr.m_Pointer) return *this;

			DecreaseRefCountAndReset();
			m_Pointer = st::utils::CheckedDynamicCastUpDown<U, T>(otherPtr.m_Pointer);
			IncreaseRefCount();

			return *this;
		}

		//rcptr
		wptr& operator=(const rcptr<T>& strongPointer)
		{
			if (m_Pointer == strongPointer.m_Pointer) return *this;

			DecreaseRefCountAndReset();
			m_Pointer = strongPointer.m_Pointer;
			IncreaseRefCount();

			return *this;
		}

		template<typename U> wptr& operator=(const rcptr<U>& strongPointer)
		{
			if (m_Pointer == strongPointer.m_Pointer) return *this;

			DecreaseRefCountAndReset();
			m_Pointer = st::utils::CheckedDynamicCastUpDown<U, T>(strongPointer.m_Pointer);
			IncreaseRefCount();

			return *this;
		}


		//MOVE ASSIGNMENT
		wptr& operator=(wptr&& pointerToMoveFrom) noexcept
 		{
			if (this == &pointerToMoveFrom)
			{
				DecreaseRefCountAndReset();
				return *this;
			}

			if (m_Pointer == pointerToMoveFrom.m_Pointer)
			{
				pointerToMoveFrom.DecreaseRefCountAndReset();
				return *this;
			}

			DecreaseRefCountAndReset();

			m_Pointer = pointerToMoveFrom.m_Pointer;
			pointerToMoveFrom.m_Pointer = nullptr;

			return *this;
		}

		template<typename U> wptr& operator=(wptr<U>&& pointerToMoveFrom) noexcept
		{
			if (this == *pointerToMoveFrom)
			{
				DecreaseRefCountAndReset();
				return *this;
			}

			if (m_Pointer == pointerToMoveFrom.m_Pointer)
			{
				pointerToMoveFrom.DecreaseRefCountAndReset();
				return *this;
			}

			DecreaseRefCountAndReset();

			m_Pointer = st::utils::CheckedDynamicCastUpDown(pointerToMoveFrom.m_Pointer);
			pointerToMoveFrom.m_Pointer = nullptr;

			return *this;
		}


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

		[[nodiscard]] int GetWeakReferenceCount() const
		{
			if (m_Pointer != nullptr)
			{
				return m_Pointer->GetWeakReferenceCount();
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
