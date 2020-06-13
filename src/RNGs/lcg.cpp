#include <string>
#include "PractRand/config.h"
//#include "PractRand/endian.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/rng_internals.h"

#include "PractRand/RNGs/clcg96_32.h"
#include "PractRand/RNGs/lcg64_32.h"
//#include "PractRand/RNGs/xlcg64_32.h"

using namespace PractRand;

PRACTRAND__POLYMORPHIC_RNG_BASICS_C32(clcg96_32)
void PractRand::RNGs::Polymorphic::clcg96_32::seed(Uint64 s) {implementation.seed(s);}
void PractRand::RNGs::Polymorphic::clcg96_32::seek_forward(Uint64 how_far) {implementation.seek_forward(how_far);}
void PractRand::RNGs::Polymorphic::clcg96_32::seek_backward(Uint64 how_far) {implementation.seek_backward(how_far);}

PRACTRAND__POLYMORPHIC_RNG_BASICS_C32(lcg64_32)
void PractRand::RNGs::Polymorphic::lcg64_32::seed(Uint64 s) {implementation.seed(s);}
void PractRand::RNGs::Polymorphic::lcg64_32::seek_forward(Uint64 how_far) {implementation.seek_forward(how_far);}
void PractRand::RNGs::Polymorphic::lcg64_32::seek_backward(Uint64 how_far) {implementation.seek_backward(how_far);}

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
/*static Uint64 rewind_lcg64 ( Uint64 how_far, Uint64 val, Uint64 mul, Uint64 add ) {
	return fast_forward_lcg64( ~how_far + 1, val, mul, add );
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
}*/
static Uint32 fast_forward_lcg32c ( Uint32 how_far, Uint32 val, Uint32 mul, Uint32 add, Uint32 mod ) {
	if (val >= mod) val %= mod;
	if (mul >= mod) mul %= mod;
	if (add >= mod) add %= mod;
	if (how_far >= mod) how_far %= mod;
	while (1) {
		if (how_far & 1) {
			val = Uint32((Uint64(val) * mul + add) % mod);
		}
		how_far >>= 1;
		if (how_far == 0) break;
		add = Uint32((Uint64(add) * mul + add) % mod);
		mul = Uint32((Uint64(mul) * mul) % mod);
	}
	return val;
}


Uint32 PractRand::RNGs::Raw::clcg96_32::raw32() {
	lcg64 = lcg64 * 2147001325 + 715136305;//period 2**64

	Uint64 tmp = Uint64(lcg32) * 1579544716;//
	lcg32 = Uint32(tmp & 0x7FffFFff) + Uint32(tmp >> 33) + 1;

	return Uint32(lcg64>>32) + lcg32;
}
void PractRand::RNGs::Raw::clcg96_32::seed(Uint64 s) {
	lcg64 = s;
	if (((s >> 32) & 0x7FffFFff) != 0x7FffFFff) lcg32 = Uint32(s>>32) & 0x7FffFFff;
	else lcg32 = Uint32(s % 0x7FffFFff);
}
void PractRand::RNGs::Raw::clcg96_32::seek_forward(Uint64 how_far) {
	lcg64 = fast_forward_lcg64 (how_far, lcg64, 2147001325, 715136305);
	lcg32 = fast_forward_lcg32c(Uint32(how_far%0x7FffFFff), lcg32, 1579544716, 1, 0x7FffFFff);
}
void PractRand::RNGs::Raw::clcg96_32::seek_backward(Uint64 how_far) {
	lcg64 = fast_forward_lcg64 (0-how_far, lcg64, 2147001325, 715136305);
	if (how_far > 0x7FffFFff) how_far %= 0x7FffFFff;
	how_far = 0x7FffFFff - how_far;
	if (how_far) lcg32 = fast_forward_lcg32c(Uint32(how_far), lcg32, 1579544716, 1, 0x7FffFFff);
}
void PractRand::RNGs::Raw::clcg96_32::walk_state(StateWalkingObject *walker) {
	walker->handle(lcg64);
	walker->handle(lcg32);
}

Uint32 PractRand::RNGs::Raw::lcg64_32::raw32() {
	lcg64 = lcg64 * 2147001325 + 123456789;
	return Uint32(lcg64>>32);
}
void PractRand::RNGs::Raw::lcg64_32::seed(Uint64 s) {
	lcg64 = s;
}
void PractRand::RNGs::Raw::lcg64_32::seek_forward(Uint64 how_far) {
	lcg64 = fast_forward_lcg64 (how_far, lcg64, 2147001325, 715136305);
}
void PractRand::RNGs::Raw::lcg64_32::seek_backward(Uint64 how_far) {
	lcg64 = fast_forward_lcg64 (0-how_far, lcg64, 2147001325, 715136305);
}
void PractRand::RNGs::Raw::lcg64_32::walk_state(StateWalkingObject *walker) {
	walker->handle(lcg64);
}


