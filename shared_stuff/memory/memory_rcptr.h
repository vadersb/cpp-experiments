//
// Created by Alexander Samarin on 07.08.2021.
//

#pragma once

#include "memory_reference_counted.h"
#include "memory_wptr.h"
#include "utils_cast.h"


namespace st::memory
{
	template<typename T> class rcptr;
	template<typename T> class wptr;

	template<typename T> class rcptr
	{
	public:

		template<typename U> friend class rcptr;
		template<typename U> friend class wptr;

		static_assert(std::is_base_of_v<ReferenceCounted, T>);

		//CONSTRUCTORS
		rcptr() : m_Pointer(nullptr)
		{

		}

		explicit rcptr(T* p) : m_Pointer(p)
		{
			StartRefCount();
		}

		template<typename U> explicit rcptr(U* ptr)
		{
			m_Pointer = st::utils::CheckedDynamicCastUpDown<U, T>(ptr);

			StartRefCount();
		}

		//COPY CONSTRUCTORS

		//rcptr
		rcptr(const rcptr& anotherPointer) : m_Pointer(anotherPointer.m_Pointer)
		{
			IncreaseRefCount();
		}

		template<typename U> explicit rcptr(const rcptr<U>& anotherPointer)
		{
			m_Pointer = st::utils::CheckedDynamicCastUpDown(anotherPointer.m_Pointer);

			IncreaseRefCount();
		}

		//wptr
		explicit rcptr(const wptr<T>& weakPointer)
		{
			if (weakPointer.m_Pointer == nullptr || weakPointer.m_Pointer->IsOutOfScope())
			{
				m_Pointer = nullptr;
			}
			else
			{
				m_Pointer = weakPointer.m_Pointer;
			}

			IncreaseRefCount();
		}


		template<typename U> explicit rcptr(const wptr<U>& weakPointer)
		{
			if (weakPointer.m_Pointer == nullptr || weakPointer.m_Pointer->IsOutOfScope())
			{
				m_Pointer = nullptr;
			}
			else
			{
				m_Pointer = st::utils::CheckedDynamicCastUpDown<U, T>(weakPointer.m_Pointer);
			}

			IncreaseRefCount();
		}


		//MOVE CONSTRUCTORS

		//rcptr
		rcptr(rcptr&& pointerToMoveFrom)  noexcept : m_Pointer(pointerToMoveFrom.m_Pointer)
		{
			pointerToMoveFrom.m_Pointer = nullptr;
		}

		template<typename U> explicit rcptr(rcptr<U>&& pointerToMoveFrom)
		{
			m_Pointer = st::utils::CheckedDynamicCastUpDown<U, T>(pointerToMoveFrom.m_Pointer);

			pointerToMoveFrom.m_Pointer = nullptr;
		}


		//DESTRUCTOR
		~rcptr()
		{
			DecreaseRefCountAndReset();
		}

		//COPY ASSIGNMENT
		rcptr& operator=(const rcptr& otherRCPtr)
		{
			if (this == &otherRCPtr) return *this;
			if (m_Pointer == otherRCPtr.m_Pointer) return *this;

			DecreaseRefCountAndReset();
			m_Pointer = otherRCPtr.m_Pointer;
			IncreaseRefCount();

			return *this;
		}


		template<typename U> rcptr<T>& operator=(const rcptr<U>& otherRCPtr)
		{
			if (this == (rcptr<T>*)&otherRCPtr) return *this;
			if (m_Pointer == otherRCPtr.m_Pointer) return *this;

			DecreaseRefCountAndReset();
			m_Pointer = st::utils::CheckedDynamicCastUpDown<U, T>(otherRCPtr.m_Pointer);
			IncreaseRefCount();

			return *this;
		}


		//MOVE ASSIGNMENT
		rcptr& operator=(rcptr&& ptrToMoveFrom) noexcept
		{
			if (this == &ptrToMoveFrom)
			{
				DecreaseRefCountAndReset();
				return *this;
			}

			if (m_Pointer == ptrToMoveFrom.m_Pointer)
			{
				ptrToMoveFrom.DecreaseRefCountAndReset();
				return *this;
			}

			DecreaseRefCountAndReset();

			m_Pointer = ptrToMoveFrom.m_Pointer;
			ptrToMoveFrom.m_Pointer = nullptr;

			return *this;
		}

		template<typename U> rcptr& operator=(rcptr<U>&& ptrToMoveFrom)
		{
			if (this == *ptrToMoveFrom)
			{
				DecreaseRefCountAndReset();
				return *this;
			}

			if (m_Pointer == ptrToMoveFrom.m_Pointer)
			{
				ptrToMoveFrom.DecreaseRefCountAndReset();
				return *this;
			}

			DecreaseRefCountAndReset();

			m_Pointer = st::utils::CheckedDynamicCastUpDown(ptrToMoveFrom.m_Pointer);
			ptrToMoveFrom.m_Pointer = nullptr;

			return *this;
		}

		//CONVERSION
		template<typename U> explicit operator rcptr<U>() const
		{
			return rcptr<U>(*this);
		}


		//RESET
		void Reset()
		{
			DecreaseRefCountAndReset();
		}

		//SWAP
		void Swap(rcptr& pointerToSwapWith)
		{
			std::swap(m_Pointer, pointerToSwapWith.m_Pointer);
		}

		//POINTER ACCESS
		inline T* operator ->() const noexcept
		{
			assert(m_Pointer != nullptr);
			return m_Pointer;
		}

		inline T& operator *() const noexcept
		{
			assert(m_Pointer != nullptr);
			return *m_Pointer;
		}

		inline T* Get() const noexcept
		{
			assert(m_Pointer != nullptr);
			return m_Pointer;
		}

		template<typename U> U* Get() const
		{
			U* pResult = st::utils::CheckedDynamicCastUpDown<T, U>(m_Pointer);
			assert(pResult != nullptr);
			return pResult;
		}

		//QUERIES
		[[nodiscard]] bool ContainsValidPointer() const
		{
			return m_Pointer != nullptr;
		}

		explicit operator bool() const noexcept
		{
			return m_Pointer != nullptr;
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

	private:

		inline void StartRefCount()
		{
			if (m_Pointer != nullptr)
			{
				m_Pointer->ReferenceCountStart();
			}
		}

		inline void IncreaseRefCount()
		{
			if (m_Pointer != nullptr)
			{
				m_Pointer->ReferenceCountIncrease();
			}
		}

		inline void DecreaseRefCountAndReset()
		{
			if (m_Pointer != nullptr)
			{
				m_Pointer->ReferenceCountDecrease();
				m_Pointer = nullptr;
			}
		}


		T* m_Pointer;
	};

	template<typename T, typename ... Args> rcptr<T> CreateRefCountedPointer(Args&& ... args)
	{
		T* p = new T(std::forward<Args>(args)...);
		return rcptr<T>(p);
	}

	template<typename T, typename U, typename ... Args> rcptr<T> CreateRefCountedPointer(Args&& ... args)
	{
		static_assert(std::is_convertible_v<U, T>);
		U* p = new U(std::forward<Args>(args)...);
		return rcptr<T>(p);
	}
}