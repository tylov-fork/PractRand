#include <string>
#include "PractRand/config.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/rng_internals.h"

#include "PractRand/RNGs/xsm32.h"
#include "PractRand/RNGs/xsm64.h"

using namespace PractRand;
using namespace PractRand::Internals;

//Xor-Shift and Multiply
//  This RNG supports random access aka seeking.  Also, it uses multiplication 
//    internally, so it will be very slow on hardware that lacks fast 
//    multiplication.  
//
//Engine      quality speed   theory  output    word    size        statespace
//xsm32       3***    3***    2**     32 bit    32 bit  16 bytes    2**95 (cycle length 2**64)
//xsm64       4****   3***    2**     64 bit    64 bit  32 bytes    2**191 (cycle length 2**128)

//polymorphic:
PRACTRAND__POLYMORPHIC_RNG_BASICS_C32(xsm32)
void PractRand::RNGs::Polymorphic::xsm32::seed(Uint64 s) {implementation.seed(s);}
void PractRand::RNGs::Polymorphic::xsm32::seek_forward128 (Uint64 how_far_low64, Uint64 how_far_high64) {implementation.seek_forward (how_far_low64);}
void PractRand::RNGs::Polymorphic::xsm32::seek_backward128(Uint64 how_far_low64, Uint64 how_far_high64) {implementation.seek_backward(how_far_low64);}

PRACTRAND__POLYMORPHIC_RNG_BASICS_C64(xsm64)
void PractRand::RNGs::Polymorphic::xsm64::seed(Uint64 s) {implementation.seed(s);}
void PractRand::RNGs::Polymorphic::xsm64::seek_forward128 (Uint64 how_far_low64, Uint64 how_far_high64) {implementation.seek_forward (how_far_low64, how_far_high64);}
void PractRand::RNGs::Polymorphic::xsm64::seek_backward128(Uint64 how_far_low64, Uint64 how_far_high64) {implementation.seek_backward(how_far_low64, how_far_high64);}

//raw:
Uint32 PractRand::RNGs::Raw::xsm32::raw32() {
	const Uint32 K = 0x6595a395;
	Uint32 tmp = lcg_high;
	tmp ^= tmp >> 16;
	tmp *= K;
	tmp += rotate(tmp ^ lcg_low, 11);
	Uint32 old = lcg_low;
	lcg_low += lcg_adder;
	lcg_high += old + ((lcg_low < lcg_adder) ? 1 : 0);
	old = history;
	history = tmp;
	return tmp + rotate(old, 16);
}
void PractRand::RNGs::Raw::xsm32::step_backwards() {
	bool carry = lcg_low < lcg_adder;
	lcg_low -= lcg_adder;
	lcg_high -= lcg_low + carry;
}
void PractRand::RNGs::Raw::xsm32::seed(Uint64 s) {
	s ^= rotate(s, 21) ^ rotate(s, 39);
	lcg_low = Uint32(s);
	lcg_high = Uint32(s>>32);
	lcg_adder = (lcg_high + lcg_low) | 1;
	raw32();
}
void PractRand::RNGs::Raw::xsm32::walk_state(StateWalkingObject *walker) {
	walker->handle(lcg_low);
	walker->handle(lcg_high);
	walker->handle(lcg_adder);
	walker->handle(history);
	if (walker->is_clumsy() && !walker->is_read_only()) {
		lcg_adder |= 1;
		step_backwards();
		raw32();
	}
}
void PractRand::RNGs::Raw::xsm32::seek_forward (Uint64 how_far) {
	if (!how_far) return;
	how_far -= 1;
	Uint64 lcg_state = lcg_low | (Uint64(lcg_high) << 32);
	lcg_state = fast_forward_lcg64(how_far, lcg_state, 0x0000000100000001ull, lcg_adder);
	lcg_low = Uint32(lcg_state);
	lcg_high = Uint32(lcg_state >> 32);
	raw32();
}
void PractRand::RNGs::Raw::xsm32::seek_backward(Uint64 how_far) {
	seek_forward(0 - how_far);
}


Uint64 PractRand::RNGs::Raw::xsm64::raw64() {
	const Uint64 K = 0x92ec64765925a395ull;
	Uint64 tmp = lcg_high;
	tmp ^= tmp >> 32;
	tmp *= K;
	tmp += rotate(tmp ^ lcg_low, 21);
	Uint64 old = lcg_low;
	lcg_low += lcg_adder;
	lcg_high += old + ((lcg_low < lcg_adder) ? 1 : 0);
	old = history;
	history = tmp;
	return tmp + rotate(old, 32);
}
void PractRand::RNGs::Raw::xsm64::step_backwards() {
	bool carry = lcg_low < lcg_adder;
	lcg_low -= lcg_adder;
	lcg_high -= lcg_low + carry;
}
void PractRand::RNGs::Raw::xsm64::seed(Uint64 s) {
	lcg_low = s ^ rotate64(s, 17) ^ rotate64(s, 29);
	lcg_high = s;
	lcg_adder = lcg_low | 1;
}
void PractRand::RNGs::Raw::xsm64::walk_state(StateWalkingObject *walker) {
	walker->handle(lcg_low);
	walker->handle(lcg_high);
	walker->handle(lcg_adder);
	walker->handle(history);
	if (walker->is_clumsy() && !walker->is_read_only()) {
		lcg_adder |= 1;
		step_backwards();
		raw64();
	}
}
void PractRand::RNGs::Raw::xsm64::seek_forward (Uint64 how_far_low, Uint64 how_far_high) {
	if (!how_far_low && !how_far_high) return;
	if (!how_far_low--) how_far_high--;
	fast_forward_lcg128(how_far_low, how_far_high, 
		lcg_low, lcg_high, 1, 1, lcg_adder, 0
	);
	raw64();
}
void PractRand::RNGs::Raw::xsm64::seek_backward(Uint64 how_far_low, Uint64 how_far_high) {
	if (!how_far_low && !how_far_high) return;
	how_far_low = 1 + ~how_far_low;
	how_far_high = ~how_far_high + (how_far_low ? 0 : 1);
	seek_forward(how_far_low, how_far_high);
}
