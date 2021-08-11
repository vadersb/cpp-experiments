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

		static_assert(std::is_base_of_v<ReferenceCounted, T>);

	}



}
