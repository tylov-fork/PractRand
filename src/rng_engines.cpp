#include "PractRand/config.h"
#include "PractRand/endian.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_engines.h"

using namespace PractRand;


static Uint64 fast_forward_lcg64 ( Uint64 how_far, Uint64 val, Uint64 mul, Uint64 add ) {
	while (1) {
		if (how_far & 1) val = val * mul + add;
		how_far >>= 1;
		if (how_far == 0) break;
		add = add * mul + add;
		mul = mul * mul;
	}
	return val;
}
static Uint64 rewind_lcg64 ( Uint64 how_far, Uint64 val, Uint64 mul, Uint64 add ) {
	return fast_forward_lcg64( ~how_far + 1, val, mul, add );
}
static Uint32 fast_forward_lcg32m1 ( Uint32 how_far, Uint32 val, Uint32 mul, Uint32 add ) {
	Uint64 tmp;
	while (1) {
		if (how_far & 1) {
			tmp = val * Uint64(mul) + add;
			val = Uint32(tmp) + Uint32(tmp >> 32);
		}
		how_far >>= 1;
		if (how_far == 0) break;
		tmp = add * Uint64(mul) + add;
		add = Uint32(tmp) + Uint32(tmp >> 32);
		tmp = mul * Uint64(mul);
		mul = Uint32(tmp) + Uint32(tmp >> 32);
	}
	return val;
}
static Uint32 pow_mod_N32(Uint32 x, Uint32 pow, Uint32 mod) {
	if (x <= 1) return x;
	if (pow == 0) return 1;
	
	pow -= 1;
	if (pow == 0) return x;

	Uint64 mul = x;

	while (1) {
		if (pow & 1) x = Uint32((x * mul) % mod);
		pow >>= 1;
		if (!pow) return x;
		mul = (mul * mul) % mod;
	}
}
static Uint32 rewind_lcg32m1 ( Uint32 how_far, Uint32 val, Uint32 mul, Uint32 add ) {
	Uint32 mul2 = pow_mod_N32(mul, 4294967295ul-2, 4294967295ul);
	Uint64 tmp = add * Uint64(mul2);
	Uint32 add2 = 4294967295ul - (Uint32(tmp) + Uint32(tmp >> 32));
	return fast_forward_lcg32m1 ( how_far, val, mul2, add2 );
}

template<int a, int c> Uint32 LCG32m1(Uint32 v) {
#if 0
	defined _MSC_VER
	enum {
		fred = a,
		marry = c
	};
	__asm {
		mov eax, v
		mov edx, fred
		mul edx
		add eax, marry
		adc eax, edx
		adc eax, 0
	}
#else
	split_int_64 r;
	r.whole = v;
	r.whole = r.whole * a + c;
	Uint32 bob = r.s.high32;
	r.s.high32 -= bob;
	r.whole += bob;
	if (r.s.high32) {
		Uint32 bob = r.s.high32;
		r.s.high32 -= bob;
		r.whole += bob;
//		if (r.s.high32) {
//			DO_ERROR
//		}
	}
	return r.s.low32;
	//return (Uint32)((r>>32) + (Uint32)r);
#endif
}

Uint32 PractRand::RNGs::Raw::clcg96_32::raw32() {
	i64 = i64 * MULTIPLIER1 + ADDITIVE1;
	i32 = LCG32m1<MULTIPLIER2,ADDITIVE2>(i32);
	return Uint32(i64>>32) + i32;
}
void PractRand::RNGs::Raw::clcg96_32::fast_forward ( Uint64 how_far ) {
	i64 = fast_forward_lcg64 ( how_far, i64, MULTIPLIER1, ADDITIVE1 );
	i32 = fast_forward_lcg32m1 ( Uint32(how_far % ((1ull<<32)-1)), i32, MULTIPLIER2, ADDITIVE2 );
}
void PractRand::RNGs::Raw::clcg96_32::rewind ( Uint64 how_far ) {
	i64 = rewind_lcg64 ( how_far, i64, MULTIPLIER1, ADDITIVE1 );
	i32 = rewind_lcg32m1 ( Uint32(how_far % ((1ull<<32)-1)), i32, MULTIPLIER2, ADDITIVE2 );
}
void PractRand::RNGs::Raw::clcg96_32::seed64 (Uint64 s) {
	i64 = s;
	i32 = 0;
	raw32();
}
void PractRand::RNGs::Raw::clcg96_32::more_seed64 (Uint64 s) {
	i32 += i64 >> 32;
	i64 ^= s;
	raw32();
}




/*
Uint32 PractRand::Generators::clcg96_32::raw32() {
	i64.whole = i64.whole * MULTIPLIER1 + ADDITIVE1;
	Uint64 tmp = i32 * MULTIPLIER2 + ADDITIVE2;
	i32 = Uint32(tmp) + Uint32(tmp >> 32);
	return i64.s.high ^ i32;
}
void PractRand::Generators::clcg96_32::fast_forward ( Uint64 how_far ) {
	i64.whole = fast_forward_lcg64( how_far, i64.whole, MULTIPLIER1, ADDITIVE1 );
	if (how_far > 4294967295ul) how_far %= 4294967295ul;
	i32 = fast_forward_lcg32m1 ( Uint32(how_far), i32, MULTIPLIER2, ADDITIVE2 );
}
void PractRand::Generators::clcg96_32::rewind ( Uint64 how_far ) {
	i64.whole = rewind_lcg64( how_far, i64.whole, MULTIPLIER1, ADDITIVE1 );
	if (how_far > 4294967295ul) how_far %= 4294967295ul;
	i32 = rewind_lcg32m1 ( Uint32(how_far), i32, MULTIPLIER2, ADDITIVE2 );
}
void PractRand::Generators::clcg96_32::seed_32 (Uint32 s) {
	i64.whole = 1492;
	i32 = 0;
	fast_forward(s);
	i32 = i64.s.high;
	i64.whole = s;
}
void PractRand::Generators::clcg96_32::seed_more_32 (Uint32 s) {
	i64.whole += s;
	raw32();
	i64.s.low ^= i64.s.high >> 32;
	raw32();
	i32 ^= i64.s.high;
	i64.s.low ^= i64.s.high >> 32;
	raw32();
	i64.s.low ^= i32;
	raw32();
}
void PractRand::Generators::clcg96_32::seed_64 (Uint64 s) {
	i64.whole = 1492;
	i32 = 0;
	fast_forward(s);
	i32 = i64.s.high ^ Uint32(s >> 32);
	i64.whole = s;
	raw32();
}
void PractRand::Generators::clcg96_32::seed_more_64 (Uint64 s) {
	i64.whole += s;
	raw32();
	i64.s.low ^= i64.s.high >> 32;
	raw32();
	i32 ^= i64.s.high;
	i64.s.low ^= i64.s.high >> 32;
	raw32();
	i64.s.low ^= i32;
	raw32();
}


//helpers used by mt19937
static inline Uint32 mt_hiBit( const Uint32& u ) { return u & 0x80000000U; }
static inline Uint32 mt_loBit( const Uint32& u ) { return u & 0x00000001U; }
static inline Uint32 mt_loBits( const Uint32& u ) { return u & 0x7fffffffU; }
static inline Uint32 mt_mixBits( const Uint32& u, const Uint32& v ) 
	{ return mt_hiBit(u) | mt_loBits(v); }
static inline Uint32 mt_twist( const Uint32& m, const Uint32& s0, const Uint32& s1 ) 
	{ return m ^ (mt_mixBits(s0,s1)>>1) ^ (mt_loBit(s1) ? 0x9908b0dfU : 0U); }

void PractRand::Generators::mt19937::seed_32( Uint32 seed )
{
    for (int i=0; i < N; i++) {
		seed = 1812433253UL * (seed ^ (seed >> 30)) + i;
        state[i] = seed;
    }
	reload();
}

void PractRand::Generators::mt19937::seed_64( Uint64 seed )
{
    for (int i=0; i < N; i++) {
		seed = 1812433253UL * (seed ^ (seed >> 60)) + i;
        state[i] = Uint32(seed >> 32);
    }
	reload();
}

void PractRand::Generators::mt19937::seed_more_32( Uint32 seed )
{
	reload();
	PractRand::Generators::jsf32 r;
	r.seed_32(seed);
	int i;
	for (i = 0; i < 10; i++) r.raw32();
	for (i = 0; i < N; i++) state[i] += r.raw32();
	reload();
}
void PractRand::Generators::mt19937::reload()
{
	// Generate N new values in state
	// Made clearer and faster by Matthew Bellew (matthew.bellew@home.com)
	register Uint32 *p = state;
	register int i;
	for( i = N - M; i--; ++p )
		*p = mt_twist( p[M], p[0], p[1] );
	for( i = M; --i; ++p )
		*p = mt_twist( p[M-N], p[0], p[1] );
	*p = mt_twist( p[M-N], p[0], state[0] );

	left = N;
}
*/


