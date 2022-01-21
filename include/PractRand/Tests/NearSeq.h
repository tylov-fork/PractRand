namespace PractRand {
	namespace Tests {
		class NearSeq : public TestBaseclass {
		protected:
			typedef Uint64 Word;
			enum {
				/*
					parameterizations of interest:
						blocks*bits=core		thresholds				evaluation
					word			extra bits			derived
					32	14x8=112	128			2/3		87.5, 35 M		meh
					64	9x7=63		128			1/2		1.2 K, 268 M	???
					64	12x5=60		128			1/2		1, 129 K		
					64	10x12=120	128			3/4		13 K, 227 M		
					32?	10x4=40		64			0/1		110, 1 B		
					64	8x8=64		128			2/3		13, 20 K		
					32	8x8=64		64?			1/2		20 K, 13 B		
					32	8x4=32		64			0/1		43, 17 M		
				*/

				WORD_BITS = sizeof(Word)* 8,
				WORD_BITS_L2 = (WORD_BITS == 8) ? 3 : ((WORD_BITS == 16) ? 4 : ((WORD_BITS == 32) ? 5 : ((WORD_BITS == 64) ? 6 : -1))),
				NUM_BUCKETS_L2 = 9,
				NUM_BUCKETS = 1 << NUM_BUCKETS_L2,
				BITS_PER_BLOCK = 7,
				BLOCKS_PER_CORE = NUM_BUCKETS_L2, 
				CORE_SEQUENCE_BITS = BLOCKS_PER_CORE * BITS_PER_BLOCK,

				CORE_WORDS = (CORE_SEQUENCE_BITS + WORD_BITS - 1) / WORD_BITS, 
				EXTRA_WORDS = 2,
				SEQUENCE_WORDS = CORE_WORDS + EXTRA_WORDS,
				SEQUENCE_BITS = SEQUENCE_WORDS * WORD_BITS,
				SEQUENCE_WORD_OFFSET = (EXTRA_WORDS + 1) / 2,
				MAX_ERRORS_PER_BLOCK = 2,
				GOOD_ERRORS_PER_BLOCK = 1,

				MAX_CORE_DISTANCES = CORE_SEQUENCE_BITS / 2,//distances of this much or higher get combined with (one less than this)
				//MAX_CORE_DISTANCES = MAX_ERRORS_PER_BLOCK * BLOCKS_PER_CORE,//distances of this much or higher get combined with (one less than this)
				//RARES_THRESHOLD = 8,//core distances less than or equal to this are required for rares
				//RARES_SIZE_L2 = 16,
			};
			struct Bucket {
				Uint64 sequence[SEQUENCE_WORDS];
			};
			Bucket buckets[NUM_BUCKETS];
			Uint64 core_distances[MAX_CORE_DISTANCES];
			Uint64 sum_extra_distances[MAX_CORE_DISTANCES];//sum of all overall distances at a given core distances... divide by the same index on core_distance to get the average
			//Uint32 rare_index;
			//Uint32 rare_warmup;
			//FixedSizeCount<Uint16, 1 << RARES_SIZE_L2> count_rares;
			//FixedSizeCount<Uint16, NUM_BUCKETS * EXTRA_WORDS * 16> count_region;
			//void handle_rare(Uint8 one);
			Uint8 *lookup_table;//used by core_to_index
			Uint8 *lookup_table2;//used by is_core_good
			int core_to_index(const Word *core) const;//returns -1 on invalid core
			int is_core_good(const Word *core) const;
			int get_core_distance(const Word *core, int bucket_index) const;
			int get_extra_distance(const Word *core, int bucket_index) const;
		public:
			NearSeq();
			virtual void init(PractRand::RNGs::vRNG *known_good);
			virtual void deinit();
			virtual std::string get_name() const;
			virtual void get_results(std::vector<TestResult> &results);

			virtual void test_blocks(TestBlock *data, int numblocks);
		};
		class NearSeq2 : public TestBaseclass {
		protected:
			typedef Uint32 Word;
			enum {
				/*
				parameterizations of interest:
				blocks*bits=core		thresholds				evaluation
				word			extra bits			derived
				32	14x8=112	128			2/3		87.5, 35 M		meh
				64	9x7=63		128			1/2		1.2 K, 268 M	???
				64	12x5=60		128			1/2		1, 129 K
				64	10x12=120	128			3/4		13 K, 227 M
				32?	10x4=40		64			0/1		110, 1 B
				64	8x8=64		128			2/3		13, 20 K
				32	8x8=64		64?			1/2		20 K, 13 B
				32	8x4=32		64			0/1		43, 17 M
				*/

				WORD_BITS = sizeof(Word)* 8,
				WORD_BITS_L2 = (WORD_BITS == 8) ? 3 : ((WORD_BITS == 16) ? 4 : ((WORD_BITS == 32) ? 5 : ((WORD_BITS == 64) ? 6 : -1))),
				NUM_BUCKETS_L2 = 8,
				NUM_BUCKETS = 1 << NUM_BUCKETS_L2,
				BITS_PER_BLOCK = 32,

				BLOCKS_PER_CORE = NUM_BUCKETS_L2,
				CORE_SEQUENCE_BITS = BLOCKS_PER_CORE * BITS_PER_BLOCK,

				CORE_WORDS = (CORE_SEQUENCE_BITS + WORD_BITS - 1) / WORD_BITS,
				EXTRA_FULL_WORDS = 2,//should be an even number, otherwise call it an invalid parameterization
				EXTRA_PARTIAL_WORD_BITS = CORE_WORDS * WORD_BITS - CORE_SEQUENCE_BITS, //should be less than 1 word, otherwise call it an invalid parameterization
				EXTRA_BITS = EXTRA_FULL_WORDS * WORD_BITS + EXTRA_PARTIAL_WORD_BITS, 
				SEQUENCE_WORDS = CORE_WORDS + EXTRA_FULL_WORDS,
				SEQUENCE_BITS = SEQUENCE_WORDS * WORD_BITS,
				SEQUENCE_WORD_OFFSET = EXTRA_FULL_WORDS / 2,
				MAX_HDIST_PER_BLOCK = 12,
				MAX_TOTAL_HDIST = MAX_HDIST_PER_BLOCK * BLOCKS_PER_CORE,
				//MAX_CONCEIVABLE_HDIST = BLOCKS_PER_CORE * (BITS_PER_BLOCK / 2),
				HDIST_BINS = 16,

				MAX_LOOKUP_L2 = 12,//otherwise the tables get too big
				CHECK_VALIDITY_EARLY = 0,//if true, checks for a bad core after each block for the first word, then once per word thereafter
			};
			struct Bucket {
				//Word ideal_sequence[SEQUENCE_WORDS];
				Uint64 core_hdist[MAX_TOTAL_HDIST + 1];
				Uint64 extra_counts[HDIST_BINS][EXTRA_BITS];
				void reset();
			};
			Bucket buckets[NUM_BUCKETS];
			Uint64 _total_cores;
			Uint64 _total_invalid_cores;

			Sint8 *lookup_table1;//bit 7: valid or invalid value for a core block, bit 0: high or low value for core block
			Uint8 *lookup_table2;//hamming distance from idealized value for core block

			Sint8 lookup1(Word value) const {
				if (BITS_PER_BLOCK < WORD_BITS) value &= (1 << BITS_PER_BLOCK) - 1;
				if (BITS_PER_BLOCK <= MAX_LOOKUP_L2) return lookup_table1[value];
				else if (BITS_PER_BLOCK <= 16) return lookup_table1[count_bits16(value)];
				else if (BITS_PER_BLOCK <= 32) return lookup_table1[count_bits32(value)];
				else return lookup_table1[count_bits64(value)];
			}
			Sint32 _lookup1(Word value) const {
				if (BITS_PER_BLOCK <= MAX_LOOKUP_L2) return lookup_table1[value];
				else if (BITS_PER_BLOCK <= 16) return lookup_table1[count_bits16(value)];
				else if (BITS_PER_BLOCK <= 32) return lookup_table1[count_bits32(value)];
				else return lookup_table1[count_bits64(value)];
			}
			Uint32 _lookup2(Word value) const {
				if (BITS_PER_BLOCK <= MAX_LOOKUP_L2) return lookup_table2[value];
				else if (BITS_PER_BLOCK <= 16) return lookup_table2[count_bits16(value)];
				else if (BITS_PER_BLOCK <= 32) return lookup_table2[count_bits32(value)];
				else return lookup_table2[count_bits64(value)];
			}
			void analyze_block(Word block_value, long &bucket, int bucket_bit, long &hdist) const {
				block_value &= (1 << BITS_PER_BLOCK) - 1;
				bucket |= (_lookup1(block_value) & 1) << bucket_bit;
				hdist += _lookup2(block_value);
			}
			int get_hdist_bin(int hdist) const;
			bool is_core_bad(const Word *core) const;
			void core_analysis(const Word *core, int &index, int &ham) const;//only call on valid cores
			void count_bits_distribution(Word bits, Uint64 *counts, int num = WORD_BITS);
		public:
			NearSeq2();
			virtual void init(PractRand::RNGs::vRNG *known_good);
			virtual void deinit();
			virtual std::string get_name() const;
			virtual void get_results(std::vector<TestResult> &results);

			virtual void test_blocks(TestBlock *data, int numblocks);
		};
	}//Tests
}//PractRand
