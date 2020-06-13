#include <string>
#include "PractRand/config.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/rng_internals.h"

#include "PractRand/RNGs/efiix8x384.h"
#include "PractRand/RNGs/efiix16x384.h"
#include "PractRand/RNGs/efiix32x384.h"
#include "PractRand/RNGs/efiix64x384.h"

#include "PractRand/RNGs/arbee.h"
typedef PractRand::RNGs::Raw::arbee SeederRNG;

using namespace PractRand;
using namespace PractRand::Internals;


PRACTRAND__POLYMORPHIC_RNG_BASICS_C8(efiix8x384)
void PractRand::RNGs::Polymorphic::efiix8x384::seed(Uint64 s) {implementation.seed(s);}
void PractRand::RNGs::Polymorphic::efiix8x384::seed(Uint64 s1, Uint64 s2, Uint64 s3, Uint64 s4) {implementation.seed(s1,s2,s3,s4);}
PRACTRAND__POLYMORPHIC_RNG_BASICS_C16(efiix16x384)
void PractRand::RNGs::Polymorphic::efiix16x384::seed(Uint64 s) {implementation.seed(s);}
void PractRand::RNGs::Polymorphic::efiix16x384::seed(Uint64 s1, Uint64 s2, Uint64 s3, Uint64 s4) {implementation.seed(s1,s2,s3,s4);}
PRACTRAND__POLYMORPHIC_RNG_BASICS_C32(efiix32x384)
void PractRand::RNGs::Polymorphic::efiix32x384::seed(Uint64 s) {implementation.seed(s);}
void PractRand::RNGs::Polymorphic::efiix32x384::seed(Uint64 s1, Uint64 s2, Uint64 s3, Uint64 s4) {implementation.seed(s1,s2,s3,s4);}
PRACTRAND__POLYMORPHIC_RNG_BASICS_C64(efiix64x384)
void PractRand::RNGs::Polymorphic::efiix64x384::seed(Uint64 s) {implementation.seed(s);}
void PractRand::RNGs::Polymorphic::efiix64x384::seed(Uint64 s1, Uint64 s2, Uint64 s3, Uint64 s4) {implementation.seed(s1,s2,s3,s4);}

//typedef Uint8 Uint4;
static inline Uint8 rotate4(Uint8 value, int bits) {
	return (value & ((1<<(4-bits))-1) << bits) | ((value >> (4-bits)) & ((1<<bits)-1));
}
#define EFIIX_ALGORITHM(BITS, SHIFT_AMOUNT) \
	Word iterated = iteration_table  [i % ITERATION_SIZE];\
	Word indirect = indirection_table[c % INDIRECTION_SIZE];\
	indirection_table[c % INDIRECTION_SIZE] = iterated + a;\
	iteration_table  [i % ITERATION_SIZE  ] = indirect;\
	Word old = a ^ b;\
	a = b + i++;\
	b = c + indirect;\
	c = old + rotate ## BITS (c, SHIFT_AMOUNT);\
	return b ^ iterated;
#define EFIIX_SEED(BITS) \
	SeederRNG seeder1; seeder1.seed(s1, s2, s3, s4);\
	for (unsigned long w=0; w < INDIRECTION_SIZE; w++) indirection_table[w] = Word(seeder1.raw64());\
	for (unsigned long w=0; w < ITERATION_SIZE  ; w++) iteration_table  [w] = Word(seeder1.raw64());\
	i = Word(seeder1.raw64()) & ~Word(255);\
	a = Word(seeder1.raw64());\
	b = Word(seeder1.raw64());\
	c = Word(seeder1.raw64());\
	for (unsigned long w=0; w < ITERATION_SIZE + INDIRECTION_SIZE + 4; w++) raw ## BITS();\
	SeederRNG seeder2; s1 = a, s2 = b, s3 = c; s4 = raw ## BITS(); seeder2.seed(s1, s2, s3, s4);\
	for (unsigned long w=0; w < ITERATION_SIZE + INDIRECTION_SIZE + 4; w++) raw ## BITS();\
	for (unsigned long w=0; w < INDIRECTION_SIZE; w++) indirection_table[w] ^= Word(seeder2.raw64());\
	for (unsigned long w=0; w < ITERATION_SIZE + 16; w++) raw ## BITS();

/*Uint8 PractRand::RNGs::Raw::efiix8x384::raw4() {
	//not for real world use
	//trial version for operating on 4 bit integers instead of 8
	//for trying to produce failures / corner cases
	EFIIX_ALGORITHM(4, 2);
}//*/
Uint8 PractRand::RNGs::Raw::efiix8x384::raw8() {
//	Word rv = raw4() & 15; return rv | ((raw4() & 15) << 4);
	EFIIX_ALGORITHM(8, 3)
}
void PractRand::RNGs::Raw::efiix8x384::seed(Uint64 s1, Uint64 s2, Uint64 s3, Uint64 s4) {
	EFIIX_SEED( 8 )
}
void PractRand::RNGs::Raw::efiix8x384::walk_state(StateWalkingObject *walker) {
	for (unsigned long w = 0; w < ITERATION_SIZE; w++) walker->handle(iteration_table[w]);
	for (unsigned long w = 0; w < INDIRECTION_SIZE; w++) walker->handle(indirection_table[w]);
	walker->handle(i);
	walker->handle(a);
	walker->handle(b);
	walker->handle(c);
}


Uint16 PractRand::RNGs::Raw::efiix16x384::raw16() {
	EFIIX_ALGORITHM(16, 7)
}
void PractRand::RNGs::Raw::efiix16x384::seed(Uint64 s1, Uint64 s2, Uint64 s3, Uint64 s4) {
	EFIIX_SEED( 16 )
}
void PractRand::RNGs::Raw::efiix16x384::walk_state(StateWalkingObject *walker) {
	for (unsigned long w = 0; w < ITERATION_SIZE; w++) walker->handle(iteration_table[w]);
	for (unsigned long w = 0; w < INDIRECTION_SIZE; w++) walker->handle(indirection_table[w]);
	walker->handle(i);
	walker->handle(a);
	walker->handle(b);
	walker->handle(c);
}


Uint32 PractRand::RNGs::Raw::efiix32x384::raw32() {
	EFIIX_ALGORITHM(32, 13)
}
void PractRand::RNGs::Raw::efiix32x384::seed(Uint64 s1, Uint64 s2, Uint64 s3, Uint64 s4) {
	EFIIX_SEED( 32 )
}
void PractRand::RNGs::Raw::efiix32x384::walk_state(StateWalkingObject *walker) {
	for (unsigned long w = 0; w < ITERATION_SIZE; w++) walker->handle(iteration_table[w]);
	for (unsigned long w = 0; w < INDIRECTION_SIZE; w++) walker->handle(indirection_table[w]);
	walker->handle(i);
	walker->handle(a);
	walker->handle(b);
	walker->handle(c);
}


Uint64 PractRand::RNGs::Raw::efiix64x384::raw64() {
	EFIIX_ALGORITHM(64, 25)
}
void PractRand::RNGs::Raw::efiix64x384::seed(Uint64 s1, Uint64 s2, Uint64 s3, Uint64 s4) {
	EFIIX_SEED( 64 )
}
void PractRand::RNGs::Raw::efiix64x384::walk_state(StateWalkingObject *walker) {
	for (unsigned long w = 0; w < ITERATION_SIZE; w++) walker->handle(iteration_table[w]);
	for (unsigned long w = 0; w < INDIRECTION_SIZE; w++) walker->handle(indirection_table[w]);
	walker->handle(i);
	walker->handle(a);
	walker->handle(b);
	walker->handle(c);
}

