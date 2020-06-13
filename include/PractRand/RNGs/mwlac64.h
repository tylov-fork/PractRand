namespace PractRand {
	namespace RNGs {
		namespace Raw {
			//implemented in RNGs/mwlac.cpp
			class mwlac64 {
			public:
				enum {
					OUTPUT_TYPE = OUTPUT_TYPES::NORMAL_1,
					OUTPUT_BITS = 64,
					FLAGS = FLAG::ENDIAN_SAFE | FLAG::USES_MULTIPLICATION
				};
			protected:
				Uint64 a, b, c, d;
			public:
				Uint64 raw64();
				void seed(Uint64 s);
				void seed(Uint64 seed1, Uint64 seed2, Uint64 seed3, Uint64 seed4) {//custom seeding
					a = seed1; b = seed2; c = seed3; d = seed4;
				}
				void walk_state(StateWalkingObject *walker);
			};
		}
		
		namespace Polymorphic {
			class mwlac64 : public vRNG64 {
				PRACTRAND__POLYMORPHIC_RNG_BASICS_H(mwlac64)
				void seed(Uint64 s);
			};
		}
	}
}
