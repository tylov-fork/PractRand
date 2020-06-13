

namespace PractRand {
	namespace RNGs { class vRNG; }
	int count_bits8 (Uint8 a);
	int count_bits16(Uint16 a);
	int count_bits32(Uint32 a);
	int count_bits64(Uint64 a);
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
		class TestBaseclass {
		public:
			enum {
				REPEATED_BLOCKS = 1,//at least 1, may be (much) more
				REPEATED_BYTES = TestBlock::SIZE * REPEATED_BLOCKS
			};
			virtual void init( PractRand::RNGs::vRNG *known_good ) {}
			virtual void deinit() {}
			virtual ~TestBaseclass()   {}
			virtual std::string get_name() const = 0;
			virtual double get_result () = 0;


			//result_to_pvalue generally produces *VERY* crude estimates
			//and sometimes returns -1, meaning unknown
			virtual double result_to_pvalue ( Uint64 blocks, double r );


			//1.  Maximum blocks per test_blocks() call is about 512MB (numblocks==1<<19)
			//2.  Maximum total length from multiple calls varies slightly but is usually 
			//       at least 256 TB, and often more.  
			//3.  If the total length is too short then result_to_pvalue will work even 
			//       worse than usual.  Typical minimum recommended lengths are on the 
			//       order of 1 MB, but some tests require much more or much less than 
			//       that.  
			//4.  You are supposed to make sure that the last REPEATED_BLOCKS blocks from 
			//       your previous test_blocks() calls are prepended on to the blocks 
			//       array.  Most tests don't care about this, but a few do.  The extra 
			//       blocks prepended are to appear at negative indices on the blocks[] 
			//       array, and are not counted in numblocks.  
			virtual void test_blocks(TestBlock *blocks, int numblocks) = 0;
		};
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
				low = (LowIntType*)realloc(low, sizeof(LowIntType) * size);
				high = (Uint64*)realloc(high, sizeof(Uint64) * size);
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
		class ListOfTests {
		public:
			std::vector<TestBaseclass*> tests;
			ListOfTests ( TestBaseclass **tests_ );
			ListOfTests ( 
				TestBaseclass *test1, TestBaseclass *test2=NULL, TestBaseclass *test3=NULL, TestBaseclass *test4=NULL, 
				TestBaseclass *test5=NULL, TestBaseclass *test6=NULL, TestBaseclass *test7=NULL, TestBaseclass *test8=NULL,
				TestBaseclass *test9=NULL, TestBaseclass *test10=NULL, TestBaseclass *test11=NULL, TestBaseclass *test12=NULL,
				TestBaseclass *test13=NULL, TestBaseclass *test14=NULL, TestBaseclass *test15=NULL, TestBaseclass *test16=NULL
			);
		};
	}//Tests
}//PractRand