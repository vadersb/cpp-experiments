//
// Created by Alexander on 11.08.2021.
//

#pragma once

#include "memory_reference_counted.h"


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
			static_assert(std::is_convertible_v<T, U> || std::is_convertible_v<U, T>);

			m_Pointer = CheckedDynamicCastFrom(ptr);

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
		//todo

		//rcptr
		explicit wptr(const rcptr<T>& strongPointer) : m_Pointer(strongPointer.m_Pointer)
		{
			IncreaseRefCount();
		}


		template<typename U> explicit wptr(const rcptr<U&> strongPointer)
		{
			static_assert(std::is_convertible_v<T, U> || std::is_convertible_v<U, T>);

			m_Pointer = CheckedDynamicCastFrom(strongPointer.m_Pointer);

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


	private:


		template<typename U> static inline T* CheckedDynamicCastFrom(U* ptr)
		{
			if (ptr == nullptr)
			{
				return nullptr;
			}

			T* pResult = dynamic_cast<T*>(ptr);

			assert(pResult != nullptr);

			return pResult;
		}

		template<typename U> static inline U* CheckedDynamicCastTo(T* ptr)
		{
			if (ptr == nullptr)
			{
				return nullptr;
			}

			U* pResult = dynamic_cast<U*>(ptr);

			assert(pResult != nullptr);

			return pResult;
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
