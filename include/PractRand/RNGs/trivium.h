namespace PractRand {
	namespace RNGs {
		namespace Raw {
			//implemented in RNGs/trivium.cpp
			class trivium {
			public:
				enum {
					OUTPUT_TYPE = OUTPUT_TYPES::NORMAL_1,
					OUTPUT_BITS = 64,
					FLAGS = FLAG::ENDIAN_SAFE | FLAG::CRYPTOGRAPHIC_SECURITY | FLAG::USES_SPECIFIED
				};
			protected:
				Uint64 a[2], b[2], c[2];
			public:
				~trivium();
				Uint64 raw64();
				void seed(Uint64 s);
				void seed(const Uint8 *seed_and_iv, int length);//length should not exceed 20
				void walk_state(StateWalkingObject *walker);
				static void self_test();
			};
		}
		
		namespace Polymorphic {
			class trivium : public vRNG64 {
				PRACTRAND__POLYMORPHIC_RNG_BASICS_H(trivium)
				void seed(Uint64 s);
				void seed(const Uint8 *seed_and_iv, int length);//length should not exeed 18
			};
		}
		PRACTRAND__LIGHT_WEIGHT_RNG(trivium)
	}
}
