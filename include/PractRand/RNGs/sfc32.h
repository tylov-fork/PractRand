namespace PractRand {
	namespace RNGs {
		namespace Raw {
			//implemented in RNGs/sfc.cpp
			class sfc32 {
			public:
				enum {
					OUTPUT_TYPE = OUTPUT_TYPES::NORMAL_1,
					OUTPUT_BITS = 32,
					FLAGS = FLAG::ENDIAN_SAFE
				};
			protected:
				Uint32 a, b, counter;
			public:
				Uint32 raw32();
				void seed(Uint64 s);
				void walk_state(StateWalkingObject *walker);
			};
		}
		
		namespace Polymorphic {
			class sfc32 : public vRNG32 {
				PRACTRAND__POLYMORPHIC_RNG_BASICS_H(sfc32)
				void seed(Uint64 s);
			};
		}
		PRACTRAND__LIGHT_WEIGHT_RNG(sfc32)
	}
}
