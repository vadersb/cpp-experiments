//
// Created by Alexander on 11.08.2021.
//

#pragma once

#include "memory_rcptr.h"
#include "utils_cast.h"


#ifdef SMARTPTR_THREAD_VALIDATION

#define WPTR_THREAD_STORE m_ThreadID = std::this_thread::get_id()
#define WPTR_THREAD_CHECK assert( m_ThreadID == std::this_thread::get_id() )

#else

#define WPTR_THREAD_STORE ((void)0)
#define WPTR_THREAD_CHECK ((void)0)

#endif

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
			WPTR_THREAD_STORE;
		}

		//naked pointers
		//considered unnecessary


		//COPY CONSTRUCTORS

		//wptr
		wptr(const wptr& pointerToCopyFrom)
		{
			WPTR_THREAD_STORE;

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == pointerToCopyFrom.m_ThreadID );
#endif

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
			WPTR_THREAD_STORE;

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == pointerToCopyFrom.m_ThreadID );
#endif

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
			WPTR_THREAD_STORE;

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == strongPointer.m_ThreadID );
#endif

			IncreaseRefCount();
		}


		template<typename U> explicit wptr(const rcptr<U>& strongPointer)
		{
			WPTR_THREAD_STORE;

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == strongPointer.m_ThreadID );
#endif

			m_Pointer = st::utils::CheckedDynamicCastUpDown<U, T>(strongPointer.m_Pointer);

			IncreaseRefCount();
		}


		//MOVE CONSTRUCTORS

		//wptr
		wptr(wptr&& pointerToMoveFrom) noexcept : m_Pointer(pointerToMoveFrom.m_Pointer)
		{
			WPTR_THREAD_STORE;

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == pointerToMoveFrom.m_ThreadID );
#endif

			pointerToMoveFrom.m_Pointer = nullptr;
		}

		template<typename U> explicit wptr(wptr<U>&& pointerToMoveFrom) noexcept
		{
			WPTR_THREAD_STORE;

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == pointerToMoveFrom.m_ThreadID );
#endif

			m_Pointer = st::utils::CheckedDynamicCastUpDown<U, T>(pointerToMoveFrom.m_Pointer);
			pointerToMoveFrom.m_Pointer = nullptr;
		}



		//DESTRUCTOR
		~wptr()
		{
			WPTR_THREAD_CHECK;
			DecreaseRefCountAndReset();
		}

		//COPY ASSIGNMENT

		//wptr
		wptr& operator=(const wptr& otherPtr)
		{
			WPTR_THREAD_CHECK;

			if (this == &otherPtr) return *this;
			if (m_Pointer == otherPtr.m_Pointer) return *this;

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == otherPtr.m_ThreadID );
#endif

			DecreaseRefCountAndReset();
			m_Pointer = otherPtr.m_Pointer;
			IncreaseRefCount();

			return *this;
		}

		template<typename U> wptr<T>& operator=(const wptr<U>& otherPtr)
		{
			WPTR_THREAD_CHECK;

			if (this == &otherPtr) return *this;
			if (m_Pointer == otherPtr.m_Pointer) return *this;

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == otherPtr.m_ThreadID );
#endif

			DecreaseRefCountAndReset();
			m_Pointer = st::utils::CheckedDynamicCastUpDown<U, T>(otherPtr.m_Pointer);
			IncreaseRefCount();

			return *this;
		}

		//rcptr
		wptr& operator=(const rcptr<T>& strongPointer)
		{
			WPTR_THREAD_CHECK;

			if (m_Pointer == strongPointer.m_Pointer) return *this;

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == strongPointer.m_ThreadID );
#endif

			DecreaseRefCountAndReset();
			m_Pointer = strongPointer.m_Pointer;
			IncreaseRefCount();

			return *this;
		}

		template<typename U> wptr& operator=(const rcptr<U>& strongPointer)
		{
			WPTR_THREAD_CHECK;

			if (m_Pointer == strongPointer.m_Pointer) return *this;

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == strongPointer.m_ThreadID );
#endif

			DecreaseRefCountAndReset();
			m_Pointer = st::utils::CheckedDynamicCastUpDown<U, T>(strongPointer.m_Pointer);
			IncreaseRefCount();

			return *this;
		}


		//MOVE ASSIGNMENT
		wptr& operator=(wptr&& pointerToMoveFrom) noexcept
 		{
		    WPTR_THREAD_CHECK;

			if (this == &pointerToMoveFrom)
			{
				DecreaseRefCountAndReset();
				return *this;
			}

#ifdef SMARTPTR_THREAD_VALIDATION
		    assert( m_ThreadID == pointerToMoveFrom.m_ThreadID );
#endif

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
			WPTR_THREAD_CHECK;

			if (this == *pointerToMoveFrom)
			{
				DecreaseRefCountAndReset();
				return *this;
			}

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == pointerToMoveFrom.m_ThreadID );
#endif

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
			WPTR_THREAD_CHECK;
			ResetIfExpired();
			return ContainsValidPointer();
		}

		//RESET
		void Reset()
		{
			WPTR_THREAD_CHECK;
			DecreaseRefCountAndReset();
		}

		//SWAP
		void Swap(wptr& pointerToSwapWith)
		{
			//todo check if calls with itself

			WPTR_THREAD_CHECK;

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == pointerToSwapWith.m_ThreadID );
#endif

			ResetIfExpired();
			pointerToSwapWith.ResetIfExpired();
			std::swap(m_Pointer, pointerToSwapWith.m_Pointer);
		}

		//LOCK
		[[nodiscard]] rcptr<T> Lock() const noexcept
		{
			WPTR_THREAD_CHECK;

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
			WPTR_THREAD_CHECK;

			if (ContainsValidPointer())
			{
				return rcptr<U>(*this);
			}
			else
			{
				return rcptr<U>();
			}
		}

		//COMPARISON
		bool operator==(const wptr& ptrToCompareWith) const
		{
			WPTR_THREAD_CHECK;

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == ptrToCompareWith.m_ThreadID );
#endif

			return m_Pointer == ptrToCompareWith.m_Pointer;
		}

		template<typename U> bool operator==(const wptr<U>& ptrToCompareWith) const
		{
			WPTR_THREAD_CHECK;

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == ptrToCompareWith.m_ThreadID );
#endif


			return m_Pointer == ptrToCompareWith.m_Pointer;
		}

		template<typename U> bool operator==(const rcptr<U>& ptrToCompareWith) const
		{
			WPTR_THREAD_CHECK;

#ifdef SMARTPTR_THREAD_VALIDATION
			assert( m_ThreadID == ptrToCompareWith.m_ThreadID );
#endif


			return m_Pointer == ptrToCompareWith.m_Pointer;
		}

		//QUERIES
		[[nodiscard]] bool ContainsValidPointer() const
		{
			WPTR_THREAD_CHECK;

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
			WPTR_THREAD_CHECK;
			return ContainsValidPointer();
		}

		[[nodiscard]] int GetUseCount() const
		{
			WPTR_THREAD_CHECK;

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
			WPTR_THREAD_CHECK;

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
			WPTR_THREAD_CHECK;

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

#ifdef SMARTPTR_THREAD_VALIDATION
		std::thread::id m_ThreadID;
#endif

	};


}


#undef WPTR_THREAD_STORE
#undef WPTR_THREAD_CHECK
