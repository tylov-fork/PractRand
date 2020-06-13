namespace PractRand {
	namespace RNGs {
		namespace Raw {
			class lcg64_32 {
			public:
				enum {
					OUTPUT_TYPE = OUTPUT_TYPES::NORMAL_1,
					OUTPUT_BITS = 32,
					FLAGS = FLAG::SUPPORTS_FASTFORWARD | FLAG::USES_MULTIPLICATION | FLAG::USES_SPECIFIED | FLAG::ENDIAN_SAFE
				};
			protected:
				Uint64 lcg64;
			public:
				Uint32 raw32();
				void seed(Uint64 s);
				void walk_state(StateWalkingObject *walker);
				void seek_forward(Uint64 how_far);
				void seek_backward(Uint64 how_far);
			};
		}
		
		namespace Polymorphic {
			class lcg64_32 : public vRNG32 {
				PRACTRAND__POLYMORPHIC_RNG_BASICS_H(lcg64_32)
				void seed(Uint64 s);
				void seek_forward(Uint64 how_far);
				void seek_backward(Uint64 how_far);
			};
		}
	}
}
