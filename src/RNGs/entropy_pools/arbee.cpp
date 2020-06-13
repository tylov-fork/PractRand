#include <string>
#include "PractRand/config.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/rng_internals.h"

#include "PractRand/RNGs/entropy_pools/arbee.h"

using namespace PractRand;

//raw:
Uint64 PractRand::RNGs::Raw::EntropyPools::arbee::do_jsflike() {
	Uint64 e = a - ((b << 51) | (b >> 13));
	a = b ^ ((c << 33) | (c >> 31));
	b = c + ~d;
	c = d + e;
	d = e + a;
	return b;
}
Uint64 PractRand::RNGs::Raw::EntropyPools::arbee::do_mix() {
	Uint64 tmp;
	a += c;
	a += 0x7ed29130fa9e79b9ull;
	tmp = (a<<13)|(a>>51);
	a = b ^ (~tmp)^(tmp<<3);
	a = (a<<13)|(a>>51);
	b = tmp;
	tmp = (b & c) | (a & ~c);
	tmp = (tmp << 19) | (tmp >> 45);
	d += tmp;
	c = (c^a)+b;
	b ^= d;
	return a;
}
void PractRand::RNGs::Raw::EntropyPools::arbee::seed(Uint64 s) {
	a = s;
	b = 1;
	c = 2;
	d = 3;
	do_jsflike();
	do_jsflike();
	do_mix();
	do_mix();
}
void PractRand::RNGs::Raw::EntropyPools::arbee::walk_state(StateWalkingObject *walker) {
	walker->handle(a);
	walker->handle(b);
	walker->handle(c);
	walker->handle(d);
}
void PractRand::RNGs::Raw::EntropyPools::arbee::add_entropy16(Uint16 value) {
	a ^= value;
	c += value;
	do_jsflike();
}
void PractRand::RNGs::Raw::EntropyPools::arbee::add_entropy32(Uint32 value) {
	a ^= value;
	c += value;
	do_jsflike();
	do_mix();
}
void PractRand::RNGs::Raw::EntropyPools::arbee::add_entropy64(Uint64 value) {
	a ^= value;
	c += value;
	do_jsflike();
	do_jsflike();
	do_mix();
	do_mix();
}


//polymorphic:
std::string PractRand::RNGs::Polymorphic::EntropyPools::arbee::get_name() const {
	return std::string("arbee");
}
Uint8  PractRand::RNGs::Polymorphic::EntropyPools::arbee::raw8 () {
	return Uint8(implementation.raw64());
}
Uint16 PractRand::RNGs::Polymorphic::EntropyPools::arbee::raw16() {
	return Uint16(implementation.raw64());
}
Uint32 PractRand::RNGs::Polymorphic::EntropyPools::arbee::raw32() {
	return Uint32(implementation.raw64());
}
Uint64 PractRand::RNGs::Polymorphic::EntropyPools::arbee::raw64() {
	return Uint64(implementation.raw64());
}
void PractRand::RNGs::Polymorphic::EntropyPools::arbee::add_entropy8 (Uint8  value) {
	implementation.add_entropy8 (value);
}
void PractRand::RNGs::Polymorphic::EntropyPools::arbee::add_entropy16(Uint16 value) {
	implementation.add_entropy16(value);
}
void PractRand::RNGs::Polymorphic::EntropyPools::arbee::add_entropy32(Uint32 value) {
	implementation.add_entropy32(value);
}
void PractRand::RNGs::Polymorphic::EntropyPools::arbee::add_entropy64(Uint64 value) {
	implementation.add_entropy64(value);
}
void PractRand::RNGs::Polymorphic::EntropyPools::arbee::seed(Uint64 s) {
	implementation.seed(s);
}
void PractRand::RNGs::Polymorphic::EntropyPools::arbee::reset_state() {
	implementation.reset_state();
}
void PractRand::RNGs::Polymorphic::EntropyPools::arbee::walk_state(StateWalkingObject *walker) {
	implementation.walk_state(walker);
}
