#include <string>
#include "PractRand/config.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/rng_internals.h"

#include "PractRand/RNGs/isaac32x256.h"
#include "PractRand/RNGs/isaac64x256.h"
#include "PractRand/RNGs/jsf64.h"

using namespace PractRand;

//polymorphic:
PRACTRAND__POLYMORPHIC_RNG_BASICS_C32(isaac32x256)
void PractRand::RNGs::Polymorphic::isaac32x256::seed(Uint64 s) {implementation.seed(s);}
void PractRand::RNGs::Polymorphic::isaac32x256::seed(vRNG *s) {implementation.seed(s);}
void PractRand::RNGs::Polymorphic::isaac32x256::flush_buffers() {implementation.flush_buffers();}

PRACTRAND__POLYMORPHIC_RNG_BASICS_C64(isaac64x256)
void PractRand::RNGs::Polymorphic::isaac64x256::seed(Uint64 s) {implementation.seed(s);}
void PractRand::RNGs::Polymorphic::isaac64x256::seed(vRNG *s) {implementation.seed(s);}
void PractRand::RNGs::Polymorphic::isaac64x256::flush_buffers() {implementation.flush_buffers();}

//raw:
#define ind32(mm,x)  (*(Uint32 *)((Uint8 *)(mm) + ((x) & ((SIZE-1)<<2))))
#define rngstep32(mix,a,b,mm,m,m2,r,x) \
{ \
  x = *m;  \
  a = (a^(mix)) + *(m2++); \
  *(m++) = y = ind32(mm,x) + a + b; \
  *(r++) = b = ind32(mm,y>>SIZE_L2) + x; \
}
void PractRand::RNGs::Raw::isaac32x256::_advance_state() {//do not change
	Uint32 *m, *m2, *mend, *r;
	Uint32 x, y;
	m = state;
	r = results;
	b += ++c;
	for (m = state, mend = m2 = m+(SIZE/2); m<mend; )
	{
		rngstep32( a<<13, a, b, state, m, m2, r, x);
		rngstep32( a>>6 , a, b, state, m, m2, r, x);
		rngstep32( a<<2 , a, b, state, m, m2, r, x);
		rngstep32( a>>16, a, b, state, m, m2, r, x);
	}
	for (m2 = state; m2<mend; )
	{
		rngstep32( a<<13, a, b, state, m, m2, r, x);
		rngstep32( a>>6 , a, b, state, m, m2, r, x);
		rngstep32( a<<2 , a, b, state, m, m2, r, x);
		rngstep32( a>>16, a, b, state, m, m2, r, x);
	}
	used = 0;
}
Uint32 PractRand::RNGs::Raw::isaac32x256::raw32() {//LOCKED, do not change
	if ( used >= SIZE ) _advance_state();
	return results[used++];
}
void PractRand::RNGs::Raw::isaac32x256::seed(Uint64 s) {//LOCKED, do not change
	PractRand::RNGs::Raw::jsf64 seeder;
	seeder.seed(s);
	for (unsigned long i=0; i < SIZE; i++) state[i] = Uint32(seeder.raw64());
	a = Uint32(seeder.raw64());
	b = Uint32(seeder.raw64());
	c = Uint32(seeder.raw64());
	_advance_state();
	used = SIZE;
}
void PractRand::RNGs::Raw::isaac32x256::seed(vRNG *s) {//LOCKED, do not change
	for (unsigned long i=0; i < SIZE; i++) state[i] = s->raw32();
	a = s->raw32();
	b = s->raw32();
	c = s->raw32();
	_advance_state();
	used = SIZE;
}
void PractRand::RNGs::Raw::isaac32x256::walk_state(StateWalkingObject *walker) {
	//LOCKED, do not change
	for (unsigned int i = 0; i < SIZE; i++) walker->handle(results[i]);
	walker->handle(used);
	for (unsigned int i = 0; i < SIZE; i++) walker->handle(state[i]);
	walker->handle(a);
	walker->handle(b);
	walker->handle(c);
	if (used > SIZE) used = SIZE;

	if (walker->get_properties() & StateWalkingObject::FLAG_CLUMSY) {
		used = SIZE;
	}
}



#define ind64(mm,x)  (*(Uint64 *)((Uint8 *)(mm) + ((x) & ((SIZE-1)<<3))))
#define rngstep64(mix,a,b,mm,m,m2,r,x) \
{ \
  x = *m;  \
  a = (a^(mix)) + *(m2++); \
  *(m++) = y = ind64(mm,x) + a + b; \
  *(r++) = b = ind64(mm,y>>SIZE_L2) + x; \
}
void PractRand::RNGs::Raw::isaac64x256::_advance_state() {//do not change
	Uint64 *m, *m2, *mend, *r;
	Uint64 x, y;
	m = state;
	r = results;
	b += ++c;
	for (m = state, mend = m2 = m+(SIZE/2); m<mend; )
	{
		rngstep64(~(a^(a<<21)), a, b, state, m, m2, r, x);
		rngstep64(  a^(a>> 5) , a, b, state, m, m2, r, x);
		rngstep64(  a^(a<<12) , a, b, state, m, m2, r, x);
		rngstep64(  a^(a>>33) , a, b, state, m, m2, r, x);
	}
	for (m2 = state; m2<mend; )
	{
		rngstep64(~(a^(a<<21)), a, b, state, m, m2, r, x);
		rngstep64(  a^(a>> 5) , a, b, state, m, m2, r, x);
		rngstep64(  a^(a<<12) , a, b, state, m, m2, r, x);
		rngstep64(  a^(a>>33) , a, b, state, m, m2, r, x);
	}
	used = 0;
}
Uint64 PractRand::RNGs::Raw::isaac64x256::raw64() {//LOCKED, do not change
	if ( used >= SIZE ) _advance_state();
	return results[used++];
}
void PractRand::RNGs::Raw::isaac64x256::seed(Uint64 s) {//LOCKED, do not change
	PractRand::RNGs::Raw::jsf64 seeder;
	seeder.seed(s);
	for (unsigned long i=0; i < SIZE; i++) state[i] = Uint32(seeder.raw64());
	a = Uint32(seeder.raw64());
	b = Uint32(seeder.raw64());
	c = Uint32(seeder.raw64());
	_advance_state();
	used = SIZE;
}
void PractRand::RNGs::Raw::isaac64x256::seed(vRNG *s) {//LOCKED, do not change
	for (unsigned long i=0; i < SIZE; i++) state[i] = s->raw32();
	a = s->raw32();
	b = s->raw32();
	c = s->raw32();
	_advance_state();
	used = SIZE;
}
void PractRand::RNGs::Raw::isaac64x256::walk_state(StateWalkingObject *walker) {
	//LOCKED, do not change
	for (unsigned int i = 0; i < SIZE; i++) walker->handle(results[i]);
	walker->handle(used);
	for (unsigned int i = 0; i < SIZE; i++) walker->handle(state[i]);
	walker->handle(a);
	walker->handle(b);
	walker->handle(c);
	if (used > SIZE) used = SIZE;

	if (walker->get_properties() & StateWalkingObject::FLAG_CLUMSY) {
		used = SIZE;
	}
}
