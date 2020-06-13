#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <string>
#include <map>
#include <vector>
#include <list>
//#include <map>

//master header, includes everything in PractRand for both 
//  practical usage and research... 
//  EXCEPT it does not include specific algorithms
#include "PractRand_full.h"


//specific testing algorithms, to detect bias in supposedly random numbers
#include "PractRand/Tests/BCFN.h"
#include "PractRand/Tests/Gap16.h"
#include "PractRand/Tests/DistC6.h"
#include "PractRand/Tests/transforms.h"
#include "PractRand/Tests/FPF.h"
#include "PractRand/Tests/CoupGap.h"

//specific RNG algorithms, to produce (pseudo-)random numbers
#include "PractRand/RNGs/lcg64_32.h"
#include "PractRand/RNGs/clcg96_32.h"
#include "PractRand/RNGs/jsf16.h"
#include "PractRand/RNGs/jsf32.h"
#include "PractRand/RNGs/jsf64.h"
#include "PractRand/RNGs/mwlac32.h"
#include "PractRand/RNGs/mwlac64.h"
#include "PractRand/RNGs/efiix8x256.h"
#include "PractRand/RNGs/efiix16x256.h"
#include "PractRand/RNGs/efiix32x256.h"
#include "PractRand/RNGs/efiix64x256.h"
#include "PractRand/RNGs/isaac32x256.h"
#include "PractRand/RNGs/isaac64x256.h"
#include "PractRand/RNGs/hc256.h"
#include "PractRand/RNGs/entropy_pools/arbee.h"
#include "PractRand/RNGs/entropy_pools/sha2_based_pool.h"
#include "PractRand/RNGs/entropy_pools/isaac_with_sha2_pool.h"

using namespace PractRand;

/*
A minimal RNG implementation, just enough to make is usable.  
Deliberately flawed, though still better than most platforms default RNGs
*/
class Raw_DummyRNG {
public:
	enum {
		OUTPUT_TYPE = PractRand::RNGs::OUTPUT_TYPES::NORMAL_1,
		OUTPUT_BITS = 32,
		FLAGS = PractRand::RNGs::FLAG::NEEDS_GENERIC_SEEDING
	};
	Uint32 a, b, c;
	Uint32 raw32() {
		Uint32 tmp = b ^ ((a << 19) | (a >> 13));
		a = ~b + c;
		b = c;
		c += tmp;
		return b;
	}
	void walk_state(StateWalkingObject *walker) {
		walker->handle(a);
		walker->handle(b);
		walker->handle(c);
	}
};
class Polymorphic_DummyRNG : public PractRand::RNGs::vRNG32 {
public:
	Raw_DummyRNG implementation;
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

void print_result(const char *tname, double result, double pvalue) {
	int len = std::strlen(tname);
	std::printf("  %s", tname);
	for (int i = len; i < 30; i++) std::printf(" ");
	std::printf("  %+9.1f   ", result);
	if (pvalue != -1 && pvalue >= 0 && pvalue <= 1) {
		if (std::fabs(pvalue-0.5) > 0.4999) std::printf("p=%7.5f  ", pvalue);
		else if (std::fabs(pvalue-0.5) > 0.499) std::printf("p=%6.4f   ", pvalue);
		else if (std::fabs(pvalue-0.5) > 0.49) std::printf("p=%5.3f    ", pvalue);
		else if (std::fabs(pvalue-0.5) > 0.4) std::printf("p=%4.2f     ", pvalue);
		else std::printf("p=%3.1f      ", pvalue);
		if (std::fabs(pvalue-0.5) > 0.49999999) printf("FAIL");
		else if (std::fabs(pvalue-0.5) > 0.4999) printf(" very suspicious");
		else if (std::fabs(pvalue-0.5) > 0.49) printf(" suspicious");
		else std::printf(" pass");
	}
	else {
		std::printf("           ");
		if (std::fabs(result) > 20) std::printf("FAIL?");
		else if (std::fabs(result) > 8) std::printf(" suspicious?");
		else std::printf(" pass?");
	}
	printf("\n");
}

int main(int argc, char **argv) {
	PractRand::initialize_PractRand();
	std::time_t start_time = std::time(NULL);

	RNGs::vRNG *known_good = new PractRand::RNGs::Polymorphic::isaac32x256(PractRand::SEED_AUTO);
	Uint32 rng_seed = known_good->raw32();

	//RNGs::vRNG *rng = new PractRand::RNGs::Polymorphic::lcg64_32(rng_seed);
	RNGs::vRNG *rng = new Polymorphic_DummyRNG();
	rng->seed(rng_seed);

	std::printf("RNG = %s, seed = 0x%x\n\n", rng->get_name().c_str(), rng_seed);
	std::printf("a random floating point number in [0,1) using that RNG: %f\n", rng->randf());
	std::printf("a random integer in [0,13) using RNG->randi: %d\n", rng->randi(0, 13));
	std::printf("a fast random integer in [0,13) using RNG->randi_fast: %d\n", rng->randi_fast(0, 13));
	std::printf("a random integer in [0,13) using RNG->randli: %d\n", rng->randli(0, 13));
	std::printf("\n");

	TestManager tman(rng);
	Tests::ListOfTests tests = Tests::Batteries::get_standard_tests(rng);
	//Tests::ListOfTests tests = Tests::Batteries::get_expanded_standard_tests(rng);
	for (unsigned int i = 0; i < tests.tests.size(); i++) tests.tests[i]->init(known_good);

	//Start by testing 32 kilobytes, then double the amount to test repeatedly.  
	//Stop after testing 32 terabytes.  
	//After each round of testing finishes print interium results.  
	//Skip the printing if it's been less than 2 seconds since this program started.  
	Uint64 test_size = 1024 * 32;
	while (test_size < 1ull<<45) {
		Uint64 blocks_left = test_size/Tests::TestBlock::SIZE - tman.blocks_so_far;
		test_size <<= 1;
		while (blocks_left) {
			tman.prep_blocks(blocks_left);
			for (unsigned int i = 0; i < tests.tests.size(); i++) tman.use_blocks(tests.tests[i]);
		}
		int time_passed = std::time(NULL) - start_time;
		if (time_passed < 2) continue;
		printf("rng=%s, seed=0x%x, length=2^%.0f bytes, time=%d seconds\n", 
			rng->get_name().c_str(), 
			rng_seed, 
			std::log(double(Tests::TestBlock::SIZE) * tman.blocks_so_far) / std::log(2.0),
			time_passed
		);
		for (unsigned int i = 0; i < tests.tests.size(); i++) {
			Tests::TestBaseclass *test = tests.tests[i];
			Tests::Transforms::multiplex *multi = dynamic_cast<Tests::Transforms::multiplex*>(test);
			if (!multi) {
				double r = test->get_result();
				double p = test->result_to_pvalue(tman.blocks_so_far, r);
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
	for (unsigned int i = 0; i < tests.tests.size(); i++) tests.tests[i]->deinit();
	for (unsigned int i = 0; i < tests.tests.size(); i++) delete tests.tests[i];
	return 0;
}




