
#include <string>
#include <sstream>
#include "PractRand/config.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/rng_internals.h"

#include "PractRand/RNGs/mt19937.h"
#include "PractRand/RNGs/other/fibonacci.h"

using namespace PractRand;
using namespace PractRand::Internals;

namespace PractRand {
	namespace Testing {
		namespace RNG_Sets{
			const char *recommended_rngs[] = {
				"hc256", "trivium",
				"efiix64x384", "efiix32x384", "efiix16x384", "efiix8x384",
				"isaac64x256", "isaac32x256",
				"chacha(8)", "salsa(8)",
				"arbee",
				"xsm64", "xsm32",
				"jsf64", "jsf32", "sfc64", "sfc32", "sfc16",
				"mt19937",
				NULL
			};
			const char *nonrecommended_simple[] = {
				"xorshift32", "xorshift64", "xorshift32of128", "xoroshiro128plus",
				"xorwow32of96", "xorwow32x6", "xsalta16", "xsaltb16", "xsaltc16",
				"sapparot", "sap16of48", "sap32of96",
				"flea32x1", "jsf16",
				"sfc_v1_16", "sfc_v1_32", "sfc_v2_16", "sfc_v2_32", "sfc_v3_16", "sfc_v3_32",
				"simpleA", "simpleB", "simpleC", "simpleD", "simpleE", "simpleF", "simpleG",
				"trivium_weakenedA", "trivium_weakenedB",
				"mo_Lesr32", "mo_ResrRers32", "mo_Rers32of64", "mo_Resdra32of64",
				"murmlacish",
				"gjishA", "gjishB", "gjishC", "gjishD",
				"ara16", "ara32", "arx16", "arx32", "hara16", "harx16", "learx16", "hlearx16", "alearx16", "arac16", "arxc16",
				"rarns16",
				NULL
			};
			const char *nonrecommended_nonLCG[] = {
				//"garthy16", "garthy32", "binarymult16", "binarymult32", "rxmult16", 
				NULL
			};
		}
	}
}




