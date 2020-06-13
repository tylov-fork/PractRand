namespace PractRand {
	namespace RNGs {
		namespace Raw {
			class jsf8 {
			public:
				//reversible multicyclic, with periods:
				//  2,423,359,581 (L2: 31.174)	30.832
				//    761,144,947 (L2: 29.504)	30.531
				//    528,259,832 (L2: 28.977)	29.546
				//    485,431,060 (L2: 28.855)	
				//     95,190,027 (L2: 26.504)
				//        855,047 (L2: 19.706)
				//        466,450 (L2: 18.831)
				//              1 (disallowed by the seeding & state-walking methods)
				//missing 260,351
//				PRACTRAND_DECLARE_RNG_OUTPUT_RAWX(32)
//				PRACTRAND_DECLARE_RNG_UNLOCKED()
//				PRACTRAND_DECLARE_SEEDING__CUSTOM_64()
			protected:
				Uint8 a, b, c, d;
			public:
				Uint8 raw8();
				void seed(Uint64 s);
				void walk_state(StateWalkingObject *walker);
			};
		}
		
		namespace Polymorphic {class jsf8 : public vRNG {
			PRACTRAND__POLYMORPHIC_RNG_BASICS_H(jsf8)
			void seed(Uint64 s);
			void seek_forward(Uint64 how_far);
			void seek_backward(Uint64 how_far);
		};}
	}
}
