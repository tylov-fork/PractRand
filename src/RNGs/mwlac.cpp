#include <string>
#include "PractRand/config.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/rng_internals.h"

//#include "PractRand/RNGs/mwlac16.h"
#include "PractRand/RNGs/mwlac32.h"
#include "PractRand/RNGs/mwlac64.h"

using namespace PractRand;

//polymorphic:
PRACTRAND__POLYMORPHIC_RNG_BASICS_C64(mwlac64)
void PractRand::RNGs::Polymorphic::mwlac64::seed(Uint64 s) {implementation.seed(s);}

PRACTRAND__POLYMORPHIC_RNG_BASICS_C32(mwlac32)
void PractRand::RNGs::Polymorphic::mwlac32::seed(Uint64 s) {implementation.seed(s);}

//raw:
Uint32 PractRand::RNGs::Raw::mwlac32::raw32() {
	Uint32 oa = a;
	a = (b * 0xa89d657d) ^ (a >> 13);
	b = c + (oa >> 3);
	c = d;
	d += ~oa;
	return c;
}
void PractRand::RNGs::Raw::mwlac32::seed(Uint64 s) {
	a = Uint32(s);
	b = Uint32(s >> 32);
	c = Uint32(s >> 16) + (a ^ b);
	d = (a & b) | ~(c | b);
}
void PractRand::RNGs::Raw::mwlac32::walk_state(StateWalkingObject *walker) {
	walker->handle(a);
	walker->handle(b);
	walker->handle(c);
	walker->handle(d);
}
Uint64 PractRand::RNGs::Raw::mwlac64::raw64() {
	Uint64 oa = a;
	a = (b * 0xa7c3ab63cf6585f5) ^ (a >> 25);
	b = c + (oa >> 4);
	c = d;
	d += ~oa;
	return c;
}
void PractRand::RNGs::Raw::mwlac64::seed(Uint64 s) {
	a = s;
	b = (s >> 8) ^ (s << 5);
	c = ~(s >> 1) ^ (s << 3);
	d = a ^ (b + c);
}
void PractRand::RNGs::Raw::mwlac64::walk_state(StateWalkingObject *walker) {
	walker->handle(a);
	walker->handle(b);
	walker->handle(c);
	walker->handle(d);
}
/*Uint16 PractRand::RNGs::Raw::mwlac16::raw16() {
	//this version fails DC6:9x1 at 128 GB
	//passes SmallCrush, passes Crush, passes BigCrush
	Uint16 oa = a;
	a = (b * 0x9785) ^ (a >> 7);
	b = c + (oa >> 2);
	c = d;
	d += ~oa;
	return c;
}
void PractRand::RNGs::Raw::mwlac16::seed(Uint64 s) {
	a = Uint16(s);
	b = Uint16(s >> 16);
	c = Uint16(s >> 32);
	d = Uint16(s >> 48);
}
void PractRand::RNGs::Raw::mwlac16::walk_state(StateWalkingObject *walker) {
	walker->handle(a);
	walker->handle(b);
	walker->handle(c);
	walker->handle(d);
}*/


