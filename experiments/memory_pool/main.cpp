//
// Created by Alexander Samarin on 06.08.2021.
//

#include <string>
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


//todo test a derived class that reverts to standard malloc/free
//THE idea is to replace regular pooling with class specific optimized pooling

void SimpleTests();
void PoolableItemTests();

int main()
{
	//init
	st::memory::MemoryPoolInit(true);

	//doing the tests
	SimpleTests();
	PoolableItemTests();

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
}