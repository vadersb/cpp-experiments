//
// Created by Alexander on 13.09.2021.
//

#include "delegate.h"
#include "iostream"

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

	rawDelegateCopy.Call(a, -1.0f);

	std::cout << "a: " << a << std::endl;
	std::cout << "result: " << callResult << std::endl;

}


