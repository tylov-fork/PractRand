namespace PractRand {
	namespace RNGs {
		namespace Raw {
			//implemented in RNGs/mwlac.cpp
			class mwlac16 {
			public://unknown period
//				PRACTRAND_DECLARE_RNG_OUTPUT_RAWX(32)
//				PRACTRAND_DECLARE_RNG_UNLOCKED()
//				PRACTRAND_DECLARE_SEEDING__CUSTOM_64()
			protected:
				Uint16 a, b, c, d;
			public:
				Uint16 raw16();
				void seed(Uint64 s);
				void seed(Uint16 seed1, Uint16 seed2, Uint16 seed3, Uint16 seed4) {//custom seeding
					a = seed1; b = seed2; c = seed3; d = seed4;
				}
				void walk_state(StateWalkingObject *walker);
			};
		}
		
		namespace Polymorphic {
			class mwlac16 : public vRNG {
				PRACTRAND__POLYMORPHIC_RNG_BASICS_H(mwlac16)
				void seed(Uint64 s);
			};
		}
	}
}
