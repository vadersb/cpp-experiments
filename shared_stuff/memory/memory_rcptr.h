//
// Created by Alexander Samarin on 07.08.2021.
//

#pragma once

#include "memory_reference_counted.h"

namespace st::memory
{
	template<typename T> class rcptr
	{
	public:

		template<typename U> friend class rcptr;

		static_assert(std::is_base_of_v<ReferenceCounted, T>);

		//CONSTRUCTOR
		rcptr() : m_Pointer(nullptr)
		{

		}

		explicit rcptr(T* p) : m_Pointer(p)
		{
			IncreaseRefCount();
		}

		template<typename U> explicit rcptr(U* ptr)
		{
			static_assert(std::is_convertible_v<T, U> || std::is_convertible_v<U, T>);

			m_Pointer = CheckedDynamicCastFrom(ptr);

			IncreaseRefCount();
		}

		//COPY CONSTRUCTORS
		rcptr(const rcptr& anotherPointer) : m_Pointer(anotherPointer.m_Pointer)
		{
			IncreaseRefCount();
		}

		template<typename U> explicit rcptr(const rcptr<U>& anotherPointer)
		{
			static_assert(std::is_convertible_v<T, U> || std::is_convertible_v<U, T>);

			m_Pointer = CheckedDynamicCastFrom(anotherPointer.m_Pointer);

			IncreaseRefCount();
		}

		//MOVE CONSTRUCTORS
		rcptr(rcptr&& pointerToMoveFrom)  noexcept : m_Pointer(pointerToMoveFrom.m_Pointer)
		{
			pointerToMoveFrom.m_Pointer = nullptr;
		}

		template<typename U> explicit rcptr(rcptr<U>&& pointerToMoveFrom)
		{
			static_assert(std::is_convertible_v<T, U> || std::is_convertible_v<U, T>);

			m_Pointer = CheckedDynamicCastFrom(pointerToMoveFrom.m_Pointer);

			pointerToMoveFrom.m_Pointer = nullptr;
		}

		//DESTRUCTOR
		~rcptr()
		{
			if (m_Pointer != nullptr)
			{
				m_Pointer->ReferenceCountDecrease();
				m_Pointer = nullptr;
			}
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

		rcptr& operator=(const T* ptr)
		{
			if (m_Pointer == ptr) return *this;

			DecreaseRefCountAndReset();

			m_Pointer = ptr;

			IncreaseRefCount();

			return *this;
		}

		template<typename U> rcptr<T>& operator=(const rcptr<U>& otherRCPtr)
		{
			static_assert(std::is_convertible_v<T, U> || std::is_convertible_v<U, T>);

			if (this == (rcptr<T>*)&otherRCPtr) return *this;
			if (m_Pointer == otherRCPtr.m_Pointer) return *this;

			DecreaseRefCountAndReset();

			m_Pointer = CheckedDynamicCastFrom(otherRCPtr.m_Pointer);

			IncreaseRefCount();

			return *this;
		}


		template<typename U> rcptr& operator=(const U* ptr)
		{
			static_assert(std::is_convertible_v<T, U> || std::is_convertible_v<U, T>);

			if (m_Pointer == (T*)ptr) return *this;

			DecreaseRefCountAndReset();

			m_Pointer = CheckedDynamicCastFrom(ptr);

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

		template<typename U> rcptr& operator=(rcptr<U>&& ptrToMoveFrom) noexcept
		{
			static_assert(std::is_convertible_v<T, U> || std::is_convertible_v<U, T>);

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

			m_Pointer = CheckedDynamicCastFrom(ptrToMoveFrom.m_Pointer);
			ptrToMoveFrom.m_Pointer = nullptr;

			return *this;
		}

		//CONVERSION
		template<typename U> explicit operator rcptr<U>()
		{
			static_assert(std::is_convertible_v<T, U> || std::is_convertible_v<U, T>);

			U* ptr = CheckedDynamicCastTo<U>(m_Pointer);

			return rcptr<U>(ptr);
		}

		//RESET
		void Reset()
		{
			DecreaseRefCountAndReset();
		}

		//POINTER ACCESS
		T* ptr()
		{
			assert(m_Pointer != nullptr);
			return m_Pointer;
		}

		const T* ptr() const
		{
			assert(m_Pointer != nullptr);
			return m_Pointer;
		}

		template<typename U> U* ptr()
		{
			static_assert(std::is_convertible_v<T, U> || std::is_convertible_v<U, T>);
			U* pResult = CheckedDynamicCastTo<U>(m_Pointer);
			assert(pResult != nullptr);
			return pResult;
		}

		//QUERIES
		[[nodiscard]] bool ContainsValidPointer() const
		{
			return m_Pointer != nullptr;
		}

	private:

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

		template<typename U> inline T* CheckedDynamicCastFrom(U* ptr)
		{
			if (ptr == nullptr)
			{
				return nullptr;
			}

			T* pResult = dynamic_cast<T*>(ptr);

			assert(pResult != nullptr);

			return pResult;
		}

		template<typename U> inline U* CheckedDynamicCastTo(T* ptr)
		{
			if (ptr == nullptr)
			{
				return nullptr;
			}

			U* pResult = dynamic_cast<U*>(ptr);

			assert(pResult != nullptr);

			return pResult;
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