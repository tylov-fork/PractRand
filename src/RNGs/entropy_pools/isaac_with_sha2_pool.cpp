#include <string>
#include "PractRand/config.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/rng_internals.h"

#include "PractRand/sha2.h"

#include "PractRand/RNGs/entropy_pools/isaac_with_sha2_pool.h"
#include "PractRand/RNGs/jsf32.h"
//#include "PractRand/RNGs/jsf64.h"


using namespace PractRand;

std::string PractRand::RNGs::Polymorphic::EntropyPools::isaac_with_sha2_pool::get_name() const {
	return std::string("isaac_with_sha2_pool");
}
Uint32 PractRand::RNGs::Polymorphic::EntropyPools::isaac_with_sha2_pool::raw32() {
	if (isaac_results_left) return isaac_results[--isaac_results_left];
	refill_output_buffer();
	return isaac_results[--isaac_results_left];
}

void PractRand::RNGs::Polymorphic::EntropyPools::isaac_with_sha2_pool::add_entropy8(Uint8 value) {
	input_buffer[--input_buffer_left] = value;
	if (!input_buffer_left) empty_input_buffer();
}
void PractRand::RNGs::Polymorphic::EntropyPools::isaac_with_sha2_pool::add_entropy16(Uint16 value) {
	add_entropy8(Uint8(value   ));
	add_entropy8(Uint8(value>>8));
}
void PractRand::RNGs::Polymorphic::EntropyPools::isaac_with_sha2_pool::add_entropy32(Uint32 value) {
	add_entropy8(Uint8(value    ));
	add_entropy8(Uint8(value>>8 ));
	add_entropy8(Uint8(value>>16));
	add_entropy8(Uint8(value>>24));
}
void PractRand::RNGs::Polymorphic::EntropyPools::isaac_with_sha2_pool::add_entropy64(Uint64 value) {
	add_entropy8(Uint8(value   ));
	add_entropy8(Uint8(value>>8));
	add_entropy8(Uint8(value>>16));
	add_entropy8(Uint8(value>>24));
	add_entropy8(Uint8(value>>32));
	add_entropy8(Uint8(value>>40));
	add_entropy8(Uint8(value>>48));
	add_entropy8(Uint8(value>>56));
}

void PractRand::RNGs::Polymorphic::EntropyPools::isaac_with_sha2_pool::flush_buffers() {
	if (input_buffer_left != INPUT_BUFFER_SIZE) empty_input_buffer();
	if (isaac_results_left != 0) isaac_results_left = 0;//refill_output_buffer();
}

void PractRand::RNGs::Polymorphic::EntropyPools::isaac_with_sha2_pool::seed(Uint64 s) {
	reset_state();
	Raw::jsf32 seeder;
	seeder.seed(s);
	unsigned long i;
	for (i = 0; i < ISAAC_SIZE; i++) isaac_state[i] = seeder.raw32();
	isaac_a = seeder.raw32();
	isaac_b = seeder.raw32();
	isaac_c = seeder.raw32();
	input_buffer_left = INPUT_BUFFER_SIZE;
	refill_output_buffer();
	isaac_results_left = 0;
}
void PractRand::RNGs::Polymorphic::EntropyPools::isaac_with_sha2_pool::reset_state() {
	seed(Uint64(0));
}
void PractRand::RNGs::Polymorphic::EntropyPools::isaac_with_sha2_pool::walk_state(StateWalkingObject *walker) {
	for (int i = 0; i < ISAAC_SIZE; i++) walker->handle(isaac_state[i]);
	for (int i = 0; i < ISAAC_SIZE; i++) walker->handle(isaac_results[i]);
	for (int i = 0; i < INPUT_BUFFER_SIZE; i++) walker->handle(input_buffer[i]);
	walker->handle(isaac_a);
	walker->handle(isaac_b);
	walker->handle(isaac_c);
	walker->handle(input_buffer_left);
	walker->handle(isaac_results_left);
	if (!input_buffer_left || input_buffer_left > INPUT_BUFFER_SIZE) input_buffer_left = INPUT_BUFFER_SIZE;
	if (isaac_results_left > ISAAC_SIZE) isaac_results_left = ISAAC_SIZE;
}
void PractRand::RNGs::Polymorphic::EntropyPools::isaac_with_sha2_pool::empty_input_buffer() {
	if (input_buffer_left == INPUT_BUFFER_SIZE) return;
	PractRand::Crypto::SHA512_CTX workspace;
	PractRand::Crypto::SHA512_Init(&workspace);
	for (unsigned int i = 0; i < input_buffer_left; i++) {
		input_buffer[i] = Uint8(raw32());
	}
	for (unsigned int i = input_buffer_left; i < INPUT_BUFFER_SIZE; i ++) {
		input_buffer[i] ^= Uint8(raw32());
	}
	PractRand::Crypto::SHA512_Update(&workspace, &input_buffer[0], INPUT_BUFFER_SIZE);
	Uint8 temp[64];
	PractRand::Crypto::SHA512_Final(&temp[0], &workspace );
	Uint8 *ptr = &temp[0];
	for (unsigned int i = 0; i < 4; i++) isaac_a += Uint32(*(ptr++)) << (i*8);
	for (unsigned int i = 0; i < 4; i++) isaac_b += Uint32(*(ptr++)) << (i*8);
	for (unsigned int i = 0; i < 4; i++) isaac_c += Uint32(*(ptr++)) << (i*8);
	//12 used
	Uint64 s = 0;
	for (unsigned int i = 0; i < 8; i++) s += Uint32(*(ptr++)) << (i*8);
	//20 used
	PractRand::RNGs::Raw::jsf32 mixer;
	mixer.seed(s);
	unsigned int a = 0;
	for (unsigned int i = 20*8; i < 40*8; i++) {
		a += (temp[i/8] >> (i&7)) & 1;
		isaac_state[a++ % ISAAC_SIZE] += mixer.raw32();
	}
	//40 used
	for (unsigned int i = 0; i < 8; i++) s += Uint32(*(ptr++)) << (i*8);
	mixer.seed(s);
	//48 used
	for (unsigned int i = 48*8; i < 64*8; i++) {
		a += (temp[i/8] >> (i&7)) & 1;
		isaac_state[a++ % ISAAC_SIZE] += mixer.raw32();
	}
	//all 64 used
	input_buffer_left = INPUT_BUFFER_SIZE;
	refill_output_buffer();
	isaac_results_left = 0;
}


#define ind32(mm,x)  (*(Uint32 *)((Uint8 *)(mm) + ((x) & ((ISAAC_SIZE-1)<<2))))
#define rngstep32(mix,a,b,mm,m,m2,r,x) \
{ \
  x = *m;  \
  a = (a^(mix)) + *(m2++); \
  *(m++) = y = ind32(mm,x) + a + b; \
  *(r++) = b = ind32(mm,y>>ISAAC_SIZE_L2) + x; \
}
void PractRand::RNGs::Polymorphic::EntropyPools::isaac_with_sha2_pool::refill_output_buffer() {
	Uint32 *m, *m2, *mend, *r;
	Uint32 x, y;
	m = isaac_state;
	r = isaac_results;
	isaac_b += ++isaac_c;
	for (m = isaac_state, mend = m2 = m+(ISAAC_SIZE/2); m<mend; )
	{
		rngstep32( isaac_a<<13, isaac_a, isaac_b, isaac_state, m, m2, r, x);
		rngstep32( isaac_a>>6 , isaac_a, isaac_b, isaac_state, m, m2, r, x);
		rngstep32( isaac_a<<2 , isaac_a, isaac_b, isaac_state, m, m2, r, x);
		rngstep32( isaac_a>>16, isaac_a, isaac_b, isaac_state, m, m2, r, x);
	}
	for (m2 = isaac_state; m2<mend; )
	{
		rngstep32( isaac_a<<13, isaac_a, isaac_b, isaac_state, m, m2, r, x);
		rngstep32( isaac_a>>6 , isaac_a, isaac_b, isaac_state, m, m2, r, x);
		rngstep32( isaac_a<<2 , isaac_a, isaac_b, isaac_state, m, m2, r, x);
		rngstep32( isaac_a>>16, isaac_a, isaac_b, isaac_state, m, m2, r, x);
	}
	isaac_results_left = ISAAC_SIZE;
}
#undef ind
#undef rngstep
