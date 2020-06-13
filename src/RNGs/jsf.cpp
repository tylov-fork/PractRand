#include <string>
#include "PractRand/config.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/rng_internals.h"

//#include "PractRand/RNGs/jsf8.h"
#include "PractRand/RNGs/jsf16.h"
#include "PractRand/RNGs/jsf32.h"
#include "PractRand/RNGs/jsf64.h"

using namespace PractRand;

//polymorphic:
PRACTRAND__POLYMORPHIC_RNG_BASICS_C64(jsf64)
void PractRand::RNGs::Polymorphic::jsf64::seed(Uint64 s) {implementation.seed(s);}

PRACTRAND__POLYMORPHIC_RNG_BASICS_C32(jsf32)
void PractRand::RNGs::Polymorphic::jsf32::seed(Uint64 s) {implementation.seed(s);}

PRACTRAND__POLYMORPHIC_RNG_BASICS_C16(jsf16)
void PractRand::RNGs::Polymorphic::jsf16::seed(Uint64 s) {implementation.seed(s);}

//raw:
Uint32 PractRand::RNGs::Raw::jsf32::raw32() {//LOCKED, do not change
	Uint32 e = a - ((b << 27) | (b >> 5));
	a = b ^ ((c << 17) | (c >> 15));
	b = c + d;
	c = d + e;
	d = e + a;
	return b;
}
void PractRand::RNGs::Raw::jsf32::seed(Uint64 s) {//LOCKED, do not change
	a = Uint32(s);
	b = Uint32(s >> 32);
	c = Uint32(s >> 16) ^ (a & b);
	d = (a & b) | ~(c | b);
	if (!(a|b) && !(c|d)) d = 1;
}
void PractRand::RNGs::Raw::jsf32::seed(Uint32 seed1, Uint32 seed2, Uint32 seed3, Uint32 seed4) {//custom seeding
	//LOCKED, do not change
	a = seed1;
	b = seed2;
	c = seed3;
	d = seed4;
	if (!(a|b) && !(c|d)) d = 1;
}
void PractRand::RNGs::Raw::jsf32::walk_state(StateWalkingObject *walker) {
	//LOCKED, do not change
	walker->handle(a);
	walker->handle(b);
	walker->handle(c);
	walker->handle(d);
	if (!(a|b) && !(c|d)) d = 1;
}
Uint64 PractRand::RNGs::Raw::jsf64::raw64() {
	Uint64 e = a - ((b << 51) | (b >> 13));
	a = b ^ ((c << 33) | (c >> 31));
	b = c + d;
	c = d + e;
	d = e + a;
	return b;
}
void PractRand::RNGs::Raw::jsf64::seed(Uint64 s) {
	a = s;
	b = (s >> 8) ^ (s << 5);
	c = ~(s >> 1) ^ (s << 3);
	d = a ^ (b + c);
	if (!(a|b) && !(c|d)) d = 1;
}
void PractRand::RNGs::Raw::jsf64::walk_state(StateWalkingObject *walker) {
	walker->handle(a);
	walker->handle(b);
	walker->handle(c);
	walker->handle(d);
	if (!(a|b) && !(c|d)) d = 1;
}
Uint16 PractRand::RNGs::Raw::jsf16::raw16() {
	Uint16 e = a - ((b << 13) | (b >> 3));
	a = b ^ ((c << 8) | (c >> 8));
	b = c + d;
	c = d + e;
	d = e + a;
	return b;
}
void PractRand::RNGs::Raw::jsf16::seed(Uint64 s) {
	a = Uint16(s);
	b = Uint16(s >> 16);
	c = Uint16(s >> 32);
	d = Uint16(s >> 48);
	if (!(a|b) && !(c|d)) d = 1;
}
void PractRand::RNGs::Raw::jsf16::walk_state(StateWalkingObject *walker) {
	walker->handle(a);
	walker->handle(b);
	walker->handle(c);
	walker->handle(d);
	if (!(a|b) && !(c|d)) d = 1;
}
/*Uint8 PractRand::RNGs::Raw::jsf8::raw8() {
	Uint8 e = a - ((b << 7) | (b >> 1));
	a = b ^ ((c << 4) | (c >> 4));
	b = c + d;
	c = d + e;
	d = e + a;
	return b;
}
void PractRand::RNGs::Raw::jsf8::seed(Uint64 s_) {
	Uint32 s = Uint32(s_ ^ Uint32(s_>>32));
	a = Uint8(s);
	b = Uint8(s >> 8);
	c = Uint8(s >> 16);
	d = Uint8(s >> 24);
	if (!(a|b) && !(c|d)) d = 1;
}
void PractRand::RNGs::Raw::jsf8::walk_state(StateWalkingObject *walker) {
	walker->handle(a);
	walker->handle(b);
	walker->handle(c);
	walker->handle(d);
	if (!(a|b) && !(c|d)) d = 1;
}*/

//light-weight:
Uint32 PractRand::RNGs::jsf32::randi(Uint32 max) {
	PRACTRAND__RANDI_IMPLEMENTATION(max)
}
Uint64 PractRand::RNGs::jsf32::randli(Uint64 max) {
	PRACTRAND__RANDLI_IMPLEMENTATION(max)
}

