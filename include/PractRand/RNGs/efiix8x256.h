namespace PractRand {
	namespace RNGs {
		namespace Raw {
			class efiix8x256 {
			public:
				enum {
					OUTPUT_TYPE = OUTPUT_TYPES::NORMAL_1,
					OUTPUT_BITS = 8,
					FLAGS = FLAG::USES_SPECIFIED | FLAG::USES_INDIRECTION | FLAG::USES_CYCLIC_BUFFER | FLAG::ENDIAN_SAFE
				};
			protected:
				enum {
					SIZE_L2 = 8,
					SIZE = 1 << SIZE_L2
				};
				typedef Uint8 Word;
				Word table[SIZE];
				Word i, a, b, c;
			public:
				Uint8 raw8();
				void seed(Uint64 s);
				void walk_state(StateWalkingObject *walker);
			};
		}
		
		namespace Polymorphic {
			class efiix8x256 : public vRNG8 {
				PRACTRAND__POLYMORPHIC_RNG_BASICS_H(efiix8x256)
				void seed(Uint64 s);
			};
		}
	}
}
