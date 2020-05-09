#include <cppunit/TestCase.h>

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "updatechecker.h"
#include "levelGen.h"

using namespace std;

class LevelTest : public CppUnit::TestCase {

public:
	CPPUNIT_TEST_SUITE( LevelTest );

	CPPUNIT_TEST( testGenerateLevel );

	CPPUNIT_TEST_SUITE_END();

	void testGenerateLevel() {

		createKruskalMaze(10);
		CPPUNIT_ASSERT (1+1 == 2);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( LevelTest );
