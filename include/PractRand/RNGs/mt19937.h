namespace PractRand {
	namespace RNGs {
		namespace Raw {
			class mt19937 {
			public:
				enum {
					OUTPUT_TYPE = OUTPUT_TYPES::NORMAL_1,
					OUTPUT_BITS = 32,
					FLAGS = FLAG::OUTPUT_IS_BUFFERED | FLAG::OUTPUT_IS_HASHED | FLAG::ENDIAN_SAFE
				};
			protected:
				enum {N=624, M=397};
				Uint32 state[N];// internal state
				Uint32 used;      // number of values left before reload needed
				void _advance_state();
			public:
				void flush_buffers() {used = N;}
				Uint32 raw32();
				void walk_state(StateWalkingObject *walker);

				//seeds less than 2**32 use the standard MT19937 seeding algorithm
				//seeds higher than 2**32 use a nonstandard MT19937 seeding algorithm
				void seed(Uint64 s);
			};
		}
		
		namespace Polymorphic {
			class mt19937 : public vRNG32 {
				PRACTRAND__POLYMORPHIC_RNG_BASICS_H(mt19937)
				void seed(Uint64 s);
				void flush_buffers();
			};
		}
	}
}
