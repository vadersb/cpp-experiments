//
// Created by Alexander Samarin on 06.08.2021.
//

#pragma once

#include <cstddef>


namespace st::memory
{
	class Poolable
	{
	public:

		static void* operator new(std::size_t size);

		static void operator delete(void* p, std::size_t size);

	protected:

		//forces any derived classes to have virtual destructor
		//it is required to receive correct *size* value in operator delete
		virtual ~Poolable() = default;

	private:

	};
}