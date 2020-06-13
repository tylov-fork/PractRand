namespace PractRand {
	namespace RNGs {
		namespace Raw {
			namespace EntropyPools {
				class arbee {
				public:
					enum {
						OUTPUT_TYPE = OUTPUT_TYPES::NORMAL_ALL,
						OUTPUT_BITS = 64,
						FLAGS = FLAG::USES_SPECIFIED | FLAG::ENDIAN_SAFE | FLAG::SUPPORTS_ENTROPY_ACCUMULATION
					};
				protected:
					Uint64 a, b, c, d;
					Uint64 do_jsflike();
					Uint64 do_mix();
				public:
					arbee() {reset_state();}
					Uint8  raw8 () {return Uint8 (do_jsflike());}
					Uint16 raw16() {return Uint16(do_jsflike());}
					Uint32 raw32() {return Uint32(do_jsflike());}
					Uint64 raw64() {return Uint64(do_jsflike());}
					void seed(Uint64 s);
					void reset_state() {a=b=c=d=0;}
					void walk_state(StateWalkingObject *walker);
					void add_entropy8 (Uint8  value) {add_entropy16(value);}
					void add_entropy16(Uint16 value);
					void add_entropy32(Uint32 value);
					void add_entropy64(Uint64 value);
				};
			}
		}
		
		namespace Polymorphic {
			namespace EntropyPools {
				class arbee : public vRNG64 {
				public:
					Raw::EntropyPools::arbee implementation;
					std::string get_name() const;
					arbee(Uint64 s) {seed(s);}
					arbee(vRNG *seeder) {seed(seeder);}
					arbee(SEED_AUTO_TYPE ) {autoseed();}
					arbee(SEED_NONE_TYPE ) {}
					arbee() {}
					Uint8  raw8 ();
					Uint16 raw16();
					Uint32 raw32();
					Uint64 raw64();
					void seed(Uint64 s);
					void reset_state();
					using vRNG::seed;
					void walk_state(StateWalkingObject *walker);
					void add_entropy8 (Uint8  value);
					void add_entropy16(Uint16 value);
					void add_entropy32(Uint32 value);
					void add_entropy64(Uint64 value);
				};
			}
		}
	}
}
