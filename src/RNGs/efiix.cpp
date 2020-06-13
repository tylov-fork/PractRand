#include <cstring>
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
//#include "PractRand/RNGs/trivium.h"

using namespace PractRand;
using namespace PractRand::Internals;


PRACTRAND__POLYMORPHIC_RNG_BASICS_C8(efiix8x384)
void PractRand::RNGs::Polymorphic::efiix8x384::seed(Uint64 s) {implementation.seed(s);}
void PractRand::RNGs::Polymorphic::efiix8x384::seed(Uint64 s1, Uint64 s2, Uint64 s3, Uint64 s4) {implementation.seed(s1,s2,s3,s4);}
std::string PractRand::RNGs::Polymorphic::efiix8x384::get_name() const {return "efiix8x384";}
PRACTRAND__POLYMORPHIC_RNG_BASICS_C16(efiix16x384)
void PractRand::RNGs::Polymorphic::efiix16x384::seed(Uint64 s) {implementation.seed(s);}
void PractRand::RNGs::Polymorphic::efiix16x384::seed(Uint64 s1, Uint64 s2, Uint64 s3, Uint64 s4) {implementation.seed(s1,s2,s3,s4);}
std::string PractRand::RNGs::Polymorphic::efiix16x384::get_name() const {return "efiix16x384";}
PRACTRAND__POLYMORPHIC_RNG_BASICS_C32(efiix32x384)
void PractRand::RNGs::Polymorphic::efiix32x384::seed(Uint64 s) {implementation.seed(s);}
void PractRand::RNGs::Polymorphic::efiix32x384::seed(Uint64 s1, Uint64 s2, Uint64 s3, Uint64 s4) {implementation.seed(s1,s2,s3,s4);}
std::string PractRand::RNGs::Polymorphic::efiix32x384::get_name() const {return "efiix32x384";}
PRACTRAND__POLYMORPHIC_RNG_BASICS_C64(efiix64x384)
void PractRand::RNGs::Polymorphic::efiix64x384::seed(Uint64 s) {implementation.seed(s);}
void PractRand::RNGs::Polymorphic::efiix64x384::seed(Uint64 s1, Uint64 s2, Uint64 s3, Uint64 s4) {implementation.seed(s1,s2,s3,s4);}
std::string PractRand::RNGs::Polymorphic::efiix64x384::get_name() const {return "efiix64x384";}

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
/*	on the algorithm:
		examining small windows of the output is never sufficient to learn anything significant, 
			due to relatively large FIFO "iteration_table"
		examining small windows at a stride of ITERATION_SIZE apart might be sufficient, but...
			if the indirection pattern is not predicted/guessed that is meaningless
			if the indirection pattern is predicted/guessed, 
				if it doesn't reach back ITERATION_SIZE elements
					that will simply add more unknowns
				if it does reach back ITERATION_SIZE elements
					you still need to establish the contents of (a,b,c)
					and besides that's an enormous amount of indirection pattern to figure out
		figuring out any significant portion of the indirection pattern should be very very difficult
	idea for an attack:
		guess values of (a,b,c) and "indirect"
			such that two consecutive indirection indeces will be identical
		guess such values again for a window ITERATION_SIZE steps ahead
		and again for ITERATION_SIZE steps ahead of that
		now guess a sequence of "iterated" values getting fed in to the first window
			such that the indirection index remains constant
		each time you extend the sequence of "iterated" values by 1
			you have to guess N bits (8/16/32/64)
			but you get twice that many bits of output you can check for consistency
		your initial overhead on guesses is 12*N bits
			to pay back the initial overhead you need that many again bits
			then you can start establishing knowledge of a window in the state
		so, 24*N bits, meaning 2**192+ tries needed for the 8 bit variant, 
			2**384+ for the 16 bit variant, etc
		I think optimizations are possible though, at least for the larger word size variants
*/

#define EFIIX_SEED(BITS) \
	PractRand::RNGs::Raw::arbee seeder;\
	seeder.seed(s1, s2, s3, s4);\
	for (unsigned long w=0; w < INDIRECTION_SIZE; w++) indirection_table[w] = Word(seeder.raw64());\
	for (unsigned long w=0; w < ITERATION_SIZE  ; w++) iteration_table  [w] = Word(seeder.raw64());\
	i = Word(0);\
	a = Word(seeder.raw64());\
	b = Word(seeder.raw64());\
	c = Word(seeder.raw64());\
	for (unsigned long w=0; w < 64; w++) raw ## BITS();\
	seeder.raw64(); s1 += seeder.raw64(); s2 += seeder.raw64(); s3 += seeder.raw64();\
	seeder.seed(s1^a, s2^b, s3^c, ~s4);\
	for (unsigned long w=0; w < INDIRECTION_SIZE; w++) indirection_table[w] ^= Word(seeder.raw64());\
	for (unsigned long w=0; w < ITERATION_SIZE+16; w++) raw ## BITS();
/*	on seeding:
		two different seedings of seeder are needed because:
			the first one gets things to a nice state, but might not be sufficiently secure
			simply skipping some output makes part of the state secure, but...
				parts of indirection_table remain unchanged no matter how much output is skipped
		the second seed to seeder uses
			(a,b,c) because it needs a function of the initial seed that is too complex for attacks
				the 64 steps taken before that make (a,b,c) very complex because of the indirection
			~s4 to guarantee that it can never be the same seed as was used in the first seeding
			a function of the first seeders final state
				because (a,b,c) may not be enough bits on smaller word sizes
				not sure that really helps much though
*/

/*
	//research version for operating on 4 bit integers instead of 8
	//for trying to produce failures / corner cases
typedef Uint8 Uint4;
static inline Uint8 rotate4(Uint8 value, int bits) {
	return ((value & ((1<<(4-bits))-1) << bits) | (((value & 15) >> (4-bits)) & ((1<<bits)-1))) & 15;
}
Uint8 PractRand::RNGs::Raw::efiix8x384::raw4() {
	EFIIX_ALGORITHM(4, 2);
}//*/

PractRand::RNGs::Raw::efiix8x384::~efiix8x384() {std::memset(this, 0, sizeof(this));}
PractRand::RNGs::Raw::efiix16x384::~efiix16x384() {std::memset(this, 0, sizeof(this));}
PractRand::RNGs::Raw::efiix32x384::~efiix32x384() {std::memset(this, 0, sizeof(this));}
PractRand::RNGs::Raw::efiix64x384::~efiix64x384() {std::memset(this, 0, sizeof(this));}


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

