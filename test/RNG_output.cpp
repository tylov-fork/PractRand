#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <cstring>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <iostream>
//#include <map>

//master header, includes everything in PractRand for both 
//  practical usage and research... 
//  EXCEPT it does not include specific algorithms
//  also it does not include PractRand/RNG_adaptors.h as that includes lots of templated stuff
#include "PractRand_Full.h"
//the full version is needed because non-recommended RNGs are supported

//specific RNG algorithms, to produce (pseudo-)random numbers
#include "PractRand/RNGs/mt19937.h"
#include "PractRand/RNGs/jsf16.h"
#include "PractRand/RNGs/jsf32.h"
#include "PractRand/RNGs/jsf64.h"
#include "PractRand/RNGs/xsm32.h"
#include "PractRand/RNGs/xsm64.h"
#include "PractRand/RNGs/sfc16.h"
#include "PractRand/RNGs/sfc32.h"
#include "PractRand/RNGs/sfc64.h"
#include "PractRand/RNGs/efiix8x384.h"
#include "PractRand/RNGs/efiix16x384.h"
#include "PractRand/RNGs/efiix32x384.h"
#include "PractRand/RNGs/efiix64x384.h"
#include "PractRand/RNGs/isaac32x256.h"
#include "PractRand/RNGs/isaac64x256.h"
#include "PractRand/RNGs/hc256.h"
#include "PractRand/RNGs/arbee.h"
#include "PractRand/RNGs/sha2_based_pool.h"

#include "PractRand/RNGs/other/transform.h"
#include "PractRand/RNGs/other/mult.h"
#include "PractRand/RNGs/other/simple.h"
#include "PractRand/RNGs/other/fibonacci.h"
#include "PractRand/RNGs/other/indirection.h"
#include "PractRand/RNGs/other/special.h"

using namespace PractRand;

RNGs::vRNG *get_recommended_rng(const char *name) {
#define RECOMMENDED_RNG(a) if (!std::strcmp(name, #a)) return new RNGs::Polymorphic:: a (SEED_AUTO);
	RECOMMENDED_RNG( xsm32 )
	RECOMMENDED_RNG( xsm64 )
	RECOMMENDED_RNG( hc256 )
	RECOMMENDED_RNG( isaac32x256 )
	RECOMMENDED_RNG( isaac64x256 )
	RECOMMENDED_RNG( efiix8x384 )
	RECOMMENDED_RNG( efiix16x384 )
	RECOMMENDED_RNG( efiix32x384 )
	RECOMMENDED_RNG( efiix64x384 )
	RECOMMENDED_RNG( mt19937 )
	RECOMMENDED_RNG( jsf16 )
	RECOMMENDED_RNG( jsf32 )
	RECOMMENDED_RNG( jsf64 )
	RECOMMENDED_RNG( sfc16 )
	RECOMMENDED_RNG( sfc32 )
	RECOMMENDED_RNG( sfc64 )
	return NULL;
}
RNGs::vRNG *get_nonrecommended_rng(const char *name) {
	RNGs::vRNG *rng = NULL;
#define NONRECOMMENDED_RNG(a) if (!std::strcmp(name, #a)) rng = new RNGs::Polymorphic::NotRecommended:: a;
	// include/PractRand/RNGs/other/simple.h
	NONRECOMMENDED_RNG(xorshift32)
	NONRECOMMENDED_RNG(xorshift64)
	NONRECOMMENDED_RNG(xorshift128_32)
	NONRECOMMENDED_RNG(xorshift128_64)
	NONRECOMMENDED_RNG(xorshift32_16)
	NONRECOMMENDED_RNG(xorshift64_32)
	NONRECOMMENDED_RNG(xorshift32x4)
	NONRECOMMENDED_RNG(sapparot)
	NONRECOMMENDED_RNG(sap48_16)
	NONRECOMMENDED_RNG(sap96_32)
	NONRECOMMENDED_RNG(flea32x1)
	NONRECOMMENDED_RNG(sfc_v1_16)
	NONRECOMMENDED_RNG(sfc_v1_32)
	NONRECOMMENDED_RNG(sfc_v2_16)
	NONRECOMMENDED_RNG(sfc_v2_32)
	NONRECOMMENDED_RNG(simpleA)
	NONRECOMMENDED_RNG(simpleB)
	NONRECOMMENDED_RNG(simpleC)
	NONRECOMMENDED_RNG(simpleD)
	NONRECOMMENDED_RNG(simpleE)
	// include/PractRand/RNGs/other/mult.h
	NONRECOMMENDED_RNG(lcg32_16)
	NONRECOMMENDED_RNG(lcg32)
	NONRECOMMENDED_RNG(lcg32_16_extended)
	NONRECOMMENDED_RNG(lcg32_extended)
	NONRECOMMENDED_RNG(garthy16)
	NONRECOMMENDED_RNG(garthy32)
	NONRECOMMENDED_RNG(binarymult16)
	NONRECOMMENDED_RNG(binarymult32)
	NONRECOMMENDED_RNG(rxmult16)
	NONRECOMMENDED_RNG(multish2x64)
	NONRECOMMENDED_RNG(multish3x32)
	NONRECOMMENDED_RNG(multish4x16)
	NONRECOMMENDED_RNG(old_mwlac16)
	// include/PractRand/RNGs/other/fibonacci.h
	NONRECOMMENDED_RNG(mm32)
	NONRECOMMENDED_RNG(mm32_16)
	NONRECOMMENDED_RNG(mm32_awc)
	NONRECOMMENDED_RNG(mwc4691)
	NONRECOMMENDED_RNG(cbuf_accum)
	NONRECOMMENDED_RNG(dual_cbuf)
	NONRECOMMENDED_RNG(dual_cbuf_accum)
	NONRECOMMENDED_RNG(ranrot32)
	NONRECOMMENDED_RNG(fibmul32_16)
	NONRECOMMENDED_RNG(ranrot32)
	NONRECOMMENDED_RNG(ranrot3tap32)
	// include/PractRand/RNGs/other/indirection.h
	NONRECOMMENDED_RNG(rc4)
	NONRECOMMENDED_RNG(rc4_weakened)

	int parameter;
	const char *ptr = strchr(name, '(');
	if (ptr && strchr(ptr, ')')) {
		parameter = atoi(ptr+1);
#define PARAMETIZED_RNG(a) if (!std::strncmp(name, #a, ptr-name)) rng = new RNGs::Polymorphic::NotRecommended:: a (parameter);
		PARAMETIZED_RNG(clcg96_8_varqual)
		PARAMETIZED_RNG(clcg96_16_varqual)
		PARAMETIZED_RNG(clcg96_32_varqual)
		PARAMETIZED_RNG(cxlcg96_8_varqual)
		PARAMETIZED_RNG(cxlcg96_16_varqual)
		PARAMETIZED_RNG(cxlcg96_32_varqual)
		PARAMETIZED_RNG(lcg64_8_varqual)
		PARAMETIZED_RNG(lcg64_16_varqual)
		PARAMETIZED_RNG(xlcg64_8_varqual)
		PARAMETIZED_RNG(xlcg64_16_varqual)
		PARAMETIZED_RNG(ibaa8)
		PARAMETIZED_RNG(ibaa16)
		PARAMETIZED_RNG(ibaa32)
	}
	if (rng) rng->autoseed();
	return rng;
}
RNGs::vRNG *get_rng(const char *name);
RNGs::vRNG *get_special_rng(const char *name) {
	RNGs::vRNG *rng = NULL;
	if (!strncmp(name, "[BDS", 4)) {//bays-durham shuffle
		name = name + 4;
		const char *ptr = strchr(name, ']');
		if (!ptr) return NULL;
		int n = atoi(name);
		if (n <= 0) return NULL;
		int ln = 0; while (ln < 8 && n > (1<<ln)) ln++;
		rng = get_rng(ptr+1);
		if (!rng) return NULL;
		rng = RNGs::Polymorphic::NotRecommended::apply_BaysDurhamShuffle(rng, ln);
		return rng;
	}
	return NULL;
}
RNGs::vRNG *get_rng(const char *name) {
	RNGs::vRNG *rng = get_recommended_rng(name);
	if (!rng) rng = get_nonrecommended_rng(name);
	if (!rng) rng = get_special_rng(name);
	return rng;
}

void print_usage(const char *program_name) {
	std::cout << "usage:\n\t" << program_name << " RNG_name bytes_to_output\n";
	std::cout << "example:\n\t" << program_name << " jsf32 16\n";
	std::cout << "prints 16 bytes using the jsf32 RNG\n";
	exit(0);
}
int main(int argc, char **argv) {
	if (argc != 3) print_usage(argv[0]);
	PractRand::initialize_PractRand();
	RNGs::vRNG *rng = get_rng(argv[1]);
	if (!rng) {std::printf("unrecognized RNG name\n"); print_usage(argv[0]);}
	double _n = atof(argv[2]);//should be atol, but on 32 bit systems that's too limited
	if (_n <= 0) {
		if (!strcmp(argv[2], "name")) {
			std::printf("%s\n", rng->get_name().c_str());
			exit(0);
		}
		std::printf("invalid number of output bytes\n"); print_usage(argv[0]);
	}
	Uint64 n = Uint64(_n);
	Uint64 buffer[64];
	while (n) {
		for (int i = 0; i < 64; i++) buffer[i] = rng->raw64();
		size_t b = 64 * 8;
		b = (b > n) ? size_t(n) : b;
		n -= b;
		//std::fwrite(&buffer[0], b, 1, std::stdout);
		std::cout.write((char*)&buffer[0], b);
		if (std::cout.bad()) {std::cerr << "output error occured" << std::endl; exit(1);}
	}
	return 0;
}
