#include <string>
#include "PractRand/config.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/rng_internals.h"

#include "PractRand/RNGs/entropy_pools/arbee.h"

using namespace PractRand;

//raw:
Uint64 PractRand::RNGs::Raw::EntropyPools::arbee::raw64() {
	Uint64 e = a - ((b << 7) | (b >> (64-7)));
	a = b ^ ((c << 13) | (c >> (64-13)));
	b = ((d << 37) | (d >> (64-37))) + c + i++;
	c = ~d + e;
	d = e + a;
	return b;
}
void PractRand::RNGs::Raw::EntropyPools::arbee::mix() {
	Uint64 e;
	e = (a & b) | (~b & c);
	d ^= ((e << 21) | (e >> (64-21))) + i++;
	e = (a & b) | (b & d) | (d & a);
	c += ((e << 19) | (e >> (64-19)));
}
void PractRand::RNGs::Raw::EntropyPools::arbee::seed(Uint64 s) {
	a = s;
	b = 1;
	c = 2;
	d = 3;
	i = 0;
	raw64();
	raw64();
	raw64();
	mix();
	raw64();
	raw64();
	raw64();
}
void PractRand::RNGs::Raw::EntropyPools::arbee::walk_state(StateWalkingObject *walker) {
	walker->handle(a);
	walker->handle(b);
	walker->handle(c);
	walker->handle(d);
	walker->handle(i);
}
void PractRand::RNGs::Raw::EntropyPools::arbee::add_entropy16(Uint16 value) {
	a ^= value;
	c += value;
	raw64();
}
void PractRand::RNGs::Raw::EntropyPools::arbee::add_entropy32(Uint32 value) {
	a ^= value;
	c += value;
	raw64();
	raw64();
}
void PractRand::RNGs::Raw::EntropyPools::arbee::add_entropy64(Uint64 value) {
	a ^= value;
	c += value;
	raw64();
	raw64();
	mix();
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
void PractRand::RNGs::Polymorphic::EntropyPools::arbee::flush_buffers() {
	implementation.flush_buffers();
}
