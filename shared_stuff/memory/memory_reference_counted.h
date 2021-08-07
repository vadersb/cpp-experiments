//
// Created by Alexander Samarin on 07.08.2021.
//

#pragma once

#include <cstdint>

namespace st::memory
{
	class ReferenceCounted
	{

	template <typename T> friend class rcptr;

	public:

	protected:

		ReferenceCounted();

		virtual ~ReferenceCounted() = default;

	private:

		void ReferenceCountIncrease();
		void ReferenceCountDecrease();

		int32_t m_ReferenceCount;
	};
}