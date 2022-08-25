//
// Created by Alexander Samarin on 07.08.2021.
//

#pragma once

#include "memory_settings.h"
#include "memory_reference_counted.h"
#include "memory_wptr.h"
#include "memory_tptr.h"
#include "utils_cast.h"

#ifdef SMARTPTR_THREAD_VALIDATION

#define RCPTR_THREAD_STORE m_ThreadID = std::this_thread::get_id()
#define RCPTR_THREAD_CHECK assert( m_ThreadID == std::this_thread::get_id() )

#else

#define RCPTR_THREAD_STORE ((void)0)
#define RCPTR_THREAD_CHECK ((void)0)

#endif



namespace st::memory
{
	template<typename T> class rcptr;
	template<typename T> class wptr;

	template<typename T> class rcptr final
	{
	public:

		//FRIENDS
		template<typename U> friend class rcptr;
		template<typename U> friend class wptr;

		template<typename TObjectType, typename ... Args> friend rcptr<TObjectType> CreateRefCountedPointer(Args&& ... args);
		template<typename TPointerType, typename TObjectType, typename ... Args> friend rcptr<TPointerType> CreateRefCountedPointer(Args&& ... args);

		template<typename TObjectType> rcptr<TObjectType> friend GetRefCountedPointer(TObjectType* pRefCountedObject);
		template<typename TPointerType, typename TObjectType> friend rcptr<TPointerType> GetRefCountedPointer(TObjectType* pRefCountedObject);

		static_assert(std::is_base_of_v<ReferenceCounted, T>);

		//CONSTRUCTORS
		rcptr() : m_Pointer(nullptr)
		{
			RCPTR_THREAD_STORE;
		}


		//COPY CONSTRUCTORS

		//rcptr
		rcptr(const rcptr& anotherPointer) : m_Pointer(anotherPointer.m_Pointer)
		{
			RCPTR_THREAD_STORE;

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == anotherPointer.m_ThreadID);
#endif

			IncreaseRefCount();
		}

		template<typename U> explicit rcptr(const rcptr<U>& anotherPointer)
		{
			RCPTR_THREAD_STORE;

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == anotherPointer.m_ThreadID);
#endif

			m_Pointer = st::utils::CheckedDynamicCastUpDown(anotherPointer.m_Pointer);

			IncreaseRefCount();
		}

		//wptr
		explicit rcptr(const wptr<T>& weakPointer)
		{
			RCPTR_THREAD_STORE;

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == weakPointer.m_ThreadID);
#endif

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
			RCPTR_THREAD_STORE;

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == weakPointer.m_ThreadID);
#endif

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
			RCPTR_THREAD_STORE;

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == pointerToMoveFrom.m_ThreadID);
#endif

			pointerToMoveFrom.m_Pointer = nullptr;
		}

		template<typename U> explicit rcptr(rcptr<U>&& pointerToMoveFrom)
		{
			RCPTR_THREAD_STORE;

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == pointerToMoveFrom.m_ThreadID);
#endif

			m_Pointer = st::utils::CheckedDynamicCastUpDown<U, T>(pointerToMoveFrom.m_Pointer);

			pointerToMoveFrom.m_Pointer = nullptr;
		}


		//DESTRUCTOR
		~rcptr()
		{
			RCPTR_THREAD_CHECK;
			DecreaseRefCountAndReset();
		}

		//COPY ASSIGNMENT
		rcptr& operator=(const rcptr& otherRCPtr)
		{
			RCPTR_THREAD_CHECK;

			if (this == &otherRCPtr) return *this;

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == otherRCPtr.m_ThreadID);
#endif

			if (m_Pointer == otherRCPtr.m_Pointer) return *this;

			DecreaseRefCountAndReset();
			m_Pointer = otherRCPtr.m_Pointer;
			IncreaseRefCount();

			return *this;
		}


		template<typename U> rcptr<T>& operator=(const rcptr<U>& otherRCPtr)
		{
			RCPTR_THREAD_CHECK;

			if (this == (rcptr<T>*)&otherRCPtr) return *this;

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == otherRCPtr.m_ThreadID);
#endif

			if (m_Pointer == otherRCPtr.m_Pointer) return *this;

			DecreaseRefCountAndReset();
			m_Pointer = st::utils::CheckedDynamicCastUpDown<U, T>(otherRCPtr.m_Pointer);
			IncreaseRefCount();

			return *this;
		}


		//MOVE ASSIGNMENT
		rcptr& operator=(rcptr&& ptrToMoveFrom) noexcept
		{
			RCPTR_THREAD_CHECK;

			if (this == &ptrToMoveFrom)
			{
				DecreaseRefCountAndReset();
				return *this;
			}

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == ptrToMoveFrom.m_ThreadID);
#endif

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
			RCPTR_THREAD_CHECK;

			if (this == *ptrToMoveFrom)
			{
				DecreaseRefCountAndReset();
				return *this;
			}

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == ptrToMoveFrom.m_ThreadID);
#endif

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
			RCPTR_THREAD_CHECK;

			return rcptr<U>(*this);
		}

		//COMPARISON
		bool operator==(const rcptr& ptrToCompareWith) const
		{
			RCPTR_THREAD_CHECK;

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == ptrToCompareWith.m_ThreadID);
#endif

			return m_Pointer == ptrToCompareWith.m_Pointer;
		}

		template<typename U> bool operator==(const rcptr<U>& ptrToCompareWith) const
		{
			RCPTR_THREAD_CHECK;

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == ptrToCompareWith.m_ThreadID);
#endif

			return m_Pointer == ptrToCompareWith.m_Pointer;
		}

		template<typename U> bool operator==(const wptr<U>& ptrToCompareWith) const
		{
			RCPTR_THREAD_CHECK;

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == ptrToCompareWith.m_ThreadID);
#endif

			return m_Pointer == ptrToCompareWith.m_Pointer;
		}


		//RESET
		void Reset()
		{
			RCPTR_THREAD_CHECK;

			DecreaseRefCountAndReset();
		}

		//SWAP
		void Swap(rcptr& pointerToSwapWith)
		{
			RCPTR_THREAD_CHECK;

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == pointerToSwapWith.m_ThreadID);
#endif

			std::swap(m_Pointer, pointerToSwapWith.m_Pointer);
		}

		//POINTER ACCESS
		inline T* operator ->() const noexcept
		{
			RCPTR_THREAD_CHECK;
			assert(m_Pointer != nullptr);
			return m_Pointer;
		}

		inline T& operator *() const noexcept
		{
			RCPTR_THREAD_CHECK;
			assert(m_Pointer != nullptr);
			return *m_Pointer;
		}

		inline T* Get() const noexcept
		{
			RCPTR_THREAD_CHECK;
			assert(m_Pointer != nullptr);
			return m_Pointer;
		}

		template<typename U> U* Get() const
		{
			RCPTR_THREAD_CHECK;
			U* pResult = st::utils::CheckedDynamicCastUpDown<T, U>(m_Pointer);
			assert(pResult != nullptr);
			return pResult;
		}

		//TEMP SCOPED POINTER/REFERENCE PASSING
		tptr<T> PassPtr(bool canBeNull = true) const
		{
			return tptr<T>(m_Pointer, canBeNull);
		}

		template<typename U> tptr<U> PassPtr(bool canBeNull = true) const
		{
			U* pResult = st::utils::CheckedDynamicCastUpDown<T, U>(m_Pointer);
			return tptr<U>(pResult, canBeNull);
		}

		tptr<T> PassRef() const
		{
			return tptr<T>(m_Pointer, false);
		}

		template<typename U> tptr<U> PassRef() const
		{
			U* pResult = st::utils::CheckedDynamicCastUpDown<T, U>(m_Pointer);
			return tptr<U>(pResult, false);
		}



		//QUERIES
		[[nodiscard]] bool ContainsValidPointer() const
		{
			RCPTR_THREAD_CHECK;
			return m_Pointer != nullptr;
		}

		explicit operator bool() const noexcept
		{
			RCPTR_THREAD_CHECK;
			return m_Pointer != nullptr;
		}

		[[nodiscard]] int GetUseCount() const
		{
			RCPTR_THREAD_CHECK;
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

		//CONSTRUCTORS
		explicit rcptr(T* p, bool isJustCreated) : m_Pointer(p)
		{
			RCPTR_THREAD_STORE;
			if (isJustCreated == false)
			{
				IncreaseRefCount();
			}
			else
			{
				if (m_Pointer != nullptr)
				{
					assert(m_Pointer->GetReferenceCount() >= 1);
				}
			}
		}

		template<typename U> explicit rcptr(U* ptr, bool isJustCreated)
		{
			RCPTR_THREAD_STORE;
			m_Pointer = st::utils::CheckedDynamicCastUpDown<U, T>(ptr);

			if (isJustCreated == false)
			{
				IncreaseRefCount();
			}
			else
			{
				if (m_Pointer != nullptr)
				{
					assert(m_Pointer->GetReferenceCount() >= 1);
				}
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

#ifdef SMARTPTR_THREAD_VALIDATION
		std::thread::id m_ThreadID;
#endif

	};


	template<typename TObjectType, typename ... Args> rcptr<TObjectType> CreateRefCountedPointer(Args&& ... args)
	{
		TObjectType* p = new TObjectType(std::forward<Args>(args)...);
		return rcptr<TObjectType>(p, true);
	}


	template<typename TPointerType, typename TObjectType, typename ... Args> rcptr<TPointerType> CreateRefCountedPointer(Args&& ... args)
	{
		static_assert(std::is_convertible_v<TObjectType, TPointerType>);
		TObjectType* p = new TObjectType(std::forward<Args>(args)...);
		return rcptr<TPointerType>(p, true);
	}


	template<typename TObjectType> rcptr<TObjectType> GetRefCountedPointer(TObjectType* pRefCountedObject)
	{
		assert(pRefCountedObject != nullptr);
		static_assert(std::is_base_of_v<ReferenceCounted, TObjectType>);

		return rcptr<TObjectType>(pRefCountedObject, false);
	}


	template<typename TPointerType, typename TObjectType> rcptr<TPointerType> GetRefCountedPointer(TObjectType* pRefCountedObject)
	{
		assert(pRefCountedObject != nullptr);
		static_assert(std::is_base_of_v<ReferenceCounted, TObjectType>);
		static_assert(std::is_convertible_v<TObjectType, TPointerType>);

		return rcptr<TPointerType>(pRefCountedObject, false);
	}
}


#undef RCPTR_THREAD_STORE
#undef RCPTR_THREAD_CHECK