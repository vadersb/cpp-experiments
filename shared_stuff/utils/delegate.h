//
// Created by Alexander on 11.09.2021.
//

#pragma once

#include <type_traits>
#include <memory>


namespace st::utils
{

	template<typename TReturnType, typename ... Args> class Delegate final
	{
	public:

		TReturnType Call(Args&& ... args)
		{
			//assert(m_Caller != nullptr);
			if (m_Caller != nullptr)
			{
				return m_Caller->Call(std::forward<Args>(args)...);
			}
			else
			{
				return TReturnType();
			}
		}

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


	private:



		class DelegateCaller
		{
		public:

			virtual TReturnType Call(Args&& ... args) = 0;

			virtual DelegateCaller* Copy() = 0;

		private:

		};

		class FunctionDelegateCaller : public DelegateCaller
		{
		private:
			typedef TReturnType (*TFunctionPointer)(Args&& ... args);

		public:

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


		DelegateCaller* GetCallerCopy()
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

		//-----
		//fields

		std::unique_ptr<DelegateCaller> m_Caller;




	};


}
