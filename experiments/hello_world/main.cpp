//
// Created by Alexander Samarin on 17.07.2021.
//

#include <iostream>
#include <filesystem>
#include <fstream>
#include "test_func.h"
#include "spdlog\spdlog.h"

void CreateTempFile(const std::filesystem::path& dir, const char* pFilename = "test_output.txt");

int main()
{
	auto curWorkingDirectory = std::filesystem::current_path();

	spdlog::info("working directory: \n{}\n", curWorkingDirectory.string());

	//spdlog::info("   after removing filename: \n {}", curWorkingDirectory.remove_filename().string());

	auto curTempDirectory = std::filesystem::temp_directory_path();

	spdlog::info("temp directory: \n{}\n", curTempDirectory.string());

	CreateTempFile(curWorkingDirectory);
	CreateTempFile(curTempDirectory);

	std::cout << "Hello, World!" << std::endl;
	std::cout << "test func output: " << st::TestFunc(123) << std::endl;
	return 0;
}


void CreateTempFile(const std::filesystem::path& dir, const char* pFilename)
{
	assert(pFilename != nullptr);

	std::string fullFilename = dir.string();

	if (fullFilename[fullFilename.size() - 1] != '\\')
	{
		fullFilename += "\\";
	}

	fullFilename += pFilename;

	//file
	std::ofstream fstream(fullFilename, std::ios::binary);

	fstream << "0 1 2 3 4 5 6 7 8 9\n\n";
	fstream << "Test text.\n\n";
	fstream << "   Test list:\n";
	fstream << "      1. One\n";
	fstream << "      2. Two\n";
	fstream << "      3. Three\n";
}