#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <cstring>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <sstream>
#include <algorithm>
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
#include "PractRand/RNGs/all.h"
#include "PractRand/RNGs/other/transform.h"
#include "PractRand/RNGs/other/mult.h"
#include "PractRand/RNGs/other/simple.h"
#include "PractRand/RNGs/other/fibonacci.h"
#include "PractRand/RNGs/other/indirection.h"
#include "PractRand/RNGs/other/special.h"

using namespace PractRand;

//some helpers for the sample programs:
#include "multithreading.h"
#include "TestManager.h"
#include "MultithreadedTestManager.h"


class SampleSet {
public:
	std::vector<double> rs;
	Uint64 duplicates;
	double sum;
	double sum_sqr;
	void _count_duplicates();
	double _get_index ( double other_result ) const;//interpolates
public:
	void _normalize();
	void _add(double result) {rs.push_back(result); sum += result; sum_sqr += result * result;}
	SampleSet() : duplicates(0), sum(0), sum_sqr(0) {}
	void add(const SampleSet &other) {
		for (unsigned int i = 0; i < other.rs.size(); i++) _add(other.rs[i]);
		_normalize();
	}
	void add(const double *new_results, int n) {
		int s = rs.size();
		rs.resize(s + n);
		for (int i = 0; i < n; i++) {
			rs[s+i] = new_results[i];
			sum += new_results[i];
			sum_sqr += new_results[i] * new_results[i];
		}
		_normalize();
	}
	void add(double new_result) {
		_add(new_result);
		_normalize();
	}
	void reset() {duplicates = 0; sum = 0; sum_sqr = 0; rs.resize(0);}
	int size() const {return rs.size();}
	int num_duplicates() const {return duplicates;}
	int get_index ( double other_result ) const {return int(0.5+_get_index(other_result));}
	double get_mean() const {if (rs.empty()) return 0; else return sum / rs.size();}
	double get_stddev() const {if (rs.empty()) return 0; double avg = get_mean(), avg_sqr = sum_sqr / rs.size(); return std::sqrt(avg_sqr - avg * avg);}
	double get_percentile ( double other_result ) const;//0 to 1
	double get_result_by_index(int i) const {return rs[i];}
	double get_result_by_percentile(double d) const;
	int get_num_elements_less_than ( double other_result ) const;
	int get_num_elements_greater_than ( double other_result ) const;
	void get_num_elements_less_and_greater ( double other_result, int &num_lower, int &num_higher ) const;
};
void SampleSet::_normalize() {
	std::sort(rs.begin(), rs.end());
//	std::sort<double*>(&rs[0], &rs[rs.size()]);
	_count_duplicates();
}
int SampleSet::get_num_elements_less_than ( double other_result ) const {
	int s = rs.size();
	if (!s) return 0;
	if (rs[s-1] < other_result) return s;
	if (rs[0] >= other_result) return 0;
	int low = 0, high = s-1;
	while (low + 1 < high) {
		int mid = (low + high) >> 1;
		if (rs[mid] < other_result) {
			low = mid;
		}
		else high = mid;
	}
	return low + 1;
//	for (i = 0; i < s && rs[i] < other_result; i++) ;
//	return i;
}
int SampleSet::get_num_elements_greater_than ( double other_result ) const {
	//could use more optimization, but who cares?
	int s = rs.size();
	if (!s) return 0;
	int i;
	if (rs[0] > other_result) return s;
	for (i = 0; i < s && rs[s-1-i] > other_result; i++) ;
	return i;
}
void SampleSet::get_num_elements_less_and_greater ( double other_result, int &num_less, int &num_greater ) const {
	//could use more optimization, but who cares?
	num_less = get_num_elements_less_than(other_result);
	num_greater = get_num_elements_greater_than(other_result);
}
void SampleSet::_count_duplicates() {
	duplicates = 0;
	if (!rs.size()) return;
	double o = rs[0];
	for (int i = 1; i < rs.size(); i++) {
		double n = rs[i];
		if (n == o) duplicates++;
		o = n;
	}
}
double SampleSet::_get_index ( double other_result ) const {
	double &r = other_result;
	int s = rs.size();
	if (!s) return 0;
	int lower, higher;
	get_num_elements_less_and_greater(r, lower, higher);
	if (lower+higher < s) {//exact match and possibly duplicates
		return lower + (s-(lower+higher)-1)/2.0;
	}
	else {//normal case
		if (higher == 0) return s-1;
		if (lower == 0) return 0;
		double a = rs[lower-1], b = rs[lower];
		return lower-1 + (r-a)/(b-a);
	}
}
double SampleSet::get_percentile ( double other_result ) const {
	double &r = other_result;
	int s = rs.size();
	if (!s) return 0;
	int lower, higher;
	get_num_elements_less_and_greater(r, lower, higher);
	double percentile;
	if (lower+higher < s) {//exact match and/or duplicates?
		percentile = (lower + (s-(lower+higher))/2.0) / s;
	}
	else {//normal case
//		return (lower + 0.5) / (s+1);
		if (higher == 0) return 1 - 0.5 / (s+1);
		if (lower == 0) return 0.5 / (s+1);
		double a = rs[lower-1], b = rs[lower];
		percentile = (lower-1 + (r-a)/(b-a)) / (s+1);
	}
	return percentile;
}
double SampleSet::get_result_by_percentile(double percentile) const {
	//lowest distinct percentile is (0.5 / s), highest is ((s-.5) / s)
	//this allows interpolation while preserving equal weighting for all samples
	std::size_t s = rs.size();
	if (!s) _asm int 3;
	double di = s * percentile - 0.5;
	if (di <= 0) return rs[0];
	if (di >= s-1) return rs[s-1];
	int ii = int(di);
	double fi = di - ii;
	return rs[ii] + (rs[ii+1] - rs[ii]) * fi;
}



double ref_p[117] = { 
	0.00001, 0.00002, 0.00005, 0.0001, 0.0002, 0.0005, 0.001, 0.002, 0.005, 
	0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.10, 
	0.11, 0.12, 0.13, 0.14, 0.15, 0.16, 0.17, 0.18, 0.19, 0.20, 
	0.21, 0.22, 0.23, 0.24, 0.25, 0.26, 0.27, 0.28, 0.29, 0.30, 
	0.31, 0.32, 0.33, 0.34, 0.35, 0.36, 0.37, 0.38, 0.39, 0.40, 
	0.41, 0.42, 0.43, 0.44, 0.45, 0.46, 0.47, 0.48, 0.49, 0.50, 
	0.51, 0.52, 0.53, 0.54, 0.55, 0.56, 0.57, 0.58, 0.59, 0.60, 
	0.61, 0.62, 0.63, 0.64, 0.65, 0.66, 0.67, 0.68, 0.69, 0.70, 
	0.71, 0.72, 0.73, 0.74, 0.75, 0.76, 0.77, 0.78, 0.79, 0.80, 
	0.81, 0.82, 0.83, 0.84, 0.85, 0.86, 0.87, 0.88, 0.89, 0.90, 
	0.91, 0.92, 0.93, 0.94, 0.95, 0.96,	0.97, 0.98, 0.99, 
	0.995, 0.998, 0.999, 0.9995, 0.9998, 0.9999, 0.99995, 0.99998, 0.99999
};
void print_ss(const SampleSet &ss, const std::string &name, Uint64 blocks) {
//	std::printf("{\"BCFN-%d/%d\",%7.0f,%5d, %d, {", tbits, 1<<stride_L2, double(Uint64(std::pow(2,length_L2) / 1024)), (int)ss.size(), (int)ss.num_duplicates());
//	for (int i = 0; i < 117; i++) std::printf("%s%+7.3f", i ? "," : "", ss.get_result_by_percentile(ref_p[i]));
//	std::printf("}, %+.4f, %+.4f, %.4f},\n", ss.get_result_by_percentile(0.5), ss.get_mean(), ss.get_stddev());
	std::printf("{\"%s\",%7.0f,%5d, %d, {", name.c_str(), (double)blocks, (long)ss.size(), (long)ss.num_duplicates());
	for (int i = 0; i < 117; i++) std::printf("%s%+7.3f", i ? "," : "", ss.get_result_by_percentile(ref_p[i]));
	std::printf("}, %+.4f, %+.4f, %.4f},\n", ss.get_result_by_percentile(0.5), ss.get_mean(), ss.get_stddev());
}


double fake_bcfn(PractRand::RNGs::vRNG *known_good, int tbits, Uint64 n) {
	PractRand::RNGs::LightWeight::sfc32 rng(known_good);
	int size = 1 << tbits;
	int mask = size - 1;
	Uint32 cur = rng.raw32();
	std::vector<Uint64> table; table.resize(size, 0);
	std::vector<double> probs; probs.resize(size, 1.0/size);
	n = (n + 7) >> 4;
	while (n) {
		Uint32 n32 = Uint32(n);
		if (n32 != n) n32 = 1<<30;
		n -= n32;
		for (; n32 > 0; n32--) {
			table[(cur >>  0) & mask]++;
			table[(cur >>  1) & mask]++;
			table[(cur >>  2) & mask]++;
			table[(cur >>  3) & mask]++;
			table[(cur >>  4) & mask]++;
			table[(cur >>  5) & mask]++;
			table[(cur >>  6) & mask]++;
			table[(cur >>  7) & mask]++;
			table[(cur >>  8) & mask]++;
			table[(cur >>  9) & mask]++;
			table[(cur >> 10) & mask]++;
			table[(cur >> 11) & mask]++;
			table[(cur >> 12) & mask]++;
			table[(cur >> 13) & mask]++;
			table[(cur >> 14) & mask]++;
			table[(cur >> 15) & mask]++;
			cur >>= 16;
			cur |= rng.raw32() << 16;
		}
	}
	int reduced_size = size;
	//if (!level) reduced_size = simplify_prob_table(size, samples / 32, &probs[0], &tempcount[0], true, true);
	double rv = PractRand::Tests::g_test(reduced_size, &probs[0], &table[0]);
	double rn = PractRand::Tests::math_chisquared_to_normal(rv, reduced_size-1);
	return rn;
}
SampleSet fake_bcfn_dist(PractRand::RNGs::vRNG *known_good, int tbits, Uint64 n, Uint32 samples) {
	SampleSet ss;
	for (Uint32 i = 0; i < samples; i++) {
		ss._add(fake_bcfn(known_good, tbits, n));
	}
	ss._normalize();
	return ss;
}
void print_fake_bcfn_dist(int tbits, int stride_L2, double length_L2, int samples) {
	PractRand::RNGs::Polymorphic::hc256 known_good(PractRand::SEED_AUTO);
	SampleSet ss;
	static double chance_skipped[15] = {
		0.0,              //1 bit
		0.5,              //2 bit
		0.375,            //4 bit
		0.2734375,        //8 bit
		0.196380615234375,//16 bit
		0.139949934091419,//32 bit
		0.0993467537479669,//64 bit
		0.0703860921700151,//128 bit
		0.0498191099361402,//256 bit
		0.0352446354858388,//512 bit
		0.0249278058726663,//1 Kbit
		0.0176287723815027,//2 Kbit
		0.0124661853439194,//4 Kbit
		0.0088151932052590,//8 Kbit
		0.0062333780055594//16 Kbit
	};
	int level = stride_L2 + 3;
	double chance = (level <= 15) ? chance_skipped[level] : (chance_skipped[15] * std::pow(0.5, 0.5 * (level-15)));
	chance = 1 - chance;
	ss = fake_bcfn_dist(&known_good, tbits, std::pow(2, length_L2 + 3 - level) * chance - tbits + 1, samples);
	std::printf("{\"BCFN-%d/%d\",%7.0f,%5d, %d, {", tbits, 1<<stride_L2, double(Uint64(std::pow(2,length_L2) / 1024)), (int)ss.size(), (int)ss.num_duplicates());
	for (int i = 0; i < 117; i++) std::printf("%s%+7.3f", i ? "," : "", ss.get_result_by_percentile(ref_p[i]));
	std::printf("}, %+.4f, %+.4f, %.4f},\n", ss.get_result_by_percentile(0.5), ss.get_mean(), ss.get_stddev());
}
void blah_bcfn() {
	for (int n = 1<<0; n <= 1<<24; n<<=2) {
		for (double len = 10; len <= 18; len++) {
			print_fake_bcfn_dist(6, 2,len,n);
			print_fake_bcfn_dist(6, 2,len+std::log(1.5)/std::log(2.0),n);
		}
	}
}
void find_test_distributions() {
	std::time_t start_time = std::time(NULL);
	std::clock_t start_clock = std::clock();

	PractRand::RNGs::Polymorphic::hc256 known_good(PractRand::SEED_AUTO);

	PractRand::RNGs::Polymorphic::efiix32x384 rng(&known_good);

	//Tests::ListOfTests tests = Tests::Batteries::get_expanded_core_tests();
	//Tests::ListOfTests tests(new Tests::DistC6(9,0, 1,0,0));
	//Tests::ListOfTests tests(new Tests::DistC6(6,1, 1,0,0));
	//Tests::ListOfTests tests(new Tests::DistC6(5,2, 1,0,0));
	//Tests::ListOfTests tests(new Tests::DistC6(4,3, 0,0,1));
	Tests::ListOfTests tests(new Tests::DistC6(5,3, 1,0,1));
	//Tests::ListOfTests tests(new Tests::CoupGap());
	//Tests::ListOfTests tests(new Tests::FPF(0,14,6));
	TestManager tman(&rng, &tests, &known_good);

	//Uint64 test_size = 1 << 16;
	//test_size *= Tests::TestBlock::SIZE;

	std::map<std::string,std::map<Uint64,SampleSet> > data;
	Uint64 next_checkpoint = 1;
	for (Uint64 n = 0; n <= 1<<20; n++) {
		if (n == next_checkpoint) {
			enum {CHUNKY = 1 << 9};
			if (next_checkpoint < CHUNKY) next_checkpoint <<= 1; else next_checkpoint += CHUNKY;
			std::printf("\n\n\n\n");
			std::printf("==================================================\n");
			std::printf("checkpoint @ %d\n", int(n) );
			/*if (test_size < 10ull << 20) std::printf("for length = %d KB\n", test_size >> 10);
			else if (test_size < 10ull << 30) std::printf("for length = %d MB\n", test_size >> 20);
			else if (test_size < 10ull << 40) std::printf("for length = %d GB\n", test_size >> 30);
			else std::printf("for length = %d TB\n", test_size >> 40);*/
			std::printf("==================================================\n");
			int test_name_index = 0;
			for (std::map<std::string,std::map<Uint64,SampleSet> >::iterator it = data.begin(); it != data.end(); it++, test_name_index++) {
				std::string name = it->first;
				for (std::map<Uint64,SampleSet>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
					Uint64 length = it2->first;
					//int length_L2 = it2->first;
					SampleSet &ss = it2->second;
					ss._normalize();
					//std::printf("//mean= %f; median= %f; stddev= %f;\n", ss.get_mean(), ss.get_result_by_percentile(0.50), ss.get_stddev());
					std::printf("\t{\"%s\",%6.0f, %d, %d, {", name.c_str(), double(length), (int)ss.rs.size(), (int)ss.num_duplicates());
					double p[] = { 
						0.00001, 0.00002, 0.00005, 0.0001, 0.0002, 0.0005, 0.001, 0.002, 0.005, -1, 
						0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.10, -1, 
						0.11, 0.12, 0.13, 0.14, 0.15, 0.16, 0.17, 0.18, 0.19, 0.20, -1, 
						0.21, 0.22, 0.23, 0.24, 0.25, 0.26, 0.27, 0.28, 0.29, 0.30, -1, 
						0.31, 0.32, 0.33, 0.34, 0.35, 0.36, 0.37, 0.38, 0.39, 0.40, -1, 
						0.41, 0.42, 0.43, 0.44, 0.45, 0.46, 0.47, 0.48, 0.49, 0.50, -1, 
						0.51, 0.52, 0.53, 0.54, 0.55, 0.56, 0.57, 0.58, 0.59, 0.60, -1, 
						0.61, 0.62, 0.63, 0.64, 0.65, 0.66, 0.67, 0.68, 0.69, 0.70, -1, 
						0.71, 0.72, 0.73, 0.74, 0.75, 0.76, 0.77, 0.78, 0.79, 0.80, -1, 
						0.81, 0.82, 0.83, 0.84, 0.85, 0.86, 0.87, 0.88, 0.89, 0.90, -1, 
						0.91, 0.92, 0.93, 0.94, 0.95, 0.96,	0.97, 0.98, 0.99, -1, 
						0.995, 0.998, 0.999, 0.9995, 0.9998, 0.9999, 0.99995, 0.99998, 0.99999, -2
					};
					for (int i = 0; p[i] != -2; i++) {
						if (p[i] == -1) continue;
						if (i) std::printf(",");
						std::printf("%+7.3f", ss.get_result_by_percentile(p[i]));
					}
					std::printf("}, %+7.3f, %+7.3f, %7.3f},\n", ss.get_mean(), ss.get_result_by_percentile(0.50), ss.get_stddev());
					//std::printf("}},\n");

					/*for (int L2 = 0; (2ull << L2) <= ss.rs.size(); L2++) {
						std::printf("//bott.L2:%2d:%+8.3f      ", L2, ss.get_result_by_index( (1ull << L2)-1 ));
						std::printf("topL2:%2d : %+8.3f\n",      L2, ss.get_result_by_index(ss.rs.size() - (1ull << L2)));
					}
					for (int L10 = 1; std::pow(10.0, double(L10)) <= ss.rs.size(); L10++) {
						double p = std::pow(0.1, double(L10));
						std::printf("//%f: %+8.3f      ", p, ss.get_result_by_percentile( p ));
						std::printf("%f: %+8.3f\n",   1-p, ss.get_result_by_percentile( 1-p ));
					}*/
				}
			}
		}
		Uint64 blocks_so_far = 0;
		for (int length_L2 = 10; length_L2 <= 28; length_L2 += 1) {
			if (length_L2 >= 13) {
				Uint64 new_blocks = (5ull << (length_L2-3)) / Tests::TestBlock::SIZE;
				tman.test(new_blocks - blocks_so_far);
				blocks_so_far = new_blocks;
				for (int i = 0; i < tests.tests.size(); i++) {
					std::vector<PractRand::TestResult> results;
					tests.tests[i]->get_results(results);
					for (int j = 0; j < results.size(); j++) data[results[j].name][new_blocks]._add(results[j].get_raw());
				}
			}
			if (length_L2 >= 12) {
				Uint64 new_blocks = (3ull << (length_L2-2)) / Tests::TestBlock::SIZE;
				tman.test(new_blocks - blocks_so_far);
				blocks_so_far = new_blocks;
				for (int i = 0; i < tests.tests.size(); i++) {
					std::vector<PractRand::TestResult> results;
					tests.tests[i]->get_results(results);
					for (int j = 0; j < results.size(); j++) data[results[j].name][new_blocks]._add(results[j].get_raw());
				}
			}
			if (length_L2 >= 13) {
				Uint64 new_blocks = (7ull << (length_L2-3)) / Tests::TestBlock::SIZE;
				tman.test(new_blocks - blocks_so_far);
				blocks_so_far = new_blocks;
				for (int i = 0; i < tests.tests.size(); i++) {
					std::vector<PractRand::TestResult> results;
					tests.tests[i]->get_results(results);
					for (int j = 0; j < results.size(); j++) data[results[j].name][new_blocks]._add(results[j].get_raw());
				}
			}
			if (true) {
				Uint64 new_blocks = (1ull << length_L2) / Tests::TestBlock::SIZE;
				tman.test(new_blocks - blocks_so_far);
				blocks_so_far = new_blocks;
				for (int i = 0; i < tests.tests.size(); i++) {
					std::vector<PractRand::TestResult> results;
					tests.tests[i]->get_results(results);
					for (int j = 0; j < results.size(); j++) data[results[j].name][new_blocks]._add(results[j].get_raw());
				}
			}
		}
		/*for (int i = 0; i < tests.tests.size(); i++) {
			std::vector<PractRand::TestResult> results;
			tests.tests[i]->get_results(results);
			for (int j = 0; j < results.size(); j++) data[results[j].name]._add(results[j].value);
		}*/
		tman.reset();
	}
}

static double _uniformity_transform(double value, double count) {
	return (std::exp(value / count) - std::log(value * count));
}
static double measure_set_uniformity(SampleSet *ss) {
	double count = ss->size()+1;
	double sum = _uniformity_transform(ss->get_result_by_index(0), count) + _uniformity_transform(1 - ss->get_result_by_index(ss->size()-1), count);
	for (int j = 1; j < ss->size(); j++) {
		sum += _uniformity_transform(ss->get_result_by_index(j) - ss->get_result_by_index(j-1), count);
	}
	return (sum - count * 1.578 + .3) / std::sqrt(count);
}
static void calibrate_set_uniformity(SampleSet *calib, int n, PractRand::RNGs::vRNG *known_good) {
	for (int i = 0; i < 1ull<<20; i++) {
		SampleSet tmp;
		for (int j = 0; j < n; j++) tmp._add(known_good->randlf());
		tmp._normalize();
		calib->_add(measure_set_uniformity(&tmp));
	}
	calib->_normalize();
}
static void simple_chisquare_test( PractRand::RNGs::vRNG *known_good ) {
	enum {SIZE = 1<<4};
	Uint64 counts[SIZE];
	double probs[SIZE];
	SampleSet ssA, ssB;
	enum {N = 8};
	for (int x = 0; x < SIZE; x++) probs[x] = 1.0 / SIZE;
	for (int i = 0; i < N; i++) {
		for (int x = 0; x < SIZE; x++) counts[x] = 0;
		for (int x = 0; x < SIZE*100; x++) counts[known_good->randi(SIZE)]++;
		double r = PractRand::Tests::g_test(SIZE, &probs[0], &counts[0]);
		double p = Tests::math_chisquared_to_pvalue(r, SIZE-1);
		double r2 = Tests::math_chisquared_to_normal(r, SIZE-1);
		double p2 = Tests::math_normaldist_to_pvalue(r2);
		ssA._add(p);
		ssB._add(p2);
	}
	ssA._normalize();
	ssB._normalize();
	SampleSet calib;
	calibrate_set_uniformity(&calib, N, known_good);
	std::printf("simple_chisquare_test: %.4f  %.4f\n", calib.get_percentile(measure_set_uniformity(&ssA)), calib.get_percentile(measure_set_uniformity(&ssB)));
}
void verify_test_distributions() {
	std::time_t start_time = std::time(NULL);
	std::clock_t start_clock = std::clock();

	PractRand::RNGs::Polymorphic::efiix64x384 known_good(PractRand::SEED_AUTO);

	PractRand::RNGs::Polymorphic::hc256 rng(PractRand::SEED_AUTO);

	//simple_chisquare_test(&known_good);

	//Tests::ListOfTests tests = Tests::Batteries::get_core_tests();
	//Tests::ListOfTests tests = Tests::Batteries::get_expanded_core_tests();
	//Tests::ListOfTests tests(new Tests::FPF(5, 14, 6));
	//Tests::ListOfTests tests(new Tests::DistC6(9,0, 1,0,0));
	Tests::ListOfTests tests(new Tests::DistC6(6,1, 1,0,0));
	//Tests::ListOfTests tests(new Tests::DistC6(5,2, 1,0,0));
	//Tests::ListOfTests tests(new Tests::DistC6(4,3, 0,0,1));
	//Tests::ListOfTests tests(new Tests::DistC6(5,3, 1,0,1));
	TestManager tman(&rng, &tests, &known_good);

	std::map<std::string,std::map<int,SampleSet> > data;
	Uint64 next_checkpoint = 1;
	for (Uint64 n = 0; n <= 1<<20; n++) {
		if (n == next_checkpoint) {
			SampleSet calib;
			calibrate_set_uniformity(&calib, n, &known_good);
			enum {CHUNKY = 1 << 8};
			if (next_checkpoint < CHUNKY) next_checkpoint <<= 1; else next_checkpoint += CHUNKY;
			std::printf("\n\n\n\n");
			std::printf("==================================================\n");
			std::printf("checkpoint @ %d\n", int(n) );
			/*if (test_size < 10ull << 20) std::printf("for length = %d KB\n", test_size >> 10);
			else if (test_size < 10ull << 30) std::printf("for length = %d MB\n", test_size >> 20);
			else if (test_size < 10ull << 40) std::printf("for length = %d GB\n", test_size >> 30);
			else std::printf("for length = %d TB\n", test_size >> 40);*/
			std::printf("==================================================\n");
			int test_name_index = 0;
			for (std::map<std::string,std::map<int,SampleSet> >::iterator it = data.begin(); it != data.end(); it++, test_name_index++) {
				std::string name = it->first;
				for (std::map<int,SampleSet>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
					int length_L2 = it2->first;
					SampleSet &ss = it2->second;
					ss._normalize();
					if (ss.num_duplicates()) continue;
					std::printf("\n\n name=\"%s\"; length_L2=%d;\n", name.c_str(), length_L2);
					std::printf("total= %d; duplicates= %d;\n", ss.rs.size(), ss.num_duplicates());
					if (!ss.num_duplicates()) {
						double sum = measure_set_uniformity(&ss);
						double p = calib.get_percentile(sum);
						std::printf("blah = %f of (%f:%f:%f)\n", sum, calib.get_result_by_percentile(.1), calib.get_result_by_percentile(.5), calib.get_result_by_percentile(.9));
						std::printf("p = %f\n", p);
						if (fabs(p-.5) > 0.499) std::printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
					}
					std::printf("\n");
				}
			}
		}
		Uint64 blocks_so_far = 0;
		for (int length_L2 = 17; length_L2 <= 21; length_L2 += 1) {
			//data["fred"][length_L2]._add(known_good.randf(0.95));continue;
			Uint64 new_blocks = (1ull << length_L2) / Tests::TestBlock::SIZE;
			tman.test(new_blocks - blocks_so_far);
			blocks_so_far = new_blocks;
			for (int i = 0; i < tests.tests.size(); i++) {
				std::vector<PractRand::TestResult> results;
				tests.tests[i]->get_results(results);
				for (int j = 0; j < results.size(); j++) data[results[j].name][length_L2]._add(results[j].get_pvalue());
			}
		}
		tman.reset();
	}
}

struct Data {
	Uint64 count;
	double vec[117];
	Data() : count(0) {
		for (int i = 0; i < 117; i++) vec[i] = 0;
	}
};
std::map<std::string,std::map<int, Data > > data;

void print_data() {
	for (std::map<std::string,std::map<int,Data > >::iterator it = data.begin(); it != data.end(); it++) {
		std::string name = it->first;
		for (std::map<int,Data >::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
			int length_L2 = it2->first;
			const Data &d = it2->second;
			std::printf("{ \"%s\", 1ull << (%d - 10), %.0f, {", name.c_str(), length_L2, (double)d.count );
			for (int i = 0; i < 117; i++) {
				std::printf("%+6.3f", d.vec[i]);
				if (i != 116) std::printf(",");
			}
			std::printf("}},\n");
		}
	}
}



int main(int argc, char **argv) {
	PractRand::initialize_PractRand();
	PractRand::self_test_PractRand();

	//blah_bcfn();
	//find_test_distributions();
	verify_test_distributions();
	//print_data();


	return 0;
}




