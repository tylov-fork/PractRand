#include <string>
//#include <ostream>
#include <sstream>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <cmath>
#include <cstdlib>

#include "PractRand/config.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/tests.h"
#include "PractRand/test_helpers.h"
#include "PractRand/test_batteries.h"
#include "PractRand/tests/gap16.h"
#include "PractRand/tests/DistC6.h"
#include "PractRand/tests/BCFN.h"
#include "PractRand/tests/FPF.h"
#include "PractRand/tests/CoupGap.h"
#include "PractRand/tests/transforms.h"


namespace PractRand {
	namespace Tests {
		namespace Batteries {
			void destruct_tests(ListOfTests &tests) {
				for (unsigned i = 0; i < tests.tests.size(); i++) {
					delete tests.tests[i];
				}
			}
			ListOfTests get_core_tests() {
				return Tests::ListOfTests(
					new Tests::BCFN(2, 13),
					new Tests::DistC6(9,0, 1,0,0),
					new Tests::Gap16()
				);
			}
			static Tests::ListOfTests standard_foldings_generic(ListOfTests (*base_tests)()) {
				Tests::ListOfTests l = base_tests();
				l.tests.push_back(new Tests::Transforms::lowbits(NULL, base_tests(), 0, 0));
				//l.tests.push_back(new Tests::Transforms::lowbits(NULL, base_tests(), 1, 1));
				l.tests.push_back(new Tests::Transforms::lowbits(NULL, base_tests(), 2, 2));
				return l;
			}
			static Tests::ListOfTests standard_foldings8(ListOfTests (*base_tests)()) {
				Tests::ListOfTests l = base_tests();
				l.tests.push_back(new Tests::Transforms::lowbits(NULL, base_tests(), 0, 0));
				return l;
			}
			static Tests::ListOfTests standard_foldings16(ListOfTests (*base_tests)()) {
				Tests::ListOfTests l = base_tests();
				Tests::ListOfTests sub4 = base_tests();
				sub4.tests.push_back(new Tests::Transforms::lowbits(NULL, base_tests(), 0, -1));
				//sub4.tests.push_back(new Tests::Transforms::lowbits(NULL, base_tests(), 1, -1));
				l.tests.push_back(new Tests::Transforms::lowbits(NULL, sub4, 2, 1));
				return l;
			}
			static Tests::ListOfTests standard_foldings32(ListOfTests (*base_tests)()) {
				Tests::ListOfTests l = base_tests();
				l.tests.push_back(new Tests::Transforms::lowbits(NULL, standard_foldings8(base_tests), 3, 2));
				return l;
			}
			static Tests::ListOfTests standard_foldings64(ListOfTests (*base_tests)()) {
				Tests::ListOfTests l = base_tests();
				Tests::ListOfTests sub16 = base_tests();
				Tests::ListOfTests sub4 = base_tests();
				sub4.tests.push_back(new Tests::Transforms::lowbits(NULL, base_tests(), 0, -1));
				//sub4.tests.push_back(new Tests::Transforms::lowbits(NULL, base_tests(), 1, -1));
				sub16.tests.push_back(new Tests::Transforms::lowbits(NULL, sub4, 2, 1));
				l.tests.push_back(new Tests::Transforms::lowbits(NULL, sub16, 4, 3));
				return l;
			}
			static Tests::ListOfTests apply_standard_foldings( const RNGs::vRNG *rng, ListOfTests (*base_tests)() ) {
				switch (rng ? rng->get_native_output_size() : 0) {
					case 16: return standard_foldings16(base_tests);
					case 32: return standard_foldings32(base_tests);
					case 64: return standard_foldings64(base_tests);
					case  8: return standard_foldings8(base_tests);
					default: return standard_foldings_generic(base_tests);
				}
			}
			Tests::ListOfTests get_standard_tests( const RNGs::vRNG *rng ) {
				return apply_standard_foldings(rng, get_core_tests);
			}
			static ListOfTests apply_extended_foldings(ListOfTests (*base_tests)()) {
				ListOfTests rv = base_tests();
				rv.tests.push_back(new Tests::Transforms::lowbits(NULL, base_tests(), 0, 0));
				rv.tests.push_back(new Tests::Transforms::lowbits(NULL, base_tests(), 0, 1));
				rv.tests.push_back(new Tests::Transforms::lowbits(NULL, base_tests(), 0, 2));
				rv.tests.push_back(new Tests::Transforms::lowbits(NULL, base_tests(), 0, 3));
				rv.tests.push_back(new Tests::Transforms::lowbits(NULL, base_tests(), 2, 1));
				rv.tests.push_back(new Tests::Transforms::lowbits(NULL, base_tests(), 2, 2));
				rv.tests.push_back(new Tests::Transforms::lowbits(NULL, base_tests(), 2, 3));
				rv.tests.push_back(new Tests::Transforms::lowbits(NULL, base_tests(), 3, 2));
				rv.tests.push_back(new Tests::Transforms::lowbits(NULL, base_tests(), 3, 3));
				return rv;
			}
			ListOfTests get_folded_tests() {
				return apply_extended_foldings(get_core_tests);
			}
			ListOfTests get_expanded_core_tests() {
				return Tests::ListOfTests(
					new Tests::BCFN(0, 14),
					new Tests::DistC6(9,0, 1,0,0),
					new Tests::DistC6(6,1, 1,0,0),
					new Tests::DistC6(5,2, 1,0,0),
					new Tests::DistC6(5,3, 1,0,1),
					new Tests::DistC6(4,3, 0,0,1),
					new Tests::Gap16(),
					new Tests::FPF(5, 14, 6),
					new Tests::FPF(2, 14, 6),
					new Tests::CoupGap(),
					NULL
				);
			}
			ListOfTests get_expanded_standard_tests(const RNGs::vRNG *rng) {
				return apply_standard_foldings(rng, get_expanded_core_tests);
			}
			ListOfTests get_expanded_folded_tests() {
				return apply_extended_foldings(get_expanded_core_tests);
			}
		}//Batteries
	}//Tests
}//PractRand
