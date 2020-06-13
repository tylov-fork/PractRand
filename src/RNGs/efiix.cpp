#include <string>
#include "PractRand/config.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/rng_internals.h"

#include "PractRand/RNGs/efiix8x256.h"
#include "PractRand/RNGs/efiix16x256.h"
#include "PractRand/RNGs/efiix32x256.h"
#include "PractRand/RNGs/efiix64x256.h"
#include "PractRand/RNGs/jsf64.h"

/*
THIS ALGORITHM IS STILL A WORK-IN-PROGRESS

Not just the implementation, but the algorithm itself is still being refined.  
It can and will change between different versions of this library until finalized.  

The current form is believed to produce extremely high quality random numbers (ie 
	have little or no bias), but be relatively weak cryptographically.  

*/


using namespace PractRand;

//polymorphic:
PRACTRAND__POLYMORPHIC_RNG_BASICS_C8(efiix8x256)
void PractRand::RNGs::Polymorphic::efiix8x256::seed(Uint64 s) {implementation.seed(s);}
PRACTRAND__POLYMORPHIC_RNG_BASICS_C16(efiix16x256)
void PractRand::RNGs::Polymorphic::efiix16x256::seed(Uint64 s) {implementation.seed(s);}
PRACTRAND__POLYMORPHIC_RNG_BASICS_C32(efiix32x256)
void PractRand::RNGs::Polymorphic::efiix32x256::seed(Uint64 s) {implementation.seed(s);}
PRACTRAND__POLYMORPHIC_RNG_BASICS_C64(efiix64x256)
void PractRand::RNGs::Polymorphic::efiix64x256::seed(Uint64 s) {implementation.seed(s);}


Uint8 PractRand::RNGs::Raw::efiix8x256::raw8() {
	enum {WORDBITS = 8 * sizeof(Word)};
	enum {SHIFT1 = 3, SHIFT2 = 2};
	Word indirect = table[a % SIZE];
	Word iterated = table[i % SIZE];
	iterated = ((iterated << SHIFT1) | (iterated >> (WORDBITS-SHIFT1)));
	table[a % SIZE] = iterated;
	table[i % SIZE] = c + i;
	Word feed_in = indirect+iterated;
	Word temp = a ^ ((b << SHIFT2) | (b >> (WORDBITS-SHIFT2)));
	a = b + c;
	b = c + feed_in;
	c = feed_in + temp;
	i++;
	return temp;
}
void PractRand::RNGs::Raw::efiix8x256::seed(Uint64 s) {
	PractRand::RNGs::Raw::jsf64 seeder;
	seeder.seed(s);
	for (unsigned long w=0; w < SIZE; w++) table[w] = Word(seeder.raw64());
	i = Word(seeder.raw64());
	a = Word(seeder.raw64());
	b = Word(seeder.raw64());
	c = Word(seeder.raw64());
	for (unsigned long w=0; w < SIZE; w++) raw8();
	a ^= Word(seeder.raw64());
	b ^= Word(seeder.raw64());
	c ^= Word(seeder.raw64());
}
void PractRand::RNGs::Raw::efiix8x256::walk_state(StateWalkingObject *walker) {
	for (unsigned long w = 0; w < SIZE; w++) walker->handle(table[w]);
	walker->handle(i);
	walker->handle(a);
	walker->handle(b);
	walker->handle(c);
}


Uint16 PractRand::RNGs::Raw::efiix16x256::raw16() {
	enum {WORDBITS = 8 * sizeof(Word)};
	enum {SHIFT1 = 5, SHIFT2 = 9};
	Word indirect = table[a % SIZE];
	Word iterated = table[i % SIZE];
	iterated = ((iterated << SHIFT1) | (iterated >> (WORDBITS-SHIFT1)));
	table[a % SIZE] = iterated;
	table[i % SIZE] = c + i;
	Word feed_in = indirect+iterated;
	Word temp = a ^ ((b << SHIFT2) | (b >> (WORDBITS-SHIFT2)));
	a = b + c;
	b = c + feed_in;
	c = feed_in + temp;
	i++;
	return temp;
}
void PractRand::RNGs::Raw::efiix16x256::seed(Uint64 s) {
	PractRand::RNGs::Raw::jsf64 seeder;
	seeder.seed(s);
	for (unsigned long w=0; w < SIZE; w++) table[w] = Word(seeder.raw64());
	i = Word(seeder.raw64());
	a = Word(seeder.raw64());
	b = Word(seeder.raw64());
	c = Word(seeder.raw64());
	for (unsigned long w=0; w < SIZE; w++) raw16();
	a ^= Word(seeder.raw64());
	b ^= Word(seeder.raw64());
	c ^= Word(seeder.raw64());
}
void PractRand::RNGs::Raw::efiix16x256::walk_state(StateWalkingObject *walker) {
	for (unsigned long w = 0; w < SIZE; w++) walker->handle(table[w]);
	walker->handle(i);
	walker->handle(a);
	walker->handle(b);
	walker->handle(c);
}


Uint32 PractRand::RNGs::Raw::efiix32x256::raw32() {
	enum {WORDBITS = 8 * sizeof(Word)};
	enum {SHIFT1 = 11, SHIFT2 = 17};
	Word indirect = table[a % SIZE];
	Word iterated = table[i % SIZE];
	iterated = ((iterated << SHIFT1) | (iterated >> (WORDBITS-SHIFT1)));
	table[a % SIZE] = iterated;
	table[i % SIZE] = c + i;
	Word feed_in = indirect+iterated;
	Word temp = a ^ ((b << SHIFT2) | (b >> (WORDBITS-SHIFT2)));
	a = b + c;
	b = c + feed_in;
	c = feed_in + temp;
	i++;
	return temp;
}
void PractRand::RNGs::Raw::efiix32x256::seed(Uint64 s) {
	PractRand::RNGs::Raw::jsf64 seeder;
	seeder.seed(s);
	for (unsigned long w=0; w < SIZE; w++) table[w] = Word(seeder.raw64());
	i = Word(seeder.raw64());
	a = Word(seeder.raw64());
	b = Word(seeder.raw64());
	c = Word(seeder.raw64());
	for (unsigned long w=0; w < SIZE; w++) raw32();
	a ^= Word(seeder.raw64());
	b ^= Word(seeder.raw64());
	c ^= Word(seeder.raw64());
}
void PractRand::RNGs::Raw::efiix32x256::walk_state(StateWalkingObject *walker) {
	for (unsigned long w = 0; w < SIZE; w++) walker->handle(table[w]);
	walker->handle(i);
	walker->handle(a);
	walker->handle(b);
	walker->handle(c);
}


Uint64 PractRand::RNGs::Raw::efiix64x256::raw64() {
	enum {WORDBITS = 8 * sizeof(Word)};
	enum {SHIFT1 = 23, SHIFT2 = 35};
	Word indirect = table[a % SIZE];
	Word iterated = table[i % SIZE];
	iterated = ((iterated << SHIFT1) | (iterated >> (WORDBITS-SHIFT1)));
	table[a % SIZE] = iterated;
	table[i % SIZE] = c + i;
	Word feed_in = indirect+iterated;
	Word temp = a ^ ((b << SHIFT2) | (b >> (WORDBITS-SHIFT2)));
	a = b + c;
	b = c + feed_in;
	c = feed_in + temp;
	i++;
	return temp;
}
void PractRand::RNGs::Raw::efiix64x256::seed(Uint64 s) {
	PractRand::RNGs::Raw::jsf64 seeder;
	seeder.seed(s);
	for (unsigned long w=0; w < SIZE; w++) table[w] = Word(seeder.raw64());
	i = Word(seeder.raw64());
	a = Word(seeder.raw64());
	b = Word(seeder.raw64());
	c = Word(seeder.raw64());
	for (unsigned long w=0; w < SIZE; w++) raw64();
	a ^= Word(seeder.raw64());
	b ^= Word(seeder.raw64());
	c ^= Word(seeder.raw64());
}
void PractRand::RNGs::Raw::efiix64x256::walk_state(StateWalkingObject *walker) {
	for (unsigned long w = 0; w < SIZE; w++) walker->handle(table[w]);
	walker->handle(i);
	walker->handle(a);
	walker->handle(b);
	walker->handle(c);
}



