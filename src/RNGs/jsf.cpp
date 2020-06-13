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
	//LOCKED, do not change
	//exception: changed in 0.81 to match Robert Jenkins code
	a = 0xf1ea5eed ^ Uint32(s >> 32);
	b = Uint32(s);
	c = b ^ Uint32(s >> 32);
	d = b;
	for (int i = 0; i < 20; i++) raw32();
}
void PractRand::RNGs::Raw::jsf32::seed(Uint32 seed1, Uint32 seed2, Uint32 seed3, Uint32 seed4) {//custom seeding
	//LOCKED, do not change
	//exception to the locked status - 
	//   when more bad cycles are found, more code might be added to prohibit them
	a = seed1;
	b = seed2;
	c = seed3;
	d = seed4;
	if ((d&0x80093300) == 0) {
		//these block the cycles of length 1
		//should search for cycles of lengths 2 to 2**64 to block them as well, but that's impractical at this time
		if (!a && !b && !c && !d) d++;
		if (a==0x77777777 && b==0x55555555 && c==0x11111111 && d==0x44444444 ) d++;
		if (a==0x5591F2E3 && b==0x69EBA6CD && c==0x2A171E3D && d==0x3FD48890 ) d++;
		if (a==0x47CB8D56 && b==0xAE9B35A7 && c==0x5C78F4A8 && d==0x522240FF ) d++;
	}
}
void PractRand::RNGs::Raw::jsf32::walk_state(StateWalkingObject *walker) {
	//LOCKED, do not change
	//exception to the locked status - 
	//   when more bad cycles are found, more code might be added to prohibit them
	walker->handle(a);
	walker->handle(b);
	walker->handle(c);
	walker->handle(d);
	if (!(walker->get_properties() & StateWalkingObject::FLAG_READONLY) && (d&0x80093300) == 0) {
		//these block the cycles of length 1
		//should search for cycles of lengths 2 to 2**64 to block them as well, but that's impractical at this time
		if (!a && !b && !c && !d) a++;
		if (a==0x77777777 && b==0x55555555 && c==0x11111111 && d==0x44444444 ) d++;
		if (a==0x5591F2E3 && b==0x69EBA6CD && c==0x2A171E3D && d==0x3FD48890 ) d++;
		if (a==0x47CB8D56 && b==0xAE9B35A7 && c==0x5C78F4A8 && d==0x522240FF ) d++;
	}
}
Uint64 PractRand::RNGs::Raw::jsf64::raw64() {
	//LOCKED, do not change
	Uint64 e = a - ((b << 39) | (b >> 25));
	a = b ^ ((c << 11) | (c >> 53));
	b = c + d;
	c = d + e;
	d = e + a;
	return b;
}
void PractRand::RNGs::Raw::jsf64::seed(Uint64 s) {
	//LOCKED, do not change
	a = 0xf1ea5eed;
	b = c = d = s;
	for (int i = 0; i < 20; i++) raw64();
}
void PractRand::RNGs::Raw::jsf64::walk_state(StateWalkingObject *walker) {
	//LOCKED, do not change
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
	if (!(a|b) && !(c|d)) d++;
	for (int i = 0; i < 20; i++) raw16();
}
void PractRand::RNGs::Raw::jsf16::walk_state(StateWalkingObject *walker) {
	walker->handle(a);
	walker->handle(b);
	walker->handle(c);
	walker->handle(d);
	if (!(a|b) && !(c|d)) d++;
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
	Uint32 s = Uint32(s_) ^ Uint32(s_>>32);
	a = Uint8(s);
	b = Uint8(s >> 8);
	c = Uint8(s >> 16);
	d = Uint8(s >> 24);
	if (!(a|b) && !(c|d)) d++;
	for (int i = 0; i < 20; i++) raw8();
}
void PractRand::RNGs::Raw::jsf8::walk_state(StateWalkingObject *walker) {
	walker->handle(a);
	walker->handle(b);
	walker->handle(c);
	walker->handle(d);
	if (!(a|b) && !(c|d)) d++;
}*/

//light-weight:
Uint32 PractRand::RNGs::jsf32::randi(Uint32 max) {
	PRACTRAND__RANDI_IMPLEMENTATION(max)
}
Uint64 PractRand::RNGs::jsf32::randli(Uint64 max) {
	PRACTRAND__RANDLI_IMPLEMENTATION(max)
}
float PractRand::RNGs::jsf32::randf() {
	PRACTRAND__RANDF_IMPLEMENTATION(*this)
}
double PractRand::RNGs::jsf32::randlf() {
	PRACTRAND__RANDLF_IMPLEMENTATION(*this)
}

