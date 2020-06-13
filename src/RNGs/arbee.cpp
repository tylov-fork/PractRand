#include <string>
#include "PractRand/config.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/rng_internals.h"

#include "PractRand/RNGs/arbee.h"

using namespace PractRand;
using namespace PractRand::Internals;

//raw:
Uint64 PractRand::RNGs::Raw::arbee::raw64() {
	Uint64 e = a + rotate(b,45);
	a = b ^ rotate(c,13);
	b = c + rotate(d, 37);
	c = e + d + i++;
	d = e + a;
	return d;
}
void PractRand::RNGs::Raw::arbee::mix() {
	Uint64 oi = i;
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) raw64();
		a += rotate(b ^ d, 11) ^ c;
		b += rotate(c ^ a, 13) ^ d;
		c += rotate(d ^ b, 17) ^ a;
		d += rotate(a ^ c, 23) ^ b;
	}
	i = oi;
}
void PractRand::RNGs::Raw::arbee::seed(Uint64 seed1, Uint64 seed2, Uint64 seed3, Uint64 seed4) {
	a = seed1;
	b = seed2;
	c = seed3;
	d = seed4;
	i = 1;
	mix();
}
void PractRand::RNGs::Raw::arbee::seed(Uint64 s) {
	a = s;
	b = 1;
	c = 2;
	d = 3;
	i = 1;
	mix();
}
void PractRand::RNGs::Raw::arbee::walk_state(StateWalkingObject *walker) {
	walker->handle(a);
	walker->handle(b);
	walker->handle(c);
	walker->handle(d);
	walker->handle(i);
	if (walker->is_seeder()) i = 1;
}
void PractRand::RNGs::Raw::arbee::add_entropy_N(const void *_data, size_t length) {
	const Uint8 *data = (const Uint8*) _data;
	//maybe add an ifdef to enable misaligned reads at compile time where appropriate?
	while (length >= 4) {
		length -= 4;
		Uint32 in = Uint32(*(data++));
		in |= Uint32(*(data++)) << 8;
		in |= Uint32(*(data++)) << 16;
		in |= Uint32(*(data++)) << 24;
		add_entropy32(in);
	}
	while (length) {
		Uint8 in = *(data++);
		if (!--length) {
			add_entropy8(in);
			return;
		}
		add_entropy16((Uint16(*(data++)) << 8) | in);
		--length;
	}
}
void PractRand::RNGs::Raw::arbee::add_entropy8(Uint8 value) {
	add_entropy16(value);
}
void PractRand::RNGs::Raw::arbee::add_entropy16(Uint16 value) {
	d ^= value;
	b += value;
	raw64();
}
void PractRand::RNGs::Raw::arbee::add_entropy32(Uint32 value) {
	d ^= value;
	b += value;
	raw64();
	raw64();
}
void PractRand::RNGs::Raw::arbee::add_entropy64(Uint64 value) {
	d ^= value;
	b += value;
	raw64();
	raw64();
	raw64();
	raw64();
}


//polymorphic:
std::string PractRand::RNGs::Polymorphic::arbee::get_name() const {
	return std::string("arbee");
}
Uint8  PractRand::RNGs::Polymorphic::arbee::raw8 () {
	return Uint8(implementation.raw64());
}
Uint16 PractRand::RNGs::Polymorphic::arbee::raw16() {
	return Uint16(implementation.raw64());
}
Uint32 PractRand::RNGs::Polymorphic::arbee::raw32() {
	return Uint32(implementation.raw64());
}
Uint64 PractRand::RNGs::Polymorphic::arbee::raw64() {
	return Uint64(implementation.raw64());
}
void PractRand::RNGs::Polymorphic::arbee::add_entropy_N(const void *data, size_t length) {
	implementation.add_entropy_N(data, length);
}
void PractRand::RNGs::Polymorphic::arbee::add_entropy8 (Uint8  value) {
	implementation.add_entropy8 (value);
}
void PractRand::RNGs::Polymorphic::arbee::add_entropy16(Uint16 value) {
	implementation.add_entropy16(value);
}
void PractRand::RNGs::Polymorphic::arbee::add_entropy32(Uint32 value) {
	implementation.add_entropy32(value);
}
void PractRand::RNGs::Polymorphic::arbee::add_entropy64(Uint64 value) {
	implementation.add_entropy64(value);
}
void PractRand::RNGs::Polymorphic::arbee::seed(Uint64 s) {
	implementation.seed(s);
}
void PractRand::RNGs::Polymorphic::arbee::reset_state() {
	implementation.reset_state();
}
void PractRand::RNGs::Polymorphic::arbee::walk_state(StateWalkingObject *walker) {
	implementation.walk_state(walker);
}
void PractRand::RNGs::Polymorphic::arbee::flush_buffers() {
	implementation.flush_buffers();
}
