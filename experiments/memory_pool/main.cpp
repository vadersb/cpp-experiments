//
// Created by Alexander Samarin on 06.08.2021.
//

#include <string>
#include <iostream>
#include <vector>
#include "memory_pool.h"
#include "memory_poolable.h"

class TestItem : public st::memory::Poolable
{
public:

	TestItem(int intValue, float floatValue) :
	m_IntValue(intValue),
	m_FloatValue(floatValue)
	{

	}

	virtual ~TestItem() = default;

	[[nodiscard]] int GetIntValue() const {return m_IntValue;}
	[[nodiscard]] float GetFloatValue() const {return m_FloatValue;}

private:

	int m_IntValue;
	float m_FloatValue;
};


class AdvancedTestItem : public TestItem
{
public:

	AdvancedTestItem(int intValue, float floatValue, const char* pStringValue) :
			TestItem(intValue, floatValue),
			m_StringValue(pStringValue)
	{

	}

	[[nodiscard]] const std::string& GetStringValue() const {return m_StringValue;}

private:

	std::string m_StringValue;

};



//THE idea is to replace regular pooling with class specific optimized pooling
class CustomTestItem : public AdvancedTestItem
{
public:

	CustomTestItem() : AdvancedTestItem(123, 456.789f, "default string value"),
	m_AnotherFloatValue(1.0f)
	{
		std::cout << "CustomTestItem constructor" << std::endl;
	}

	~CustomTestItem() override
	{
		m_AnotherFloatValue = 0.0f;
		std::cout << "CustomTestItem destructor" << std::endl;
	}

	[[nodiscard]] float GetAnotherFloatValue() const {return m_AnotherFloatValue;}

	static void* operator new(std::size_t size)
	{
		return std::malloc(size);
	}

	static void operator delete(void* p, std::size_t size)
	{
		std::free(p);
	}


private:

	float m_AnotherFloatValue;

};



void SimpleTests();
void PoolableItemTests();
void MassiveNumberOfItemsTests();

int main()
{
	//init
	st::memory::MemoryPoolInit(true);

	//doing the tests
	SimpleTests();
	PoolableItemTests();
	MassiveNumberOfItemsTests();

	//cleanup
	st::memory::MemoryPoolRelease();

	return 0;
}

void SimpleTests()
{
	int* pInt = st::memory::MemoryPoolAllocate<int>();

	*pInt = 123;

	st::memory::MemoryPoolDeallocate(pInt);

	auto pAnotherInt = st::memory::MemoryPoolAllocate<int>();

	*pAnotherInt = 456;

	st::memory::MemoryPoolDeallocate(pAnotherInt);
}

void PoolableItemTests()
{
	//base item
	auto pItem = new TestItem(123, 456.789f);

	int intValue = pItem->GetIntValue();
	intValue += 234;
	float floatValue = pItem->GetFloatValue();
	floatValue += 1.2f;

	delete pItem;

	//derived item
	auto pAdvancedItem = new AdvancedTestItem(intValue, floatValue, "some string!");

	std::string stringValue = pAdvancedItem->GetStringValue();
	stringValue += "ok ok ";

	delete pAdvancedItem;

	//item with custom new/delete
	TestItem* pCustomItem = new CustomTestItem();

	float anotherFloatValue = pCustomItem->GetFloatValue();

	delete pCustomItem;
}

void MassiveNumberOfItemsTests()
{
	const int MassiveNumber = 30000;

	std::vector<TestItem*> items;

	for (int i = 0; i < MassiveNumber; i++)
	{
		auto pItem = new AdvancedTestItem(i, (float)i * 2, "some string");
		items.push_back(pItem);
	}

	for (int i = 0; i < MassiveNumber; i++)
	{
		delete items[i];
		items[i] = nullptr;
	}


}