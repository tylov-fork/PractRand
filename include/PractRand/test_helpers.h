
namespace PractRand {
	namespace Tests {
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
		double math_normaldist_to_pvalue ( double normal );
		double math_pvalue_to_normaldist ( double pvalue );
		//long double gap_probs( int first, int last, long double baseprob = (255.0 / 256.0) );

		int count_bits8 (Uint8 a);
		int count_bits16(Uint16 a);
		int count_bits32(Uint32 a);
		int count_bits64(Uint64 a);

		Uint8  reverse_bits8 (Uint8);
		Uint16 reverse_bits16(Uint16);
		Uint32 reverse_bits32(Uint32);
		Uint64 reverse_bits64(Uint64);

		template<typename LowIntType, int size>
		class FixedSizeCount {
			LowIntType low[size];
			Uint64 high[size];
		public:
			int get_size() {return size;}
			void reset_counts() {
				for (int i = 0; i < size; i++) low[i] = 0;
				for (int i = 0; i < size; i++) high[i] = 0;
			}
			FixedSizeCount() {reset_counts();}
			void increment(int index) {if (!++low[index]) high[index] += 1ull << (8*sizeof(LowIntType));}
			const Uint64 &operator[] (int index) {
				high[index] += low[index];
				low[index] = 0;
				return high[index];
			}
			void flush() {
				for (int i = 0; i < size; i++) {
					high[i] += low[i];
					low[i] = 0;
				}
			}
			const Uint64 *get_array() {flush(); return &high[0];}
		};
		template<typename LowIntType>
		class VariableSizeCount {
			LowIntType *low;
			Uint64 *high;
			int size;
		public:
			int get_size() {return size;}
			void reset_counts() {
				for (int i = 0; i < size; i++) low[i] = 0;
				for (int i = 0; i < size; i++) high[i] = 0;
			}
			void set_size(int size_) {
				size = size_;
				low = (LowIntType*)std::realloc(low, sizeof(LowIntType) * size);
				high = (Uint64*)std::realloc(high, sizeof(Uint64) * size);
				reset_counts();
			}
			VariableSizeCount() : size(0), low(NULL), high(NULL) {}
			VariableSizeCount(int size_) : size(0), low(NULL), high(NULL) {set_size(size_);}
			void increment(int index) {if (!++low[index]) high[index] += 1ull << (8*sizeof(LowIntType));}
			const Uint64 &operator[] (int index) {
				high[index] += low[index];
				low[index] = 0;
				return high[index];
			}
			void flush() {
				for (int i = 0; i < size; i++) {
					high[i] += low[i];
					low[i] = 0;
				}
			}
			const Uint64 *get_array() {flush(); return &high[0];}
		};

		struct RawTestCalibrationData_117 {
			//for use on tests that produce (very) roughly a normal distribution
			//should be based upon at least 512 samples
			const char *name;  //e.g. "Gap-16:A"
			Uint64 blocks;     //e.g. 32 for 32 KB
			Uint64 num_samples;//e.g. 65536 for that many results of known good RNGs used to construct raw_table
			Uint64 num_duplicates;

			double table[117];
			double median;//redundant
			double mean;
			double stddev;

			static const double ref_p[117];

			double get_median_sample() const {return table[49 + 9];}

			//linear interpolation
			double sample_to_index(double sample) const;
			double index_to_sample(double index) const;
			static double pvalue_to_index(double pvalue);
			static double index_to_pvalue(double index);
			double pvalue_to_sample(double pvalue) const {return index_to_sample(pvalue_to_index(pvalue));}
			double sample_to_pvalue(double sample) const {return index_to_pvalue(sample_to_index(sample));}
		};
	}//Tests
}//PractRand
