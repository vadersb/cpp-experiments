//
// Created by Alexander on 11.09.2021.
//

#include "delegate.h"
#include <iostream>

int main()
{
	st::utils::Delegate<void, int, int> someDelegate;

	someDelegate.Call(1, 2);

	//std::cout << "return value: " << returnValue << std::endl;

}