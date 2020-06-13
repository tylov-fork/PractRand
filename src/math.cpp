#include <string>
//#include <ostream>
//#include <sstream>
#include <vector>
//#include <list>
//#include <set>
//#include <map>
#include <cmath>
#include <cstdlib>

#include "PractRand/config.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/rng_internals.h"

namespace PractRand {
	namespace Internals {
		static void add_128 ( const Uint32 *a, const Uint32 *b, Uint32 *result ) {
			Uint64 tmp = 0;
			for (int x = 0; x < 4; x++) {
				tmp += a[x];
				tmp += b[x];
				result[x] = Uint32(tmp);
				tmp >>= 32;
			}
		}
		static void multiply_128 ( const Uint32 *a, const Uint32 *b, Uint32 *result ) {
			Uint32 buffer[4];
			Uint64 current = 0, carry = 0, tmp;
			for (int x = 0; x < 4; x++) {
				for (int y = 0; y < x; y++) {
					tmp = Uint64(a[y]) * b[x-1-y];
					carry += tmp >> 32;
					current += Uint32(tmp);
				}
				buffer[x] = Uint32(current);
				current = (current >> 32) + carry;
				carry = 0;
			}
			for (int i = 0; i < 4; i++) result[i] = buffer[i];
		}
		static void convert128_64to32( Uint64 low, Uint64 high, Uint32 *destination ) {
			destination[0] = Uint32(low);  destination[1] = Uint32(low >> 32);
			destination[2] = Uint32(high); destination[3] = Uint32(high >> 32);
		}
		void fast_forward_lcg128 ( Uint64 how_far_low, Uint64 how_far_high, Uint64 &value_low, Uint64 &value_high, Uint64 mul_low, Uint64 mul_high, Uint64 add_low, Uint64 add_high ) {
			Uint32 value[4], mul[4], add[4], tmp[4];
			convert128_64to32(value_low, value_high, value);
			convert128_64to32(mul_low, mul_high, mul);
			convert128_64to32(add_low, add_high, add);
			while (1) {
				if (how_far_low & 1) {
					multiply_128(value, mul, value);
					add_128(value, add, value);
					//val = val * mul + add;
				}
				how_far_low = (how_far_low >> 1) | (how_far_high << 63);
				how_far_high >>= 1;
				if (how_far_low == 0 && how_far_high == 0) break;
				multiply_128(add, mul, tmp);
				add_128(add, tmp, add);
				//add = add * mul + add;
				multiply_128(mul, mul, mul);
				//mul = mul * mul;
			}
			value_low  = value[0] | (Uint64(value[1]) << 32);
			value_high = value[2] | (Uint64(value[3]) << 32);
			return;
		}
		Uint64 fast_forward_lcg64 ( Uint64 how_far, Uint64 val, Uint64 mul, Uint64 add ) {
			while (1) {
				if (how_far & 1) val = val * mul + add;
				how_far >>= 1;
				if (how_far == 0) break;
				add = add * mul + add;
				mul = mul * mul;
			}
			return val;
		}
		Uint32 fast_forward_lcg32 ( Uint32 how_far, Uint32 val, Uint32 mul, Uint32 add ) {
			while (1) {
				if (how_far & 1) val = val * mul + add;
				how_far >>= 1;
				if (how_far == 0) break;
				add = add * mul + add;
				mul = mul * mul;
			}
			return val;
		}
		/*static Uint64 rewind_lcg64 ( Uint64 how_far, Uint64 val, Uint64 mul, Uint64 add ) {
			return fast_forward_lcg64( ~how_far + 1, val, mul, add );
		}
		static Uint32 pow_mod_N32(Uint32 x, Uint32 pow, Uint32 mod) {
			if (x <= 1) return x;
			if (pow == 0) return 1;
	
			pow -= 1;
			if (pow == 0) return x;

			Uint64 mul = x;

			while (1) {
				if (pow & 1) x = Uint32((x * mul) % mod);
				pow >>= 1;
				if (!pow) return x;
				mul = (mul * mul) % mod;
			}
		}*/
		Uint32 fast_forward_lcg32c ( Uint32 how_far, Uint32 val, Uint32 mul, Uint32 add, Uint32 mod ) {
			if (val >= mod) val %= mod;
			if (mul >= mod) mul %= mod;
			if (add >= mod) add %= mod;
			if (how_far >= mod) how_far %= mod;
			while (1) {
				if (how_far & 1) {
					val = Uint32((Uint64(val) * mul + add) % mod);
				}
				how_far >>= 1;
				if (how_far == 0) break;
				add = Uint32((Uint64(add) * mul + add) % mod);
				mul = Uint32((Uint64(mul) * mul) % mod);
			}
			return val;
		}


		/*XorshiftMatrix::XorshiftMatrix( int size_, bool identity ) {
			size = size_;
			bits.resize(size * size, false);
			if (identity) for (int i = 0; i < size; i++) set(i,i,true);
		}
		void XorshiftMatrix::apply(const std::vector<bool> &input, std::vector<bool> &output) {
			if (input.size() != size) issue_error();
			output.resize(size);
			for (int i = 0; i < size; i++) {
				bool r = false;
				for (int j = 0; j < size; j++) r ^= input[j] & get(j, i);
				output[i] = r;
			}
		}
		bool XorshiftMatrix::operator==(const XorshiftMatrix &other) const {
			return size == other.size && bits == other.bits;
		}
		XorshiftMatrix XorshiftMatrix::operator*(const XorshiftMatrix &other) const {
			XorshiftMatrix rv(size, false);
			if (other.size != size) issue_error();
			for (int i = 0; i < size; i++) {
				for (int j = 0; j < size; j++) {
					bool r = false;
					for (int k = 0; k < size; k++) {
						r ^= get(j, k) & other.get(k,i);
					}
					rv.set(j, i, r);
				}
			}
			return rv;
		}
		XorshiftMatrix XorshiftMatrix::exponent(Uint64 exponent_value) const {
			XorshiftMatrix rv(size, true), tmp(*this);
			if (!exponent_value) return rv;
			while (true) {
				if (exponent_value & 1) rv = rv * tmp;
				exponent_value >>= 1;
				if (!exponent_value) break;
				tmp = tmp * tmp;
			}
			return rv;
		}
		XorshiftMatrix XorshiftMatrix::exponent2Xminus1(Uint64 x) const {
			//exponent(2**X-1), X may be >=64
			XorshiftMatrix rv(size, true);
			while (x) {
				rv = rv * rv * *this;
				x--;
			}
			return rv;
		}
		bool XorshiftMatrix::verify_period_factorization(const std::vector<Uint64> &factors) const {
			for (size_t i = 0; i < factors.size(); i++) {
				XorshiftMatrix tmp = *this;
				for (size_t j = 0; j < factors.size(); j++) {
					if (i != j) tmp = tmp.exponent(factors[j]);
				}
				bool is_identity = (tmp * *this) == *this;
				if (is_identity != (i == factors.size())) return false;
			}
			return true;
		}*/
	}
	namespace Tests {
		//categories = old # of entries in tables
		//return value = new # of entries in tables
		//combines adjacent entries
		//N should be on the order of the sum of all counts (but, constant)
		//low N will combine many probs, high N fewer combinations
		//if aggressive is true, it will treat N as a hard limit on how low probabilities can be
		//otherwise, it will treat it as a soft limit
		//linear combines only adjacent entries; non-linear is not yet implemented
		int simplify_prob_table ( unsigned long categories, double N, double *prob_table, Uint64 *counts, bool linear, bool aggressive ) {
			if (N < 2.0) N = 2.0;
			double E = 1.0 / N;
			int reduced_size = categories;
			if (!linear) {
				issue_error();
				//not yet implemented
			}
			else {//linear
				double E2 = aggressive ? 0.5 : E;
				for (int i = 0; i < 100; i++) {
					int combined = 0;
					for (int j = 0; j < reduced_size; j++) {
						double below = 9;
						double above = 9;
						if (j > combined) below = prob_table[j-combined-1];
						if (j < reduced_size-1) above = prob_table[j+1];
						double other = (below < above) ? below : above;
						if ((prob_table[j] < E) && (other < E2)) {
							if (below < above) {
								prob_table[j-combined-1] += prob_table[j];
								counts[j-combined-1] += counts[j];
								combined ++;
							}
							else {
								prob_table[j-combined] = prob_table[j] + prob_table[j+1];
								counts[j-combined] = counts[j] + counts[j+1];
								combined ++;
								j++;
							}
						}
						else {
							prob_table[j-combined] = prob_table[j];
							counts[j-combined] = counts[j];
						}
					}
					reduced_size -= combined;
					if (!combined) break;
				}
			}
			return reduced_size;
		}


		double chi_squared_test ( unsigned long categories, const double *prob_table, const Uint64 *counts ) {
			unsigned long i;
			long double sum = 0, v = 0;

			for (i=0; i<categories; ++i) {
				sum += (long double) counts[i];
			}
			for (i=0; i<categories; ++i)
			{
				long double expected = sum * prob_table[i];
				long double diff = ((long double)counts[i]) - expected;
				diff = fabs(diff) - 0.5;
				v += (diff*diff)/expected;
			}
		//	double normal = (V-(categories-1))/sqrt((double)(categories-1));
			return (double)v;
		}
		double g_test ( unsigned long categories, const double *prob_table, const Uint64 *counts ) {
			Uint64 total64 = 0;
			for (unsigned long i = 0; i < categories; i++) total64 += counts[i];
			long double total = (long double)total64;
			long double sum = 0;
			for (unsigned long i = 0; i < categories; i++) {
				long double expected = total * prob_table[i];
				long double observed = (long double)counts[i];
				if (observed) sum += 2 * observed * std::log(observed / expected);
			}
			return (double)sum;
		}

		static double math_factorial ( double a ) {
			//only an aproximation, but a decent one
			if (!a) return 1;
			static double halfL2Pi = log(3.14159265358979 * 2)/2;
			static double halfLPi = log(3.14159265358979)/2;
			double L = log(a);
			double r = a * (L - 1) + log(a * (1 + 4 * a * (1 + 2 * a)))/6 + halfLPi;
		//	double r = a * (L - 1) + L/2 + halfL2Pi;
			return exp(r);
		}
		static double math_erf ( double a ) {
			double scale = 2 / std::sqrt(3.14159265358979);
			double a2 = a*a;
			double x = a2*a;
			//double f = 1;
			double r = a - x / 3;
			int i = 1;
			while (x > 0.000000000000000001) {
				x *= a2;
				x /= i*2;
				r += x / (4.0 * i + 1);
				x *= a2;
				x /= i*2+1;
				r -= x / (4.0 * i + 3);
				i++;
			}
			return scale * r;
		}
		static double math_inverse_erf ( double x ) {
			if (x < 0) return -math_inverse_erf(-x);
			if (x > 1) {
				issue_error("inverse_erf: invalid input\n");
			}
			if (!x) return 0;
			double max = 0.5;
			while (math_erf(max) < x) {
				max *= 2;
				if (max > 999999) return max;
			}
			double min = 0;
			double emin = 0, emax = math_erf(max);
			while (1) {
				double mid = (min+max)/2;
				if (emin == emax) return mid;
				double emid = math_erf(mid);
				if (emid < x) {
					if (mid == min) return mid;
					emin = emid;
					min = mid;
				}
				else if (emid > x) {
					if (mid == max) return mid;
					emax = emid;
					max = mid;
				}
				else {
					return mid;
				}
			}
		}
		static double math_lower_incomplete_gamma ( double a, double x ) {
			if (a == 1) return 1 - std::exp(-x);
			if (a == 0.5) return std::sqrt(3.14159265358979) * math_erf(std::sqrt(x));
			if (a > 1) return (a-1) * math_lower_incomplete_gamma( a-1, x ) - std::pow(x, a-1) * std::exp(-x);
			issue_error();return -1;
		}
		static double math_gamma_function ( double a ) {
			if (a == 0.5) return std::sqrt(3.14159265358979);
			if (a == 1) return 1;
			if (a == 2) return 1;
			if (a > 1) return math_gamma_function(a-1) * (a-1);
			issue_error();return -1;
		}
		double math_upper_incomplete_gamma ( double a, double x ) {
			if (a == 1) return std::exp(-x);
			if (fabs(floor(a+.5)-a) <= 0.00000000001) {
				if (!x) return math_factorial(a-1);
				int max = int(floor(a+.5) - 1);
				long double sum = 0;
				long double inv = 1;
				for (int i = 0; i < max; i++) {
					sum += std::pow(x,i) * inv;
					inv /= (i+1);
				}
				return (double)sum;
			}
			if (a == 0.5) return sqrt(3.14159265358979) * (1 - math_erf(sqrt(x)));
		//	if (a == 0.0 && x>0) return -math_exponent_integral(-x)
			if (a > 1) return (a-1) * math_upper_incomplete_gamma( a-1, x ) + pow(x, a-1) * ::exp(-x);
			issue_error();return -1;
		}
		double math_chisquared_to_pvalue ( double chisquared, double DoF ) {
			long double p = math_lower_incomplete_gamma(DoF/2,chisquared/2) / math_gamma_function(DoF/2);
			if (p < 0) p = 0;
			if (p > 1) p = 1;
			return (double)p;
		}
		double math_chisquared_to_normal ( double chisquared, double DoF ) {
			return ( chisquared - DoF ) / std::sqrt(DoF);
		}
		double math_pvalue_to_chisquared ( double pvalue, double DoF ) {
			double chisquared = 1.0;
			double step = 4.0;
			int extra = 0;
			while (math_chisquared_to_pvalue(chisquared, DoF) > pvalue) chisquared /= step;
			while (extra < 10) {
				if (step <= 1.0000001) extra ++;
				step = sqrt(step);
				while (math_chisquared_to_pvalue(chisquared, DoF) < pvalue) chisquared *= step;
				chisquared /= step;
			}
			return chisquared;
		}

		//long double gap_probs( int first, int last, long double baseprob = (255.0 / 256.0) );

		static const Uint8 reverse_table[256] = {
		//	  0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15
			0  , 128,  64, 192,  32, 160,  96, 224,  16, 144,  80, 208,  48, 176, 112, 240,//0
			8  , 136,  72, 200,  40, 168, 104, 232,  24, 152,  88, 216,  56, 184, 120, 248,//16
			4  , 132,  68, 196,  36, 164, 100, 228,  20, 148,  84, 212,  52, 180, 116, 244,//32
			12 , 140,  76, 204,  44, 172, 108, 236,  28, 156,  92, 220,  60, 188, 124, 252,//48
			2  , 130,  66, 194,  34, 162,  98, 226,  18, 146,  82, 210,  50, 178, 114, 242,//64
			10 , 138,  74, 202,  42, 170, 106, 234,  26, 154,  90, 218,  58, 186, 122, 250,//80
			6  , 134,  70, 198,  38, 166, 102, 230,  22, 150,  86, 214,  54, 182, 118, 246,//96
			14 , 142,  78, 206,  46, 174, 110, 238,  30, 158,  94, 222,  62, 190, 126, 254,//112
			1  , 129,  65, 193,  33, 161,  97, 225,  17, 145,  81, 209,  49, 177, 113, 241,//0+128
			9  , 137,  73, 201,  41, 169, 105, 233,  25, 153,  89, 217,  57, 185, 121, 249,//16+128
			5  , 133,  69, 197,  37, 165, 101, 229,  21, 149,  85, 213,  53, 181, 117, 245,//32+128
			13 , 141,  77, 205,  45, 173, 109, 237,  29, 157,  93, 221,  61, 189, 125, 253,//48+128
			3  , 131,  67, 195,  35, 163,  99, 227,  19, 147,  83, 211,  51, 179, 115, 243,//64+128
			11 , 139,  75, 203,  43, 171, 107, 235,  27, 155,  91, 219,  59, 187, 123, 251,//80+128
			7  , 135,  71, 199,  39, 167, 103, 231,  23, 151,  87, 215,  55, 183, 119, 247,//96+128
			15 , 143,  79, 207,  47, 175, 111, 239,  31, 159,  95, 223,  63, 191, 127, 255,//112+128
		};
		Uint8 reverse_bits8(Uint8 a) {return reverse_table[a];}
		Uint16 reverse_bits16(Uint16 a) {return reverse_bits8 (a >>  8) + (Uint16(reverse_bits8 (Uint8 (a)))<<8);}
		Uint32 reverse_bits32(Uint32 a) {return reverse_bits16(a >> 16) + (Uint32(reverse_bits16(Uint16(a)))<<16);}
		Uint64 reverse_bits64(Uint64 a) {return reverse_bits32(a >> 32) + (Uint64(reverse_bits32(Uint32(a)))<<32);}
	}//Tests
}//PractRand


