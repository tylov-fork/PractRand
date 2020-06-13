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
static inline unsigned long twist32( unsigned long m, unsigned long s0, unsigned long s1 ) {
	static const Uint32 gfsr_twist_table[2] = {0, 0x9908b0dful};
	return m ^ gfsr_twist_table[s1&1] ^ (((s0&0x80000000ul)|(s1&0x7ffffffful))>>1);
}
void PractRand::RNGs::Raw::mt19937::_advance_state() {//LOCKED, do not change
	Uint32 *p = state;
	long i;
	for( i = ARRAY_SIZE - OFFSET; i--; ++p )
		*p = Uint32(twist32( p[OFFSET], p[0], p[1] ));
	for( i = OFFSET; --i; ++p )
		*p = Uint32(twist32( *(p - (ARRAY_SIZE-OFFSET)), p[0], p[1] ));
	*p = Uint32(twist32( *(p - (ARRAY_SIZE-OFFSET)), p[0], state[0] ));

	used = 0;
}
Uint32 PractRand::RNGs::Raw::mt19937::raw32() {//LOCKED, do not change
	//it might seem to make sense to arrange the array backwards
	//so that the comparison could be with 0 instead of ARRAY_SIZE
	//but in testing arranging the array backwards was 5% slower
	//so I didn't do it
	Uint32 r;
	if ( used >= ARRAY_SIZE ) {
		_advance_state();
		r = state[used++];
	}
	else r = state[used++];
	
	r ^= (r >> 11);
	r ^= (r <<  7) & 0x9d2c5680u;
	r ^= (r << 15) & 0xefc60000u;
	return r ^ (r >> 18);
}
void PractRand::RNGs::Raw::mt19937::seed(Uint64 s) {//LOCKED, do not change
	if (s < (Uint64(1) << 32)) {
		state[0]= Uint32(s);
		for (long i=1; i < ARRAY_SIZE; i++) {
			state[i] = 1812433253UL * (state[i-1] ^ (state[i-1] >> 30)) + i; 
		}
		used = ARRAY_SIZE;
	}
	else {
		PractRand::RNGs::Raw::jsf32 seeder;
		seeder.seed(s);
		for (unsigned long i=0; i < ARRAY_SIZE; i++) state[i] = seeder.raw32();
		used = ARRAY_SIZE;
	}
}
void PractRand::RNGs::Raw::mt19937::walk_state(StateWalkingObject *walker) {
	//LOCKED, do not change
	walker->handle(used);
	for (unsigned int i = 0; i < ARRAY_SIZE; i++) walker->handle(state[i]);
	if (used > ARRAY_SIZE) used = ARRAY_SIZE;
}
