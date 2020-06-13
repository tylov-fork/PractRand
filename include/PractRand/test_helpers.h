
namespace PractRand {
	//categories = old # of entries in tables
	//return value = new # of entries in tables
	//combines adjacent entries
	//N should be on the order of the sum of all counts (but, constant)
	//low N will combine many probs, high N fewer combinations
	//if aggressive is true, it will treat N as a hard limit on how low probabilities can be
	//otherwise, it will treat it as a soft limit
	//linear combines only adjacent entries; non-linear is not yet implemented
	int simplify_prob_table ( unsigned long categories, double N, double *prob_table, Uint64 *counts, bool linear, bool aggressive );
	double chi_squared_test ( unsigned long categories, const double *prob_table, const Uint64 *counts );
	double g_test ( unsigned long categories, const double *prob_table, const Uint64 *counts );
	double math_chisquared_to_pvalue ( double chisquared, double DoF );
	double math_chisquared_to_normal ( double chisquared, double DoF );
	double math_pvalue_to_chisquared ( double pvalue, double DoF );

	//long double gap_probs( int first, int last, long double baseprob = (255.0 / 256.0) );
}
