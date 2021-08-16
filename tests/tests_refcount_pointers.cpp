//
// Created by Alexander on 16.08.2021.
//

#include "catch.hpp"
#include "memory_rcptr.h"
//#include "memory_wptr.h"

class RefCountedItem : public st::memory::ReferenceCounted
{
public:

	explicit RefCountedItem(int intValue) :
	st::memory::ReferenceCounted(),
	m_IntValue(intValue)
	{

	}

	[[nodiscard]] int GetIntValue() const {return m_IntValue;}

private:

	int m_IntValue;
};

class DerivedRefCountedItem : public RefCountedItem
{
public:

	DerivedRefCountedItem(int intValue, float floatValue) :
	RefCountedItem(intValue),
	m_FloatValue(floatValue)
	{

	}

	[[nodiscard]] float GetFloatValue() const {return m_FloatValue;}

private:

	float m_FloatValue;
};


TEST_CASE("rcptr basic usage")
{
	auto ptr = st::memory::CreateRefCountedPointer<RefCountedItem>(123);

	REQUIRE( ptr.ContainsValidPointer() == true );
	REQUIRE(ptr->GetIntValue() == 123 );
	REQUIRE( ptr.GetUseCount() == 1 );

	ptr.Reset();

	REQUIRE( ptr.ContainsValidPointer() == false );
}


st::memory::wptr<DerivedRefCountedItem> GetWeakPointer(st::memory::rcptr<DerivedRefCountedItem> derivedStrongPointer)
{
	st::memory::wptr<DerivedRefCountedItem> weakPointer(derivedStrongPointer);

	return weakPointer;
}


TEST_CASE("wptr copy constructors")
{
	auto ptr = st::memory::CreateRefCountedPointer<DerivedRefCountedItem>(123, 456.0f);

	st::memory::wptr<DerivedRefCountedItem> weakPtr(ptr);

	REQUIRE ( weakPtr.GetWeakReferenceCount() == 1 );

	st::memory::wptr<RefCountedItem> baseWeakPtr(weakPtr);

	REQUIRE( weakPtr.GetWeakReferenceCount() == 2 );

	REQUIRE( baseWeakPtr.ContainsValidPointer() == true );
	REQUIRE( baseWeakPtr.IsExpired() == false );

	ptr.Reset();

	REQUIRE( weakPtr.IsExpired() == true );
	REQUIRE( baseWeakPtr.IsExpired() == true );

	REQUIRE( weakPtr.GetWeakReferenceCount() == 2 );
	REQUIRE( baseWeakPtr.GetWeakReferenceCount() == 2 );

	weakPtr.Refresh();
	baseWeakPtr.Refresh();

	REQUIRE( weakPtr.ContainsValidPointer() == false );
	REQUIRE( baseWeakPtr.ContainsValidPointer() == false );

	REQUIRE( weakPtr.GetWeakReferenceCount() == 0 );
	REQUIRE( baseWeakPtr.GetWeakReferenceCount() == 0 );

}

TEST_CASE("wptr move constructors")
{
	auto ptr = st::memory::CreateRefCountedPointer<DerivedRefCountedItem>(123, 456.0f);

	st::memory::wptr<RefCountedItem> baseWeakPointer(GetWeakPointer(ptr));

	REQUIRE( baseWeakPointer.ContainsValidPointer() == true );

	ptr.Reset();

	REQUIRE( baseWeakPointer.ContainsValidPointer() == false );
}