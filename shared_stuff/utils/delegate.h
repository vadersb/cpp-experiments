//
// Created by Alexander on 11.09.2021.
//

#pragma once

#include <cassert>
#include <type_traits>
#include <memory>
#include <functional>
#include "memory_poolable.h"
#include "memory_rcptr.h"
#include "utils_cast.h"


namespace st::utils
{
	template<typename TReturnType, typename ... Args> class Delegate;
	template<typename TReturnType, typename ... Args> Delegate<TReturnType, Args...> CreateDelegateFromFunction(TReturnType (*pFunctionPointer)(Args...));
	template<typename TObjectType, typename TReturnType, typename ... Args> Delegate<TReturnType, Args...> CreateDelegateFromRawPointer(TObjectType* pObject, TReturnType (TObjectType::*pFunctionPointer)(Args...));
	template<typename TObjectType, typename TReturnType, typename ... Args> Delegate<TReturnType, Args...> CreateDelegateFromRefCountedPointer(const memory::rcptr<TObjectType>& ptr, TReturnType (TObjectType::*pFunctionPointer)(Args...));
	template<typename TObjectType, typename TReturnType, typename ... Args> Delegate<TReturnType, Args...> CreateDelegateFromWeakRefCountedPointer(const memory::wptr<TObjectType>& ptr, TReturnType (TObjectType::*pFunctionPointer)(Args...));
	template<typename TObjectType, typename TReturnType, typename ... Args> Delegate<TReturnType, Args...> CreateDelegateFromSharedPointer(const std::shared_ptr<TObjectType>& ptr, TReturnType (TObjectType::*pFunctionPointer)(Args...));
	template<typename TObjectType, typename TReturnType, typename ... Args> Delegate<TReturnType, Args...> CreateDelegateFromSharedPointerWeakRef(const std::shared_ptr<TObjectType>& ptr, TReturnType (TObjectType::*pFunctionPointer)(Args...));
	template<typename TReturnType, typename ... Args> Delegate<TReturnType, Args...> CreateDelegateFromStdFunction(const std::function<TReturnType(Args...)>& function);

	template<typename TReturnType, typename ... Args> class Delegate final
	{

		//friends
		friend Delegate CreateDelegateFromFunction<>(TReturnType (*pFunctionPointer)(Args...));
		template<typename TObjectType, typename TReturnTypeS, typename ... ArgsS> friend Delegate<TReturnTypeS, ArgsS...> CreateDelegateFromRawPointer(TObjectType* pObject, TReturnTypeS (TObjectType::*pFunctionPointer)(ArgsS...));
		template<typename TObjectType, typename TReturnTypeS, typename ... ArgsS> friend Delegate<TReturnTypeS, ArgsS...> CreateDelegateFromRefCountedPointer(const memory::rcptr<TObjectType>& ptr, TReturnTypeS (TObjectType::*pFunctionPointer)(ArgsS...));
		template<typename TObjectType, typename TReturnTypeS, typename ... ArgsS> friend Delegate<TReturnTypeS, ArgsS...> CreateDelegateFromWeakRefCountedPointer(const memory::wptr<TObjectType>& ptr, TReturnTypeS (TObjectType::*pFunctionPointer)(ArgsS...));
		template<typename TObjectType, typename TReturnTypeS, typename ... ArgsS> friend Delegate<TReturnTypeS, ArgsS...> CreateDelegateFromSharedPointer(const std::shared_ptr<TObjectType>& ptr, TReturnTypeS (TObjectType::*pFunctionPointer)(ArgsS...));
		template<typename TObjectType, typename TReturnTypeS, typename ... ArgsS> friend Delegate<TReturnTypeS, ArgsS...> CreateDelegateFromSharedPointerWeakRef(const std::shared_ptr<TObjectType>& ptr, TReturnTypeS (TObjectType::*pFunctionPointer)(ArgsS...));
		friend Delegate CreateDelegateFromStdFunction<>(const std::function<TReturnType(Args...)>& function);

	public:

		//DELEGATE CALL
		TReturnType Call(Args&& ... args) const
		{
			assert(m_Caller != nullptr);
			return m_Caller->Call(std::forward<Args>(args)...);
		}

		//operator option
		TReturnType operator()(Args&& ... args) const
		{
			assert(m_Caller != nullptr);
			return m_Caller->Call(std::forward<Args>(args)...);
		}


		//variant that handles lack of the caller
		TReturnType TryCall(Args&& ... args) const
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


		//COMPARISON
		bool operator==(const Delegate& delegateToCompareWith) const
		{
			if (this == &delegateToCompareWith) return true;

			if (m_Caller != nullptr && delegateToCompareWith.m_Caller != nullptr)
			{
				return m_Caller->IsEqual(delegateToCompareWith.m_Caller.get());
			}
			else
			{
				if (m_Caller == nullptr && delegateToCompareWith.m_Caller == nullptr)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}


	private:




		class DelegateCaller : public st::memory::Poolable<true>
		{
		public:

			virtual TReturnType Call(Args&& ... args) = 0;

			virtual DelegateCaller* Copy() = 0;

			virtual bool IsExpired()
			{
				return false;
			}

			virtual bool IsEqual(DelegateCaller* pCallerToCompareWith) = 0;

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

			virtual bool IsEqual(DelegateCaller* pCallerToCompareWith)
			{
				auto castedPtr = dynamic_cast<FunctionDelegateCaller*>(pCallerToCompareWith);

				if (castedPtr == nullptr)
				{
					return false;
				}
				else
				{
					return castedPtr->m_pFunction == m_pFunction;
				}
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

			virtual bool IsEqual(DelegateCaller* pCallerToCompareWith)
			{
				auto castedPtr = dynamic_cast<RawPointerMemberFunctionCaller*>(pCallerToCompareWith);

				if (castedPtr == nullptr)
				{
					return false;
				}
				else
				{
					return castedPtr->m_pFunctionPointer == m_pFunctionPointer && castedPtr->m_pPointer == m_pPointer;
				}
			}

		private:

			TObject* m_pPointer;
			TFunctionPointer m_pFunctionPointer;
		};


		template<typename TObject> class RefCountedPointerMemberFunctionCaller : public DelegateCaller
		{
		public:

			typedef TReturnType (TObject::*TFunctionPointer)(Args ... args);

			RefCountedPointerMemberFunctionCaller(const memory::rcptr<TObject>& ptr, TFunctionPointer pMemberFunctionPointer) : m_Pointer(ptr)
			{
				assert(ptr.ContainsValidPointer());
				assert(pMemberFunctionPointer != nullptr);

				m_pFunctionPointer = pMemberFunctionPointer;
			}

			virtual TReturnType Call(Args&& ... args)
			{
				return std::invoke(m_pFunctionPointer, *m_Pointer, std::forward<Args>(args)...);
			}

			virtual DelegateCaller* Copy()
			{
				return new RefCountedPointerMemberFunctionCaller(m_Pointer, m_pFunctionPointer);
			}

			virtual bool IsEqual(DelegateCaller* pCallerToCompareWith)
			{
				auto castedPtr = dynamic_cast<RefCountedPointerMemberFunctionCaller*>(pCallerToCompareWith);

				if (castedPtr == nullptr)
				{
					return false;
				}
				else
				{
					return castedPtr->m_pFunctionPointer == m_pFunctionPointer && castedPtr->m_Pointer == m_Pointer;
				}
			}

		private:

			memory::rcptr<TObject> m_Pointer;
			TFunctionPointer m_pFunctionPointer;
		};


		template<typename TObject> class WeakRefCountedPointerMemberFunctionCaller : public DelegateCaller
		{
		public:

			typedef TReturnType (TObject::*TFunctionPointer)(Args ... args);

			WeakRefCountedPointerMemberFunctionCaller(const memory::wptr<TObject>& ptr, TFunctionPointer pMemberFunctionPointer) : m_Pointer(ptr)
			{
				assert(ptr.ContainsValidPointer());
				assert(pMemberFunctionPointer != nullptr);

				m_pFunctionPointer = pMemberFunctionPointer;
			}

			virtual TReturnType Call(Args&& ... args)
			{
				auto ptr = m_Pointer.Lock();

				if (ptr.ContainsValidPointer())
				{
					return std::invoke(m_pFunctionPointer, *ptr, std::forward<Args>(args)...);
				}
				else
				{
					return TReturnType();
				}
			}

			virtual DelegateCaller* Copy()
			{
				return new WeakRefCountedPointerMemberFunctionCaller(m_Pointer, m_pFunctionPointer);
			}

			virtual bool IsExpired()
			{
				return m_Pointer.IsExpired();
			}

			virtual bool IsEqual(DelegateCaller* pCallerToCompareWith)
			{
				auto castedPtr = dynamic_cast<WeakRefCountedPointerMemberFunctionCaller*>(pCallerToCompareWith);

				if (castedPtr == nullptr)
				{
					return false;
				}
				else
				{
					return castedPtr->m_pFunctionPointer == m_pFunctionPointer && castedPtr->m_Pointer == m_Pointer;
				}
			}

		private:

			memory::wptr<TObject> m_Pointer;
			TFunctionPointer m_pFunctionPointer;
		};

		template<typename TObject> class SharedPointerMemberFunctionCaller : public DelegateCaller
		{
		public:

			typedef TReturnType (TObject::*TFunctionPointer)(Args ... args);

			SharedPointerMemberFunctionCaller(const std::shared_ptr<TObject>& ptr, TFunctionPointer pFunctionPointer) : m_Pointer(ptr)
			{
				assert(ptr != nullptr);
				assert(pFunctionPointer != nullptr);

				m_pFunction = pFunctionPointer;
			}

			virtual TReturnType Call(Args&& ... args)
			{
				return std::invoke(m_pFunction, *m_Pointer, std::forward<Args>(args)...);
			}

			virtual DelegateCaller* Copy()
			{
				return new SharedPointerMemberFunctionCaller(m_Pointer, m_pFunction);
			}

			virtual bool IsEqual(DelegateCaller* pCallerToCompareWith)
			{
				auto castedPtr = dynamic_cast<SharedPointerMemberFunctionCaller*>(pCallerToCompareWith);

				if (castedPtr == nullptr)
				{
					return false;
				}
				else
				{
					return castedPtr->m_pFunction == m_pFunction && castedPtr->m_Pointer == m_Pointer;
				}
			}

		private:

			std::shared_ptr<TObject> m_Pointer;
			TFunctionPointer m_pFunction;

		};


		template<typename TObject> class WeakSharedPointerMemberFunctionCaller : public DelegateCaller
		{
		public:

			typedef TReturnType (TObject::*TFunctionPointer)(Args ... args);

			WeakSharedPointerMemberFunctionCaller(const std::shared_ptr<TObject>& ptr, TFunctionPointer pFunctionPointer) : m_Pointer(ptr)
			{
				assert(ptr != nullptr);
				assert(pFunctionPointer != nullptr);

				m_pFunction = pFunctionPointer;
			}

			virtual TReturnType Call(Args&& ... args)
			{
				auto ptr = m_Pointer.lock();

				if (ptr != nullptr)
				{
					return std::invoke(m_pFunction, *ptr, std::forward<Args>(args)...);
				}
				else
				{
					return TReturnType();
				}
			}

			virtual DelegateCaller* Copy()
			{
				auto ptr = m_Pointer.lock();
				return new WeakSharedPointerMemberFunctionCaller(ptr, m_pFunction);
			}

			virtual bool IsEqual(DelegateCaller* pCallerToCompareWith)
			{
				auto castedPtr = dynamic_cast<WeakSharedPointerMemberFunctionCaller*>(pCallerToCompareWith);

				if (castedPtr == nullptr)
				{
					return false;
				}
				else
				{
					auto myPtr = m_Pointer.lock();
					auto otherPtr = castedPtr->m_Pointer.lock();

					return castedPtr->m_pFunction == m_pFunction && myPtr == otherPtr;
				}
			}

		private:

			std::weak_ptr<TObject> m_Pointer;
			TFunctionPointer m_pFunction;
		};


		class StdFunctionDelegateCaller : public DelegateCaller
		{
			//friend Delegate<TReturnType, Args...> CreateDelegate(TReturnType (*pFunctionPointer)(Args&& ... args));

		public:

			typedef TReturnType (*TFunctionPointer)(Args ... args);

			StdFunctionDelegateCaller(const std::function<TReturnType(Args...)>& function) : m_Function(function)
			{
				assert(m_Function != nullptr);
			}

			virtual TReturnType Call(Args&& ... args)
			{
				return m_Function(std::forward<Args>(args)...);
			}

			virtual DelegateCaller* Copy()
			{
				return new StdFunctionDelegateCaller(m_Function);
			}

			virtual bool IsEqual(DelegateCaller* pCallerToCompareWith)
			{
				return false;
			}

		private:

			std::function<TReturnType(Args...)> m_Function;
		};


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
		typedef typename Delegate<TReturnType, Args...>::FunctionDelegateCaller FunctionDelegateCaller;
		auto pCaller = new FunctionDelegateCaller(pFunctionPointer);
		return Delegate<TReturnType, Args...>(pCaller);
	}

	template<typename TObjectType, typename TReturnType, typename ... Args> Delegate<TReturnType, Args...> CreateDelegateFromRawPointer(TObjectType* pObject, TReturnType (TObjectType::*pFunctionPointer)(Args...))
	{
		auto pCaller = new typename Delegate<TReturnType, Args...>::RawPointerMemberFunctionCaller(pObject, pFunctionPointer);
		return Delegate<TReturnType, Args...>(pCaller);
	}

	template<typename TObjectType, typename TReturnType, typename ... Args> Delegate<TReturnType, Args...> CreateDelegateFromRefCountedPointer(const memory::rcptr<TObjectType>& ptr, TReturnType (TObjectType::*pFunctionPointer)(Args...))
	{
		auto pCaller = new typename Delegate<TReturnType, Args...>::RefCountedPointerMemberFunctionCaller(ptr, pFunctionPointer);
		return Delegate<TReturnType, Args...>(pCaller);
	}

	template<typename TObjectType, typename TReturnType, typename ... Args> Delegate<TReturnType, Args...> CreateDelegateFromWeakRefCountedPointer(const memory::wptr<TObjectType>& ptr, TReturnType (TObjectType::*pFunctionPointer)(Args...))
	{
		assert(ptr.ContainsValidPointer());
		auto pCaller = new typename Delegate<TReturnType, Args...>::WeakRefCountedPointerMemberFunctionCaller(ptr, pFunctionPointer);
		return Delegate<TReturnType, Args...>(pCaller);
	}

	template<typename TObjectType, typename TReturnType, typename ... Args> Delegate<TReturnType, Args...> CreateDelegateFromSharedPointer(const std::shared_ptr<TObjectType>& ptr, TReturnType (TObjectType::*pFunctionPointer)(Args...))
	{
		assert(ptr != nullptr);
		auto pCaller = new typename Delegate<TReturnType, Args...>::SharedPointerMemberFunctionCaller(ptr, pFunctionPointer);
		return Delegate<TReturnType, Args...>(pCaller);
	}

	template<typename TObjectType, typename TReturnType, typename ... Args> Delegate<TReturnType, Args...> CreateDelegateFromSharedPointerWeakRef(const std::shared_ptr<TObjectType>& ptr, TReturnType (TObjectType::*pFunctionPointer)(Args...))
	{
		assert(ptr != nullptr);
		auto pCaller = new typename Delegate<TReturnType, Args...>::WeakSharedPointerMemberFunctionCaller(ptr, pFunctionPointer);
		return Delegate<TReturnType, Args...>(pCaller);
	}


	//limitation: std::function objects are not comparable, so it's impossible to use them in event dispatcher listeners container
	template<typename TReturnType, typename ... Args> Delegate<TReturnType, Args...> CreateDelegateFromStdFunction(const std::function<TReturnType(Args...)>& function)
	{
		assert(function != nullptr);
		auto pCaller = new typename Delegate<TReturnType, Args...>::StdFunctionDelegateCaller(function);
		return Delegate<TReturnType, Args...>(pCaller);
	}

}
