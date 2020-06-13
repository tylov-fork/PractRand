/*
Canidates currently under consideration:


1. a variant of Ranrot (RanrotVariant64/32/16/8)
	OVERALL EVALUTION: no close competitors atm, but no clear need for it either
	niche: medium to fast speed, medium to low quality, medium size
	speed: occasionally faster than any recommended RNG, sometimes only middling though
	statistical quality: decent at word sizes past 16 bit, poor @ 8 & 16 bit
	portability: uses only simple operations
	word sizes: viable at 8, 16, 32, & 64 bit ; quality suffers @ 8 & 16 bit though
	full word output: yes
	buffered: yes
	random access: no
	cryptographic security: none
	short cycle protection: limited
	cycle type: reversible multicyclic
	size: 19 words
	statespace: full on 18 words, 17 states on the last word
		(17 * 2**144 @ 8 bit, 17 * 2**288 @ 16 bit, 17 * 2**576 @ 32 bit, 17 * 2**1152 @ 64 bit)
	alternatives under consideration:
		single-tap with 2 accumulators - entropy difuses across the buffer much more slowly, 
		but overall quality is higher and speed is comparable

3. a small fast RNG (VeryFast64/32/16)
	OVERALL EVALUTION: insufficiently different from jsf64/32/16
	niche: very fast small RNG
	speed: often faster than any recommended RNG, generally at least competitive with the fastest
	statistical quality: decent @ 32 bit & 64 bit, poor @ 16 bit
	portability: uses only simple operations
	word sizes: viable at 16, 32, & 64 bit
	full word output: yes
	buffered: no
	random access: no
	cryptographic security: none
	short cycle protection: none
	cycle type: reversible multicyclic (also considering some irreversible variants)
	size: 3 words
	statespace: full on all 3 words, except for the all-zeroes case
		(2**48-1 @ 16 bit, 2**96-1 @ 32 bit, 2**192-1 @ 64 bit)

4. a possible new revision to the current sfc RNG (sfc_alternative64/32/16)
	OVERALL EVALUTION: hard to justify
	niche: same as sfc - 
		a combination of the strengths of small fast RNG with the advantage of a guaranteed cycle length
		it's certainly better in terms of state space
		and it looks better than sfc to me in terms of statistical quality
		in some speed tests it's significantly faster than sfc, but in others it is not
	speed: fast to very fast - sometimes appears faster than sfc, usually appears faster than jsf
	statistical quality: almost as good as jsf
	portability: uses only simple operations
	word sizes: viable at 16, 32, & 64 bit
	full word output: yes
	buffered: no
	random access: no
	cryptographic security: none
	short cycle protection: limited
	cycle type: reversible multicyclic
	size: 4 words
	statespace: full on all 4 words
		(2**64 @ 16 bit, 2**128 @ 32 bit, 2**256 @ 64 bit)

*/
#define POLYMORPHIC_CANDIDATE(rng, bits) \
class polymorphic_ ## rng ## bits : public PractRand::RNGs::vRNG ## bits {\
public:\
	typedef raw_ ## rng ## bits ImplementationType;\
	enum {OUTPUT_BITS = ImplementationType ::OUTPUT_BITS,FLAGS = ImplementationType ::FLAGS};\
	polymorphic_ ## rng ## bits (PractRand::SEED_NONE_TYPE) {}\
	polymorphic_ ## rng ## bits (PractRand::SEED_AUTO_TYPE) {autoseed();}\
	polymorphic_ ## rng ## bits (Uint64 seed_value) {seed(seed_value);}\
	ImplementationType implementation;\
	Uint ## bits raw ## bits () {return implementation.raw ## bits ();}\
	void walk_state(StateWalkingObject *walker) {implementation.walk_state(walker);}\
	std::string get_name() const {return std::string(#rng) + #bits ;}\
};

template<typename Word, int LAG1, int LAG2, int SHIFT1, int SHIFT2, int SHIFT3>
class _RanrotVariant {
public:
	enum {
		OUTPUT_TYPE = PractRand::RNGs::OUTPUT_TYPES::NORMAL_1,
		OUTPUT_BITS = 8 * sizeof(Word),
		FLAGS = PractRand::RNGs::FLAG::NEEDS_GENERIC_SEEDING
	};
	Word buffer[LAG1], position, counter;
	static Word rotate(Word value, int bits) {return (value << bits) | (value >> (8*sizeof(value)-bits));}
	Word step( Word prior, Word current, Word other ) {
		return (rotate(current, SHIFT1) ^ rotate(other, SHIFT2)) + rotate(prior, SHIFT3);
		//return (rotate(current, SHIFT1) ^ rotate(other, SHIFT2)) + (rotate(prior, SHIFT3) ^ prior);
		//prior += prior << 2; prior ^= prior >> 3; return (current ^ other) + prior;
	}
	Word _raw_native() {
		if (position) return buffer[--position];
		Word prior = buffer[LAG1-1] + counter++;
		for (int i = 0; i < LAG2; i++)
			prior = buffer[i] = step(prior, buffer[i], buffer[i+LAG1-LAG2]);
		for (int i = LAG2; i < LAG1; i++) prior = buffer[i] = 
			prior = buffer[i] = step(prior, buffer[i], buffer[i     -LAG2]);
		return buffer[position = LAG1 - 1];
	}
	void walk_state(StateWalkingObject *walker) {
		for (int i = 0; i < LAG1; i++) walker->handle(buffer[i]);
		walker->handle(position);
		walker->handle(counter);
		if (position > LAG1) position = 0;
	}
};
class raw_ranrot_variant64 : public _RanrotVariant<Uint64,17,5,0,0,29> {public: Uint64 raw64() {return _raw_native();}};
class raw_ranrot_variant32 : public _RanrotVariant<Uint32,17,5,0,0,13> {public: Uint32 raw32() {return _raw_native();}};
class raw_ranrot_variant16 : public _RanrotVariant<Uint16,17,5,0,0, 7> {public: Uint16 raw16() {return _raw_native();}};
class raw_ranrot_variant8  : public _RanrotVariant<Uint8 ,17,5,0,0, 3> {public: Uint8  raw8 () {return _raw_native();}};
POLYMORPHIC_CANDIDATE(ranrot_variant, 64)
POLYMORPHIC_CANDIDATE(ranrot_variant, 32)
POLYMORPHIC_CANDIDATE(ranrot_variant, 16)
POLYMORPHIC_CANDIDATE(ranrot_variant, 8)

template<typename Word, int ROTATE, int RSHIFT, int LSHIFT>
class _VeryFast {
public:
	enum {
		OUTPUT_TYPE = PractRand::RNGs::OUTPUT_TYPES::NORMAL_1,
		OUTPUT_BITS = 8 * sizeof(Word),
		FLAGS = PractRand::RNGs::FLAG::NEEDS_GENERIC_SEEDING
	};
	Word a, b, c;
	Word rotate(Word value, int bits) {return ((value << bits) | (value >> (OUTPUT_BITS - bits)));}
	Word _raw_native() {
		Word old;
		//very good speed, 32 bit version failed @ 2 TB
		/*old = a + b;
		a = b ^ c;
		b = c + old;
		c = old + bshift(c,ROTATE);
		return old;//*/
		//very good speed, 16 bit version fails @ 64 GB, 32 bit version passed 8 TB
		old = a + b;
		a = b ^ (b >> RSHIFT);
		b = c + (c << LSHIFT);
		c = old + rotate(c,ROTATE);
		return old;//*/
		//not as fast, 16 bit version fails @ 256 GB
		/*old = a + (a << LSHIFT);
		a = b + c;
		b = c ^ (c >> RSHIFT);
		c = rotate(c,ROTATE) + old;
		return old;//*/
		//?, 16 bit version fails @ ? GB, 32 bit version ?
		/*old = a + b;
		a = b + (b << LSHIFT);
		b = c ^ (c >> RSHIFT);
		c = old + rotate(c,ROTATE);
		return old;//*/
	}
	void walk_state(StateWalkingObject *walker) {
		walker->handle(a);
		walker->handle(b);
		walker->handle(c);
	}
};
class raw_VeryFast64 : public _VeryFast<Uint64,29,9,3> {public: Uint64 raw64() {return _raw_native();}};
class raw_VeryFast32 : public _VeryFast<Uint32,13,7,3> {public: Uint32 raw32() {return _raw_native();}};
class raw_VeryFast16 : public _VeryFast<Uint16, 7,3,2> {public: Uint16 raw16() {return _raw_native();}};
class raw_VeryFast8  : public _VeryFast<Uint8 , 3,2,2> {public: Uint8  raw8 () {return _raw_native();}};
POLYMORPHIC_CANDIDATE(VeryFast, 64)
POLYMORPHIC_CANDIDATE(VeryFast, 32)
POLYMORPHIC_CANDIDATE(VeryFast, 16)
POLYMORPHIC_CANDIDATE(VeryFast,  8)

template<typename Word, int SHIFT1, int SHIFT2, int SHIFT3>
class _sfc_alternative {
public:
	enum {
		OUTPUT_TYPE = PractRand::RNGs::OUTPUT_TYPES::NORMAL_1,
		OUTPUT_BITS = 8 * sizeof(Word),
		FLAGS = PractRand::RNGs::FLAG::NEEDS_GENERIC_SEEDING
	};
	Word a, b, c, counter;
	static Word rotate(Word value, int bits) {return (value << bits) | (value >> (8*sizeof(value)-bits));}
	Word _raw_native() {
		//better speed, 16 bit version >8 TB (64 GB w/o counter)
		Word old = a + b + counter++;//64 GB? on counter, 8 TB on b
		a = b ^ (b >> SHIFT2);//128 GB?
		b = c + (c << SHIFT3);//1 TB
		c = old + rotate(c,SHIFT1);//important!
		return old;//*/
		//okay speed, 16 bit version >2 TB (256 GB w/o counter), 32 bit @ ?
		/*Word old = a + (a << SHIFT3);
		a = b + c + counter++;
		b = c ^ (c >> SHIFT2);
		c = rotate(c,SHIFT1) + old;
		return old;//*/
		//faster, 16 bit version failed @ 64-128 GB (4 GB w/o counter), 32 bit @ ? (passed 16 TB w/o counter)
		/*Word old = a + b;
		a = b + counter++;
		b = c ^ (c >> SHIFT2);
		c = old + rotate(c,SHIFT1);
		return old;//*/
		//good speed, 16 bit version failed @ >512 GB (32 GB w/o counter), 32 bit @ ? (? w/o counter)
		/*Word old = a + b + counter++;
		a = b + (b << SHIFT3);
		b = c ^ (c >> SHIFT2);
		c = old + rotate(c,SHIFT1);
		return old;//*/
		//current SFC:
		/*Word tmp = a + b + counter++;
		a = b ^ (b >> SHIFT2);
		b = rotate(b,SHIFT1) + tmp;
		return tmp;//*/
		//maximum speed?
		//Word tmp = a;
		//a = rotate(b,SHIFT1);
		//b += a;
		//Word tmp = a;
		//a += 1;
		//return tmp;
		//return a += b;//*/
	}
	void walk_state(StateWalkingObject *walker) {
		walker->handle(a);
		walker->handle(b);
		walker->handle(c);
		walker->handle(counter);
	}
};
class raw_sfc_alternative8  : public _sfc_alternative<Uint8 , 3,2,2> {public: Uint8  raw8 () {return _raw_native();}};
class raw_sfc_alternative16 : public _sfc_alternative<Uint16, 7,3,2> {public: Uint16 raw16() {return _raw_native();}};
class raw_sfc_alternative32 : public _sfc_alternative<Uint32,13,7,3> {public: Uint32 raw32() {return _raw_native();}};
class raw_sfc_alternative64 : public _sfc_alternative<Uint64,29,9,3> {public: Uint64 raw64() {return _raw_native();}};
POLYMORPHIC_CANDIDATE(sfc_alternative, 64)
POLYMORPHIC_CANDIDATE(sfc_alternative, 32)
POLYMORPHIC_CANDIDATE(sfc_alternative, 16)
POLYMORPHIC_CANDIDATE(sfc_alternative, 8)

/*#include <intrin.h>
static Uint32 bswap32a(Uint32 v) {v = (v << 16) | (v >> 16); v = ((v & 0xff00ff00) >> 8) | ((v & 0x00ff00ff) << 8); return v;}
static Uint32 bswap32b(Uint32 v) {union {Uint32 whole; Uint8 parts[4];};whole=v; parts[0]^=parts[3];parts[3]^=parts[0];parts[0]^=parts[3]; parts[1]^=parts[2];parts[2]^=parts[1];parts[1]^=parts[2]; return whole;}
static Uint32 bswap32c(Uint32 v) {union {Uint32 whole; Uint8 parts[4];};whole=v; Uint8 tmp=parts[0];parts[0]=parts[3];parts[3]=tmp; tmp=parts[1];parts[1]=parts[2];parts[2]=tmp; return whole;}
static Uint32 bswap32d(Uint32 v) {return _byteswap_ulong(v);}
static Uint32 bswap32e(Uint32 v) {__asm {mov eax, v} __asm {bswap eax}}*/


/*static Uint64 bswap64a(Uint64 v) {v = (v << 32) | (v >> 32); v = ((v & 0xff00ff00ff00ff00) >> 8) | ((v & 0x00ff00ff00ff00ff) << 8); v = ((v & 0xffff0000ffff0000) >> 16) | ((v & 0x0000ffff0000ffff) << 16); return v;}
static Uint64 bswap64b(Uint64 v) {
	union {Uint64 whole; Uint8 parts[8];};whole=v;
	parts[0]^=parts[7];parts[7]^=parts[0];parts[0]^=parts[7];
	parts[1]^=parts[6];parts[6]^=parts[1];parts[1]^=parts[6];
	parts[2]^=parts[5];parts[5]^=parts[2];parts[2]^=parts[5];
	parts[3]^=parts[4];parts[4]^=parts[3];parts[3]^=parts[4];
	return whole;
}
static Uint64 bswap64c(Uint64 v) {
	union {Uint64 whole; Uint8 parts[8];};whole=v;
	Uint8 tmp=parts[0];parts[0]=parts[7];parts[7]=tmp;
	tmp=parts[1];parts[1]=parts[6];parts[6]=tmp;
	tmp=parts[2];parts[2]=parts[5];parts[5]=tmp;
	tmp=parts[3];parts[3]=parts[4];parts[4]=tmp;
	return whole;
}
static Uint64 bswap64d(Uint64 v) {return __builtin_bswap64(v);}
static Uint64 bswap64e(Uint64 v) {
	__asm__("bswap %0" : "+r" (v) : "0" (v) );
	return v;
}//*/


template<typename Word, int rotate_bits, Uint64 K_>
class _mcx {
	//Word a, b, c, d, e, f, g, h, i;
	//Uint64 a64, b64, c64;
	Uint32 state_0, state_1, state_2, state_3;
	Uint32 lfsr_0, lfsr_1, lfsr_01;
	Uint32 cycle;
public:
	enum {
		OUTPUT_TYPE = PractRand::RNGs::OUTPUT_TYPES::NORMAL_1,
		OUTPUT_BITS = sizeof(Word) * 8,
		FLAGS = PractRand::RNGs::FLAG::NEEDS_GENERIC_SEEDING
	};
	Word rotate(Word value, int bits) {return (value << bits) | (value >> (OUTPUT_BITS - bits));}
	Word lfsr(Word n, Word m) {return (n >> 1) ^ (-(n & 1) & m);}
	Word _raw_native() {
		Uint32 save = state_0 ^ (state_0 << 13);
		state_0 = state_1;
		state_1 = state_2;
		state_2 = state_3;
		state_3 = state_3 ^ (state_3 >> 17) ^ (save ^ (save >> 7));//*/
		/*Uint32 save = state_0;
		state_0 = state_1;
		state_1 = state_2 ^ (state_2 >> 7);
		state_2 = state_3 ^ (state_3 << 13);
		state_3 = state_3 ^ (state_3 >> 17) ^ save;//*/

		lfsr_0 = lfsr(lfsr_0, 0xE28F75ED);
		//lfsr_1 = lfsr(lfsr_1, 0x400005B6);
		//lfsr_01 = (lfsr_0 & ~lfsr_01) | (lfsr_1 & lfsr_01);

		cycle += 0x5D31995A;
		return (state_3 + lfsr_0) ^ cycle; 
		return state_3 ^ cycle;

		//const Word K = (Word)K_;
		//Word old = a + b + counter++;//64 GB? on counter, 8 TB on b
		//a = b ^ (b >> SHIFT2);//128 GB?
		//b = c + (c << SHIFT3);//1 TB
		//c = old + rotate(c,SHIFT1);//important!
		//return old;
		/*Uint64 rv;
		asm (
			"lea (%1, %2), %0\n"
			"mov %2, %1\n"
			"shr $9, %2\n"
			"xor %2, %1\n"
			"lea (%3,%3,8),%2\n"
			"add %0, %3\n"
			:"=r"(rv), "=r"(a), "=r"(b), "=r"(c)//outputs
			:"1"(a), "2"(b), "3"(c)//inputs
			://clobbers
		);*/
		//return rv;//*/
		/*Uint64 rv;
		asm (
			"mov    0(%%rdi), %%rax \n"
			"mov    8(%%rdi), %%rbx \n"
			"mov    16(%%rdi), %%rcx \n"
			"mov    24(%%rdi), %%rdx \n"
			"add    %%rbx, %%rax \n"
			"add    %%rdx, %%rax \n"
			"inc    %%rdx \n"
			"mov    %%rdx, 24(%%rdi) \n"
			"mov    %%rbx, %%rdx \n"
			"shr    $9, %%rbx \n"
			"xor    %%rdx, %%rbx \n"
			"lea    (%%rcx,%%rcx,8), %%rdx\n"
			"rol    $29, %%rcx \n"
			"add    %%rax, %%rcx \n"
			"mov    %%rbx, 0(%%rdi)\n"
			"mov    %%rcx, 16(%%rdi) \n"
			"mov    %%rdx, 8(%%rdi) \n"
			:"=a"(rv)//outputs
			:"D"(this)//inputs
			:"memory", "%rbx", "%rcx", "%rdx"//clobbers
		);
		return rv;//*/

//	printf("%8x%8x\n", Uint32(Candidates::bswap64e(0x123456789abcdefull)>>32), Uint32(Candidates::bswap64e(0x123456789abcdefull)));
		/*Uint64 x = b;
		x *= K;
		Uint32 old_a = a;
		a += K;
		b += old_a + (a < K);
		x = Uint32(x) ^ Uint32(x >> 32) ^ old_a;
		x *= K;
		return Uint32(x) + Uint32(x >> 32) + old_a;*/
		//Uint64 rv;
		/*asm volatile (
			"mov    8(%%rdi), %%rax \n"
			"movabs $0x6595a395a1ec531b, %%rcx \n"
			"mov    (%%rdi), %%rsi \n"
			"mul    %%rcx \n"
			"add    %%rcx, (%%rdi) \n"
			"adc    %%rsi, 8(%%rdi) \n"
			"xor    %%rsi, %%rax \n"
			"xor    %%rdx, %%rax \n"
			"mul    %%rcx \n"
			"add    %%rsi, %%rax \n"
			"add    %%rdx, %%rax \n"
			:"=a"(rv)//outputs
			:"D"(this)//inputs
			:"memory", "%rcx", "%rsi", "%rdx"//clobbers
		);*/
		/*asm(
			"movq %2, %0\n"		//rv = b;
			"mulq %3\n"			//rv, high = mul_high(b, K)
			"addq %3, %1\n"		//a += K;
			"adcq %4, %2\n"		//b += old_a + (a < K);
			"xorq %4, %0\n"		//rv ^= old_a;
			"xorq %%rdx, %0\n"	//rv ^= high;
			"mulq %3\n"			//rv, high = mul_high(rv, K)
			"addq %4, %0\n"		//rv += old_a;
			"addq %%rdx, %0\n"	//rv += high;
			: "=%rax" (rv), "=r" (a), "=r" (b)
			: "r"(K), "r"(a), "1"(1), "2" (b)
			: "%rdx"
		);*/
		//return rv;
		/*Word rv = a ^ b;
		a += K;
		b += K + ((a < K) ? 1 : 0);
		return (rv * K) + a;*/
	}
	void walk_state(StateWalkingObject *walker) {
		//walker->handle(a); walker->handle(b);walker->handle(c); walker->handle(d); walker->handle(e);
		//walker->handle(f); walker->handle(g); walker->handle(h);
		walker->handle(state_0); walker->handle(state_1); walker->handle(state_2); walker->handle(state_3);
		walker->handle(lfsr_0); walker->handle(lfsr_1); walker->handle(lfsr_01);
		walker->handle(cycle);
	}
};
//class raw_mcx16 : public _mcx<Uint16, 7,0x6595a395a1ec531b> {public: Uint16 raw16() {return _raw_native();}};
class raw_mcx32 : public _mcx<Uint32,13,0x6595a395a1ec531b> {public: Uint32 raw32() {return _raw_native();}};
//class raw_mcx64 : public _mcx<Uint64,27,0x6595a395a1ec531b> {public: Uint64 raw64() {return _raw_native();}};
//POLYMORPHIC_CANDIDATE(mcx, 16)
POLYMORPHIC_CANDIDATE(mcx, 32)
//POLYMORPHIC_CANDIDATE(mcx, 64)
