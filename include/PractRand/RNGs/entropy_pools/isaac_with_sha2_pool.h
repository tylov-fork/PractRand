namespace PractRand {
	namespace RNGs {
		namespace Polymorphic {
			namespace EntropyPools {
				class isaac_with_sha2_pool : public vRNG32 {
				public:
					enum {
						OUTPUT_TYPE = OUTPUT_TYPES::NORMAL_ALL,
						OUTPUT_BITS = 32,
						FLAGS = FLAG::USES_SPECIFIED | FLAG::USES_INDIRECTION | FLAG::ENDIAN_SAFE | FLAG::SUPPORTS_ENTROPY_ACCUMULATION | FLAG::CRYPTOGRAPHIC_INPUT | FLAG::CRYPTOGRAPHIC_OUTPUT
					};
				protected:
					enum {
						ISAAC_SIZE_L2 = 8, 
						ISAAC_SIZE = 1<<ISAAC_SIZE_L2,
						INPUT_BUFFER_SIZE=128//must be a multiple of four
					};
					typedef Uint32 ISAAC_Word;
					ISAAC_Word isaac_results[ISAAC_SIZE];
					ISAAC_Word isaac_state[ISAAC_SIZE];
					ISAAC_Word isaac_a, isaac_b, isaac_c;
					Uint8 input_buffer[INPUT_BUFFER_SIZE];
					int input_buffer_left;
					int isaac_results_left;

				public:
					isaac_with_sha2_pool(Uint64 s) {seed(s);}
					isaac_with_sha2_pool(vRNG *seeder) {seed(seeder);}
					isaac_with_sha2_pool(SEED_AUTO_TYPE ) {autoseed();}
					isaac_with_sha2_pool(SEED_NONE_TYPE ) {reset_state();}
					isaac_with_sha2_pool() {reset_state();}

					std::string get_name() const;

					Uint32 raw32();
					void seed(Uint64 s);
					void reset_state();
					using vRNG::seed;
					void walk_state(StateWalkingObject *walker);
					void add_entropy8 (Uint8  value);
					void add_entropy16(Uint16 value);
					void add_entropy32(Uint32 value);
					void add_entropy64(Uint64 value);
					void flush_buffers();
				protected:
					void empty_input_buffer();
					void refill_output_buffer();
				};
			}
		}
	}
}
