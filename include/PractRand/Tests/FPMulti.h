namespace PractRand {
	namespace Tests {
		class FPMulti : public TestBaseclass {
			/*
				can a stride shorter than 64 bits really work for this?  it works fine for FPF, but... other subtests may be a lot more sensitive to the resulting issues

			subtests:
				frequency - same as FPF
					intraplatter - easy, marginally worthwhile
					individual FP values - good stuff
					exponent only - easy, probably a good idea if only as a sanity check for everything else
					EVALUATION: is this really the right place to do this?  it prefers a smaller stride than gap tests, right?
				gap - 
					intraplatter - probably worthless? and would you index by platter hits or by global position?
					individual FP values - probably worthwhile
					exponent only - may be redundant with individual FP values, may not be
					note that space for recording individual spacings is too expensive, so probably we calculate the geometric mean of gaps & expected value, in the spirit of Mauer's Universal Test
					EVALUATION: good idea
				coupon collecting - 
					intraplatter - easy, and it might even be worthwhile.  probably not really, but if they're giving rotating results on FP frequency, this could keep them honest.  Maybe.
					individual FP values - ...I don't think a coupon test on assympetric probs is a good idea, harder to evaluate at the very least
					exponent only - ...I don't think a coupon test on assympetric probs is a good idea, harder to evaluate at the very least
					EVALULATION: meh... could be worse I guess
				???
					average nears - keep a copy of the bytes surrounding the previous occurance.  reserve space for another equal size block, initialized those those around the 1st occurance and updated by majority vote on a per-bit basis. record the average hamming distance from it
			*/
		protected:
			//typedef Uint32 COUP_WORD;//must be 4 or 8 bytes
			enum {
				EXP_BITS = 6,
				BASE_SIG_BITS = 9,
				MAX_EXP = 64 - BASE_SIG_BITS,

				//FREQ_SIG_BITS = 12,
				//COUP_SIG_BITS = 12,
				GAP_SIG_BITS = 9,

				//COUP_WORD_SIZE = 8 * sizeof(COUP_WORD),
				//COUP_WORD_SIZE_L2 = sizeof(COUP_WORD) == 4 ? 5 : 6,
				//COUP_MASK_SIZE = 1 << (COUP_SIG_BITS - COUP_WORD_SIZE_L2),
			};
			//static unsigned long count_leading_zeroes32( Uint32 value );
			//const int stride_bits_L2, skip_platters;
			bool autofail;
			struct Platter {
				Uint64 total_count;//total number of times this exponent has occurred

				//FixedSizeCount<Uint16, 1 << (SEQ_SIG_BITS * SEQ_N)> seq_count;
				//Uint32 seq_current;

				Uint64 gap_global_history[1 << GAP_SIG_BITS];//4 kilobytes at current settings
				double gap_product;
				double gap_expected_inverse;
				Sint64 gap_product_extracted_L2;
				Uint64 gap_hits;
				double gap_warmed_up;
				//Uint32 history1[1 << GAP_SIG_BITS];//relative to this platter only
				//Uint64 history2[1 << GAP_SIG_BITS];//global - implementation is not efficient
				//Uint64 total_count;
				//double gap_sum1;//our gap test is actually closer to Maurer's Universal Statitical Test, though they're basically the same thing
				//Uint64 gap_negative_count1;
				//double gap_sum2;//using history2 instead of history1
				//Uint64 gap_negative_count2;
				//double expected_gap2;

				//FixedSizeCount<Uint16, 1 << FREQ_SIG_BITS> freq_count;
				//COUP_WORD coup_mask[COUP_MASK_SIZE];
				//Uint64 last_coup;
				//FixedSizeCount<Uint16, 1024> coup_count;
				void reset(PractRand::RNGs::vRNG *known_good, unsigned long e);
			};
			Platter platter[MAX_EXP + 1];
			void process(Uint64 position, unsigned long exp, unsigned long sig);
		public:
			FPMulti();// (int stride_bits_L2_ = 6, int skip_platters_ = 6);
			virtual void init(PractRand::RNGs::vRNG *known_good);
			virtual void deinit();
			virtual std::string get_name() const;
			virtual void get_results(std::vector<TestResult> &results);

			virtual void test_blocks(TestBlock *data, int numblocks);
		};
	}//Tests
}//PractRand
