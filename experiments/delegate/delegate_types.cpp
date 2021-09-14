//
// Created by Alexander on 13.09.2021.
//

#include "delegate.h"
#include "iostream"
#include "memory_reference_counted.h"

int SimpleFunction(int& refInt, float floatValue)
{
	refInt *= 2;

	int result;

	if (floatValue > 0.0f)
	{
		result = refInt + 10;
	}
	else
	{
		result = refInt - 10;
	}

	return result;
}

class SimpleClass
{
public:

	SimpleClass():m_Value(100)
	{

	}

	int SimpleFunction(int& refInt, float floatValue)
	{
		refInt *= 2;

		int result;

		if (floatValue > 0.0f)
		{
			result = refInt + m_Value;
		}
		else
		{
			result = refInt - m_Value;
		}

		return result;
	}


private:

	int m_Value;

};

class RefCountedClass : public st::memory::ReferenceCounted
{
public:

	RefCountedClass() : ReferenceCounted(), m_Value(200)
	{

	}

	int SimpleFunction(int& refInt, float floatValue)
	{
		refInt *= 2;

		int result;

		if (floatValue > 0.0f)
		{
			result = refInt + m_Value;
		}
		else
		{
			result = refInt - m_Value;
		}

		return result;
	}

private:

	int m_Value;

};


void DelegateTypesTest()
{

	//function
	auto functionDelegate = st::utils::CreateDelegateFromFunction(&SimpleFunction);

	int a = 10;

	int callResult = functionDelegate.Call(a, 1.0f);

	std::cout << "a: " << a << std::endl;
	std::cout << "result: " << callResult << std::endl;


	//raw pointer
	SimpleClass simpleObject;
	auto rawPointerDelegate = st::utils::CreateDelegateFromRawPointer(&simpleObject, &SimpleClass::SimpleFunction);

	callResult = rawPointerDelegate.Call(a, 1.0f);

	std::cout << "a: " << a << std::endl;
	std::cout << "result: " << callResult << std::endl;

	//raw pointer delegate copy
	auto rawDelegateCopy(rawPointerDelegate);

	callResult = rawDelegateCopy.Call(a, -1.0f);

	std::cout << "a: " << a << std::endl;
	std::cout << "result: " << callResult << std::endl;


	//----------------------------------------------------
	//ref counted pointer delegate
	auto ptr = st::memory::CreateRefCountedPointer<RefCountedClass>();

	auto rcDelegate = st::utils::CreateDelegateFromRefCountedPointer(ptr, &RefCountedClass::SimpleFunction);

	callResult = rcDelegate.Call(a, 1.0f);

	std::cout << "a: " << a << std::endl;
	std::cout << "result: " << callResult << std::endl;

	//weak ptr
	auto weak_ptr = st::memory::wptr(ptr);

	auto wrcDelegate = st::utils::CreateDelegateFromWeakRefCountedPointer(weak_ptr, &RefCountedClass::SimpleFunction);

	callResult = wrcDelegate.Call(a, 1.0f);

	std::cout << "a: " << a << std::endl;
	std::cout << "result: " << callResult << std::endl;

	//-------------------------------------------------------
	//std smart pointers
	auto shPtr = std::make_shared<SimpleClass>();

	auto shDelegate = st::utils::CreateDelegateFromSharedPointer(shPtr, &SimpleClass::SimpleFunction);

	callResult = shDelegate.Call(a, 1.0f);

	std::cout << "a: " << a << std::endl;
	std::cout << "result: " << callResult << std::endl;

	auto wshDelegate = st::utils::CreateDelegateFromSharedPointerWeakRef(shPtr, &SimpleClass::SimpleFunction);

	callResult = wshDelegate.Call(a, 1.0f);

	std::cout << "a: " << a << std::endl;
	std::cout << "result: " << callResult << std::endl;

	//-------------------------------------------------------
	//lambdas

	auto someLambda = [](int& refInt, float floatValue)
			{
				refInt *= 2;

				int result;

				if (floatValue > 0.0f)
				{
					result = refInt + 35;
				}
				else
				{
					result = refInt - 35;
				}

				return result;
			};

	std::function stdFunction(someLambda);

	std::cout << "someLambda size: " << sizeof(someLambda) << std::endl;
	std::cout << "stdFunction size: " << sizeof(stdFunction) << std::endl;

	auto stdFuncDelegate = st::utils::CreateDelegateFromStdFunction(stdFunction);

	callResult = stdFuncDelegate(a, 1.0f);

	std::cout << "a: " << a << std::endl;
	std::cout << "result: " << callResult << std::endl;

}

void DelegatePoolableDebugTest()
{
	auto delegate = st::utils::CreateDelegateFromFunction(&SimpleFunction);

	int a = 10;

	int callResult = delegate(a, 1.0f);

	std::cout << "call result: " << callResult << std::endl;
	std::cout << "a: " << a << std::endl;
}
