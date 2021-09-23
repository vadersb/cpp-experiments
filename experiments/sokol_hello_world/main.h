//
// Created by Alexander on 23.09.2021.
//

#pragma once

template<typename T> void CleanupStruct(T& structToCleanup)
{
	memset(&structToCleanup, 0, sizeof(T));
}
