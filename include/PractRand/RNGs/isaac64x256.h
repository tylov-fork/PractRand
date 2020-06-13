namespace PractRand {
	namespace RNGs {
		namespace Raw {
			class isaac64x256 {
			public:
				enum {
					OUTPUT_TYPE = OUTPUT_TYPES::NORMAL_1,
					OUTPUT_BITS = 64,
					FLAGS = FLAG::CRYPTOGRAPHIC_OUTPUT | FLAG::OUTPUT_IS_BUFFERED | FLAG::ENDIAN_SAFE
				};
			protected:
				enum {
					SIZE_L2 = 8,
					SIZE = 1 << SIZE_L2
				};
				Uint64 results[SIZE];
				Uint64 state[SIZE];
				Uint64 a, b, c;
				Uint32 used;
				void _advance_state();
			public:
				void flush_buffers() {used = SIZE;}
				Uint64 raw64();
				void seed(Uint64 s);
				void seed(vRNG *s);
				void walk_state(StateWalkingObject *walker);
			};
		}
		
		namespace Polymorphic {
			class isaac64x256 : public vRNG64 {
				PRACTRAND__POLYMORPHIC_RNG_BASICS_H(isaac64x256)
				void seed(Uint64 s);
				void seed(vRNG *s);
				void flush_buffers();
			};
		}
	}
}
