//
// Created by Alexander on 11.09.2021.
//

#pragma once

#include <cassert>
#include <type_traits>
#include <memory>
#include "memory_poolable.h"


namespace st::utils
{
	template<typename TReturnType, typename ... Args> class Delegate;
	template<typename TReturnType, typename ... Args> Delegate<TReturnType, Args...> CreateDelegateFromFunction(TReturnType (*pFunctionPointer)(Args...));
	template<typename TObjectType, typename TReturnType, typename ... Args> Delegate<TReturnType, Args...> CreateDelegateFromRawPointer(TObjectType* pObject, TReturnType (TObjectType::*pFunctionPointer)(Args...));


	template<typename TReturnType, typename ... Args> class Delegate final
	{
		//friends
		friend Delegate CreateDelegateFromFunction<>(TReturnType (*pFunctionPointer)(Args...));
		template<typename TObjectType, typename TReturnTypeS, typename ... ArgsS> friend Delegate<TReturnTypeS, ArgsS...> CreateDelegateFromRawPointer(TObjectType* pObject, TReturnTypeS (TObjectType::*pFunctionPointer)(ArgsS...));

	public:

		//DELEGATE CALL
		TReturnType Call(Args&& ... args)
		{
			assert(m_Caller != nullptr);
			return m_Caller->Call(std::forward<Args>(args)...);
		}

		TReturnType TryCall(Args&& ... args)
		{
			if (m_Caller != nullptr)
			{
				return m_Caller->Call(std::forward<Args>(args)...);
			}
			else
			{
				return TReturnType();
			}
		}

		//CONSTRUCTORS
		Delegate() : m_Caller()
		{

		}

		Delegate(const Delegate& delegateToCopyFrom) : m_Caller(delegateToCopyFrom.GetCallerCopy())
		{

		}

		Delegate(Delegate&& delegateToMoveFrom)
		{
			std::swap(m_Caller, delegateToMoveFrom.m_Caller);
		}


		//is valid/is expired
		bool IsValid() const
		{
			return m_Caller != nullptr;
		}

		bool IsExpired() const
		{
			if (m_Caller == nullptr)
			{
				return true;
			}
			else
			{
				return m_Caller->IsExpired();
			}
		}

		void Refresh()
		{
			if (m_Caller != nullptr)
			{
				if (m_Caller->IsExpired())
				{
					m_Caller.reset();
				}
			}
		}

		void Reset()
		{
			m_Caller.reset();
		}




	private:




		class DelegateCaller// : public st::memory::Poolable
		{
		public:

			virtual TReturnType Call(Args&& ... args) = 0;

			virtual DelegateCaller* Copy() = 0;

			virtual bool IsExpired()
			{
				return false;
			}

		private:

		};

		class FunctionDelegateCaller : public DelegateCaller
		{
		//friend Delegate<TReturnType, Args...> CreateDelegate(TReturnType (*pFunctionPointer)(Args&& ... args));

		public:

			typedef TReturnType (*TFunctionPointer)(Args ... args);

			FunctionDelegateCaller(TFunctionPointer pFunction)
			{
				assert(pFunction != nullptr);
				m_pFunction = pFunction;
			}

			virtual TReturnType Call(Args&& ... args)
			{
				return m_pFunction(std::forward<Args>(args)...);
			}

			virtual DelegateCaller* Copy()
			{
				return new FunctionDelegateCaller(m_pFunction);
			}

			private:

				TFunctionPointer m_pFunction;
		};

		template <typename TObject> class RawPointerMemberFunctionCaller : public DelegateCaller
		{
		public:

			typedef TReturnType (TObject::*TFunctionPointer)(Args ... args);

			RawPointerMemberFunctionCaller(TObject* pointer, TFunctionPointer pMemberFunctionPointer)
			{
				assert(pointer != nullptr);
				assert(pMemberFunctionPointer != nullptr);

				m_pPointer = pointer;
				m_pFunctionPointer = pMemberFunctionPointer;
			}

			virtual TReturnType Call(Args&& ... args)
			{
				//return (*m_pPointer).m_pFunctionPointer(std::forward<Args>(args)...);
				return std::invoke(m_pFunctionPointer, *m_pPointer, std::forward<Args>(args)...);
			}

			virtual DelegateCaller* Copy()
			{
				return new RawPointerMemberFunctionCaller(m_pPointer, m_pFunctionPointer);
			}

		private:

			TObject* m_pPointer;
			TFunctionPointer m_pFunctionPointer;
		};

		//todo rcptr caller
		//todo wptr caller
		//todo shared_ptr caller
		//todo weak_ptr caller

		DelegateCaller* GetCallerCopy() const
		{
			if (m_Caller != nullptr)
			{
				return m_Caller->Copy();
			}
			else
			{
				return nullptr;
			}
		}

		//CONSTRUCTOR
		Delegate(DelegateCaller* pCaller) : m_Caller(pCaller)
		{
			assert(pCaller != nullptr);
			//m_Caller = pCaller;
		}

		//-----
		//fields
		std::unique_ptr<DelegateCaller> m_Caller;



	};


	template<typename TReturnType, typename ... Args> Delegate<TReturnType, Args...> CreateDelegateFromFunction(TReturnType (*pFunctionPointer)(Args...))
	{
		auto pCaller = new typename Delegate<TReturnType, Args...>::FunctionDelegateCaller(pFunctionPointer);
		return Delegate<TReturnType, Args...>(pCaller);
	}

	template<typename TObjectType, typename TReturnType, typename ... Args> Delegate<TReturnType, Args...> CreateDelegateFromRawPointer(TObjectType* pObject, TReturnType (TObjectType::*pFunctionPointer)(Args...))
	{
		auto pCaller = new typename Delegate<TReturnType, Args...>::RawPointerMemberFunctionCaller(pObject, pFunctionPointer);
		return Delegate<TReturnType, Args...>(pCaller);
	}

}
