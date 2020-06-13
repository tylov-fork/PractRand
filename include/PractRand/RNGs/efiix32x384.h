namespace PractRand {
	namespace RNGs {
		namespace Raw {
			class efiix32x384 {
			public:
				enum {
					OUTPUT_TYPE = OUTPUT_TYPES::NORMAL_1,
					OUTPUT_BITS = 32,
					FLAGS = FLAG::USES_SPECIFIED | FLAG::USES_INDIRECTION | FLAG::USES_CYCLIC_BUFFER | FLAG::ENDIAN_SAFE
				};
			protected:
				typedef Uint32 Word;
				enum {
					ITERATION_SIZE_L2 = 7,
					ITERATION_SIZE = 1 << ITERATION_SIZE_L2,
					INDIRECTION_SIZE_L2 = 8,
					INDIRECTION_SIZE = 1 << INDIRECTION_SIZE_L2
				};
				Word indirection_table[INDIRECTION_SIZE], iteration_table[ITERATION_SIZE];
				Word i, a, b, c;
			public:
				~efiix32x384();
				Uint32 raw32();
				void seed(Uint64 s1, Uint64 s2, Uint64 s3, Uint64 s4);
				void seed(Uint64 s) {seed(s,0,0,0);}
				void walk_state(StateWalkingObject *walker);
			};
		}
		
		namespace Polymorphic {
			class efiix32x384 : public vRNG32 {
				PRACTRAND__POLYMORPHIC_RNG_BASICS_H(efiix32x384)
				void seed(Uint64 s1, Uint64 s2, Uint64 s3, Uint64 s4);
				void seed(Uint64 s);
			};
		}
		PRACTRAND__LIGHT_WEIGHT_RNG(efiix32x384)
	}
}
