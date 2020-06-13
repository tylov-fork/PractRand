#include <string>
#include "PractRand/config.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/rng_internals.h"

#include "PractRand/RNGs/mt19937.h"
#include "PractRand/RNGs/jsf32.h"

using namespace PractRand;

//polymorphic:
PRACTRAND__POLYMORPHIC_RNG_BASICS_C32(mt19937)
void PractRand::RNGs::Polymorphic::mt19937::seed(Uint64 s) {implementation.seed(s);}
void PractRand::RNGs::Polymorphic::mt19937::flush_buffers() {implementation.flush_buffers();}

//raw:
static Uint32 hiBit32( const Uint32 u ) { return u & 0x80000000U; }
//static Uint32 loBit32( const Uint32 u ) const { return u & 0x00000001U; }
static Uint32 loBits32( const Uint32 u ) { return u & 0x7fffffffU; }
//static Uint32 hiBits32( const Uint32 u ) const { return u & 0xfffffffeU; }
static Uint32 mixBits32( const Uint32 u, const Uint32 v ) {
	return hiBit32(u) | loBits32(v);
}
static Uint32 twist32( const Uint32 m, const Uint32 s0, const Uint32 s1 ) {
	enum {TWIST=0x9908b0dfU};
	return m ^ (mixBits32(s0,s1)>>1) ^ ((s1&1) ? Uint32(TWIST) : 0U);
}

void PractRand::RNGs::Raw::mt19937::_advance_state() {//do not change
	Uint32 *p = state;
	unsigned long i;
	for( i = N - M; i--; ++p )
		*p = twist32( p[M], p[0], p[1] );
	for( i = M; --i; ++p )
		*p = twist32( p[M-N], p[0], p[1] );
	*p = twist32( p[M-N], p[0], state[0] );

	used = 0;
}
Uint32 PractRand::RNGs::Raw::mt19937::raw32() {//LOCKED, do not change
	if ( used >= N ) _advance_state();
	
	Uint32 r;
	r = state[used++];
	r ^= (r >> 11);
	r ^= (r <<  7) & 0x9d2c5680u;
	r ^= (r << 15) & 0xefc60000u;
	return ( r ^ (r >> 18) );
}
void PractRand::RNGs::Raw::mt19937::seed(Uint64 s) {//LOCKED, do not change
	if (s <= 0xFFffFFff) {
		state[0]= Uint32(s);
		int i;
		for (i=1; i < N; i++) {
			state[i] = 1812433253UL * (state[i-1] ^ (state[i-1] >> 30)) + i; 
		}
		_advance_state();
	}
	else {
		PractRand::RNGs::Raw::jsf32 seeder;
		seeder.seed(s);
		for (unsigned long i=0; i < N; i++) state[i] = seeder.raw32();
		used = N;
	}
}
void PractRand::RNGs::Raw::mt19937::walk_state(StateWalkingObject *walker) {
	//LOCKED, do not change
	walker->handle(used);
	for (unsigned int i = 0; i < N; i++) walker->handle(state[i]);
	if (used > N) used = N;
}
