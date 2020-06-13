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
typedef PractRand::RNGs::Raw::arbee SeederRNG;

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
/*
	on the algorithm:
		examining small windows of the output is never sufficient to learn anything significant, 
			due to relatively large FIFO "iteration_table"
		examining small windows at a stride of ITERATION_SIZE apart might be sufficient, but...
			if the indirection pattern is not predicted/guessed that is meaningless
			if the indirection pattern is predicted, 
				if it doesn't reach back ITERATION_SIZE elements
					that will simply add more unknowns
				if it does reach back ITERATION_SIZE elements
					you still need multiple successive guesses to establish the contents of (a,b,c)
	a possible attack:
		guess a value for (a,b,c) such that the next step will use an identical index in to indirection_table
		use the output and the guessed "b" to figure out "iterated"
		guess a value for "indirect" such that the 3rd step will use an identical index in to indirection_table
		step forward once, use the new output to find the new "iterated"
		step forward once, use the new output to find the new "iterated"
		step forward ITERATION_SIZE-2 times
		repeart the first 5 parts of this
		if the new values of "iterated" match the old values of "indirect" then we've found the state
			or at least a large part of it, enough to start unraveling the rest
			takes 2**(N*8) tries to get a good chance of success though
				(where ** denotes an exponent, and N is 8 for 8 bit efiix, 16 for 16 bit efiix, etc)
			can probably do significantly better with some work though... maybe 2**((N+8)*4) or so?
*/

#define EFIIX_SEED(BITS) \
	SeederRNG seeder;\
	seeder.seed(s1, s2, s3, s4);\
	for (unsigned long w=0; w < INDIRECTION_SIZE; w++) indirection_table[w] = Word(seeder.raw64());\
	for (unsigned long w=0; w < ITERATION_SIZE  ; w++) iteration_table  [w] = Word(seeder.raw64());\
	i = Word(0);\
	a = Word(seeder.raw64());\
	b = Word(seeder.raw64());\
	c = Word(seeder.raw64());\
	for (unsigned long w=0; w < 64; w++) raw ## BITS();\
	seeder.seed(a ^ s1^s2, b ^ s3^s4, c ^ seeder.raw64(), ~s4);\
	for (unsigned long w=0; w < INDIRECTION_SIZE; w++) indirection_table[w] ^= Word(seeder.raw64());\
	for (unsigned long w=0; w < 64+16; w++) raw ## BITS();

/*
	on seeding:
		two different seedings of seeder are needed because:
			the first one gets things to a nice state, but might not be sufficiently secure
			simply skipping some output makes part of the state secure, but...
				parts of indirection_table remain unchanged no matter how much output is skipped
		the second seed to seeder uses
			(a,b,c) because it needs a function of the initial seed that is too complex for attacks
				the 64 steps taken before that make (a,b,c) very complex because of the indirection
			~s4 to guarantee that it can never be the same seed as was used in the first seeding
			(s1,s2,s3) because (a,b,c) may have insufficient entropy when operating on smaller word sizes
				not that that really helps anything significantly...
				realistically, the benefit of adding (s1,s2,s3) is modest at best
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

