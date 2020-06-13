namespace PractRand {
	namespace RNGs {
		namespace Raw {
			//implemented in RNGs/lcg.cpp
			class clcg96_32 {//combination LCG, m1=2**64, m2=2**31-1
			public:
				enum {
					OUTPUT_TYPE = OUTPUT_TYPES::NORMAL_1,
					OUTPUT_BITS = 32,
					FLAGS = FLAG::SUPPORTS_FASTFORWARD | FLAG::USES_MULTIPLICATION | FLAG::USES_SPECIFIED | FLAG::ENDIAN_SAFE
				};
			protected:
				Uint64 lcg64;
				Uint32 lcg32;
			public:
				Uint32 raw32();
				void seed(Uint64 s);
				void walk_state(StateWalkingObject *walker);
				void seek_forward(Uint64 how_far);
				void seek_backward(Uint64 how_far);
			};
		}
		namespace Polymorphic {
			class clcg96_32 : public vRNG32 {
				PRACTRAND__POLYMORPHIC_RNG_BASICS_H(clcg96_32)
				void seed(Uint64 s);
				void seek_forward(Uint64 how_far);
				void seek_backward(Uint64 how_far);
			};
		}
	}
}
