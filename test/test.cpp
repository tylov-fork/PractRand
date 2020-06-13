#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <ctime>
#include <string>
#include <map>
#include <vector>
#include <list>
//#include <map>

#include "PractRand/config.h"


//using namespace PractRand;
//#include "struct_text.h"

#include "PractRand_full.h"
#include "PractRand/Tests/BCFN.h"
#include "PractRand/Tests/gap16.h"
#include "PractRand/Tests/DistC6.h"
#include "PractRand/Tests/transforms.h"

#include "PractRand/RNGs/lcg64_32.h"
#include "PractRand/RNGs/clcg96_32.h"
#include "PractRand/RNGs/jsf16.h"
#include "PractRand/RNGs/jsf32.h"
#include "PractRand/RNGs/jsf64.h"
#include "PractRand/RNGs/efiix8x256.h"
#include "PractRand/RNGs/efiix16x256.h"
#include "PractRand/RNGs/efiix32x256.h"
#include "PractRand/RNGs/efiix64x256.h"
#include "PractRand/RNGs/isaac32x256.h"
#include "PractRand/RNGs/isaac64x256.h"
#include "PractRand/RNGs/hc256.h"
#include "PractRand/RNGs/mt19937.h"

using namespace PractRand;

/*
A bastardized minimal RNG implementation to test

I just made this algorithm up off the top of my head.  
I tested it with TestU01: it passes SmallCrush and failed only a 
single test in Crush when I tried it, though it got 
suspicious results (p=10**-4) on 3 other tests in Crush as well.  
It takes 13 seconds for it to fail the testing in this program 
on my computer.  
*/
class DummyRNG {
public:
	enum {
		OUTPUT_TYPE = PractRand::RNGs::OUTPUT_TYPES::NORMAL_1,
		OUTPUT_BITS = 32,
		FLAGS = 0
	};
	Uint32 a, b, c;
	Uint32 raw32() {
		Uint32 tmp = b ^ ((a << 19) | (a >> 13));
		Uint32 tmp2 = a;
		a = b + c;
		b = c + tmp2;
		c = tmp + 0xa7f91431;
		return tmp;
	}
	void walk_state(StateWalkingObject *walker) {
		walker->handle(a);
		walker->handle(b);
		walker->handle(c);
	}
};
class Polymorphic_DummyRNG : public PractRand::RNGs::vRNG32 {
public:
	DummyRNG implementation;
	Uint32 raw32() {return implementation.raw32();}
	void walk_state(StateWalkingObject *walker) {implementation.walk_state(walker);}
	std::string get_name() const {return std::string("DummyRNG");}
};

class TestManager {
public:
	std::vector<Tests::TestBlock> buffer;
	RNGs::vRNG *rng;
	int max_buffer_amount;
	int prefix_blocks;
	int main_blocks;
	Uint64 blocks_so_far;
	TestManager(RNGs::vRNG *rng_, int max_buffer_amount_ = 1 << (25-10)) {
		rng = rng_;
		blocks_so_far = 0;
		max_buffer_amount = max_buffer_amount_;
		prefix_blocks = 0;
		main_blocks = 0;
		buffer.resize(max_buffer_amount + Tests::TestBaseclass::REPEATED_BLOCKS);
	}
	void use_blocks(Tests::TestBaseclass *test) {test->test_blocks(&buffer[prefix_blocks], main_blocks);}
	int prep_blocks(Uint64 &blocks) {
		Uint64 _delta_blocks = blocks;
		if (_delta_blocks > max_buffer_amount) _delta_blocks = max_buffer_amount;
		int delta_blocks = int(_delta_blocks);
		blocks -= delta_blocks;
		size_t repeat_region_start, repeat_region_size;
		if (prefix_blocks + main_blocks >= Tests::TestBaseclass::REPEATED_BLOCKS) {
			repeat_region_start = prefix_blocks + main_blocks - Tests::TestBaseclass::REPEATED_BLOCKS;
			repeat_region_size = Tests::TestBaseclass::REPEATED_BLOCKS;
		}
		else {
			repeat_region_start = 0;
			repeat_region_size = prefix_blocks + main_blocks;
		}
		if (repeat_region_start != 0)
			memmove(&buffer[0], &buffer[repeat_region_start], repeat_region_size * Tests::TestBlock::SIZE);
		prefix_blocks = repeat_region_size;
		main_blocks = delta_blocks;
		buffer[prefix_blocks].fill(rng, main_blocks);
		blocks_so_far += delta_blocks;
		return delta_blocks;
	}
	int prep_more_blocks(Uint64 &blocks) {
		Uint64 _delta_blocks = blocks;
		if (_delta_blocks + main_blocks > max_buffer_amount) _delta_blocks = max_buffer_amount - main_blocks;
		int delta_blocks = int(_delta_blocks);
		blocks -= delta_blocks;
		if (delta_blocks)
			buffer[prefix_blocks + main_blocks].fill(rng, int(delta_blocks));

		main_blocks += delta_blocks;
		blocks_so_far += delta_blocks;
		return delta_blocks;
	}
};

Tests::ListOfTests get_core_tests() {
	return Tests::ListOfTests(
		new Tests::BCFN(2, 13),
		new Tests::DistC6(9,0, 1,0,0),
		new Tests::Gap16()
	);
}
Tests::ListOfTests get_standard_tests8() {
	Tests::ListOfTests l = get_core_tests();
	l.tests.push_back(
		new Tests::Transforms::lowbits("core", get_core_tests(), 0, 0)
	);
	return l;
}
Tests::ListOfTests get_standard_tests16() {
	Tests::ListOfTests l = get_core_tests();
	l.tests.push_back(new Tests::Transforms::lowbits(NULL, get_core_tests(), 2, 1));
	l.tests.push_back(new Tests::Transforms::lowbits(NULL, get_core_tests(), 0, 1));
	return l;
}
Tests::ListOfTests get_standard_tests32() {
	Tests::ListOfTests l = get_core_tests();
	l.tests.push_back(new Tests::Transforms::lowbits(NULL, get_standard_tests8(), 3, 2));
	return l;
}
Tests::ListOfTests get_standard_tests64() {
	Tests::ListOfTests l = get_core_tests();
	l.tests.push_back(new Tests::Transforms::lowbits(NULL, get_standard_tests8(), 3, 3));
	return l;
}
Tests::ListOfTests get_standard_tests( int bits ) {
	switch (bits) {
		case 16: return get_standard_tests16();
		case 32: return get_standard_tests32();
		case 64: return get_standard_tests64();
		default:
		case 8:
			return get_standard_tests8();
	}
}

void print_result(const char *tname, double result, double pvalue) {
	int len = std::strlen(tname);
	printf("  %s", tname);
	for (int i = len; i < 30; i++) printf(" ");
	printf("  %+9.1f   ", result);
	if (pvalue != -1) {
		if (fabs(pvalue-0.5) > 0.4999) printf("p=%7.5f  ", pvalue);
		else if (fabs(pvalue-0.5) > 0.499) printf("p=%6.4f   ", pvalue);
		else if (fabs(pvalue-0.5) > 0.49) printf("p=%5.3f    ", pvalue);
		else if (fabs(pvalue-0.5) > 0.4) printf("p=%4.2f     ", pvalue);
		else printf("p=%3.1f      ", pvalue);
	}
	else printf("p=?        ");
	if (pvalue == -1) {
		if (fabs(result) > 20) printf("FAIL?");
		else if (fabs(result) > 8) printf(" suspicious?");
		else printf(" pass?");
	}
	else if (pvalue >= 0 && pvalue <= 1) {
		if (fabs(pvalue-0.5) > 0.49999999) printf("FAIL");
		else if (fabs(pvalue-0.5) > 0.4999) printf(" very suspicious");
		else if (fabs(pvalue-0.5) > 0.49) printf(" suspicious");
		else printf(" pass");
	}
	else {
		issue_error();
	}

	printf("\n");
}
void main(int argc, char **argv) {
	PractRand::initialize_PractRand();
	std::time_t start_time = std::time(NULL);

	RNGs::vRNG *known_good = new PractRand::RNGs::Polymorphic::hc256(PractRand::SEED_AUTO);
	Uint64 rng_seed = known_good->raw32();//known_good->raw64();
	//RNGs::vRNG *rng = new PractRand::RNGs::Polymorphic::lcg64_32(rng_seed);
	RNGs::vRNG *rng = new Polymorphic_DummyRNG();
	rng->seed(rng_seed);
	printf("rng = %s, seed = 0x%llx\n\n", rng->get_name().c_str(), rng_seed);
	TestManager targ(rng);
	Tests::ListOfTests tests = get_standard_tests(rng->get_native_output_size());
	for (unsigned int i = 0; i < tests.tests.size(); i++) tests.tests[i]->init(known_good);
	Uint64 test_size = 1<<10;
	while (test_size < 1ull<<35) {
		Uint64 blocks_left = test_size - targ.blocks_so_far;
		test_size <<= 1;
		while (blocks_left) {
			targ.prep_blocks(blocks_left);
			for (unsigned int i = 0; i < tests.tests.size(); i++) targ.use_blocks(tests.tests[i]);
		}
		printf("rng=%s, seed=0x%llx, length=2^%.0f bytes, time=%d seconds\n", 
			rng->get_name().c_str(), 
			rng_seed, 
			std::log(double(Tests::TestBlock::SIZE) * targ.blocks_so_far) / std::log(2.0),
			std::time(NULL) - start_time
		);
		for (unsigned int i = 0; i < tests.tests.size(); i++) {
			Tests::TestBaseclass *test = tests.tests[i];
			Tests::Transforms::multiplex *multi = dynamic_cast<Tests::Transforms::multiplex*>(test);
			if (!multi) {
				double r = test->get_result();
				double p = test->result_to_pvalue(targ.blocks_so_far, r);
				print_result(test->get_name().c_str(), r, p);
			}
			else {
				int n = multi->get_num_children();
				if (multi->get_blocks_passed_through() >= 1<<10) {
					for (int i = 0; i < n; i++) {
						test = multi->get_child(i);
						double r = test->get_result();
						double p = test->result_to_pvalue(
							multi->get_blocks_passed_through(), r);
						print_result(
							(multi->get_name() + test->get_name().c_str()).c_str(), 
							r, p);
					}
				}
			}
		}
	}
	for (int i = 0; i < 3; i++) tests.tests[i]->deinit();
	for (int i = 0; i < 3; i++) delete tests.tests[i];
}





