#include <string>
#include "PractRand/config.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/rng_internals.h"

#include "PractRand/RNGs/mwlac16.h"
#include "PractRand/RNGs/mwlac32.h"
#include "PractRand/RNGs/mwlac64.h"

using namespace PractRand;

//multiply with lagged accumulated carry (should change "accumulated" to "barrel-shifted")
//excellent:
// 1. good quality... passes all statistical tests to date, though not exhaustive yet
// 2. fast, faster than even jsf on my compiler/computer
// 3. fairly small
// 4. no short cycles for 64 bit version, no seeds that are near each other on any cycle
//Engine      quality speed   theory  output    word    size        statespace
//mwlac16     2**     4****   0       16 bit    16 bit  8 bytes     2**64-1 multicyclic
//mwlac32     3***    4****   0       32 bit    32 bit  16 bytes    2**128-4 multicyclic
//mwlac64     3***    4****   0       64 bit    64 bit  32 bytes    2**256-1 multicyclic


//polymorphic:
PRACTRAND__POLYMORPHIC_RNG_BASICS_C16(mwlac16)
void PractRand::RNGs::Polymorphic::mwlac16::seed(Uint64 s) {implementation.seed(s);}

PRACTRAND__POLYMORPHIC_RNG_BASICS_C32(mwlac32)
void PractRand::RNGs::Polymorphic::mwlac32::seed(Uint64 s) {implementation.seed(s);}

PRACTRAND__POLYMORPHIC_RNG_BASICS_C64(mwlac64)
void PractRand::RNGs::Polymorphic::mwlac64::seed(Uint64 s) {implementation.seed(s);}


//raw:
Uint16 PractRand::RNGs::Raw::mwlac16::raw16() {
	enum {WORDBITS=16, SHIFT1 = 7, SHIFT2 = 5};
	Uint16 tmp = a ^ b ^ counter++;
	a = b * Uint16(0xAD55);
	b = ((b << SHIFT1) | (b >> (WORDBITS-SHIFT1))) + c;
	c = ((c << SHIFT2) | (c >> (WORDBITS-SHIFT2))) + tmp;
	return tmp;
}
void PractRand::RNGs::Raw::mwlac16::seed(Uint64 s) {
	a = Uint16(s);
	b = Uint16(s >> 16);
	c = Uint16(s >> 32);
	counter = Uint16(s >> 48);
	for (int i = 0; i < 5; i++) raw16();
}
void PractRand::RNGs::Raw::mwlac16::walk_state(StateWalkingObject *walker) {
	walker->handle(a);
	walker->handle(b);
	walker->handle(c);
	walker->handle(counter);
}
Uint32 PractRand::RNGs::Raw::mwlac32::raw32() {
	enum {WORDBITS=32, SHIFT1 = 13, SHIFT2 = 9};
	Uint32 tmp = a ^ b ^ counter++;
	a = b * Uint32(0xC74EAD55);
	b = ((b << SHIFT1) | (b >> (WORDBITS-SHIFT1))) + c;
	c = ((c << SHIFT2) | (c >> (WORDBITS-SHIFT2))) + tmp;
	return tmp;
}
void PractRand::RNGs::Raw::mwlac32::seed(Uint64 s) {
	a = Uint32(s);
	b = Uint32(s >> 32);
	c = Uint32(s >> 16) ^ (a + b);
	counter = a ^ b;
	for (int i = 0; i < 7; i++) raw32();
}
void PractRand::RNGs::Raw::mwlac32::walk_state(StateWalkingObject *walker) {
	walker->handle(a);
	walker->handle(b);
	walker->handle(c);
	walker->handle(counter);
}
Uint64 PractRand::RNGs::Raw::mwlac64::raw64() {
	enum {WORDBITS=64, SHIFT1 = 25, SHIFT2 = 18};
	Uint64 tmp = a ^ b ^ counter++;
	a = b * Uint32(0xC74EAD55);
	b = ((b << SHIFT1) | (b >> (WORDBITS-SHIFT1))) + c;
	c = ((c << SHIFT2) | (c >> (WORDBITS-SHIFT2))) + tmp;
	return tmp;
}
void PractRand::RNGs::Raw::mwlac64::seed(Uint64 s) {
	counter = a = b = c = s;
	for (int i = 0; i < 9; i++) raw64();
}
void PractRand::RNGs::Raw::mwlac64::walk_state(StateWalkingObject *walker) {
	walker->handle(a);
	walker->handle(b);
	walker->handle(c);
	walker->handle(counter);
}


