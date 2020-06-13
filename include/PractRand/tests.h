#ifndef __PRACTRAND_TESTS_H__
#define __PRACTRAND_TESTS_H__


namespace PractRand {
	namespace RNGs { class vRNG; }
	namespace Tests {
		union TestBlock {
			enum {
				SIZE_L2 = 10,
				SIZE = 1<<SIZE_L2
			};
			Uint8  as8 [SIZE  ];
			Uint16 as16[SIZE/2];
			Uint32 as32[SIZE/4];
			Uint64 as64[SIZE/8];
			void fill(RNGs::vRNG *rng, unsigned long numblocks = 1);
		};
		class ListOfTests;
		class TestBaseclass {
		protected:
			Uint64 blocks_tested;
		public:
			enum {
				REPEATED_BLOCKS = 1,//at least 1, may be (much) more
				REPEATED_BYTES = TestBlock::SIZE * REPEATED_BLOCKS
			};
			virtual void init( PractRand::RNGs::vRNG *known_good ) {}
			virtual void deinit() {}
			virtual ~TestBaseclass()   {}
			virtual std::string get_name() const = 0;

			//1.  Maximum blocks per test_blocks() call is about 512MB (numblocks==1<<19)
			//2.  Maximum total length from multiple calls varies slightly but is usually 
			//       at least 256 TB, and often more.  
			//3.  If the total length is too short then result_to_pvalue will work even 
			//       worse than usual.  Typical minimum recommended lengths are on the 
			//       order of 1 MB, but some tests require much more or can get by on 
			//       much less than that.  
			//4.  You are supposed to make sure that the last REPEATED_BLOCKS blocks from 
			//       your previous test_blocks() calls are prepended on to the blocks 
			//       array.  Most tests don't care about this, but a few do.  The extra 
			//       blocks prepended are to appear at negative indices on the blocks[] 
			//       array, and are not counted in numblocks.  
			virtual void test_blocks(TestBlock *blocks, int numblocks) = 0;


			virtual double get_result () = 0;

			//result_to_pvalue generally produces *VERY* crude estimates
			//and sometimes returns -1, meaning unknown
			virtual double result_to_pvalue ( Uint64 blocks, double r );
			virtual bool recommend_subtest_tree_descent() const;

			virtual int get_num_subtests() const;
			virtual std::string get_subtest_name  (int index) const;
			//virtual double      get_subtest_result(int index);
			//virtual double subtest_result_to_pvalue(int index, Uint64 blocks, double result);
		protected:
			//std::vector<double> cached_results;
			//Uint64 cached_results_blocks;
			//virtual void cache_current_results() const = 0;
		};
	}//Tests
}//PractRand

#endif //__PRACTRAND_TESTS_H__