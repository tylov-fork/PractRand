#ifndef __RNG_ADAPTORS_H__
#define __RNG_ADAPTORS_H__

#if 1
namespace PractRand {
	namespace RNGs {
		namespace Raw {
			namespace Adapators {
			//	template<class base_rng> class RAW_RNG_TO_NORMAL_RNG;
			//	template<class base_rng> class NORMAL_RNG_TO_POLYMORPHIC_RNG;
				template<class base_rng> class NORMALIZE_OUTPUT;
				template<class base_rng> class NORMALIZE_SEEDING;
				template<class base_rng> class NORMALIZE_DISTRIBUTIONS;


				template<class base_rng> class _NORMALIZE;
				template<class base_rng> class _NORMALIZE_OUTPUT;
			//	template<class base_rng> class _TEMPLATIZE_OUTPUT;
				template<class base_rng> class _NORMALIZE_SEEDING;
				template<class base_rng> class _NORMALIZE_DISTRIBUTIONS;

				namespace _Internal {
					template<class base_rng, int bits> class ADAPT_OUTPUT_1_TO_ALL;

					template<class base_rng, bool needs_int_seeding> class ADAPT_SEEDING;
					template<class base_rng> class ADAPT_SEEDING<base_rng,true> : public base_rng {
					public:
						enum {FLAGS = base_rng::FLAGS & ~ RNGs::FLAG::NEEDS_GENERIC_SEEDING};
//						enum {RNG_WRAPPER_LEVEL=base_rng::RNG_WRAPPER_LEVEL+1};
						typedef base_rng base_rng_type;
						void seed     (Uint64 seed) {StateWalkingObject *walker = int_to_rng_seed(seeder); walk_state(walker); delete walker;}
						void seed     (vRNG *seeder){StateWalkingObject *walker = vrng_to_rng_seeder(seeder); walk_state(walker); delete walker;}
						void autoseed ()            {StateWalkingObject *walker = get_autoseeder(this); walk_state(walker); delete walker;}
					};
					template<class base_rng> class ADAPT_SEEDING<base_rng,false> : public base_rng {
					public:
						enum {FLAGS = base_rng::FLAGS & ~ RNGs::FLAG::NEEDS_GENERIC_SEEDING};
//						enum {RNG_WRAPPER_LEVEL=base_rng::RNG_WRAPPER_LEVEL+1};
						typedef base_rng base_rng_type;
						using base_rng :: seed;
						void seed     (vRNG *seeder){StateWalkingObject *walker = vrng_to_rng_seeder(seeder); walk_state(walker); delete walker;}
						void autoseed ()            {StateWalkingObject *walker = get_autoseeder(this); walk_state(walker); delete walker;}
					};
//*/

					template<class base_rng> class ADAPT_OUTPUT_1_TO_ALL<base_rng, 8> : public base_rng {
					public:
						enum {OUTPUT_TYPE = OUTPUT_TYPES::NORMAL_ALL};
//						enum {RNG_WRAPPER_LEVEL=base_rng::RNG_WRAPPER_LEVEL+1};
						typedef base_rng base_rng_type;
						Uint16 raw16() {return raw8()  + (((Uint16) raw8()) <<  8);}
						Uint32 raw32() {return raw16() + (((Uint32)raw16()) << 16);}
						Uint64 raw64() {return raw32() + (((Uint64)raw32()) << 32);}
					};
					template<class base_rng> class ADAPT_OUTPUT_1_TO_ALL<base_rng, 16> : public base_rng {
					public:
						enum {OUTPUT_TYPE = OUTPUT_TYPES::NORMAL_ALL};
//						enum {RNG_WRAPPER_LEVEL=base_rng::RNG_WRAPPER_LEVEL+1};
						typedef base_rng base_rng_type;
						Uint8  raw8()  {return (Uint8)raw16();}
						Uint32 raw32() {return raw16() + (((Uint32)raw16()) << 16);}
						Uint64 raw64() {return raw32() + (((Uint64)raw32()) << 32);}
					};
					template<class base_rng> class ADAPT_OUTPUT_1_TO_ALL<base_rng, 32> : public base_rng {
					public:
						enum {OUTPUT_TYPE = OUTPUT_TYPES::NORMAL_ALL};
//						enum {RNG_WRAPPER_LEVEL=base_rng::RNG_WRAPPER_LEVEL+1};
						typedef base_rng base_rng_type;
						Uint8  raw8()  {return (Uint8)raw32();}
						Uint16 raw16() {return (Uint16)raw32();}
						Uint64 raw64() {return raw32() + (((Uint64)raw32()) << 32);}
					};
					template<class base_rng> class ADAPT_OUTPUT_1_TO_ALL<base_rng, 64> : public base_rng {
					public:
						enum {OUTPUT_TYPE = OUTPUT_TYPES::NORMAL_ALL};
//						enum {RNG_WRAPPER_LEVEL=base_rng::RNG_WRAPPER_LEVEL+1};
						typedef base_rng base_rng_type;
						Uint8  raw8()  {return (Uint8)raw64();}
						Uint16 raw16() {return (Uint16)raw64();}
						Uint32 raw32() {return (Uint32)raw64();}
					};


					template<class base_rng, int output_type, int output_bits> class _NORMALIZE_OUTPUT_HELPER;
					template<class base_rng, int output_bits> class _NORMALIZE_OUTPUT_HELPER<base_rng, OUTPUT_TYPES::NORMAL_1, output_bits> {
						public:typedef ADAPT_OUTPUT_1_TO_ALL<base_rng, output_bits> t;
					};
					template<class base_rng, int output_bits> class _NORMALIZE_OUTPUT_HELPER<base_rng, OUTPUT_TYPES::NORMAL_ALL, output_bits> {
						public:typedef base_rng t;
					};

					template<class base_rng> class ADD_DISTRIBUTIONS : public base_rng {
					public:
//						enum {DISTRIBUTIONS_TYPE = DISTRIBUTIONS_TYPE__NORMAL};
						Uint32 randi ( Uint32 max ) {
							Uint32 mask, tmp;
							max -= 1;
							mask = max;
							mask |= mask >> 1; mask |= mask >>  2; mask |= mask >> 4;
							mask |= mask >> 8; mask |= mask >> 16;
							while (1) {
								tmp = raw32() & mask;
								if (tmp <= max) return tmp;
							}
						}
						Uint32 randi ( Uint32 min, Uint32 max ) {return randi(max-min) + min;}

						Uint64 randli ( Uint64 max ) {
							Uint64 mask, tmp;
							max -= 1;
							mask = max;
							mask |= mask >> 1; mask |= mask >>  2; mask |= mask >>  4;
							mask |= mask >> 8; mask |= mask >> 16; mask |= mask >> 32;
							while (1) {
								tmp = raw64() & mask;
								if (tmp <= max) return tmp;
							}
						}
						Uint64 randli ( Uint64 min, Uint64 max ) {return randli(max-min) + min;}

						Uint32 randi_fast ( Uint32 max ) {return randi_fast_implementation(raw32(), max);}
						Uint32 randi_fast ( Uint32 min, Uint32 max ) {return randi_fast(max-min) + min;}

						//random floating point numbers:
						float randf ( ) { return float(raw32() * (1.0 / 4294967296.0)); }
						float randf ( float m ) { return randf() * m; }
						float randf ( float min, float max ) { return randf() * (max-min) + min; }

						double randlf ( ) { return ((double)raw64()) * (double)(1.0 / 18446744073709551616.0); }
						double randlf ( double m ) { return randlf() * m; }
						double randlf ( double min, double max ) { return randlf() * (max-min) + min; }
					};
					template<class base_rng, bool needs_distributions_added> class _NORMALIZE_DISTRIBUTIONS_HELPER;
					template<class base_rng> class _NORMALIZE_DISTRIBUTIONS_HELPER<base_rng,true> {
					public:typedef ADD_DISTRIBUTIONS<base_rng> t;
					};
					template<class base_rng> class _NORMALIZE_DISTRIBUTIONS_HELPER<base_rng,false> {
					public:typedef base_rng t;
					};

//*/
				}//namespace _Internal

				template<class base_rng> class _NORMALIZE_SEEDING {
				public: typedef typename 
					_Internal::ADAPT_SEEDING<
						base_rng, base_rng::FLAGS & RNGs::FLAG::NEEDS_GENERIC_SEEDING
					> t;
				//public:typedef typename base_rng t;
				};
				template<class base_rng> class NORMALIZE_SEEDING : public _NORMALIZE_SEEDING<base_rng>::t {};

				template<class base_rng> class _NORMALIZE_OUTPUT {
					public:typedef typename _Internal::_NORMALIZE_OUTPUT_HELPER<base_rng,base_rng::OUTPUT_TYPE, base_rng::OUTPUT_BITS>::t t;
				};
				template<class base_rng> class NORMALIZE_OUTPUT : public _NORMALIZE_OUTPUT<base_rng>::t {};

				template<class base_rng> class _NORMALIZE_DISTRIBUTIONS {
					//public:typedef typename _Internal::_NORMALIZE_DISTRUBTIONS_HELPER<base_rng,bool(base_rng::DISTRUBTIONS_TYPE & DISTRIBUTIONS_TYPE__NORMAL)>::t t;
					public:typedef _Internal::ADD_DISTRIBUTIONS<base_rng> t;
				};
				template<class base_rng> class NORMALIZE_DISTRIBUTIONS : public _NORMALIZE_DISTRIBUTIONS<base_rng>::t {};

				template<class base_rng> class _NORMALIZE {
					public:typedef typename _NORMALIZE_SEEDING<
						typename _NORMALIZE_DISTRIBUTIONS<
							typename _NORMALIZE_OUTPUT<base_rng>::t
						>::t
					>::t t;
				};
				template<class base_rng> class RAW_TO_LIGHT_WEIGHT_RNG : public _NORMALIZE<base_rng>::t {
				public:
					RAW_TO_LIGHT_WEIGHT_RNG(SEED_AUTO_TYPE) {autoseed();}
					RAW_TO_LIGHT_WEIGHT_RNG(SEED_NONE_TYPE) {}
					RAW_TO_LIGHT_WEIGHT_RNG(Uint64 s) {seed(s);}
					RAW_TO_LIGHT_WEIGHT_RNG(vRNG *seeder) {seed(seeder);}
				};
				//to do:
				//template<class base_rng> class RAW_TO_POLYMORPHIC_RNG;
				//template<class base_rng> class NORMAL_TO_POLYMORPHIC_RNG;
			}//namespace Adaptors
		}//namespace Raw
	}//namespace RNGs
}//namespace PractRand
#endif

#endif //__RNG_ADAPTORS_H__
