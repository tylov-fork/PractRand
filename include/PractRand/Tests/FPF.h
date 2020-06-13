namespace PractRand {
	namespace Tests {
		class FPF : public TestBaseclass {
		protected:
			enum {
				COUNT_LEADING_ZEROES_TABLE_BITS = 8
				,COUNT_LEADING_ZEROES_TABLE_SIZE = 1 << COUNT_LEADING_ZEROES_TABLE_BITS,
				MAX_SIG_BITS = 16,
//				DIRECTION = 0 //0 = low to high, 1 = high to low
			};
			static Uint8 count_leading_zeroes_table[COUNT_LEADING_ZEROES_TABLE_SIZE];
			static unsigned long count_leading_zeroes32( Uint32 value );
			unsigned int sig_bits, exp_bits;
			unsigned int stride_bits_L2;
			VariableSizeCount<Uint8> counts;
		public:
			FPF(int stride_bits_L2_ = 3, int sig_bits_ = 8, int exp_bits_ = 4);
			virtual void init( PractRand::RNGs::vRNG *known_good );
			virtual void deinit( );
			virtual std::string get_name() const;
			virtual double get_result();
		//	virtual double result_to_pvalue ( Uint64 blocks, double r );

			virtual void test_blocks(TestBlock *data, int numblocks);
		};
	}//Tests
}//PractRand
