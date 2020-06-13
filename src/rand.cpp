#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string>

#include "PractRand/config.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/rng_internals.h"

#include "PractRand/RNGs/entropy_pools/arbee.h"
#include "PractRand/RNGs/entropy_pools/sha2_based_pool.h"

namespace PractRand {
	void issue_error ( const char *msg ) {
		if (msg) fprintf(stderr, msg);
		throw msg;
	}
	class GenericIntegerSeedingStateWalker : public StateWalkingObject {
	public:
		PractRand::RNGs::Raw::EntropyPools::arbee seeder;
		GenericIntegerSeedingStateWalker(Uint64 seed) : seeder() {
			seeder.add_entropy64(seed);
			for (int i = 0; i < 4; i++) seeder.raw32();
		}
		virtual void handle(bool   &v) {v = (seeder.raw8() & 1) ? true : false;}
		virtual void handle(Uint8  &v) {v = seeder.raw8 ();}
		virtual void handle(Uint16 &v) {v = seeder.raw16();}
		virtual void handle(Uint32 &v) {v = seeder.raw32();}
		virtual void handle(Uint64 &v) {v = seeder.raw64();}
		virtual void handle(float  &) {issue_error("RNGs with default integer seeding should not contain floating point values");}
		virtual void handle(double &) {issue_error("RNGs with default integer seeding should not contain floating point values");}
		virtual void handle(void  *&) {}
		virtual Uint32 get_properties() const {return FLAG_CLUMSY;}
	};
	class GenericSeedingStateWalker : public StateWalkingObject {
	public:
		PractRand::RNGs::vRNG *seeder;
		GenericSeedingStateWalker(RNGs::vRNG *seeder_) : seeder(seeder_) {}
		virtual void handle(bool   &v) {v = (seeder->raw8() & 1) ? true : false;}
		virtual void handle(Uint8  &v) {v = seeder->raw8 ();}
		virtual void handle(Uint16 &v) {v = seeder->raw16();}
		virtual void handle(Uint32 &v) {v = seeder->raw32();}
		virtual void handle(Uint64 &v) {v = seeder->raw64();}
		virtual void handle(float  &) {issue_error("RNGs with default seeding should not contain floating point values");}
		virtual void handle(double &) {issue_error("RNGs with default seeding should not contain floating point values");}
		virtual void handle(void  *&) {}
		virtual Uint32 get_properties() const {return FLAG_CLUMSY;}
	};
	class AutoSeedingStateWalker : public StateWalkingObject {
	public:
		enum {SIZE = 4};
		static bool initialized;
		static Uint64 shared_data[SIZE];

		PractRand::RNGs::Raw::EntropyPools::arbee seeder;
		//PractRand::RNGs::Polymorphic::EntropyPools::sha2_based_pool seeder;
		AutoSeedingStateWalker(void *ptr1) : seeder() {
			if (!initialized) initialize();
			
			//item 1: pointer at RNG being autoseeded
			//if the RNG being seeded is global or static then this should be unique to it
			seeder.add_entropy64((Uint64)ptr1);

			//item 2: pointer at an address on the stack
			//hopefully this will at least be functionally unique on a per-thread basis
			seeder.add_entropy64((Uint64)&seeder);

			//items 3 & 4: time in 2 slightly different formats and resolutions
			//the resolution is low but with luck this will distinguish different runs of the program from each other
			seeder.add_entropy64(clock());
			seeder.add_entropy64(time(NULL));

#ifdef PRACTRAND_THREAD_LOCAL_STORAGE
			//item 5 (optional): number of times RNGs have been autoseeded in this thread
			//good for making sure repeated autoseedings of the same RNG in the same thread produce different seeds
			static PRACTRAND_THREAD_LOCAL_STORAGE Uint64 autoseeding_count = 0;
			seeder.add_entropy64(autoseeding_count++);
#endif
			for (int i = 0; i < SIZE; i++) seeder.add_entropy64(shared_data[i]);
			seeder.flush_buffers();
			//for (int i = 0; i < 2; i++) seeder.raw64();
		}
		static void initialize() {
			if (initialized) return;
			initialized = true;
			PractRand::RNGs::Polymorphic::EntropyPools::sha2_based_pool entropy_pool;
			//PractRand::RNGs::Polymorphic::EntropyPools::arbee entropy_pool;
			entropy_pool.add_entropy_automatically(1);
			for (int i = 0; i < SIZE; i++) {
				shared_data[i] = entropy_pool.raw64();
			}
			for (int i = 0; i < SIZE; i++) {
				while (!shared_data[i]) shared_data[i] = entropy_pool.raw64();
			}
		}

		virtual void handle(bool   &v) {v = (seeder.raw8() & 1) ? true : false;}
		virtual void handle(Uint8  &v) {v = seeder.raw8 ();}
		virtual void handle(Uint16 &v) {v = seeder.raw16();}
		virtual void handle(Uint32 &v) {v = seeder.raw32();}
		virtual void handle(Uint64 &v) {v = seeder.raw64();}
		virtual void handle(float  &) {issue_error("RNGs with auto-seeding should not contain floating point values");}
		virtual void handle(double &) {issue_error("RNGs with auto-seeding should not contain floating point values");}
		virtual void handle(void  *&) {}
		virtual Uint32 get_properties() const {return FLAG_CLUMSY;}
	};
	bool AutoSeedingStateWalker::initialized = false;
	Uint64 AutoSeedingStateWalker::shared_data[SIZE] = {0};
	Uint32 randi_fast_implementation(Uint32 random_value, Uint32 max) {
		return Uint32((Uint64(max) * random_value) >> 32);
	}
	StateWalkingObject *int_to_rng_seed(Uint64 i) {
		return new GenericIntegerSeedingStateWalker(i);
	}
	StateWalkingObject *vrng_to_rng_seeder(RNGs::vRNG *rng) {
		return new GenericSeedingStateWalker(rng);
	}
	StateWalkingObject *get_autoseeder(void *ptr) {
		return new AutoSeedingStateWalker(ptr);
	}
	namespace RNGs {
		Uint32 vRNG::randi(Uint32 max) {
			PRACTRAND__RANDI_IMPLEMENTATION(max)
		}
		Uint64 vRNG::randli(Uint64 max) {
			PRACTRAND__RANDLI_IMPLEMENTATION(max)
		}
		Uint32 vRNG::randi_fast(Uint32 max) {
			return randi_fast_implementation(raw32(), max);
		}
		float vRNG::randf() {PRACTRAND__RANDF_IMPLEMENTATION(*this)}
		double vRNG::randlf() {PRACTRAND__RANDLF_IMPLEMENTATION(*this)}
		void vRNG::seed(Uint64 i) {
			GenericIntegerSeedingStateWalker walker(i);
			walk_state(&walker);
			flush_buffers();
		}
		void vRNG::seed(vRNG *rng) {
			GenericSeedingStateWalker walker(rng);
			walk_state(&walker);
			flush_buffers();
		}
		void vRNG::autoseed() {
			AutoSeedingStateWalker walker(this);
			walk_state(&walker);
			flush_buffers();
		}
		Uint64 vRNG::get_flags() const {return 0;}
		void vRNG::seek_forward (Uint64 ) {}
		void vRNG::seek_backward(Uint64 ) {}
		void vRNG::add_entropy8 (Uint8)  {}
		void vRNG::add_entropy16(Uint16) {}
		void vRNG::add_entropy32(Uint32) {}
		void vRNG::add_entropy64(Uint64) {}
		void vRNG::add_entropy_N(const void *_data, size_t length) {
			const Uint8 *data = (const Uint8*)_data;
			for (unsigned long i = 0; i < length; i++) add_entropy8(data[i]);
		}
		void vRNG::add_entropy_automatically(int milliseconds) {
			PractRand::Internals::add_entropy_automatically(this, milliseconds);
		}
		void vRNG::flush_buffers() {}

		int vRNG8::get_native_output_size() const {return 8;}
		Uint16 vRNG8::raw16() {
			Uint16 r = raw8();
			return r | (Uint16(raw8())<<8);
		}
		Uint32 vRNG8::raw32() {
			Uint32 r = raw8();
			r = r | (Uint32(raw8()) << 8);
			r = r | (Uint32(raw8()) << 16);
			return r | (Uint32(raw8()) << 24);
		}
		Uint64 vRNG8::raw64() {
			Uint64 r = raw8();
			r = r | (Uint64(raw8()) << 8);
			r = r | (Uint64(raw8()) << 16);
			r = r | (Uint64(raw8()) << 24);
			r = r | (Uint64(raw8()) << 32);
			r = r | (Uint64(raw8()) << 40);
			r = r | (Uint64(raw8()) << 48);
			return r | (Uint64(raw8()) << 56);
		}

		int vRNG16::get_native_output_size() const {return 16;}
		Uint8 vRNG16::raw8()  {return Uint8(raw16());}
		Uint32 vRNG16::raw32() {
			Uint32 r = raw16();
			return r | (Uint32(raw16()) << 16);
		}
		Uint64 vRNG16::raw64() {
			Uint64 r = raw16();
			r = r | (Uint64(raw16()) << 16);
			r = r | (Uint64(raw16()) << 32);
			return r | (Uint64(raw16()) << 48);
		}

		int vRNG32::get_native_output_size() const {return 32;}
		Uint8  vRNG32::raw8()  {return Uint8 (raw32());}
		Uint16 vRNG32::raw16() {return Uint16(raw32());}
		Uint64 vRNG32::raw64() {
			Uint32 r = raw32();
			return r | (Uint64(raw32()) << 32);
		}

		int vRNG64::get_native_output_size() const {return 64;}
		Uint8  vRNG64::raw8()  {return Uint8 (raw64());}
		Uint16 vRNG64::raw16() {return Uint16(raw64());}
		Uint32 vRNG64::raw32() {return Uint32(raw64());}
	}
	void initialize_PractRand() {
		if (!AutoSeedingStateWalker::initialized) 
			AutoSeedingStateWalker::initialize();
		union {
			Uint64 as64[1];
			Uint32 as32[2];
			Uint16 as16[4];
			Uint8   as8[8];
		};
		as64[0] = 0x0123456789ABCDEFull;
#if defined PRACTRAND_TARGET_IS_LITTLE_ENDIAN
		if (as8[7] != 0x01) {
#elif defined PRACTRAND_TARGET_IS_BIG_ENDIAN
		if (as8[0] != 0x01) {
#endif
			fprintf(stderr, "PractRand - endianness configured incorrectly\n");
			issue_error("PractRand - endianness configured incorrectly");
		}
	}
}

