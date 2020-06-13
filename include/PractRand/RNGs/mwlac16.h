namespace PractRand {
	namespace RNGs {
		namespace Raw {
			//implemented in RNGs/mwlac.cpp
			class mwlac16 {
			public:
				enum {
					OUTPUT_TYPE = OUTPUT_TYPES::NORMAL_1,
					OUTPUT_BITS = 16,
					FLAGS = FLAG::ENDIAN_SAFE | FLAG::USES_MULTIPLICATION
				};
			protected:
				Uint16 a, b, c, counter;
			public:
				Uint16 raw16();
				void seed(Uint64 s);
				void walk_state(StateWalkingObject *walker);
			};
		}
		
		namespace Polymorphic {
			class mwlac16 : public vRNG16 {
				PRACTRAND__POLYMORPHIC_RNG_BASICS_H(mwlac16)
				void seed(Uint64 s);
			};
		}
	}
}
