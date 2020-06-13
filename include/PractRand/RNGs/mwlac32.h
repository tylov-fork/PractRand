namespace PractRand {
	namespace RNGs {
		namespace Raw {
			//implemented in RNGs/mwlac.cpp
			class mwlac32 {
			public:
				enum {
					OUTPUT_TYPE = OUTPUT_TYPES::NORMAL_1,
					OUTPUT_BITS = 32,
					FLAGS = FLAG::ENDIAN_SAFE | FLAG::USES_MULTIPLICATION
				};
			protected:
				Uint32 a, b, c, counter;
			public:
				Uint32 raw32();
				void seed(Uint64 s);
				void seed(Uint32 seed1, Uint32 seed2, Uint32 seed3, Uint32 seed4) {//custom seeding
					a = seed1; b = seed2; c = seed3; counter = seed4;
				}
				void walk_state(StateWalkingObject *walker);
			};
		}
		
		namespace Polymorphic {
			class mwlac32 : public vRNG32 {
				PRACTRAND__POLYMORPHIC_RNG_BASICS_H(mwlac32)
				void seed(Uint64 s);
			};
		}
	}
}
