namespace PractRand {
	namespace RNGs {
		namespace Raw {
			class efiix64x256 {
			public:
				enum {
					OUTPUT_TYPE = OUTPUT_TYPES::NORMAL_1,
					OUTPUT_BITS = 64,
					FLAGS = FLAG::USES_SPECIFIED | FLAG::USES_INDIRECTION | FLAG::USES_CYCLIC_BUFFER | FLAG::ENDIAN_SAFE
				};
			protected:
				enum {
					SIZE_L2 = 8,
					SIZE = 1 << SIZE_L2
				};
				typedef Uint64 Word;
				Word table[SIZE];
				Word i, a, b, c;
			public:
				Uint64 raw64();
				void seed(Uint64 s);
				void walk_state(StateWalkingObject *walker);
			};
		}
		
		namespace Polymorphic {
			class efiix64x256 : public vRNG64 {
				PRACTRAND__POLYMORPHIC_RNG_BASICS_H(efiix64x256)
				void seed(Uint64 s);
			};
		}
	}
}
