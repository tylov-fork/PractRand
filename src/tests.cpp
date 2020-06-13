#include <string>
//#include <ostream>
#include <sstream>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <cmath>
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

using namespace PractRand;

static long double gap_probs ( int first, int last, long double baseprob ) {
	return pow(baseprob, (long double) first) - pow(baseprob, (long double) last+1);
}
static const Uint8 distance_table[256] = {
	0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};
int PractRand::Tests::count_bits8 (Uint8 a) {
	return distance_table[a];
}
int PractRand::Tests::count_bits16(Uint16 a) {
	return distance_table[Uint8(a)] + distance_table[Uint8(a>>8)];
}
int PractRand::Tests::count_bits32(Uint32 a) {
	return distance_table[Uint8(a)] + distance_table[Uint8(a>>8)] + distance_table[Uint8(a>>16)] + distance_table[Uint8(a>>24)];
}
int PractRand::Tests::count_bits64(Uint64 a) {
	return distance_table[Uint8(a)] + distance_table[Uint8(a>>8)] + distance_table[Uint8(a>>16)] + distance_table[Uint8(a>>24)] +
		distance_table[Uint8(a>>32)] + distance_table[Uint8(a>>40)] + distance_table[Uint8(a>>48)] + distance_table[Uint8(a>>56)];
}

void Tests::TestBlock::fill(RNGs::vRNG *rng, unsigned long numblocks) {
	int b = rng->get_native_output_size();
	unsigned long n = numblocks * SIZE;
	switch (b) {
		default:
		case 8:
			n >>= 0;
			for (unsigned long i = 0; i < n; i++) as8[i] = rng->raw8();
			break;
		case 16:
			n >>= 1;
			for (unsigned long i = 0; i < n; i++) {
				as16[i] = rng->raw16();
			}
			break;
		case 32:
			n >>= 2;
			for (unsigned long i = 0; i < n; i++) as32[i] = rng->raw32();
			break;
		case 64:
			n >>= 3;
			for (unsigned long i = 0; i < n; i++) as64[i] = rng->raw64();
			break;
	}
}
Tests::ListOfTests::ListOfTests(TestBaseclass **tests_) {
	for (int i = 0; tests_[i]; i++) tests.push_back(tests_[i]);
}
Tests::ListOfTests::ListOfTests( 
	TestBaseclass *test1, TestBaseclass *test2, TestBaseclass *test3, TestBaseclass *test4, 
	TestBaseclass *test5, TestBaseclass *test6, TestBaseclass *test7, TestBaseclass *test8,
	TestBaseclass *test9, TestBaseclass *test10, TestBaseclass *test11, TestBaseclass *test12,
	TestBaseclass *test13, TestBaseclass *test14, TestBaseclass *test15, TestBaseclass *test16
) {
	if (test1) tests.push_back(test1);
	if (test2) tests.push_back(test2);
	if (test3) tests.push_back(test3);
	if (test4) tests.push_back(test4);
	if (test5) tests.push_back(test5);
	if (test6) tests.push_back(test6);
	if (test7) tests.push_back(test7);
	if (test8) tests.push_back(test8);
	if (test9) tests.push_back(test9);
	if (test10) tests.push_back(test10);
	if (test11) tests.push_back(test11);
	if (test12) tests.push_back(test12);
	if (test13) tests.push_back(test13);
	if (test14) tests.push_back(test14);
	if (test15) tests.push_back(test15);
	if (test16) tests.push_back(test16);
}

double PractRand::Tests::TestBaseclass::result_to_pvalue ( Uint64 blocks, double r ) {
	return -1;
}

std::string PractRand::Tests::Gap16::get_name() const {
	return std::string("Gap-16");
}
void PractRand::Tests::Gap16::init( PractRand::RNGs::vRNG *known_good ) {
	int i;
	for (i = 0; i < 65536; i += 1) {
		last[i] = -1;
	}
	counts.reset_counts();
	autofail = 0;
	blocks = 0;
}

void PractRand::Tests::Gap16::test_blocks(TestBlock *data, int numblocks) {
	Sint32 ofs = Uint32(blocks) * TestBlock::SIZE / 2;
	unsigned long max = TestBlock::SIZE * numblocks / 2;
	for (unsigned long i = 0; i < max; i++, ofs++) {
		int a = data[0].as16[i];
		Sint32 l = last[a];
		last[a] = ofs & 0x7fffffff;
		if (l == -1) continue;
		l = (ofs - l - 1) & 0x7fffffff;
		int index;
		if (l < (SIZE1 << SET1_SHIFT)) index = l >> SET1_SHIFT;
		else {
			int tmp = l - (SIZE1 << SET1_SHIFT);
			if (tmp < SIZE2 << SET2_SHIFT) {
				index = SIZE1 + (tmp >> SET2_SHIFT);
			}
			else {
				tmp = (tmp - (SIZE2 << SET2_SHIFT)) >> SET3_SHIFT;
				if (tmp >= SIZE3) tmp = SIZE3-1;
				index = SIZE1 + SIZE2 + tmp;
			}
		}

		counts.increment(index);
	}
	Uint64 oblocks = blocks;
	blocks += numblocks;
	if ((oblocks>>18) != (blocks>>18)) {//once every 256 megabytes or so... prevent overflow
		for (int i = 0; i < 65536; i++) {
			int n = last[i];
			if (n == -1) continue;
			if (((ofs - n) & 0x7fffffff) > 0x18000000) {
				last[i] = -1;
				autofail += 1;
			}
		}
	}
}
double PractRand::Tests::Gap16::get_result() {
	double baseprob = 65535.0 / 65536.0;
	enum {TSIZE = SIZE1 + SIZE2 + SIZE3};
	std::vector<double> probs; probs.resize(TSIZE);
	if (autofail) return 9876543210.;
	//correct probs for startup region:
	double lopped = 0;
	double inv_total_samples = 1.0 / (blocks * (TestBlock::SIZE/2.));
	for (int i = 0; i < TSIZE; i++) {
		int first, last;
		if (i < SIZE1) {
			first = i << SET1_SHIFT;
			last = first + (1 << SET1_SHIFT) - 1;
		}
		else if (i < SIZE1+SIZE2) {
			int tmp = i - SIZE1;
			first = (SIZE1 << SET1_SHIFT) + (tmp << SET2_SHIFT);
			last = first + (1 << SET2_SHIFT) - 1;
		}
		else if (i < TSIZE-1) {
			int tmp = i - SIZE1 - SIZE2;
			first = (SIZE1 << SET1_SHIFT) + (SIZE2 << SET2_SHIFT) + (tmp << SET3_SHIFT);
			last = first + (1 << SET3_SHIFT) - 1;
		}
		else {
			first = (SIZE1 << SET1_SHIFT) + (SIZE2 << SET2_SHIFT) + ((SIZE3-1) << SET3_SHIFT);
			last = 123456789;
		}
		double fraction = ((first+last)/2.+1.) * inv_total_samples;
		if (fraction > 1) fraction = 1;
		double p = gap_probs(first, last, baseprob);
		lopped += p * fraction;
		probs[i] = p * (1 - fraction);
	}
	for (int i = 0; i < TSIZE; i++) {
		probs[i] /= 1 - lopped;
	}

	const Uint64 *count_ = counts.get_array();
	Uint64 *count = new Uint64[TSIZE];
	for (int i = 0; i < TSIZE; i++) count[i] = count_[i];

	double r1, r2;
	int reduced_size = simplify_prob_table(
		TSIZE, 
		(blocks * TestBlock::SIZE/2.) / 40., 
		&probs[0], count, true, true
	);
	r1 = g_test(reduced_size, &probs[0], count);
	r1 = math_chisquared_to_normal(r1, reduced_size-1);
	reduced_size = simplify_prob_table(
		reduced_size, 
		sqrt(blocks * TestBlock::SIZE/2.), 
		&probs[0], count, true, true
	);
	r2 = g_test(reduced_size, &probs[0], count);
	r2 = math_chisquared_to_normal(r2, reduced_size-1);
	double r;
	if (r1 + 2.0 > fabs(r2)) r = r1 - 1;
	else if (r1 - 2.0 < -fabs(r2)) r = r1 + 1;
	else r = r2;
	return r;
}

static double apply_callibration_data( double points[21], double value ) {
	//TO DO: this whole function needs more work
	//it is not used yet
	static double positions[21] = {
		1.0,//0
		0.9999999,//1
		0.999999,//2
		0.99999,//3 extrapolated
		0.9999,//4
		0.999,//5
		0.99,//6
		0.9,//7
		0.75,//8
		0.6,//9
		0.5,//10
		0.4,//11
		0.25,//12
		0.1,//13
		0.01,//14
		0.001,//15
		0.0001,//16
		0.00001,//17 extrapolated
		0.000001,//18
		0.0000001,//19
		0//20
	};
	int closest = 0, next_closest = 0;
	double closest_dist = fabs(points[0] - value);
	double next_closest_dist = fabs(points[0] - value);
	for (int i = 1; i < 21; i++) {
		double dist = fabs(points[i] - value);
		if (dist < closest_dist) {
			next_closest = closest;
			next_closest_dist = closest_dist;
			closest_dist = dist;
			closest = i;
		}
		else if (dist < next_closest_dist) {
			next_closest_dist = dist;
			next_closest = i;
		}
	}
	if ((closest == 0) && value < points[0]) return 1.0;
	if ((closest == 20) && value > points[20]) return 0.0;
	double interp = (value - points[closest]) / (points[next_closest] - points[closest]);
	return interp * (positions[next_closest] - positions[closest]) + positions[closest];
}
double PractRand::Tests::Gap16::result_to_pvalue ( Uint64 blocks, double r ) {
	if (1) {//very crude aproximation:
		if (r < -9) return 1.0;
		if (r < -7) return 0.99999;
		if (r < -6) return 0.9999;
		if (r < -5) return 0.999;
		if (r < -4) return 0.99;
		if (r < -3) return 0.9;
		if (r > 12) return 0.0;
		if (r > 10) return 0.00001;
		if (r >  9) return 0.0001;
		if (r >  8) return 0.001;
		if (r >  7) return 0.01;
		if (r >  6) return 0.1;
		return 0.5;
	}
}




PractRand::Tests::DistC6::DistC6 ( 
	int length_, 
	int unitsL_, 
	int bits_clipped_0_, 
	int bits_clipped_1_, 
	int bits_clipped_2_
)
:
	length(length_),
	unitsL(unitsL_),
	bits_clipped_0(bits_clipped_0_),
	bits_clipped_1(bits_clipped_1_),
	bits_clipped_2(bits_clipped_2_)
{
//	mode = mode_;

	bits_per_sample = 3 + unitsL - bits_clipped_0 - bits_clipped_1 - bits_clipped_2;
	size = 1 << (bits_per_sample * length); 

	generate_reorder_codes();
	mask_pre = Uint32(-1);
	Uint32 tmp = (1 << bits_per_sample) - 1;
	Uint32 index = _advance_index(0, reorder_bits(0));
	for (int i = 0; i < length-1; i++) {
		index = _advance_index(index, reorder_bits(tmp));
	}
	mask_pre = index;
}
Uint32 PractRand::Tests::DistC6::_reorder_bits ( Uint32 code, int bits_per_sample, int length ) {
	if (!ENABLE_REORDER) return code;
	int tbits = bits_per_sample * length;
	int r = 0;
	for (int i = 0; i < length; i++) {
		for (int j = 0; j < bits_per_sample; j++) {
			int b = (code >> (i*bits_per_sample + j)) & 1;
			r |= b << (j*length+i);
		}
	}
	return r;
}
int PractRand::Tests::DistC6::transform_bitcount ( int bit_count ) const {
	int k = bit_count - (1 << (3 - 1 + unitsL));
	if (bits_clipped_1) {
		int min = -1 << (3 - 1 + unitsL - bits_clipped_1);
		int max = 1 - min;
		if (k < min) k = min;
		if (k > max) k = max;
	}
	return (((k >> bits_clipped_0) + (1 << (bits_per_sample-1)))) & ((1 << bits_per_sample) - 1);
}
void PractRand::Tests::DistC6::init( PractRand::RNGs::vRNG *known_good ) {
	counts.set_size(size);
	counts.reset_counts();
	last_index = 0;
	for (int i = 0; i <= (1 << (3+unitsL)); i++) {
		int trans = transform_bitcount(i);
		int rc = reorder_codes(trans);
		int rb = reorder_bits(rc);
		lookup_table[i] = reorder_bits(reorder_codes(transform_bitcount(i)));
	}
	warmup = length-1;
	blocks = 0;
}
void PractRand::Tests::DistC6::deinit( ) {
}
std::string PractRand::Tests::DistC6::get_name() const {
	std::ostringstream tmp;
	tmp << "DC6:" << length << "x" << (1<<unitsL) << "Bytes:" << 
		(bits_clipped_0 + 10 * bits_clipped_1 + 100 * bits_clipped_2);
	return tmp.str();
//	return make_string("DC6:%dx%dByte:%d", length, 1 << unitsL, 
//		bits_clipped_0 + 10 * bits_clipped_1 + 100 * bits_clipped_2
//	);
}
void PractRand::Tests::DistC6::test_blocks(TestBlock *data, int numblocks) {
	int max = numblocks * (TestBlock::SIZE >> unitsL);
	int i = 0;
	while (warmup && max > i) {
		int bits;
		switch (unitsL) {
			case 0: bits = count_bits8 (data->as8 [i]); break;
			case 1: bits = count_bits16(data->as16[i]); break;
			case 2: bits = count_bits32(data->as32[i]); break;
			case 3: bits = count_bits64(data->as64[i]); break;
			default: {
				issue_error();
				bits=0;//just to make the compiler happy
			} break;
		}
		advance_index(bits);
		i++;
		warmup--;
	}
	Uint32 index = last_index;
	switch (unitsL) {
		case 0: {//8bit
			for (; i < max; i++) {
				index = _advance_index(index, lookup_table[count_bits8 (data->as8 [i])]);
				counts.increment(index);
			}
		}
		break;
		case 1: {//16bit
			for (; i < max; i++) {
				index = _advance_index(index, lookup_table[count_bits16(data->as16[i])]);
				counts.increment(index);
			}
		}
		break;
		case 2: {//32bit
			for (; i < max; i++) {
				index = _advance_index(index, lookup_table[count_bits32(data->as32[i])]);
				counts.increment(index);
			}
		}
		break;
		case 3: {//64bit
			for (; i < max; i++) {
				index = _advance_index(index, lookup_table[count_bits64(data->as64[i])]);
				counts.increment(index);
			}
		}
		break;
		default: {
			issue_error();
			return;
		}
		break;
	}
	last_index = index;
	blocks += numblocks;
}

void PractRand::Tests::DistC6::generate_reorder_codes ( ) {
	long double _probs[64+1];
	long double _tprobs[64];
	int i;
	//probabilities of bit-counts
	int hNP = 4 << unitsL;
	int NP = 1 + (8 << unitsL);
	_probs[0] = _probs[NP-1] = 1 / pow(double(2), double(NP-1));
	for (i = 1; i <= hNP; i++) {
		_probs[i] = _probs[i-1] * double(NP - i) / double(i);
		_probs[NP-i-1] = _probs[i];
	}
	
	//probabilities of codes
	for (i = 0; i < (1 << bits_per_sample); i++) _tprobs[i] = 0;
	for (i = 0; i < NP; i++) _tprobs[transform_bitcount(i)] += _probs[i];
	//reordering codes
	for (i = 0; i < (1 << bits_per_sample); i++) {
		reorder_codes_table[i] = Uint8(i);
		unreorder_codes_table[i] = Uint8(i);
	}
//	return;
	if (!ENABLE_REORDER) return;
	for (i = 1; i < (1 << bits_per_sample); i++) {
		for (int j = i-1; j >= 0; j--) {
			if (_tprobs[j] < _tprobs[j+1]) {
				long double tmp_ld = _tprobs[j];
				_tprobs[j] = _tprobs[j+1];
				_tprobs[j+1] = tmp_ld;
				Uint8 tmp_i = unreorder_codes_table[j];
				unreorder_codes_table[j] = unreorder_codes_table[j+1];
				unreorder_codes_table[j+1] = tmp_i;
				tmp_i = reorder_codes_table[unreorder_codes_table[j+1]];
				reorder_codes_table[unreorder_codes_table[j+1]] = reorder_codes_table[unreorder_codes_table[j]];
				reorder_codes_table[unreorder_codes_table[j]] = tmp_i;
			}
		}
	}
}
double PractRand::Tests::DistC6::get_result() {
	long double _probs[64+1];
	long double _tprobs[64];
	int i;
	//probabilities of bit-counts
	int hNP = 4 << unitsL;
	int NP = 1 + (8 << unitsL);
	_probs[0] = _probs[NP-1] = 1 / pow(double(2), double(NP-1));
	for (i = 1; i <= hNP; i++) {
		_probs[i] = _probs[i-1] * double(NP - i) / double(i);
		_probs[NP-i-1] = _probs[i];
	}

	//probabilities of codes
	for (i = 0; i < (1 << bits_per_sample); i++) _tprobs[i] = 0;
	for (i = 0; i < NP; i++) _tprobs[transform_bitcount(i)] += _probs[i];

	long double sum1 = 0;
	for (i = 0; i < NP; i++) sum1 += _probs[i];
//	sum1 = sqrt(sum1);
//	for (i = 0; i < (1 << bits_per_sample); i++) _probs[i] /= sum1;
	sum1 = 0;
	for (i = 0; i < (1 << bits_per_sample); i++) sum1 += _tprobs[i];

	//duplicate counts and applying probabilities
	Uint64 *tmp_counts = new Uint64[size];
	double *probs = new double[size];
	//retaining the reordered ordering
	const Uint64 *count_ = counts.get_array();
	if (1) {
		for (i = 0; i < size; i++) {
			//int x = reorder_bits(i);
			int x = 0;
			for (int j = 0; j < length; j++) {
				Uint32 k = (i >> ((length-j-1) * bits_per_sample)) & ((1 << bits_per_sample)-1);
				x = _advance_index ( x, reorder_bits(reorder_codes(k)) );
			}
			tmp_counts[i] = count_[x];
		}
		for (i = 0; i < size; i++) {
			long double p = 1.0;
			Uint32 x = i;//unreorder_bits(i);
			for (int j = 0; j < length; j++) {
				Uint32 k = (x >> (j * bits_per_sample)) & ((1 << bits_per_sample)-1);
				p *= _tprobs[k];
			}
  			probs[i] = p;
		}
	}
	else {
		for (i = 0; i < size; i++) {
			int x = i;
			tmp_counts[i] = count_[x];
		}
		for (i = 0; i < size; i++) {
			long double p = 1.0;
			Uint32 x = unreorder_bits(i);
			for (int j = 0; j < length; j++) {
				Uint32 k = (x >> (j * bits_per_sample)) & ((1 << bits_per_sample)-1);
				k = unreorder_codes(k);
				p *= _tprobs[k];
			}
  			probs[i] = p;
		}
	}

/*	Uint32 counts_hash = complex_hash32( tmp_counts, size * sizeof(Sint64));
//	std::sort(&tmp_counts[0], &tmp_counts[size]);
	Uint32 counts_hash2 = complex_hash32( tmp_counts, size * sizeof(Sint64));
	Uint32 probs_hash  = complex_hash32( probs, size * sizeof(double));
	Sint64 ts = 0; for (int i = 0; i < size; i++) {ts += tmp_counts[i];}
	printf(" %3d %3d %3d %3d\n", counts_hash % 1000, counts_hash2 % 1000, probs_hash % 1000, int(ts % 1000));//*/

	//finishing
	int reduced_size = simplify_prob_table(size, 
		blocks * (TestBlock::SIZE >> unitsL) / 25.0,
		probs, tmp_counts, true, true);
	double r = g_test(reduced_size, probs, tmp_counts);
	r = math_chisquared_to_normal(r, reduced_size - 1);
	delete[] probs;
	delete[] tmp_counts;
	return r;
}
double PractRand::Tests::DistC6::result_to_pvalue ( Uint64 blocks, double r ) {
	//very crude aproximation:
	if (unitsL == 0 && length == 9 && bits_clipped_0 == 1 && bits_clipped_1 == 0 && bits_clipped_2 == 0) {
		if (blocks * TestBlock::SIZE <= 1024 * 2) {//1 KB to 2 KB
			if (r < -7) return 1.0;
			if (r < -5) return 0.99999;
			if (r < -4) return 0.9999;
			if (r < -3.5) return 0.999;
			if (r < -3) return 0.99;
			if (r < -2) return 0.9;
			if (r > 24) return 0.0;
			if (r > 16) return 0.00001;
			if (r > 12) return 0.0001;
			if (r > 10) return 0.001;
			if (r >  7) return 0.01;
			if (r >  3) return 0.1;
			return 0.5;
		}
		else if (blocks * TestBlock::SIZE <= 1024 * 256) {//2 KB to 256 KB
			if (r < -9) return 1.0;
			if (r < -7) return 0.99999;
			if (r < -6) return 0.9999;
			if (r < -5.5) return 0.999;
			if (r < -4) return 0.99;
			if (r < -2.5) return 0.9;
			if (r > 15) return 0.0;
			if (r > 12) return 0.00001;
			if (r > 10) return 0.0001;
			if (r > 7.5) return 0.001;
			if (r > 5.5) return 0.01;
			if (r >  3) return 0.1;
			return 0.5;
		}
		else {//256 KB to infinity
			if (r < -9) return 1.0;
			if (r < -7) return 0.99999;
			if (r < -6.5) return 0.9999;
			if (r < -5.5) return 0.999;
			if (r < -4) return 0.99;
			if (r < -2) return 0.9;
			if (r > 13) return 0.0;
			if (r > 9) return 0.00001;
			if (r > 8) return 0.0001;
			if (r > 7) return 0.001;
			if (r > 5.3) return 0.01;
			if (r > 3) return 0.1;
			return 0.5;
		}
	}
	return -1;
	/*else if (0) {//closer aproximations:
//1.0,		0.9999999,	0.999999,	0.99999,	0.9999,		0.999,		0.99,
//0.9,		0.75,		0.6,		0.5,		0.4,		0.25,		0.1,
//0.01,		0.001,		0.0001,		0.00001,	0.000001,	0.0000001,	0
	}*/
}


static const char bit_count_table8[256] = {
	0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};
PractRand::Tests::BCFN::BCFN( int unitsL2_, int tbits_ ) {
	unitsL2 = unitsL2_;
	tbits = tbits_;
}
void PractRand::Tests::BCFN::init( PractRand::RNGs::vRNG *known_good ) {
	for (int i = 0; i < LEVELS; i++) {
		int tbitsl = tbits;// - (i+3)/4;
		if (tbitsl <= 0) {
			issue_error();
		}
		int tsize = 1 << tbitsl;
		mask[i] = tsize - 1;
		leftovers[i] = 0;
		total[i] = 0;
		warmup[i] = tbitsl - 1;
		cur[i] = 0;
		even[i] = false;
		counts[i].set_size(tsize);
		counts[i].reset_counts();
	}
	autofail = false;
	blocks = 0;
}
void PractRand::Tests::BCFN::deinit() {
	for (int i = 0; i < LEVELS; i++) {
		counts[i].reset_counts();
	}
}
std::string PractRand::Tests::BCFN::get_name() const {
	std::ostringstream f;
	f << "BCFN-" << tbits << "/" << (1<<this->unitsL2);
	return f.str();
	//return make_string("BCFN-%d", tbits);
}
double PractRand::Tests::BCFN::get_result() {
	if (autofail) return 999999999999.0;
	std::vector<Uint64> tempcount;
	std::vector<double> probs;
	double rs[LEVELS];
	for (int level = 0; level < LEVELS; level++) {
		rs[level] = 0;
		int N = int(log(mask[level]+1.) / log(2.) + 0.5);
		double samples = blocks * TestBlock::SIZE * pow(0.5, level+unitsL2) - N + 1;
		if (samples < 0) samples = 0;
		int size = 1 << N;
		//double E = samples / size;
		if (level && (samples < 16384)) continue;
		tempcount.resize(size);
		probs.resize(size);
		double p = 1. / size;
		const Uint64 *_counts = counts[level].get_array();
		for (int i = 0; i < size; i++) {
			probs[i] = p;
			tempcount[i] = _counts[i];
		}
		int reduced_size = simplify_prob_table(size, samples / (5.0 + 2*tbits), &probs[0], &tempcount[0], true, true);
		rs[level] = g_test(reduced_size, &probs[0], &tempcount[0]);
		rs[level] = math_chisquared_to_normal(rs[level], reduced_size-1);
	}
	//to do: combine two adjacent levels together?
	double r = rs[0];
	int chosen_level = -1;
	double MINDIFF = 5.0;
	for (int level = 0+1; level < LEVELS; level++) {
		if (fabs(r)+MINDIFF < fabs(rs[level])) {
			r = rs[level];
			chosen_level = level;
		}
	}
	if (chosen_level != -1) r -= (r>0?1:-1) * MINDIFF;
	if (fabs(r) > 0) {
//		for (int i = 0; i < maxlevel; i++) printf("%f\n", rs[i]);
	}
//	printf("chosen level = %d\n", chosen_level);
	return r;
//	return rs[0];
	//return r / pow((double)maxlevel, 0.25);
}
void PractRand::Tests::BCFN::handle_high_levels ( int level, int bits ) {
	if (level >= LEVELS) return;
	if (!even[level]) {
		even[level] = true;
		leftovers[level] = bits;
		return;
	}
	else {
		even[level] = false;
		bits += leftovers[level];
	}

	if (bits) {
		int tmp = bits >> 31;
		int index = ((cur[level] << 1) - tmp) & mask[level];
		cur[level] = index;
		if (warmup[level]) warmup[level] --;
		else {
			counts[level].increment(index);
			total[level]++;
		}
	}
	handle_high_levels(level+1, bits);
}
void PractRand::Tests::BCFN::test_blocks(TestBlock *data, int numblocks) {
	if (!blocks && numblocks > 1) {
		test_blocks(data, 1);
		data += 1;
		numblocks -= 1;
	}
	if (autofail) return;
	if (!blocks) {
#define GET_BITS8(pos)  (count_bits8 (data[0].as8 [i+(pos)]) - 4)
#define GET_BITS16(pos) (count_bits16(data[0].as16[i+(pos)]) - 8)
#define GET_BITS32(pos) (count_bits32(data[0].as32[i+(pos)]) - 16)
#define GET_BITS64(pos) (count_bits64(data[0].as64[i+(pos)]) - 32)
#define HANDLE_BITS(level,var) if (var){tmp=var>>31;cur[level]=((cur[level]<<1)-tmp)&mask[level];if (warmup[level]) warmup[level]--; else counts[level].increment(cur[level]);}
		switch (unitsL2) {
			case 0: {
				for (int i = 0; i < TestBlock::SIZE / 1; i+=1) {
					int bits0;
					int tmp;
					//0
					bits0 = GET_BITS8(0);
					HANDLE_BITS(0,bits0);
					handle_high_levels(1, bits0);
				}
			}
			break;
			case 1: {
				for (int i = 0; i < TestBlock::SIZE / 2; i+=1) {
					int bits0;
					int tmp;
					//0
					bits0 = GET_BITS16(0);
					HANDLE_BITS(0,bits0);
					handle_high_levels(1, bits0);
				}
			}
			break;
			case 2: {
				for (int i = 0; i < TestBlock::SIZE / 4; i+=1) {
					int bits0;
					int tmp;
					//0
					bits0 = GET_BITS32(0);
					HANDLE_BITS(0,bits0);
					handle_high_levels(1, bits0);
				}
			}
			break;
			case 3: {
				for (int i = 0; i < TestBlock::SIZE / 8; i+=1) {
					int bits0;
					int tmp;
					//0
					bits0 = GET_BITS64(0);
					HANDLE_BITS(0,bits0);
					handle_high_levels(1, bits0);
				}
			}
			break;
			default:
				issue_error();
		}
#undef HANDLE_BITS
		if (warmup[4]) autofail = true;
		blocks += 1;
		if (numblocks > 1) test_blocks(data+1,numblocks-1);
		return;
	}
	else {
		unsigned long max = numblocks * TestBlock::SIZE >> unitsL2;
		switch (unitsL2) {
#define GET_BITS(a) GET_BITS8(a)
			case 0: {
				for (unsigned long i = 0; i < max; i+=8) {
					long bits0, bits1, bits2, bits3;
					long tmp;
#define HANDLE_BITS(level,var) if (var){tmp=var>>31;cur[level]=((cur[level]<<1)-tmp)&mask[level];if (warmup[level]) warmup[level]--; else counts[level].increment(cur[level]);}
					//0
					bits1 = bits0 = GET_BITS(0);
					HANDLE_BITS(0,bits0);
					//1
					bits2 = bits1 += bits0 = GET_BITS(1);
					HANDLE_BITS(0,bits0);
					HANDLE_BITS(1,bits1);
					//2
					bits1 = bits0 = GET_BITS(2);
					HANDLE_BITS(0,bits0);
					//3
					bits3 = bits2 += bits1 += bits0 = GET_BITS(3);
					HANDLE_BITS(0,bits0);
					HANDLE_BITS(1,bits1);
					HANDLE_BITS(2,bits2);
					//4
					bits1 = bits0 = GET_BITS(4);
					HANDLE_BITS(0,bits0);
					//5
					bits2 = bits1 += bits0 = GET_BITS(5);
					HANDLE_BITS(0,bits0);
					HANDLE_BITS(1,bits1);
					//6
					bits1 = bits0 = GET_BITS(6);
					HANDLE_BITS(0,bits0);
					//7
					bits3 += bits2 += bits1 += bits0 = GET_BITS(7);
					HANDLE_BITS(0,bits0);
					HANDLE_BITS(1,bits1);
					HANDLE_BITS(2,bits2);
					HANDLE_BITS(3,bits3);
					handle_high_levels ( 4, bits3 );
				}
			}
			break;
#undef GET_BITS
#define GET_BITS(a) GET_BITS16(a)
			case 1: {
				for (unsigned long i = 0; i < max; i+=8) {
					long bits0, bits1, bits2, bits3;
					long tmp;
					//0
					bits1 = bits0 = GET_BITS(0);
					HANDLE_BITS(0,bits0);
					//1
					bits2 = bits1 += bits0 = GET_BITS(1);
					HANDLE_BITS(0,bits0);
					HANDLE_BITS(1,bits1);
					//2
					bits1 = bits0 = GET_BITS(2);
					HANDLE_BITS(0,bits0);
					//3
					bits3 = bits2 += bits1 += bits0 = GET_BITS(3);
					HANDLE_BITS(0,bits0);
					HANDLE_BITS(1,bits1);
					HANDLE_BITS(2,bits2);
					//4
					bits1 = bits0 = GET_BITS(4);
					HANDLE_BITS(0,bits0);
					//5
					bits2 = bits1 += bits0 = GET_BITS(5);
					HANDLE_BITS(0,bits0);
					HANDLE_BITS(1,bits1);
					//6
					bits1 = bits0 = GET_BITS(6);
					HANDLE_BITS(0,bits0);
					//7
					bits3 += bits2 += bits1 += bits0 = GET_BITS(7);
					HANDLE_BITS(0,bits0);
					HANDLE_BITS(1,bits1);
					HANDLE_BITS(2,bits2);
					HANDLE_BITS(3,bits3);
					handle_high_levels ( 4, bits3 );
				}
			}
			break;
#undef GET_BITS
#define GET_BITS(a) GET_BITS32(a)
			case 2: {
				for (unsigned long i = 0; i < max; i+=8) {
					long bits0, bits1, bits2, bits3;
					long tmp;
					//0
					bits1 = bits0 = GET_BITS(0);
					HANDLE_BITS(0,bits0);
					//1
					bits2 = bits1 += bits0 = GET_BITS(1);
					HANDLE_BITS(0,bits0);
					HANDLE_BITS(1,bits1);
					//2
					bits1 = bits0 = GET_BITS(2);
					HANDLE_BITS(0,bits0);
					//3
					bits3 = bits2 += bits1 += bits0 = GET_BITS(3);
					HANDLE_BITS(0,bits0);
					HANDLE_BITS(1,bits1);
					HANDLE_BITS(2,bits2);
					//4
					bits1 = bits0 = GET_BITS(4);
					HANDLE_BITS(0,bits0);
					//5
					bits2 = bits1 += bits0 = GET_BITS(5);
					HANDLE_BITS(0,bits0);
					HANDLE_BITS(1,bits1);
					//6
					bits1 = bits0 = GET_BITS(6);
					HANDLE_BITS(0,bits0);
					//7
					bits3 += bits2 += bits1 += bits0 = GET_BITS(7);
					HANDLE_BITS(0,bits0);
					HANDLE_BITS(1,bits1);
					HANDLE_BITS(2,bits2);
					HANDLE_BITS(3,bits3);
					handle_high_levels ( 4, bits3 );
				}
			}
			break;
#undef GET_BITS
#define GET_BITS(a) GET_BITS64(a)
			case 3: {
				for (unsigned long i = 0; i < max; i+=8) {
					int bits0, bits1, bits2, bits3;
					int tmp;
					//0
					bits1 = bits0 = GET_BITS(0);
					HANDLE_BITS(0,bits0);
					//1
					bits2 = bits1 += bits0 = GET_BITS(1);
					HANDLE_BITS(0,bits0);
					HANDLE_BITS(1,bits1);
					//2
					bits1 = bits0 = GET_BITS(2);
					HANDLE_BITS(0,bits0);
					//3
					bits3 = bits2 += bits1 += bits0 = GET_BITS(3);
					HANDLE_BITS(0,bits0);
					HANDLE_BITS(1,bits1);
					HANDLE_BITS(2,bits2);
					//4
					bits1 = bits0 = GET_BITS(4);
					HANDLE_BITS(0,bits0);
					//5
					bits2 = bits1 += bits0 = GET_BITS(5);
					HANDLE_BITS(0,bits0);
					HANDLE_BITS(1,bits1);
					//6
					bits1 = bits0 = GET_BITS(6);
					HANDLE_BITS(0,bits0);
					//7
					bits3 += bits2 += bits1 += bits0 = GET_BITS(7);
					HANDLE_BITS(0,bits0);
					HANDLE_BITS(1,bits1);
					HANDLE_BITS(2,bits2);
					HANDLE_BITS(3,bits3);
					handle_high_levels ( 4, bits3 );
				}
			}
			break;
#undef GET_BITS
#undef GET_BITS8
#undef GET_BITS16
#undef GET_BITS32
#undef GET_BITS64
#undef HANDLE_BITS
			default:
				issue_error();
		}
	}
	blocks += numblocks;
}
double PractRand::Tests::BCFN::result_to_pvalue ( Uint64 blocks, double r ) {
	if (tbits == 13) {//very crude aproximation:
		blocks >>= unitsL2;
		if (blocks < 1<<9) {
			return -1;
		}
		blocks *= TestBlock::SIZE;
		if (blocks < 1<<22) {
			if (r <-18) return 1.0;
			if (r <-12) return 0.99999;
			if (r < -8.2) return 0.9999;
			if (r < -6.8) return 0.999;
			if (r < -5) return 0.99;
			if (r < -2.6) return 0.9;
			if (r > 18) return 0.0;
			if (r > 12) return 0.00001;
			if (r > 10.5) return 0.0001;
			if (r >  8.3) return 0.001;
			if (r >  6.4) return 0.01;
			if (r >  3.8) return 0.1;
			return 0.5;
		}
		if (blocks < 1<<27) {
			if (r <-15) return 1.0;
			if (r <-10) return 0.99999;
			if (r < -9) return 0.9999;
			if (r < -7.4) return 0.999;
			if (r < -5.6) return 0.99;
			if (r < -3.1) return 0.9;
			if (r > 18) return 0.0;
			if (r > 12) return 0.00001;
			if (r > 10.8) return 0.0001;
			if (r >  8.0) return 0.001;
			if (r >  6.0) return 0.01;
			if (r >  3.3) return 0.1;
			return 0.5;
		}
		else {
			if (r <-15) return 1.0;
			if (r <-12) return 0.99999;
			if (r < -9) return 0.9999;
			if (r < -8.2) return 0.999;
			if (r < -5.8) return 0.99;
			if (r < -3.2) return 0.9;
			if (r > 15) return 0.0;
			if (r > 12) return 0.00001;
			if (r >  9) return 0.0001;
			if (r >  8.2) return 0.001;
			if (r >  6.2) return 0.01;
			if (r >  3.5) return 0.1;
			return 0.5;
		}
	}
	return -1;
	/*else if (0) {//closer aproximations:
//1.0,		0.9999999,	0.999999,	0.99999,	0.9999,		0.999,		0.99,
//0.9,		0.75,		0.6,		0.5,		0.4,		0.25,		0.1,
//0.01,		0.001,		0.0001,		0.00001,	0.000001,	0.0000001,	0
	}*/
}










Uint8 PractRand::Tests::FPF::count_leading_zeroes_table[COUNT_LEADING_ZEROES_TABLE_SIZE];
PractRand::Tests::FPF::FPF(int stride_bits_L2_ , int sig_bits_ , int exp_bits_ ) {
	stride_bits_L2 = stride_bits_L2_;
	sig_bits = sig_bits_;
	exp_bits = exp_bits_;
	if (sig_bits > MAX_SIG_BITS) issue_error();
}
void PractRand::Tests::FPF::init( RNGs::vRNG *known_good ) {
	unsigned long max_exp = (1 << exp_bits) - 1;
	if (max_exp - 1 + sig_bits > 64) max_exp = 64 + 1 - sig_bits;
	unsigned long max_sig = (1 << sig_bits) - 1;
	unsigned long footprint = sig_bits + max_exp;
	unsigned long stride_bits = 1 << stride_bits_L2;

	int total_size = (max_sig + 1) * (max_exp + 1);
//	int bits = sig_bits + exp_bits;
	counts.set_size(total_size);
	static bool zeroes_table_inited = false;
	if (!zeroes_table_inited) {
		zeroes_table_inited = true;
		for (int i = 0; i < COUNT_LEADING_ZEROES_TABLE_SIZE; i++) {
			int k;
			for (k=0; (k < COUNT_LEADING_ZEROES_TABLE_BITS) && !((i>>k)&1); k++) ;//number of leading 0s, max 7
			count_leading_zeroes_table[i] = k;
		}
	}
	blocks = 0;
}
void PractRand::Tests::FPF::deinit( ) {
	counts.set_size(0);
}
std::string PractRand::Tests::FPF::get_name() const {
	std::ostringstream str;
	str << "FPF-" << sig_bits << "+" << exp_bits << "/" << (1 << stride_bits_L2);
	return str.str();
}
unsigned long PractRand::Tests::FPF::count_leading_zeroes32( Uint32 value ) {
	unsigned long part = value;
	part = value & (COUNT_LEADING_ZEROES_TABLE_SIZE-1);
	unsigned long r = count_leading_zeroes_table[part];
	if (part) return r;
	while (r < 32) {
		value >>= COUNT_LEADING_ZEROES_TABLE_BITS;
		part = value & (COUNT_LEADING_ZEROES_TABLE_SIZE-1);
		r += count_leading_zeroes_table[part];
		if (part) return r;
	}
	return 32;
}
double PractRand::Tests::FPF::get_result() {
	unsigned long max_exp = (1 << exp_bits) - 1;
	if (max_exp - 1 + sig_bits > 64) max_exp = 64 + 1 - sig_bits;
	unsigned long max_sig = (1 << sig_bits) - 1;
	unsigned long footprint = sig_bits + max_exp;
	unsigned long stride_bits = 1 << stride_bits_L2;

	int total_size = (max_sig + 1) * (max_exp + 1);
	double samples = (blocks * TestBlock::SIZE * 8.0 + 1 - footprint) / stride_bits;
	if (samples < 0) return 0;
	std::vector<double> probs; probs.resize(total_size);
	const Uint64 *counts_ = counts.get_array();
	std::vector<Uint64> counts2; counts2.resize(total_size);
	double actual_samples = 0;
	for (int i = 0; i < total_size; i++) {
		counts2[i] = counts_[i];
		actual_samples += counts_[i];
	}
	unsigned int usable_exp = max_exp;
	for (unsigned int fe = 0; fe <= max_exp; fe++) {
		int lossL2 = fe;
		if (fe == max_exp) lossL2 = fe-1;
		double p = pow(.5, sig_bits + lossL2 + 1.);
		double subcount = 0;
		for (unsigned int i = 0; i <= max_sig; i++) {
			int index = (fe << sig_bits) + i;
			probs[index] = p;
			subcount += counts2[index];
		}
		double expected = p * pow(2.0,(double)sig_bits) * samples;
		if (subcount < 40 && usable_exp > fe) usable_exp = fe;
	}

	total_size = (max_sig + 1) * (usable_exp + 1);
	double thresh = 5 + footprint * 2.0 / stride_bits;
	int reduced_size = simplify_prob_table(total_size, samples / thresh, &probs[0], &counts2[0], true, true);
	double r = g_test(reduced_size, &probs[0], &counts2[0]);
	r = math_chisquared_to_normal(r, reduced_size-1);
	return r;
}
void PractRand::Tests::FPF::test_blocks(TestBlock *data, int numblocks) {
	unsigned long max_exp = (1 << exp_bits) - 1;
	if (max_exp - 1 + sig_bits > 64) max_exp = 64 + 1 - sig_bits;
	unsigned long max_sig = (1 << sig_bits) - 1;
	unsigned long footprint = sig_bits + max_exp;
	unsigned long stride_bits = 1 << stride_bits_L2;

	if (max_exp + sig_bits - 1 <= 32) {
		if (stride_bits >= 32) {//small footprint, long stride
			long stride32 = 1 << (stride_bits_L2 - 5);
			long max = numblocks * (TestBlock::SIZE / 4);
			for (long i = 0; i < max; i += stride32) {
				Uint32 cur = data->as32[i];
				unsigned long e = count_leading_zeroes32(cur);
				unsigned long sig;
				if (e >= max_exp) {
					e = max_exp;
					sig = (cur >> e) & max_sig;
				}
				else {
					sig = (cur >> (e+1)) & max_sig;
				}
				unsigned long index = (e << sig_bits) + sig;
				counts.increment(index);
			}
		}
		else {//small footprint, short stride
			long max32 = numblocks * (TestBlock::SIZE / 4);
			Uint32 cur;
			long start;
			if (blocks) {
				cur = reverse_bits32(data->as32[-1]);
				start = 0;
			}
			else {
				cur = reverse_bits32(data->as32[0]);
				start = 1;
			}
			unsigned long word_parts = 32 >> stride_bits_L2;
			for (long i = start; i < max32; i += 1) {
				Uint32 word = data->as32[i];
				for (unsigned long sub_word = 0; sub_word < word_parts; sub_word++) {
					for (unsigned long j = 0; j < stride_bits; j++) {
						cur = (cur << 1) | (word & 1);
						word >>= 1;
					}
					unsigned long e = count_leading_zeroes32(cur);
					unsigned long sig;
					if (e >= max_exp) {
						e = max_exp;
						sig = (cur >> e) & max_sig;
					}
					else {
						sig = (cur >> (e+1)) & max_sig;
					}
					unsigned long index = (e << sig_bits) + sig;
					counts.increment(index);
				}
			}
		}
	}
	else {
		if (stride_bits >= 32) {//large footprint, long stride
			long stride32 = 1 << (stride_bits_L2 - 5);
			long max = numblocks * (TestBlock::SIZE / 4) - 1;
			for (long i = blocks ? -1 : 0; i < max; i += stride32) {
				Uint32 cur = data->as32[i];
				unsigned long e = count_leading_zeroes32(cur);
				unsigned long sig;
				if (e < 32 - sig_bits) {
					sig = (cur >> (e+1)) & max_sig;
				}
				else {
					Uint64 cur2 = cur | (Uint64(reverse_bits32(data->as32[i+1])) << 32);
					if (e < max_exp) {
						sig = Uint32(cur2 >> (e+1)) & max_sig;
					}
					else {
						e = max_exp;
						sig = Uint32(cur2 >> e) & max_sig;
					}
				}
				unsigned long index = (e << sig_bits) + sig;
				counts.increment(index);
			}
		}
		else {//large footprint, short stride
			long max32 = numblocks * (TestBlock::SIZE / 4) - 1;
			long start;
			Uint32 cur;
			if (blocks) {
				cur = reverse_bits32(data->as32[-2]);
				start = -1;
			}
			else {
				cur = reverse_bits32(data->as32[0]);
				start = 1;
			}
			unsigned long word_parts = 32 >> stride_bits_L2;
			for (long i = start; i < max32; i += 1) {
				Uint32 word = data->as32[i];
				for (unsigned long sub_word = 0; sub_word < word_parts; sub_word++) {
					for (unsigned long j = 0; j < stride_bits; j++) {
						cur = (cur << 1) | (word & 1);
						word >>= 1;
					}
					unsigned long e = count_leading_zeroes32(cur);
					unsigned long sig;
					if (e < 32 - sig_bits) {
						sig = (cur >> (e+1)) & max_sig;
					}
					else {
						Uint64 cur2 = cur | (Uint64(reverse_bits32(data->as32[i+1])) << 32);
						if (e < max_exp) {
							sig = Uint32(cur2 >> (e+1)) & max_sig;
						}
						else {
							e = max_exp;
							sig = Uint32(cur2 >> e) & max_sig;
						}
					}
					unsigned long index = (e << sig_bits) + sig;
					counts.increment(index);
				}
			}
		}
	}
	blocks += numblocks;
}


void PractRand::Tests::CoupGap::init( RNGs::vRNG *known_good ) {
	autofail = 0;
	blocks = 0;
	symbols_ready = 0;
	for (int i = 0; i < 256; i++) {
		sym_has_appeared[i] = false;
		next_younger_sym[i] = (i+1) & 255;
		youngest_sym = 255;
		oldest_sym = 0;
//		last_sym_pos[i] = 0;
	}
	count_syms_by_oldest_sym.reset_counts();
//	count_gaps_by_oldest_sym.reset_counts();
}
std::string PractRand::Tests::CoupGap::get_name( ) const {
	return std::string("CoupGap");
}
double PractRand::Tests::CoupGap::get_result() {
	if (autofail) return 9876543210.;
	if (blocks < 256) return 0;

	std::vector<double> probs;
	probs.resize(65536);
	for (int i = 0; i < 65536; i++) probs[i] = 1 / 65536.0;
	const Uint64 *counts_ = count_syms_by_oldest_sym.get_array();
	const double *probs_ = &probs[0];
	double raw = g_test(65536, probs_, counts_);
	return (raw - 3 * 65536) / (256 * 32);
}
void PractRand::Tests::CoupGap::test_blocks(TestBlock *data, int numblocks) {
	if (autofail) return;
	int i;
	Uint32 ofs = Uint32(blocks) * TestBlock::SIZE;
	int max = TestBlock::SIZE * numblocks;
	for (i = 0; i < max; i++, ofs++) {
		unsigned long sym = data[0].as8[i];
//		Uint32 last_pos = last_sym_pos[sym];

		if (symbols_ready == 256) {
//			Uint32 oldest_age = ofs - last_sym_pos[oldest_sym] - 256;
//			if (oldest_age > MAX_OLDEST_AGE-1) oldest_age = MAX_OLDEST_AGE-1;
//			Uint32 current_age = ofs - last_pos - 1;
//			if (current_age > MAX_CURRENT_AGE-1) current_age = MAX_CURRENT_AGE-1;
			count_syms_by_oldest_sym.increment(sym * 256 + oldest_sym);
//			count_gaps_by_oldest_sym.increment(oldest_sym + (current_age << 8));
		}
		else if (!sym_has_appeared[sym]) {
			sym_has_appeared[sym] = true;
			symbols_ready ++;
		}

		//update linked list
//		last_sym_pos[sym] = ofs;
		if (oldest_sym == sym) {
			oldest_sym = next_younger_sym[sym];
		}
		next_younger_sym[youngest_sym] = Uint8(sym);
		youngest_sym = sym;
	}
	Uint64 oblocks = blocks;
	blocks += numblocks;
//	if ((oblocks>>17) != (blocks>>17)) {//once every 128 megabytes or so... prevent overflow
//		for (i = 0; i < 256; i++) {
//			int n = last_sym_pos[i];
//			if (n == -1) continue;
//			if ((ofs - n) > 12345678) {
//				autofail += 1;
//			}
//		}
//	}
}







PractRand::Tests::Transforms::multiplex::multiplex(const char *name_, const ListOfTests &testlist)
:
	subtests(testlist)
{
//	for (unsigned int i = 0; i < testlist.tests.size(); i++) subtests.push_back(testlist.tests[i]);
	if (name_) name = name_;
	else if (subtests.tests.size() == 1) name = subtests.tests[0]->get_name();
	else {
		//std::ostringstream str;
		//str << "{" << int(subtests.size()) << "}";
		//name = str.str();
		name = "";
	}
//	else name = make_string("");
}

void PractRand::Tests::Transforms::multiplex::deinit() {
	for (std::vector<Tests::TestBaseclass*>::iterator it = subtests.tests.begin(); it != subtests.tests.end(); it++)
		(*it)->deinit();
}
void PractRand::Tests::Transforms::multiplex::init( RNGs::vRNG *known_good ) {
	blocks_already = 0;
	for (std::vector<Tests::TestBaseclass*>::iterator it = subtests.tests.begin(); it != subtests.tests.end(); it++)
		(*it)->init(known_good);
}
PractRand::Tests::Transforms::multiplex::~multiplex ( ) {
	for (std::vector<Tests::TestBaseclass*>::iterator it = subtests.tests.begin(); it != subtests.tests.end(); it++)
		delete (*it);
	subtests.tests.clear();
}
std::string PractRand::Tests::Transforms::multiplex::get_name() const {
	return name.c_str();
}
void PractRand::Tests::Transforms::multiplex::test_blocks(TestBlock *data, int numblocks) {
	for (std::vector<Tests::TestBaseclass*>::iterator it = subtests.tests.begin(); it != subtests.tests.end(); it++)
		(*it)->test_blocks(data, numblocks);
	blocks_already += numblocks;
}
double PractRand::Tests::Transforms::multiplex::get_result() {
/*	double sum = 0;
	for (std::vector<TestBase*>::iterator it = subtests.begin(); it != subtests.end(); it++) {
		double tmp = (*it)->get_result();
		tmp *= tmp;
		if (tmp < 1) tmp += .5;
		else if (tmp > 1) tmp = tmp / 2 + .5;
		if (tmp > 30) tmp = tmp * 2 - 30;
		sum += tmp;
	}
	return sqrt(sum) - subtests.size();*/
	double highest = 0;
	for (std::vector<Tests::TestBaseclass*>::iterator it = subtests.tests.begin(); it != subtests.tests.end(); it++) {
		double r = (*it)->get_result();
		if (fabs(r) > fabs(highest)) highest = r;
	}
	return highest;
}
int PractRand::Tests::Transforms::multiplex::get_num_children() const {return subtests.tests.size();}
Tests::TestBaseclass *PractRand::Tests::Transforms::multiplex::get_child  (int index) const {return subtests.tests[index];}
//std::string PractRand::Tests::Transforms::multiplex::get_child_name  (int index) const {return subtests[index]->get_name();}
//double      PractRand::Tests::Transforms::multiplex::get_child_result(int index) {return subtests[index]->get_result();}

Uint64 PractRand::Tests::Transforms::multiplex::get_blocks_passed_through(int index) const {
	return blocks_already;
}
Uint64 PractRand::Tests::Transforms::switching::get_blocks_passed_through(int index) const {
	return blocks_already_per[index];
}


PractRand::Tests::Transforms::switching::switching(
	const char *name_, 
	const ListOfTests &testlist, 
	std::vector<Uint64> lengths_)
:
	multiplex(name_, testlist),
	lengths(lengths_),
	phase(0),
	which(0)
{
	if (lengths.size() != testlist.tests.size()) issue_error();
	blocks_already_per.resize(lengths.size());
	for (int i = 0; i < blocks_already_per.size(); i++)
		blocks_already_per[i] = 0;
	total_length = 0;
	for (int i = 0; i < blocks_already_per.size(); i++) total_length += lengths[i];
}
PractRand::Tests::Transforms::switching::switching(
	const char *name_, 
	const ListOfTests &testlist, 
	Uint64 length)
:
	multiplex(name_, testlist)
{
	lengths.resize(testlist.tests.size());
	blocks_already_per.resize(lengths.size());
	for (int i = 0; i < blocks_already_per.size(); i++)
		lengths[i] = length;
	total_length = length * blocks_already_per.size();
}
void PractRand::Tests::Transforms::switching::init( RNGs::vRNG *known_good ) {
	for (int i = 0; i < blocks_already_per.size(); i++)
		blocks_already_per[i] = 0;
	phase = 0;
	which = 0;
	multiplex::init(known_good);
}
void PractRand::Tests::Transforms::switching::test_blocks( TestBlock *data, int numblocks_ ) {
	Uint64 numblocks = numblocks_;
	if (phase + numblocks < lengths[which]) {
		phase += numblocks;
		subtests.tests[which]->test_blocks(data, numblocks_);
		return;
	}
	Uint64 part = lengths[which] - phase;
	subtests.tests[which]->test_blocks(data, Uint32(part));
	phase = 0;
	if (++which == subtests.tests.size()) which = 0;
	data += part;
	numblocks_ -= part;
	test_blocks(data, numblocks_);
}
//double PractRand::Tests::Transforms::switching::get_result() {
//}
	
void PractRand::Tests::Transforms::Transform_Baseclass::init( RNGs::vRNG *known_good ) {
	Transforms::multiplex::init(known_good);
	leftovers = 0;
//	buffered.reserve( flush_size * TESTBLOCK_SIZE );
	buffered.clear();
}
void PractRand::Tests::Transforms::Transform_Baseclass::flush(bool aggressive) {
	int minblocks = aggressive ? 1 : flush_size;
	int numblocks = buffered.size() - (leftovers?1:0);
	Uint32 old = (blocks_already > TestBaseclass::REPEATED_BLOCKS) ? TestBaseclass::REPEATED_BLOCKS : (int)blocks_already;
	numblocks -= old;
	if (numblocks < minblocks) return;
	Transforms::multiplex::test_blocks((TestBlock*)&buffered[old], numblocks);
//	int newold = blocks_already;
//	if (blocks_already > TestBaseclass::REPEATED_BLOCKS) newold = TestBaseclass::REPEATED_BLOCKS;
	int newold = (blocks_already > TestBaseclass::REPEATED_BLOCKS) ? TestBaseclass::REPEATED_BLOCKS : (int)blocks_already;
	int blocks_to_move = newold + (leftovers?1:0);
	if (!blocks_to_move) return;
	int how_far = buffered.size() - blocks_to_move;
	if (!how_far) return;
	memmove(&buffered[0], &buffered[how_far], blocks_to_move * TestBlock::SIZE);
	buffered.resize(blocks_to_move);
}


std::string PractRand::Tests::Transforms::FirstNofM::get_name() const {
	std::ostringstream str;
	str << "[1st" << bytes_used << "/" << bytes_stride << "B]" << Transform_Baseclass::get_name().c_str();
	return str.str();
//	return make_string("[1st%d/%dB]%s", bytes_used, bytes_stride, Transform_Baseclass::get_name().c_str());
}
void PractRand::Tests::Transforms::FirstNofM::init( RNGs::vRNG *known_good ) {
	Transform_Baseclass::init(known_good);
	input_phase = 0;
	if (bytes_stride < bytes_used) issue_error();
}
void PractRand::Tests::Transforms::FirstNofM::test_blocks(TestBlock *data, int numblocks) {
	int max = numblocks * TestBlock::SIZE;
	Uint8 *inptr = &data->as8[0];

	int begin_length = bytes_stride - input_phase;
	if (begin_length == bytes_stride) begin_length = 0;
	int begin_out = begin_length > bytes_used ? bytes_used : begin_length;
	int full_sets = (max - begin_length) / bytes_stride;
	int main_out = full_sets * bytes_used;
	int end_length = (max - begin_length) % bytes_stride;
	int end_out = end_length > bytes_used ? bytes_used : end_length;
	int total_out = begin_out + main_out + end_out;
	int leftover_blocks = leftovers ? 1 : 0;
	int alloc_already = leftover_blocks * TestBlock::SIZE - leftovers;
	int alloc_more = total_out - alloc_already;
	int old_blocks = buffered.size();
	if (alloc_more > 0) {
		int blocks_more = (alloc_more + TestBlock::SIZE - 1) / TestBlock::SIZE;
		buffered.resize(old_blocks + blocks_more);
	}
	Uint8 *outptr = &buffered[old_blocks].as8[0] - leftovers;

	if (begin_out) memcpy(outptr, inptr, begin_out);
	outptr += begin_out;
	inptr += begin_length;
	max -= begin_length;
	if (main_out) {
		if (bytes_used == 1) {
			while (max > bytes_stride) {
				*outptr = *inptr;
				outptr += bytes_used;
				inptr += bytes_stride;
				max -= bytes_stride;
			}
		}
		else if (bytes_used == 2) {
			while (max > bytes_stride) {
				outptr[0] = inptr[0];
				outptr[1] = inptr[1];
				outptr += bytes_used;
				inptr += bytes_stride;
				max -= bytes_stride;
			}
		}
		else {
			while (max > bytes_stride) {
				memcpy(outptr, inptr, bytes_used);
				outptr += bytes_used;
				inptr += bytes_stride;
				max -= bytes_stride;
			}
		}
	}
	if (end_out) memcpy(outptr, inptr, end_out);
	outptr += end_out;
	inptr += end_length;
	max -= end_length;
	input_phase = bytes_stride - end_length;
	if (!end_length) input_phase = 0;
	flush(false);
}
std::string PractRand::Tests::Transforms::lowbits::get_name() const {
	std::ostringstream str;
	str << "[Low" << (1 << lowbitsL) << "/" << (1<<(3+unitsL)) << "]" << Transform_Baseclass::get_name().c_str();
	return str.str();
	//return make_string("[Low%d/%d]%s", 1 << lowbitsL, 1<<(3+unitsL),TestTransform_Base::get_name().c_str());
}
void PractRand::Tests::Transforms::lowbits::init( RNGs::vRNG *known_good ) {
	Transform_Baseclass::init(known_good);
	if ((lowbitsL > 5) || (lowbitsL > unitsL+3)) {
		issue_error();
	}
}
void PractRand::Tests::Transforms::lowbits::test_blocks(TestBlock *data, int numblocks) {
	int max = numblocks * (TestBlock::SIZE >> unitsL);
	int lowbits = 1 << lowbitsL;
	Uint32 *dest_ptr;
	if (1) {//allocate space in vector:
		int spare_words = (TestBlock::SIZE/4 - leftovers) & (TestBlock::SIZE/4-1);
		int needed_words = (max * lowbits) / 32 - spare_words;
		int needed_blocks = (needed_words + TestBlock::SIZE/4 - 1) / (TestBlock::SIZE/4);
		int oldblocks = buffered.size();
		buffered.resize(buffered.size() + needed_blocks);
		dest_ptr = &(buffered[oldblocks-(leftovers?1:0)].as32[leftovers]);
		leftovers = (leftovers + needed_words) & (TestBlock::SIZE/4-1);
	}
	switch (unitsL) {
		case 0: {
			Uint32 mask = (1 << lowbits) - 1;
			int i = 0;
			while (i < max) {
				Uint32 word = data->as8[i++] & mask;
				for (int j = lowbits; j < 32; j+=lowbits) {
					word |= Uint32(data->as8[i++] & mask) << j;
				}
				*dest_ptr = word;
				dest_ptr++;
			}
		}
		break;
		case 1: {//16 bit words
			Uint32 mask = (1 << lowbits) - 1;
			int i = 0;
			while (i < max) {
				Uint32 word = data->as16[i++] & mask;
				for (int j = lowbits; j < 32; j+=lowbits) {
					word |= Uint32(data->as16[i++] & mask) << j;
				}
				*dest_ptr = word;
				dest_ptr++;
			}
		}
		break;
		case 2: {//32 bit words
			Uint32 mask = (1 << lowbits) - 1;
			int i = 0;
			while (i < max) {
				Uint32 word = data->as32[i++] & mask;
				for (int j = lowbits; j < 32; j+=lowbits) {
					word |= Uint32(data->as32[i++] & mask) << j;
				}
				*dest_ptr = word;
				dest_ptr++;
			}
		}
		break;
		case 3: {//64 bit words
			Uint32 mask = Uint32((Uint64(1) << lowbits) - 1);
			int i = 0;
			while (i < max) {
				Uint32 word = Uint32(data->as64[i++]) & mask;
				for (int j = lowbits; j < 32; j+=lowbits) {
					word |= (Uint32(data->as64[i++]) & mask) << j;
				}
				*dest_ptr = word;
				dest_ptr++;
			}
		}
		break;
		default:{
			issue_error();
		}
		break;
	}
	flush(false);
}

