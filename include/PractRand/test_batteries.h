#ifndef _test_batteries_h
#define _test_batteries_h

namespace PractRand {
	namespace Tests {
		class TestBaseclass;

		class ListOfTests {
		public:
			std::vector<TestBaseclass*> tests;
			ListOfTests ( TestBaseclass **tests_ );
			ListOfTests ( 
				TestBaseclass *test1, TestBaseclass *test2=NULL, TestBaseclass *test3=NULL, TestBaseclass *test4=NULL, 
				TestBaseclass *test5=NULL, TestBaseclass *test6=NULL, TestBaseclass *test7=NULL, TestBaseclass *test8=NULL,
				TestBaseclass *test9=NULL, TestBaseclass *test10=NULL, TestBaseclass *test11=NULL, TestBaseclass *test12=NULL,
				TestBaseclass *test13=NULL, TestBaseclass *test14=NULL, TestBaseclass *test15=NULL, TestBaseclass *test16=NULL
			);
		};

		//batteries
		namespace Batteries {
			//simply calls the destructors on all tests in list of tests
			void destruct_tests(const ListOfTests &tests);

			//recommended tests
			//recommended data transformations
			//(which data transforms are recommended is a function of RNG metadata)
			ListOfTests get_standard_tests(const RNGs::vRNG *rng);

			//recommended tests
			//no data transformations
			ListOfTests get_core_tests();

			//recommended tests
			//extra data transformations
			ListOfTests get_folded_tests();

			//extra tests
			//no data transformations
			ListOfTests get_expanded_core_tests();

			//extra tests
			//recommended data transformations
			//(which data transforms are recommended is a function of RNG metadata)
			ListOfTests get_expanded_standard_tests(const RNGs::vRNG *rng);

			//extra tests
			//extra data transformations
			ListOfTests get_expanded_folded_tests();

		}//Batteries
	}//Tests
}//PractRand

#endif //_test_batteries_h
