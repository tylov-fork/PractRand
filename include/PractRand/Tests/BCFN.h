namespace PractRand {
	namespace Tests {
		class BCFN : public TestBaseclass {
		protected:
			enum { LEVELS = 40 };
			enum { BASE_LEVEL = 0};
			VariableSizeCount<Uint16> counts[LEVELS];
			bool autofail;
			long mask[LEVELS];
			long cur[LEVELS];
			bool even[LEVELS];//<4 is never used?
			long leftovers[LEVELS];//<4 is never used? <10 not strictly needed?
			long total[LEVELS];//<4 isn't updated
			long warmup[LEVELS];//<4 is never used?
			long tbits;
			long unitsL2;
			Uint64 blocks;
		public:
			BCFN(int unitsL2_ = 0, int tbits_ = 10);
			virtual void init( PractRand::RNGs::vRNG *known_good );
			virtual void deinit( );
			virtual std::string get_name() const;
			virtual double get_result();
			virtual double result_to_pvalue ( Uint64 blocks, double r );

			virtual void test_blocks(TestBlock *data, int numblocks);
			void handle_high_levels ( int level, int bits );
		};
	}//Tests
}//PractRand