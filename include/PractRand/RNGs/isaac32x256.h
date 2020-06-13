namespace PractRand {
	namespace RNGs {
		namespace Raw {
			class isaac32x256 {
			public:
				enum {
					OUTPUT_TYPE = OUTPUT_TYPES::NORMAL_1,
					OUTPUT_BITS = 32,
					FLAGS = FLAG::CRYPTOGRAPHIC_OUTPUT | FLAG::OUTPUT_IS_BUFFERED | FLAG::ENDIAN_SAFE
				};
			protected:
				enum {
					SIZE_L2 = 8,
					SIZE = 1 << SIZE_L2
				};
				Uint32 results[SIZE];
				Uint32 used;
				Uint32 state[SIZE];
				Uint32 a, b, c;
				void _advance_state();
			public:
				void flush_buffers() {used = SIZE;}
				Uint32 raw32();
				void seed(Uint64 s);
				void seed(vRNG *s);
				void walk_state(StateWalkingObject *walker);
			};
		}
		
		namespace Polymorphic {
			class isaac32x256 : public vRNG32 {
				PRACTRAND__POLYMORPHIC_RNG_BASICS_H(isaac32x256)
				void seed(Uint64 s);
				void seed(vRNG *s);
				void flush_buffers();
			};
		}
	}
}
