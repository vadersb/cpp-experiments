//
// Created by Alexander on 14.08.2021.
//

#pragma once

namespace st::utils
{

	template<typename TFrom, typename TTo> TTo* CheckedDynamicCastUpDown(TFrom* pointer)
	{
		static_assert(std::is_base_of_v<TFrom, TTo> || std::is_base_of_v<TTo, TFrom>);

		if (pointer == nullptr)
		{
			return nullptr;
		}

		TTo* result = dynamic_cast<TTo*>(pointer);

		assert(result != nullptr);

		return result;
	}


	template<typename TFrom, typename TTo> TTo* CheckedDynamicCastUp(TFrom* pointer)
	{
		static_assert(std::is_base_of_v<TTo, TFrom>);

		if (pointer == nullptr)
		{
			return nullptr;
		}

		TTo* result = dynamic_cast<TTo*>(pointer);

		assert(result != nullptr);

		return result;
	}


	template<typename TFrom, typename TTo> TTo* CheckedDynamicCastDown(TFrom* pointer)
	{
		static_assert(std::is_base_of_v<TFrom, TTo>);

		if (pointer == nullptr)
		{
			return nullptr;
		}

		TTo* result = dynamic_cast<TTo*>(pointer);

		assert(result != nullptr);

		return result;
	}



}