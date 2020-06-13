#include <string>
#include "PractRand/config.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/rng_internals.h"

#include "PractRand/RNGs/sfc16.h"
#include "PractRand/RNGs/sfc32.h"
#include "PractRand/RNGs/sfc64.h"

//small-fast-counting RNG, original for PractRand
//seems pretty decent so far...
// 1. no multiplication or fancy stuff, should work well on embeded CPUs
// 2. does decently on statistical tests
// 3. very fast... faster than jsf
// 4. no short cycles for 64 bit version, no seeds that are near each other on any cycle
// 5. fairly small
//Engine      quality speed   theory  output    word    size        statespace
//sfc16       1*      5*****  0       16 bit    16 bit  6 bytes     2**48 multicyclic
//sfc32       2**     5*****  0       32 bit    32 bit  12 bytes    2**96 multicyclic
//sfc64       3***    5*****  1*      64 bit    64 bit  24 bytes    2**192 multicyclic


using namespace PractRand;

//polymorphic:
PRACTRAND__POLYMORPHIC_RNG_BASICS_C64(sfc64)
void PractRand::RNGs::Polymorphic::sfc64::seed(Uint64 s) {implementation.seed(s);}

PRACTRAND__POLYMORPHIC_RNG_BASICS_C32(sfc32)
void PractRand::RNGs::Polymorphic::sfc32::seed(Uint64 s) {implementation.seed(s);}

PRACTRAND__POLYMORPHIC_RNG_BASICS_C16(sfc16)
void PractRand::RNGs::Polymorphic::sfc16::seed(Uint64 s) {implementation.seed(s);}

//raw:
Uint16 PractRand::RNGs::Raw::sfc16::raw16() {
	Uint16 tmp = a + b + counter++;
	a = b ^ (b >> 2);
	enum {BARREL_SHIFT = 5};
	b = ((b << BARREL_SHIFT) | (b >> (16-BARREL_SHIFT))) + tmp;
	return tmp;
}
void PractRand::RNGs::Raw::sfc16::seed(Uint64 s) {
	a = Uint16(s);
	b = Uint16(s >> 16);
	counter = Uint16(s >> 32);
	for (int i = 0; i < 5; i++) raw16();
}
void PractRand::RNGs::Raw::sfc16::walk_state(StateWalkingObject *walker) {
	walker->handle(a);
	walker->handle(b);
	walker->handle(counter);
}
Uint32 PractRand::RNGs::Raw::sfc32::raw32() {
	Uint32 tmp = a + b + counter++;
	a = b ^ (b >> 5);
	enum {BARREL_SHIFT = 12};
	b = ((b << BARREL_SHIFT) | (b >> (32-BARREL_SHIFT))) + tmp;
	return tmp;
}
void PractRand::RNGs::Raw::sfc32::seed(Uint64 s) {
	a = Uint32(s >> 0);
	b = Uint32(s >> 32);
	counter = a ^ b;
	for (int i = 0; i < 6; i++) raw32();
}
void PractRand::RNGs::Raw::sfc32::walk_state(StateWalkingObject *walker) {
	walker->handle(a);
	walker->handle(b);
	walker->handle(counter);
}
Uint64 PractRand::RNGs::Raw::sfc64::raw64() {
	Uint64 tmp = a + b + counter++;
	a = b ^ (b >> 7);
	enum {BARREL_SHIFT = 41};
	b = ((b << BARREL_SHIFT) | (b >> (64-BARREL_SHIFT))) + tmp;
	return tmp;
}
void PractRand::RNGs::Raw::sfc64::seed(Uint64 s) {
	counter = s;
	a = b = 0;
	for (int i = 0; i < 7; i++) raw64();
}
void PractRand::RNGs::Raw::sfc64::walk_state(StateWalkingObject *walker) {
	walker->handle(a);
	walker->handle(b);
	walker->handle(counter);
}


