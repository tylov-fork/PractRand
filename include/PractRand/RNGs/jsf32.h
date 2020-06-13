namespace PractRand {
	namespace RNGs {
		namespace Raw {
			//implemented in RNGs/jsf.cpp
			class jsf32 {
			public:
				enum {
					OUTPUT_TYPE = OUTPUT_TYPES::NORMAL_1,
					OUTPUT_BITS = 32,
					FLAGS = FLAG::USES_SPECIFIED | FLAG::ENDIAN_SAFE
				};
			protected:
				Uint32 a, b, c, d;
			public:
				Uint32 raw32();
				void seed(Uint64 s);
				void seed(Uint32 seed1, Uint32 seed2, Uint32 seed3, Uint32 seed4);//custom seeding
				void walk_state(StateWalkingObject *walker);
			};
		}
		class jsf32 : public Raw::jsf32 {
		public:
			enum {
				OUTPUT_TYPE = OUTPUT_TYPES::NORMAL_ALL,
			};
		public:
			Uint8  raw8 () {return Uint8( raw32());}
			Uint16 raw16() {return Uint16(raw32());}
			Uint64 raw64() {Uint32 r = raw32(); return (Uint64(raw32()) << 32) | r;}

			jsf32(Uint64 seed_) {seed(seed_);}
			jsf32(vRNG *rng) {seed(rng);}
			jsf32(SEED_AUTO_TYPE ) {autoseed();}
			jsf32(SEED_NONE_TYPE ) {}
			void seed(Uint64 s) {Raw::jsf32::seed(s);}
			void seed(vRNG *rng) {StateWalkingObject *walker = vrng_to_rng_seeder(rng); walk_state(walker); delete walker;}
			void autoseed() {StateWalkingObject *walker = get_autoseeder(this); walk_state(walker); delete walker;}

			Uint32 randi(Uint32 max);
			Uint32 randi(Uint32 min, Uint32 max) {return randi(max-min)+min;}
			Uint32 randi_fast(Uint32 max) {return randi_fast_implementation(raw32(), max);}
			Uint32 randi_fast(Uint32 min, Uint32 max) {return randi_fast(max-min)+min;}
			Uint64 randli(Uint64 max);
			Uint64 randli(Uint64 min, Uint64 max) {return randli(max-min)+min;}
			float randf();
			float randf(float max) {return randf() * max;}
			float randf(float min, float max) {return randf() * (max-min) + min;}
			double randlf();
			double randlf(double max) {return randlf() * max;}
			double randlf(double min, double max) {return randlf() * (max-min) + min;}

			//Boost / C++0x TR1 compatibility:
			typedef Uint32 result_type;
			result_type operator()() {return raw32();}
			static const bool has_fixed_value = true;
			static const result_type min_value = 0;
			static const result_type max_value = (result_type)~min_value;
			result_type min() const {return min_value;}
			result_type max() const {return max_value;}
		};
		
		namespace Polymorphic {
			class jsf32 : public vRNG32 {
				PRACTRAND__POLYMORPHIC_RNG_BASICS_H(jsf32)
				void seed(Uint64 s);
			};
		}
	}
}
