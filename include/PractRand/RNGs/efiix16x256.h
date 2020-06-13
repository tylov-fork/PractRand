namespace PractRand {
	namespace RNGs {
		namespace Raw {
			class efiix16x256 {
			public:
				enum {
					OUTPUT_TYPE = OUTPUT_TYPES::NORMAL_1,
					OUTPUT_BITS = 16,
					FLAGS = FLAG::USES_SPECIFIED | FLAG::USES_INDIRECTION | FLAG::USES_CYCLIC_BUFFER | FLAG::ENDIAN_SAFE
				};
			protected:
				enum {
					SIZE_L2 = 8,
					SIZE = 1 << SIZE_L2
				};
				typedef Uint16 Word;
				Word table[SIZE];
				Word i, a, b, c;
			public:
				Uint16 raw16();
				void seed(Uint64 s);
				void walk_state(StateWalkingObject *walker);
			};
		}
		
		namespace Polymorphic {
			class efiix16x256 : public vRNG16 {
				PRACTRAND__POLYMORPHIC_RNG_BASICS_H(efiix16x256)
				void seed(Uint64 s);
			};
		}
	}
}
