namespace PractRand {
	namespace RNGs {
		namespace Raw {
			//implemented in RNGs/chacha.cpp
#ifdef PRACTRAND_ALIGN_128
			class __declspec(align(16)) chacha {
#else
			class chacha {
#endif
			public:
				enum {
					OUTPUT_TYPE = OUTPUT_TYPES::NORMAL_1,
					OUTPUT_BITS = 32,
					FLAGS = FLAG::USES_SPECIFIED | FLAG::OUTPUT_IS_BUFFERED | FLAG::OUTPUT_IS_HASHED | FLAG::ENDIAN_SAFE | FLAG::CRYPTOGRAPHIC_SECURITY
				};
			protected:
				Uint32 outbuf[16];
				Uint32 state[12];
				Uint32 used;
				Uint32 position_overflow;
				Uint8 rounds;
				bool extend_cycle;//true allows carries from the position field to overflow in to the upper word of the IV
				bool short_seed;
				Uint8 padding;//just to make the size a round number

				void _advance_1();
				//void _reverse_1();
				void _set_position(Uint64 low, Uint64 high);
				void _get_position(Uint64 &low, Uint64 &high) const;

				void _core();
				Uint32 _refill_and_raw32();
			public:
				chacha() : rounds(20) {}
				~chacha();
				Uint32 raw32() {
					if (used < 16) return outbuf[used++];
					return _refill_and_raw32();
				}
				void seed(Uint64 s);
				void seed( const Uint32 seed_and_iv[10], bool extend_cycle_ = false );
				void seed_short( const Uint32 seed_and_iv[6], bool extend_cycle_ = false );
				void walk_state(StateWalkingObject *walker);
				void seek_forward (Uint64 how_far_low, Uint64 how_far_high);
				void seek_backward(Uint64 how_far_low, Uint64 how_far_high);

				//normally rounds is 8, 12, or 20, but it can be anywhere from 1 to 255
				//the default is 20
				//the author recommends 8 for weak crypto or non-crypto, 12 for moderate crypto, and 20 for strong crypto
				//I'd say that you can go as low as 4 or 5 rounds without adversely effecting the output for non-cryptographic purposes
				//when using an odd number of rounds the final transposition gets skipped - this may or may not match other implementations
				//therefore it is recommended that only even numbers of rounds be used
				void set_rounds(int rounds_);
				int get_rounds() const {return rounds;}

				static void self_test();
			};
			class not_chacha {
				/*
					similar basic concept to ChaCha
					similiarities:
					1. most mixing done in the output function to allow random access
					2. mid-sized state conceptualized as 4x4 matrix
					3. structure optimized for SIMD
					4. does mixing on a per-column basis, then rearranges elements between columns at the end of each round
					differences:
					1. the state matrix is premixed in the seeding process, allowing every bit of seed/IV to effect a substantial portion of the state matrix
					2. the position in the output stream is stored *outside* of the main matrix - it is introduced simultaneously to each column during mixing
					(the combination of the above two allow it to gain an effective extra round or two at reduced cost, provided that the seed & IV don't change too often)
					3. a different mixing function is used
					4. the math is all 64/256 bit SIMD instead of 32/128 bit SIMD
					5. the per-round inter-column mixing is less aggressive, though still sufficient
				*/
			public:
				enum {
					OUTPUT_TYPE = OUTPUT_TYPES::NORMAL_1,
					OUTPUT_BITS = 64,
					FLAGS = FLAG::USES_SPECIFIED | FLAG::OUTPUT_IS_BUFFERED | FLAG::OUTPUT_IS_HASHED | FLAG::ENDIAN_SAFE | FLAG::CRYPTOGRAPHIC_SECURITY
				};
			protected:
				enum {OUTBUF_SIZE = 16};
				Uint64 outbuf[16];
				Uint8 used;
				Uint8 rounds;
				Uint8 init_rounds;
				Uint8 padding[5];//just to make the size a round number
				Uint64 position;//overflows go in to the last word of base_state, requiring reinitialization
				Uint64 state[16];
				Uint64 base_state[8];

				void _set_position(Uint64 low, Uint64 high);
				void _get_position(Uint64 &low, Uint64 &high) const;
				void _advance_1();
				void initialize();
				static Uint64 rotate(Uint64 value, int bits) {return (value << bits) | (value >> (64-bits));}
				enum {
					DEFAULT_ROUNDS    = 1
					,ROUND_COMPLEXITY = 2
					,INIT_ROUNDS = 4
				};
				static void base_transform(Uint64 &a, Uint64 &b, Uint64 &c, Uint64 &d) {
					//  salsa(3) - 12
					//  salsa(4) - 37
					//  salsa(5) - >41
					// chacha(3) - 25
					// chacha(4) - maybe 55 as a WAG?
					//																																	1x2	2x1	1x3	3x1	1x4	2x2	4x1	1x5
					//a += b; d = rotate(a ^ d, 16); c += d; b = rotate(c ^ b, 12); a += b; d = rotate(a ^ d, 8); c += d; b = rotate(c ^ b, 7); //		21	21	45	46+	?
					//a += b; d = rotate(a ^ d, 32); c += d; b = rotate(c ^ b, 25); a += b; d = rotate(a ^ d, 16); c += d; b = rotate(c ^ b, 13);//		21	22	46+	*?	?
					//d += rotate(a + b, 24); c += d; b ^= c+(c<<3); a+=b; d ^= rotate(a + b, 16); c += d; b ^= c^(c>>7); a += b^d; //					?	20	33	
					//b+=d;b^=rotate(a,24)+a; c+=a;c^=rotate(b,37)+b; d+=b;d^=rotate(c,16)+c; a+=c;a^=rotate(d,7)+d;//									?	25	40+	
					//b+=d;b^=rotate(a,24)+a; c+=a;c^=b+(b<<3); d+=b;d+=c^(c>>7); a+=c;a^=rotate(d,35)+d;//												?	23	43+	*

					//Uint64 tmp=(rotate(a,24)+a)^b; d+=a;d^=tmp; c+=tmp;c^=d; b+=d;b+=c^(c>>7); a+=c;a^=rotate(b,37) + b;  //							12	13	35	
					//Uint64 tmp=(rotate(a,24)+a)^b; d+=a;d^=tmp; c+=tmp;c^=d+(d<<1); b+=d;b+=c^(c>>7); a+=c;a^=rotate(b,37) + b;//						14	16	44+
					//Uint64 tmp=(rotate(a,24)+a)^b; d+=a;d^=tmp; c+=tmp;c^=d+(d<<3); b+=d;b+=c^(c>>7); a+=c;a^=rotate(b,37) + b;//						16	21	38+	
					//Uint64 tmp=(rotate(a,24)+a)^b; d+=a;d^=rotate(tmp,19)+tmp; c+=tmp;c^=d; b+=d;b+=c^(c>>7); a+=c;a^=rotate(b,37) + b;//				31	33	44+	
					//Uint64 tmp=(rotate(a,24)+a)^b; d+=a;d^=rotate(tmp,19)+tmp; c+=tmp;c^=d+(d<<1); b+=d;b+=c^(c>>7); a+=c;a^=rotate(b,37) + b;//		37	38	?	
					//Uint64 tmp=(rotate(a,24)+a)^b; d+=a;d^=rotate(tmp,19)+tmp; c+=tmp;c^=d+(d<<2); b+=d;b+=c^(c>>7); a+=c;a^=rotate(b,37) + b;//		39	38	?	
					Uint64 tmp=(rotate(a,24)+a)^b; d+=a;d^=rotate(tmp,19)+tmp; c+=tmp;c^=d+(d<<3); b+=d;b+=c^(c>>7); a+=c;a^=rotate(b,37) + b;//		41	42	?	*
					//Uint64 tmp=(rotate(a,24)+a)^b; d+=a;d^=rotate(tmp,19)+tmp; c+=tmp;c^=d+(d<<3); b+=d;b+=c^(c>>7); a+=c;a^=rotate(b,32) + b;//		34	38	?	
					//Uint64 tmp=(rotate(a,24)+a)^b; d+=a;d^=rotate(tmp,13)+tmp; c+=tmp;c^=d+(d<<3); b+=d;b+=c^(c>>7); a+=c;a^=rotate(b,37) + b;//		37	37	?	
					//Uint64 tmp=(a+(a<<3))^b^(b>>7); d+=a;d^=rotate(tmp,24)+tmp; c+=tmp;c^=(rotate(d,16)+d); b+=d;b+=(rotate(c,12)+c); a+=c;a^=rotate(b,7)+b;//37	?	?	
					//Uint64 tmp=(rotate(a,24)+a)^b; d+=a;d^=rotate(tmp,21)+tmp; c+=tmp;c^=d+(d<<3); b+=d;b+=c^(c>>7); a+=c;a^=rotate(b,30) + b;//		?	?	?	

					//Uint64 tmp;
					//   function																						1-per-batch,no-hash   /  full-batch,no-hash      1-per-batch,w/hash     full-batch,w/hash
					//   																										1x2			2x1				1x3   3x1
					//c += rotate(a + b, 24); b ^= c+(c<<3); a+=b; c ^= rotate(a + b, 16); b += c; a+=b^(b>>7);	//				32/24		34/27 28,27,27	/42   /44+
					//c += rotate(a ^ b, 24); b += c ^ (c >> 7); a+=b+(b<<3);//	
					//c += rotate(a ^ c, 24); b += c ^ (c >> 7); a+=b+(b<<3);//	irreversible
					//c ^= rotate(a + b, 24); b ^= c+a; a+=b+(b<<3); c ^= rotate(a + b, 16); b += c; a+=b^(b>>7);//				??/29		??/29 29,28,29	/
					//c ^= rotate(a + b, 24); b ^= c+a; b+=b<<3; a+=b; c ^= rotate(a + b, 16); b += c; a+=b^(b>>7); //			??/31		??/32			/
					//b+=rotate(a,24)+c; a^=rotate(c,16)+b+c; c+=rotate(b,54)^a; b^=rotate(a,20)+c; a+=rotate(c,13)^b; c^=rotate(b,21)+a;// ?/38
					//b+=(rotate(a,24)+a)^c; a^=rotate(c,16)+b; c+=(rotate(b,54)+b)^a; b^=rotate(a,20)+c; a+=(rotate(c,13)+c)^b; c^=rotate(b,21)+a;// ?/43+

					//chacha original:
					//a += b; d = rotate32(a ^ d, 16); c += d; b = rotate32(c ^ b, 12); a += b; d = rotate32(a ^ d, 8); c += d; b = rotate32(c ^ b, 7); 
					// reversible
					// 8 adds/xors, 2 aligned-rotates, 2 rotates, 0 moves (on 2-register architectures)
					//     x1           x1.5             x2        x0.5
					//     8     +        3      +       4    +     0    = 15 abstract cycles
					// rotates cost 2 cycles because they take 1 cycle on some architectures but ~3 on others
					// aligned-rotates cost 1.5 cycles because they take 1 cycle on ISAs with fast rotates and 1 cycle on ISAs with fast shuffles, but ~3 on others
					// moves cost 0.5 because they are unnecessary or extra-fast on many ISAs
					//
					//this sort of reversible variant:
					//c += rotate(a + b, 24); b ^= c+(c<<3); a+=b; c ^= rotate(a + b, 16); b += c; a+=b^(b>>7);
					//works out to:
					// 10 adds/xors, 2 aligned-rotates, 2 shifts, 4 moves (on 2-register architectures)
					//     x1           x1.5             x1        x0.5
					//     10     +       3      +       2    +     2    = 17 abstract cycles
					//
					//arguably this variant should make up some of the difference on the simpler equivalent to chachas matrix-transpose operation
					//saving maybe 1 cycle that way.  but a much bigger deal is that this is based upon 64 bit math & 256 bit SIMD while ChaCha 
					//is based on 32 bit math & 128 bit SIMD.  If implemented on an unexpected SIMD width, ChaCha will waste computations but be fine, 
					//while this will slow down considerably.  And ChaCha produces 64 bytes at a time while this produces 96 bytes at a time.  
				}
				static void round_a(Uint64 *base) {
					for (int i = 0; i < ROUND_COMPLEXITY; i++) base_transform(base[0], base[4], base[ 8], base[12]);
					for (int i = 0; i < ROUND_COMPLEXITY; i++) base_transform(base[1], base[5], base[ 9], base[13]);
					for (int i = 0; i < ROUND_COMPLEXITY; i++) base_transform(base[2], base[6], base[10], base[14]);
					for (int i = 0; i < ROUND_COMPLEXITY; i++) base_transform(base[3], base[7], base[11], base[15]);
				}
				static void round_b(Uint64 *base) {
					/*for (int i = 0; i < ROUND_COMPLEXITY; i++) base_transform(base[0], base[5], base[10], base[15]); //full diagonals
					for (int i = 0; i < ROUND_COMPLEXITY; i++) base_transform(base[1], base[6], base[11], base[12]);
					for (int i = 0; i < ROUND_COMPLEXITY; i++) base_transform(base[2], base[7], base[ 8], base[13]);
					for (int i = 0; i < ROUND_COMPLEXITY; i++) base_transform(base[3], base[4], base[ 9], base[14]);//*/

					for (int i = 0; i < ROUND_COMPLEXITY; i++) base_transform(base[1], base[6], base[ 8], base[12]); //limited diagonals
					for (int i = 0; i < ROUND_COMPLEXITY; i++) base_transform(base[2], base[7], base[ 9], base[13]);
					for (int i = 0; i < ROUND_COMPLEXITY; i++) base_transform(base[3], base[4], base[10], base[14]);
					for (int i = 0; i < ROUND_COMPLEXITY; i++) base_transform(base[0], base[5], base[11], base[15]);//*/

					/*for (int i = 0; i < ROUND_COMPLEXITY; i++) base_transform(base[0], base[5], base[10], base[12]); //square geometry, not as good IMHO
					for (int i = 0; i < ROUND_COMPLEXITY; i++) base_transform(base[1], base[4], base[11], base[13]);
					for (int i = 0; i < ROUND_COMPLEXITY; i++) base_transform(base[2], base[7], base[ 8], base[14]);
					for (int i = 0; i < ROUND_COMPLEXITY; i++) base_transform(base[3], base[6], base[ 9], base[15]);//*/
				}
				void _core();
				Uint64 _refill_advance_raw64();
			public:
				not_chacha() : rounds(DEFAULT_ROUNDS), init_rounds(INIT_ROUNDS) {}
				Uint64 raw64() {
					if (used < OUTBUF_SIZE) return outbuf[used++];
					return _refill_advance_raw64();
				}
				void seed(Uint64 s);
				void seed( Uint64 seed_and_iv[6] );
				void walk_state(StateWalkingObject *walker);
				//void seek_forward (Uint64 how_far_low, Uint64 how_far_high);
				//void seek_backward(Uint64 how_far_low, Uint64 how_far_high);

				void set_rounds(int rounds_);
				int get_rounds() const {return rounds;}
				void set_precalculated_rounds(int init_rounds_);
				int get_precalculated_rounds() const {return init_rounds;}
			};
		}
		namespace Polymorphic {
			class chacha : public vRNG32 {
				PRACTRAND__POLYMORPHIC_RNG_BASICS_H(chacha)
				chacha(Uint32 seed_and_iv[10], bool extend_cycle_ = false) {seed(seed_and_iv, extend_cycle_);}
				void seed(Uint64 s);
				void seed(Uint32 seed_and_iv[10], bool extend_cycle_ = false);
				void seed_short(Uint32 seed_and_iv[6], bool extend_cycle_ = false);
				void seek_forward128 (Uint64 how_far_low64, Uint64 how_far_high64);
				void seek_backward128(Uint64 how_far_low64, Uint64 how_far_high64);

				//normally rounds is 8, 12, or 20, but lower and higher values are also possible
				//default is 20
				//for non-crypto applications, 4 rounds is sufficient to qualify for a 3 star quality rating, 6 rounds for a 5 star quality rating
				//for crypto applications, 8 rounds is sufficient to qualify for a 1 star crypto rating, 12 for a 3 star crypto rating, 20 for a 4 star crypto rating
				void set_rounds(int rounds_);
				int get_rounds() const;
			};
			class not_chacha : public vRNG64 {
				PRACTRAND__POLYMORPHIC_RNG_BASICS_H(not_chacha)
				not_chacha(Uint64 seed_and_iv[6]) {seed(seed_and_iv);}
				void seed(Uint64 s);
				void seed( Uint64 seed_and_iv[6] );
				//void seek_forward128 (Uint64 how_far_low64, Uint64 how_far_high64);
				//void seek_backward128(Uint64 how_far_low64, Uint64 how_far_high64);

				void set_rounds(int rounds_);
				int get_rounds() const;
				void set_precalculated_rounds(int init_rounds_);
				int get_precalculated_rounds() const ;
			};
		}
		PRACTRAND__LIGHT_WEIGHT_RNG(chacha)
		PRACTRAND__LIGHT_WEIGHT_RNG(not_chacha)
	}
}
