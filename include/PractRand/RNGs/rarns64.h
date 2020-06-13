namespace PractRand {
	namespace RNGs {
		namespace Raw {
			//implemented in RNGs/rarns.cpp
			class rarns64 {
			public:
				enum {
					OUTPUT_TYPE = OUTPUT_TYPES::NORMAL_1,
					OUTPUT_BITS = 64,
					FLAGS = FLAG::ENDIAN_SAFE | FLAG::USES_SPECIFIED
				};
			protected:
				Uint64 xs1, xs2, xs3;
			public:
				Uint64 raw64();
				void seed(Uint64 s);
				void seed(Uint64 s1, Uint64 s2);
				void walk_state(StateWalkingObject *walker);
			};
		}

		namespace Polymorphic {
			class rarns64 : public vRNG64 {
				PRACTRAND__POLYMORPHIC_RNG_BASICS_H(rarns64)
				void seed(Uint64 s);
				void seed(Uint64 s1, Uint16 s2);
			};
		}
		PRACTRAND__LIGHT_WEIGHT_RNG(rarns64)
	}
}
