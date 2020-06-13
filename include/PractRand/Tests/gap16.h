namespace PractRand {
	namespace Tests {
		class Gap16 : public TestBaseclass {
		public:
			virtual void init( PractRand::RNGs::vRNG *known_good );
			virtual std::string get_name() const;// {return std::string("Gap16");}
			//virtual double get_result();
			//virtual double result_to_pvalue ( Uint64 blocks, double r );
			virtual void get_results ( std::vector<TestResult> &results );

			virtual void test_blocks(TestBlock *data, int numblocks);
		protected:
			enum { 
				SIZE1 = 1<<18, //handles the common case, tracks sets of (1 << SET1_SHIFT) gaps
				SIZE2 = 1<<17, //handles the uncommon cases, tracks each set of (1 << SET2_SHIFT) gaps
				SIZE3 = 1<<10, //handles the rare cases, tracks a number of sets which double in size after the second one - the smallest is SET3_BASE in size
				SET1_SHIFT = 1,
				SET2_SHIFT = 4,
				SET3_SHIFT = 8
			};
			FixedSizeCount<Uint8, SIZE1 + SIZE2 + SIZE3> counts;
			bool autofail;
			Sint32 last[65536];
		};
	}//Tests
}//PractRand
