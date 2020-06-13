namespace PractRand {
	namespace RNGs {
		namespace Raw {
			//implemented in RNGs/jsf.cpp
			class jsf16 {
			public:
				enum {
					OUTPUT_TYPE = OUTPUT_TYPES::NORMAL_1,
					OUTPUT_BITS = 16,
					FLAGS = FLAG::USES_SPECIFIED | FLAG::ENDIAN_SAFE
				};
			protected:
				Uint16 a, b, c, d;
			public:
				Uint16 raw16();
				void seed(Uint64 s);
				void walk_state(StateWalkingObject *walker);
			};
		}
		
		namespace Polymorphic {
			class jsf16 : public vRNG16 {
				PRACTRAND__POLYMORPHIC_RNG_BASICS_H(jsf16)
				void seed(Uint64 s);
			};
		}
	}
}
