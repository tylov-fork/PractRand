//disabled as it's failing validation at this time
//will have to figure out what I screwed up and fix it for the next version

#if 0
#include <string>
#include "PractRand/config.h"
#include "PractRand/endian.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/rng_internals.h"

#include "PractRand/RNGs/trivium.h"

using namespace PractRand;

//polymorphic:
PRACTRAND__POLYMORPHIC_RNG_BASICS_C64(trivium)
void PractRand::RNGs::Polymorphic::trivium::seed(Uint64 s) {implementation.seed(s);}
void PractRand::RNGs::Polymorphic::trivium::seed(const Uint8 *seed_and_iv, int length) {implementation.seed(seed_and_iv, length);}

static Uint64 shift_array64( Uint64 vec[2], unsigned long bits ) {
	bits -= 64;
	if (!(bits % 64)) return vec[bits/64];
	return (vec[bits / 64] >> (bits & 63)) | (vec[1 + bits / 64] << (64-(bits & 63)));
}
//raw:
Uint64 PractRand::RNGs::Raw::trivium::raw64() {//LOCKED, do not change
	Uint64 tmpa = shift_array64(c, 66) ^ shift_array64(c, 111);
	Uint64 tmpb = shift_array64(a, 66) ^ shift_array64(a, 93);
	Uint64 tmpc = shift_array64(b, 69) ^ shift_array64(b, 84);
	Uint64 newa = shift_array64(a, 69) ^ tmpa ^ (shift_array64(c, 110) & shift_array64(c, 109));
	Uint64 newb = shift_array64(b, 78) ^ tmpb ^ (shift_array64(a, 92) & shift_array64(a, 91));
	Uint64 newc = shift_array64(c, 87) ^ tmpc ^ (shift_array64(b, 83) & shift_array64(b, 82));
	a[1] = a[0]; a[0] = newa;
	b[1] = b[0]; b[0] = newb;
	c[1] = c[0]; c[0] = newc;

	return tmpa ^ tmpb ^ tmpc;
}
void PractRand::RNGs::Raw::trivium::seed(Uint64 s) {//LOCKED, do not change
	Uint8 vec[8];
	for (int i = 0; i < 8; i++) {
		vec[i] = Uint8(s); 
		s >>= 8;
	}
	seed(vec, 8);
}
void PractRand::RNGs::Raw::trivium::seed(const Uint8 *seed_and_iv, int length) {//custom seeding
	//needs verification
	if (length > 20) issue_error("trivium seeded with invalid length");

	union SeedVec{
		Uint8 as8[16];
		Uint64 as64[2];
	};
	SeedVec s;
	int elen = length > 10 ? 10 : length;
	for (int i = 0; i < elen; i++) s.as8[i] = seed_and_iv[i];
	for (int i = elen; i < 16; i++) s.as8[i] = 0;
	for (int i = 0; i < 2; i++) this->a[i] = little_endian_conversion64(s.as64[i]);

	length -= 10;
	length = length > 0 ? length : 0;
	SeedVec iv;
	for (int i = 0; i < length; i++) iv.as8[i] = seed_and_iv[i+10];
	for (int i = length; i < 16; i++) iv.as8[i] = 0;
	for (int i = 0; i < 2; i++) this->b[i] = little_endian_conversion64(iv.as64[i]);

	c[0] = Uint64(7) << (128-111);
	c[1] = 0;
	//for (int i = 0; i < 1152; i+=64) raw64();
	for (int i = 0; i < 18; i++) raw64();
}
void PractRand::RNGs::Raw::trivium::walk_state(StateWalkingObject *walker) {
	//LOCKED, do not change
	walker->handle(a[0]);
	walker->handle(a[1]);
	walker->handle(b[0]);
	walker->handle(b[1]);
	walker->handle(c[0]);
	walker->handle(c[1]);
}
void PractRand::RNGs::Raw::trivium::self_test() {
	Raw::trivium rng;
	Uint8 seed_and_iv[10+8] = {0};
	seed_and_iv[0] = 0x80;
	rng.seed(seed_and_iv, 10+8);
	Uint64 r = rng.raw64();
	if (Uint32(r) != 0x26bfe0fb) {
		printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", Uint8(r>>0), Uint8(r>>8), Uint8(r>>16), Uint8(r>>24), Uint8(r>>32), Uint8(r>>40), Uint8(r>>48), Uint8(r>>56));
		issue_error("trivium::self_test() failed");
	}
}
#endif//0