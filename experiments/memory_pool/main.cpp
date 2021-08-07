//
// Created by Alexander Samarin on 06.08.2021.
//

#include <string>
#include <iostream>
#include <vector>
#include "memory_pool.h"
#include "memory_poolable.h"
#include "memory_reference_counted.h"
#include "memory_rcptr.h"

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


class RefCountedTestItem : public TestItem, public st::memory::ReferenceCounted
{
public:

	RefCountedTestItem(int intValue, float floatValue) :
			TestItem(intValue, floatValue),
			st::memory::ReferenceCounted(),
			m_DoubleIntValue(intValue * 2)
	{

	}

	[[nodiscard]] int GetDoubleIntValue() const
	{
		return m_DoubleIntValue;
	}

private:

	int m_DoubleIntValue;

};


class DerivedRefCountedItem : public RefCountedTestItem
{
public:

	DerivedRefCountedItem(int intValue, float floatValue) :
			RefCountedTestItem(intValue, floatValue),
			m_DoubleFloatValue(floatValue * 2.0f)
	{

	}

	[[nodiscard]] float GetDoubleFloatValue() const
	{
		return m_DoubleFloatValue;
	}

private:

	float m_DoubleFloatValue;
};

class DoubleDerivedRefCountedItem : public DerivedRefCountedItem
{
public:

	DoubleDerivedRefCountedItem(int intValue, float floatValue) :
			DerivedRefCountedItem(intValue, floatValue),
			m_TripleIntValue(intValue * 3)
	{

	}

	[[nodiscard]] int GetTripleIntValue() const
	{
		return m_TripleIntValue;
	}

private:

	int m_TripleIntValue;
};


void SimpleTests();
void PoolableItemTests();
void MassiveNumberOfItemsTests();
void RefCountedTests();

int main()
{
	//init
	st::memory::MemoryPoolInit(true);

	//doing the tests
	SimpleTests();
	PoolableItemTests();
	MassiveNumberOfItemsTests();
	RefCountedTests();

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
	std::cout << "Massive number of items tests started... " << std::endl;

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

	std::cout << "DONE. " << std::endl;

}

void RefCountedTests()
{
	//st::memory::rcptr<RefCountedTestItem> ptr(st::memory::CreateRefCountedPointer<DerivedRefCountedItem>(123, 456.789f));

	auto ptr = st::memory::CreateRefCountedPointer<RefCountedTestItem, DerivedRefCountedItem>(123, 456.789f);

	st::memory::rcptr<RefCountedTestItem> basePtr;

	basePtr = ptr;

	assert(ptr.ContainsValidPointer() == true);
	assert(basePtr.ContainsValidPointer() == true);

	float doubleFloatValue = basePtr.ptr<DerivedRefCountedItem>()->GetDoubleFloatValue();

	std::cout << "double float value: " << doubleFloatValue << std::endl;

	int doubleIntValue = basePtr.ptr()->GetDoubleIntValue();

	std::cout << "double int value: " << doubleIntValue << std::endl;

	//-----------
	st::memory::rcptr<DerivedRefCountedItem> thirdPtr;

	assert(thirdPtr.ContainsValidPointer() == false);

	thirdPtr = basePtr;

	assert(thirdPtr.ContainsValidPointer() == true);

	doubleFloatValue = thirdPtr.ptr()->GetDoubleFloatValue();

	std::cout << "again double float value: " << doubleFloatValue << std::endl;

	basePtr = thirdPtr;


	//invalid cast test
	//[[maybe_unused]]
	//int tripleIntValue = thirdPtr.ptr<DoubleDerivedRefCountedItem>()->GetTripleIntValue();


	//---------

	basePtr.Reset();

	assert(basePtr.ContainsValidPointer() == false);
	assert(ptr.ContainsValidPointer() == true);

	std::cout << "size of pointer object: " << sizeof(basePtr) << std::endl;

	//test pointer for non ref counted class
	//st::memory::rcptr<TestItem> testPtr;
}