#include <string>
//#include <ostream>
//#include <sstream>
#include <vector>
//#include <list>
//#include <set>
//#include <map>
#include <cmath>
#include "PractRand/config.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/tests.h"
#include "PractRand/test_helpers.h"
#include "PractRand/tests/gap16.h"
#include "PractRand/tests/DistC6.h"
#include "PractRand/tests/BCFN.h"

namespace PractRand {
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
		return v;
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
		return sum;
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
		double f = 1;
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
		issue_error();
	}
	static double math_gamma_function ( double a ) {
		if (a == 0.5) return std::sqrt(3.14159265358979);
		if (a == 1) return 1;
		if (a == 2) return 1;
		if (a > 1) return math_gamma_function(a-1) * (a-1);
		issue_error();
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
			return sum;
		}
		if (a == 0.5) return sqrt(3.14159265358979) * (1 - math_erf(sqrt(x)));
	//	if (a == 0.0 && x>0) return -math_exponent_integral(-x)
		if (a > 1) return (a-1) * math_upper_incomplete_gamma( a-1, x ) + pow(x, a-1) * ::exp(-x);
		issue_error();
	}
	double math_chisquared_to_pvalue ( double chisquared, double DoF ) {
		long double p = math_lower_incomplete_gamma(DoF/2,chisquared/2) / math_gamma_function(DoF/2);
		if (p < 0) p = 0;
		if (p > 1) p = 1;
		return p;
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
}


