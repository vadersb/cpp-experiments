//
// Created by Alexander on 29.09.2021.
//

#pragma once



//simple template test
template<bool flag>
int g_SomeGlobalValue = 0;

template<bool flag>
void SetGlobalValue(int value)
{
	g_SomeGlobalValue<flag> = value;
}

auto SetTrueGlobalValue = SetGlobalValue<true>;

template<bool flag>
int GetGlobalValue()
{
	return g_SomeGlobalValue<flag>;
}