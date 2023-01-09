//
// Created by Alexander Samarin on 06.08.2021.
//



#include "memory_allocator.h"
#include "memory_pool.h"
#include "memory_poolable.h"
#include "memory_reference_counted.h"
#include "memory_rcptr.h"
#include "memory_wptr.h"

#include <string>
#include <iostream>
#include <vector>
#include <cassert>

class TestItem : public st::memory::Poolable<false>
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

	[[maybe_unused]]
	[[nodiscard]] float GetAnotherFloatValue() const {return m_AnotherFloatValue;}

	static void* operator new(std::size_t size)
	{
		return std::malloc(size);
	}

	static void operator delete(void* p, [[maybe_unused]] std::size_t size)
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


class [[maybe_unused]] DoubleDerivedRefCountedItem : public DerivedRefCountedItem
{
public:

	[[maybe_unused]] DoubleDerivedRefCountedItem(int intValue, float floatValue) :
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
void AllocatorTests();
void SmartPointerThreadViolationTest();
void ReceiveDerivedItemReferenceAndPrint(const DerivedRefCountedItem& item);

int main()
{
	//init
	st::memory::MemoryPoolSingleThreaded::Init();
	st::memory::MemoryPoolMultiThreaded::Init();

	//doing the tests
	SimpleTests();
	PoolableItemTests();
	MassiveNumberOfItemsTests();
	RefCountedTests();
	AllocatorTests();

	//SmartPointerThreadViolationTest(); //uncomment for the test

	//cleanup
	st::memory::MemoryPoolMultiThreaded::Release();
	st::memory::MemoryPoolSingleThreaded::Release();

	return 0;
}

void SimpleTests()
{
	int* pInt = st::memory::MemoryPoolSingleThreaded::Allocate<int>();

	*pInt = 123;

	st::memory::MemoryPoolSingleThreaded::Deallocate(pInt);

	auto pAnotherInt = st::memory::MemoryPoolSingleThreaded::Allocate<int>();

	*pAnotherInt = 456;

	st::memory::MemoryPoolSingleThreaded::Deallocate(pAnotherInt);
}

void PoolableItemTests()
{
	//base item
	auto pItem = std::make_unique<TestItem>(123, 456.789f);

	int intValue = pItem->GetIntValue();
	intValue += 234;
	float floatValue = pItem->GetFloatValue();
	floatValue += 1.2f;


	//derived item
	auto pAdvancedItem = std::make_unique<AdvancedTestItem>(intValue, floatValue, "some string!");

	std::string stringValue = pAdvancedItem->GetStringValue();
	stringValue += "ok ok ";


	//item with custom new/delete
	TestItem* pCustomItem = new CustomTestItem();

	[[maybe_unused]]
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
	//st::memory::rcptr<RefCountedTestItem> Get(st::memory::CreateRefCountedPointer<DerivedRefCountedItem>(123, 456.789f));

	auto ptr = st::memory::CreateRefCountedPointer<RefCountedTestItem, DerivedRefCountedItem>(123, 456.789f);

	st::memory::rcptr<RefCountedTestItem> basePtr;

	basePtr = ptr;

	assert(ptr.ContainsValidPointer() == true);
	assert(basePtr.ContainsValidPointer() == true);

	float doubleFloatValue = basePtr.Get<DerivedRefCountedItem>()->GetDoubleFloatValue();

	std::cout << "double float value: " << doubleFloatValue << std::endl;

	int doubleIntValue = basePtr->GetDoubleIntValue();

	std::cout << "double int value: " << doubleIntValue << std::endl;

	//-----------
	st::memory::rcptr<DerivedRefCountedItem> thirdPtr;

	assert(thirdPtr.ContainsValidPointer() == false);

	thirdPtr = basePtr;

	assert(thirdPtr.ContainsValidPointer() == true);

	doubleFloatValue = thirdPtr->GetDoubleFloatValue();


	std::cout << "again double float value: " << doubleFloatValue << std::endl;

	basePtr = thirdPtr;

	//--------
	//weak ptr

	st::memory::wptr<DerivedRefCountedItem> weakPtr(thirdPtr);

	assert(weakPtr.ContainsValidPointer() == true);

	std::cout <<"use count from weak pointer: " << weakPtr.GetUseCount() << std::endl;

	ReceiveDerivedItemReferenceAndPrint(weakPtr.PassRef());

	//copy
	auto anotherWeekPtr(weakPtr);

	std::cout <<"use count from copied weak pointer: " << anotherWeekPtr.GetUseCount() << std::endl;

	anotherWeekPtr.Reset();

	//base copy
	st::memory::wptr<RefCountedTestItem> baseWeakPtr(anotherWeekPtr);

	//comparison test
	if (anotherWeekPtr == baseWeakPtr)
	{
		std::cout << "weak ptrs are equal" << std::endl;
	}
	else
	{
		std::cout << "weak ptrs are not equal" << std::endl;
	}

	baseWeakPtr.Refresh();

	//refresh
	weakPtr.Refresh();

	if (weakPtr.IsExpired())
	{
		std::cout << "weak ptr is expired!" << std::endl;
	}


	//lock
	auto lockedPointer = weakPtr.Lock<RefCountedTestItem>();

	if (lockedPointer == weakPtr)
	{
		std::cout << "lockedPointer is equal to weakPtr " << std::endl;
	}

	std::cout << "use count after lock: " << weakPtr.GetUseCount() << std::endl;

	std::cout << "use count from locked pointer: " << lockedPointer.GetUseCount() << std::endl;

	lockedPointer.Reset();

	std::cout << "use count after locked pointer reset: " << weakPtr.GetUseCount() << std::endl;

	//invalid cast test
	//[[maybe_unused]]
	//int tripleIntValue = thirdPtr.Get<DoubleDerivedRefCountedItem>()->GetTripleIntValue();


	//---------

	basePtr.Reset();

	assert(basePtr.ContainsValidPointer() == false);
	assert(ptr.ContainsValidPointer() == true);

	std::cout << "size of pointer object: " << sizeof(basePtr) << std::endl;

	//test pointer for non ref counted class
	//st::memory::rcptr<TestItem> testPtr;
}

void PrintString(const std::string& stringToPrint);
void PrintString(std::string_view stringToPrint);
//template<typename TChar, typename TAllocator> void PrintString(std::basic_string<TChar, std::char_traits<TChar>, TAllocator> stringToPrint);

void ReceiveDerivedItemReferenceAndPrint(const DerivedRefCountedItem& item)
{
	std::cout << "float value of passed pointer: " << item.GetFloatValue() << std::endl;
}


void AllocatorTests()
{
	std::vector<int, st::memory::AllocatorSingleThreaded<int>> testVector;

	testVector.reserve(64);

	for (int i = 0; i < 1024; i++)
	{
		testVector.push_back(i);
	}

	using stringWithAlloc = std::basic_string<char, std::char_traits<char>, st::memory::AllocatorMultiThreaded<char>>;

	stringWithAlloc testString;

	testString = "ok!";

	std::cout << "short string: " << testString << std::endl;

	testString = "some long string!";

	std::cout << "string: " << testString << std::endl;

	testString = "final string value";

	PrintString(testString);

	std::string stdTestString(testString);
	stdTestString += " and some more...";

	PrintString(stdTestString);

	std::cout << "and btw, size of string object: " << sizeof(std::string) << std::endl;
	std::cout << "and size of string with custom allocator: " << sizeof(stringWithAlloc) <<std::endl;
}


void PrintString(const std::string& stringToPrint)
{
	std::cout << "string: " << stringToPrint << std::endl;
}

void PrintString(const std::string_view stringToPrint)
{
	std::cout << "string (view): " << stringToPrint << std::endl;
}

//template<typename TChar, typename TAllocator> void PrintString(const std::basic_string<TChar, std::char_traits<TChar>, TAllocator> stringToPrint)
//{
//	std::cout << "string: " << stringToPrint << std::endl;
//}


template<typename T> void CheckIfPointerIsValid(const st::memory::rcptr<T>& ptrToCheck)
{
	if (ptrToCheck.ContainsValidPointer() == false)
	{
		std::cout << "pointer is not valid! \n";
	}
}

void SmartPointerThreadViolationTest()
{
	auto ptr = st::memory::CreateRefCountedPointer<RefCountedTestItem, DerivedRefCountedItem>(123, 456.789f);

	std::thread thread(CheckIfPointerIsValid<RefCountedTestItem>, ptr);

	thread.join();
}