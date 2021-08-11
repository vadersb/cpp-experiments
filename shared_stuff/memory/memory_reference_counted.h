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
	template <typename T> friend class wptr;

	public:

	protected:

		ReferenceCounted();

		virtual ~ReferenceCounted() = default;

		virtual void OnNoReferenceCountingOwnersLeft() {};

	private:

		[[nodiscard]] int GetReferenceCount() const {return m_ReferenceCount;}
		[[nodiscard]] int GetWeakReferenceCount() const {return m_WeakReferenceCount;}

		void ReferenceCountStart();
		void ReferenceCountIncrease();
		void ReferenceCountDecrease();

		void WeakReferenceCountIncrease();
		void WeakReferenceCountDecrease();

		int32_t m_ReferenceCount;
		int32_t m_WeakReferenceCount;
	};
}