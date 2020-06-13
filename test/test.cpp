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
#include "PractRand/RNGs/mt19937.h"
#include "PractRand/RNGs/jsf16.h"
#include "PractRand/RNGs/jsf32.h"
#include "PractRand/RNGs/jsf64.h"
#include "PractRand/RNGs/mwlac16.h"
#include "PractRand/RNGs/mwlac32.h"
#include "PractRand/RNGs/mwlac64.h"
#include "PractRand/RNGs/sfc16.h"
#include "PractRand/RNGs/sfc32.h"
#include "PractRand/RNGs/sfc64.h"
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

#include "PractRand/RNGs/other/transform.h"
#include "PractRand/RNGs/other/mult.h"
#include "PractRand/RNGs/other/simple.h"
#include "PractRand/RNGs/other/fibonacci.h"
#include "PractRand/RNGs/other/indirection.h"
#include "PractRand/RNGs/other/special.h"


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

	//RNG state goes here:
	Uint32 a, b, c;

	Uint32 raw32() {
		//RNG algorithm goes here:
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


void print_result(const std::string &tname, double result, double pvalue) {
	std::printf("  %s", tname.c_str());
	int len = tname.length();
	if (len <= 30) {
		for (int i = len; i < 30; i++) std::printf(" ");
		std::printf("%+12.1f   ", result);
	}
	else {
		for (int i = len; i < 32; i++) std::printf(" ");
		std::printf("%+10.1f   ", result);
	}
	if (pvalue != -1 && pvalue >= 0 && pvalue <= 1) {
		if (std::fabs(pvalue-0.5) > 0.4999) std::printf("p=%7.5f  ", pvalue);
		else if (std::fabs(pvalue-0.5) > 0.499) std::printf("p=%6.4f   ", pvalue);
		else if (std::fabs(pvalue-0.5) > 0.49) std::printf("p=%5.3f    ", pvalue);
		else if (std::fabs(pvalue-0.5) > 0.4) std::printf("p=%4.2f     ", pvalue);
		else std::printf("p=%3.1f      ", pvalue);
		if (std::fabs(pvalue-0.5) > 0.49999999) std::printf("FAIL");
		else if (std::fabs(pvalue-0.5) > 0.4999) std::printf(" very suspicious");
		else if (std::fabs(pvalue-0.5) > 0.49) std::printf(" suspicious");
		else std::printf(" pass");
	}
	else {
		std::printf("           ");
		if (std::fabs(result) >= 20) std::printf("FAIL?");
		else if (std::fabs(result) >= 12.5) std::printf(" very suspicious?");
		else if (std::fabs(result) >= 8) std::printf(" suspicious?");
		else std::printf(" pass?");
	}
	std::printf("\n");
}
void handle_results(Tests::TestBaseclass *test, Uint64 blocks, std::string prefix, int *total_tests, int *suspicious_tests) {
	Tests::Transforms::multiplex *multi = dynamic_cast<Tests::Transforms::multiplex*>(test);
	if (!multi) {
		if (total_tests) total_tests[0]++;
		double r = test->get_result();
		double p = test->result_to_pvalue(blocks, r);
		if ((r >= 8 && blocks >= 512) || (p >= 0 && fabs(p-.5) > 0.49)) {
			if (suspicious_tests) suspicious_tests[0]++;
			print_result(prefix + test->get_name(), r, p);
		}
	}
	else {
		int n = multi->get_num_children();
		for (int i = 0; i < n; i++) {
			handle_results(
				multi->get_child(i), 
				multi->get_blocks_passed_through(i), 
				prefix + multi->get_name(),
				total_tests, suspicious_tests);
		}
	}
}
template<typename RNG> double get_RNG_performance(RNG *rng) {
	enum {NUM_CLOCKS_TO_TEST = int(CLOCKS_PER_SEC * .5) + 1};
	//RAW_RNG rng(PractRand::SEED_AUTO);
	Uint32 buffy[1024] = {0};
	int clock0 = clock();
	int clock1, clock2;
	while ((clock1 = clock()) == clock0) ;
	int j = 0;
	do {
		for (int i = 0; i < 1024; i++) buffy[i] ^= rng->raw32();
		j++;
	} while ((clock2=clock())-clock1 < NUM_CLOCKS_TO_TEST);

	double delta = (clock2 - clock1) / double(CLOCKS_PER_SEC);//seconds
	double amount = j / 256.0;//megabytes
	double rate = amount / delta;

	//just to make very sure that some smart compiler won't optimize everything away:
	Uint64 sum = 0;
	for (int i = 0; i < 1024; i++) sum += buffy[i];
	if (sum > Uint64(1) << 60) printf("impossible!");

	return rate;
}
template<typename RAW_RNG> double get_raw_RNG_performance() {
	RAW_RNG rng;
	rng.seed(1);
	return get_RNG_performance(&rng);
}
template<typename RNG> double get_polymorphic_RNG_performance() {
	RNG rng(PractRand::SEED_AUTO);
	return get_RNG_performance(&rng);
}
#define PERF(RNG) printf("  %5.3f s/GB  :  %5.3f s/GB  :  %s\n", 1000.0 / get_raw_RNG_performance<PractRand::RNGs::Raw:: RNG >(), 1000.0 / get_polymorphic_RNG_performance<PractRand::RNGs::Polymorphic:: RNG >(), #RNG );

void do_performance_measurement_stuff() {
	printf("  raw            polymorphic    name\n");
	printf("small fast RNGs:\n");
	PERF(jsf32);
	PERF(mwlac32);
	PERF(sfc32);
	printf("seekable RNGs:\n");
	PERF(lcg64_32);
	PERF(clcg96_32);
	printf("popular RNGs:\n");
	PERF(mt19937);
	printf("indirection-based RNGs:\n");
	PERF(isaac32x256);
	PERF(efiix32x256);
	PERF(hc256);
	printf("dummy RNG:\n");
	Raw_DummyRNG raw_dummy_rng;
	StateWalkingObject *walker = get_autoseeder(&raw_dummy_rng);
	raw_dummy_rng.walk_state(walker);
	delete walker;
	Polymorphic_DummyRNG polymorphic_dummy_rng;
	polymorphic_dummy_rng.autoseed();
	printf("  %5.3f s/GB  :  %5.3f s/GB  :  %s\n", 1000.0 / get_RNG_performance(&raw_dummy_rng), 1000.0 / get_RNG_performance(&polymorphic_dummy_rng), "DummyRNG" );
}
void do_statistical_tests() {
	std::time_t start_time = std::time(NULL);

	PractRand::RNGs::Polymorphic::isaac32x256 known_good(PractRand::SEED_AUTO);
	Uint32 rng_seed = known_good.raw32();

//	RNGs::vRNG *rng = new PractRand::RNGs::Polymorphic::NotRecommended::lcg32_16_extended;
//	RNGs::vRNG *rng = new PractRand::RNGs::Polymorphic::NotRecommended::clcg64_16;
	RNGs::vRNG *rng = new PractRand::RNGs::Polymorphic::sfc32(PractRand::SEED_NONE);
//	RNGs::vRNG *rng = new Polymorphic_DummyRNG();
	rng->seed(rng_seed);

	std::printf("RNG = %s, seed = 0x%x\n\n", rng->get_name().c_str(), rng_seed);
//	std::printf("rng->randf():          %8f  -  a random floating point number in [0..1)\n", rng->randf());
//	std::printf("rng->randi(0,13):      %8d  -  a random integer in [0..13)\n", rng->randi(0, 13));
//	std::printf("rng->randi_fast(0, 13):%8d  -  a fast random integer in [0..13)\n", rng->randi_fast(0, 13));
//	std::printf("rng->randli(0, 13):    %8d  -  a random integer in [0..13)\n", int(rng->randli(0, 13)));
//	std::printf("\n");

	TestManager tman(rng);
	Tests::ListOfTests tests = Tests::Batteries::get_standard_tests(rng);
//	Tests::ListOfTests tests = Tests::Batteries::get_expanded_standard_tests(rng);
	for (unsigned int i = 0; i < tests.tests.size(); i++) tests.tests[i]->init(&known_good);

	//Start by testing 32 kilobytes, then double the amount to test repeatedly.  
	//Stop after testing 32 terabytes.  
	//After each round of testing finishes print interium results.  
	//Skip the printing if it's been less than 3 seconds since this started.  
	Uint64 test_size = 1024 * 32;
	while (test_size < 1ull<<45) {
		Uint64 blocks_left = test_size/Tests::TestBlock::SIZE - tman.blocks_so_far;
		test_size <<= 1;
		while (blocks_left) {
			tman.prep_blocks(blocks_left);
			for (unsigned int i = 0; i < tests.tests.size(); i++) tman.use_blocks(tests.tests[i]);
		}
		int time_passed = std::time(NULL) - start_time;
		if (time_passed < 3) continue;
		std::printf("rng=%s, seed=0x%x, length=2^%.0f bytes, time=%d seconds\n", 
			rng->get_name().c_str(), 
			rng_seed, 
			std::log(double(Tests::TestBlock::SIZE) * tman.blocks_so_far) / std::log(2.0),
			time_passed
		);
		int total_tests = 0;
		int suspicious_tests = 0;
		for (unsigned int i = 0; i < tests.tests.size(); i++) {
			Tests::TestBaseclass *test = tests.tests[i];
			handle_results(test, tman.blocks_so_far, "", &total_tests, &suspicious_tests);
		}
		if (suspicious_tests == total_tests)
			;
		else if (suspicious_tests == 0)
			std::printf("  no anomalies in %d subtest(s)\n", total_tests);
		else
			std::printf("  ...and %d subtest(s) without anomalies\n", total_tests - suspicious_tests);
		std::printf("\n");
	}
	for (unsigned int i = 0; i < tests.tests.size(); i++) tests.tests[i]->deinit();
	for (unsigned int i = 0; i < tests.tests.size(); i++) delete tests.tests[i];
}

int main(int argc, char **argv) {
	PractRand::initialize_PractRand();

	printf("*** doing some performance measurements:\n");
	do_performance_measurement_stuff();
	printf("\n");
	printf("*** doing some statistical tests:\n");
	do_statistical_tests();

	return 0;
}




