namespace PractRand {
	namespace RNGs {
		namespace Raw {
			//implemented in RNGs/rarns.cpp
			class rarns32 {
			public:
				enum {
					OUTPUT_TYPE = OUTPUT_TYPES::NORMAL_1,
					OUTPUT_BITS = 32,
					FLAGS = FLAG::ENDIAN_SAFE | FLAG::USES_SPECIFIED
				};
			protected:
				Uint32 xs1, xs2, xs3;
			public:
				Uint32 raw32();
				void seed(Uint64 s);
				void seed(Uint32 s1, Uint32 s2, Uint32 s3);// { xs1 = s2; xs2 = s2; xs3 = s3; if (!(s1 | s2 | s3)) xs1 = 1; }
				void walk_state(StateWalkingObject *walker);
			};
		}

		namespace Polymorphic {
			class rarns32 : public vRNG32 {
				PRACTRAND__POLYMORPHIC_RNG_BASICS_H(rarns32)
				void seed(Uint64 s);
				void seed(Uint32 s1, Uint32 s2, Uint32 s3);
			};
		}
		PRACTRAND__LIGHT_WEIGHT_RNG(rarns32)
	}
}
