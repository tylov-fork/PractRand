#include <ctime>
#include <cstdio>
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib>

#include "PractRand/config.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/rng_internals.h"

//for use in seeding:
#include "PractRand/RNGs/arbee.h"
#include "PractRand/RNGs/sha2_based_pool.h"

//for self-tests:
#include "PractRand/RNGs/mt19937.h"
#include "PractRand/RNGs/jsf16.h"
#include "PractRand/RNGs/jsf32.h"
#include "PractRand/RNGs/jsf64.h"
#include "PractRand/RNGs/sfc16.h"
#include "PractRand/RNGs/sfc32.h"
#include "PractRand/RNGs/sfc64.h"
#include "PractRand/RNGs/efiix8x384.h"
#include "PractRand/RNGs/efiix16x384.h"
#include "PractRand/RNGs/efiix32x384.h"
#include "PractRand/RNGs/efiix64x384.h"
#include "PractRand/RNGs/isaac32x256.h"
#include "PractRand/RNGs/isaac64x256.h"
#include "PractRand/RNGs/hc256.h"
#include "PractRand/RNGs/trivium.h"
#include "PractRand/RNGs/xsm32.h"
#include "PractRand/RNGs/xsm64.h"

namespace PractRand {
	void issue_error ( const char *msg ) {
		if (msg) std::fprintf(stderr, "%s", msg);
		std::printf("\n");
		throw msg;
	}
	class SerializingStateWalker : public StateWalkingObject {
	public:
		char *buffer;
		size_t max_size;
		size_t size_used;
		SerializingStateWalker( char *buffer_, size_t max_size_ ) 
			: buffer(buffer_), max_size(max_size_), size_used(0)
		{}
		void push(Uint8 value) {
			size_t index = size_used++;
			if (index < max_size) buffer[index] = value;
		}
		virtual void handle(bool   &v) {push(v ? 1 : 0);}
		virtual void handle(Uint8  &v) {push(v);}
		virtual void handle(Uint16 &v) {Uint8  tmp=Uint8 (v); handle(tmp); tmp=Uint8 (v>> 8); handle(tmp);}
		virtual void handle(Uint32 &v) {Uint16 tmp=Uint16(v); handle(tmp); tmp=Uint16(v>>16); handle(tmp);}
		virtual void handle(Uint64 &v) {Uint32 tmp=Uint32(v); handle(tmp); tmp=Uint32(v>>32); handle(tmp);}
		virtual void handle(float  &v) {
			//uses excess bits to hopefully safely handle floats that might not be exactly IEEE
			bool sign = v < 0;
			v = std::fabs(v);
			int exp;
			double n = std::frexp(v, &exp);
			Uint16 tmp_exp = Uint16( (exp<<1) + (sign?1:0));
			handle(tmp_exp);
			Uint32 tmp_sig = Uint32(std::ldexp(n - 0.5, 32));
			handle(tmp_sig);
		}
		virtual void handle(double &v) {
			//uses excess bits to hopefully safely handle floats that might not be exactly IEEE
			bool sign = v < 0;
			v = std::fabs(v);
			int exp;
			double n = std::frexp(v, &exp);
			Uint32 tmp_exp = Uint32(exp + (sign?0x80000000:0));
			handle(tmp_exp);
			Uint64 tmp_sig = Uint64(std::ldexp(n - 0.5, 64));
			handle(tmp_sig);
		}
		virtual Uint32 get_properties() const {return FLAG_READ_ONLY;}
	};
	class DeserializingStateWalker : public StateWalkingObject {
	public:
		const char *buffer;
		size_t max_size;
		size_t size_used;
		DeserializingStateWalker( const char *buffer_, size_t max_size_ ) 
			: buffer(buffer_), max_size(max_size_), size_used(0)
		{}
		Uint8 pop8() {
			size_t index = size_used++;
			if (index < max_size) return buffer[index];
			else return 0;
		}
		Uint16 pop16() {Uint16 tmp=pop8 (); tmp|=Uint16(pop8 ())<< 8; return tmp;}
		Uint32 pop32() {Uint32 tmp=pop16(); tmp|=Uint32(pop16())<<16; return tmp;}
		Uint64 pop64() {Uint64 tmp=pop32(); tmp|=Uint64(pop32())<<32; return tmp;}
		virtual void handle(bool   &v) {v = pop8() ? true : false;}
		virtual void handle(Uint8  &v) {v = pop8();}
		virtual void handle(Uint16 &v) {v = pop16();}
		virtual void handle(Uint32 &v) {v = pop32();}
		virtual void handle(Uint64 &v) {v = pop64();}
		virtual void handle(float  &v) {
			Uint16 tmp_exp = pop16();
			Uint32 tmp_sig = pop32();
			bool sign = (tmp_exp & 1) ? true : false;
			int exp = int(tmp_exp >> 1);
			if (exp >= 0x4000) exp -= 0x8000;
			v = (sign ? -1.0f : 1.0f) * float(std::ldexp((double)tmp_sig, exp-32));
		}
		virtual void handle(double &v) {
			Uint16 tmp_exp = pop16();
			Uint64 tmp_sig = pop64();
			bool sign = (tmp_exp & 1) ? true : false;
			int exp = int(tmp_exp >> 1);
			if (exp >= 0x4000) exp -= 0x8000;
			v = (sign ? -1.0 : 1.0) * std::ldexp((double)tmp_sig, exp-64);
		}
		virtual Uint32 get_properties() const {return FLAG_READ_ONLY;}
	};
	class GenericIntegerSeedingStateWalker : public StateWalkingObject {
	public:
		PractRand::RNGs::Raw::arbee seeder;
		GenericIntegerSeedingStateWalker(Uint64 seed) : seeder(seed) {}
		virtual void handle(bool   &v) {v = (seeder.raw8() & 1) ? true : false;}
		virtual void handle(Uint8  &v) {v = seeder.raw8 ();}
		virtual void handle(Uint16 &v) {v = seeder.raw16();}
		virtual void handle(Uint32 &v) {v = seeder.raw32();}
		virtual void handle(Uint64 &v) {v = seeder.raw64();}
		virtual void handle(float  &) {issue_error("RNGs with default integer seeding should not contain floating point values");}
		virtual void handle(double &) {issue_error("RNGs with default integer seeding should not contain floating point values");}
		virtual Uint32 get_properties() const {return FLAG_CLUMSY | FLAG_SEEDER;}
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
		virtual Uint32 get_properties() const {return FLAG_CLUMSY | FLAG_SEEDER;}
	};
	class AutoSeedingStateWalker : public StateWalkingObject {
	public:
		enum {SIZE = 4};
		static bool initialized;
		static Uint64 shared_data[SIZE];

		PractRand::RNGs::Raw::arbee seeder;
		//PractRand::RNGs::Polymorphic::sha2_based_pool seeder;
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
			seeder.add_entropy64((Uint64)clock());
			seeder.add_entropy64((Uint64)time(NULL));

#ifdef PRACTRAND_THREAD_LOCAL_STORAGE
			//item 5 (optional): number of times RNGs have been autoseeded in this thread
			//good for making sure repeated autoseedings of the same RNG in the same thread produce different seeds
			static PRACTRAND_THREAD_LOCAL_STORAGE Uint64 autoseeding_count = 0;
			seeder.add_entropy64(autoseeding_count++);
#endif
			//item 6: entropy produced in initialize() (via add_entropy_automatically)
			//if add_entropy_automatically is any good (which it should be on windows & *nix), 
			//  then it can distinguish different runs from each other
			for (int i = 0; i < SIZE; i++) seeder.add_entropy64(shared_data[i]);
			seeder.flush_buffers();
		}
		static void initialize() {
			if (initialized) return;
			initialized = true;
			PractRand::RNGs::Polymorphic::sha2_based_pool entropy_pool;
			//PractRand::RNGs::Polymorphic::arbee entropy_pool;
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
		virtual Uint32 get_properties() const {return FLAG_CLUMSY | FLAG_SEEDER;}
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
		vRNG::~vRNG() {}
		long vRNG::serialize( char *buffer, long buffer_size ) {//returns serialized size, or zero on failure
			SerializingStateWalker serializer(buffer, buffer_size);
			walk_state(&serializer);
			if (serializer.size_used <= (size_t)buffer_size) return serializer.size_used;
			return 0;
		}
		char *vRNG::serialize( size_t *size_ ) {//returns malloced block, or NULL on error, sets *size to size of block
			SerializingStateWalker byte_counter(NULL, 0);
			walk_state(&byte_counter);
			size_t size = byte_counter.size_used;
			*size_ = size;
			if (!size) return NULL;
			char *buffer = (char*)std::malloc(size);
			SerializingStateWalker serializer(buffer, size);
			if (serializer.size_used != size) {
				std::free(buffer);
				return NULL;
			}
			return buffer;
		}
		bool vRNG::deserialize( const char *buffer, long size ) {//returns number of bytes used, or zero on error
			DeserializingStateWalker deserializer(buffer, size);
			walk_state(&deserializer);
			return deserializer.size_used == size;
		}
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
		Uint64 vRNG::get_flags() const {return 0;}
		void vRNG::seek_forward128 (Uint64, Uint64) {}
		void vRNG::seek_backward128(Uint64, Uint64) {}
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
		Uint8  vRNG16::raw8()  {return Uint8(raw16());}
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

		void vRNG::add_entropy8 (Uint8)  {}
		void vRNG::add_entropy16(Uint16) {}
		void vRNG::add_entropy32(Uint32) {}
		void vRNG::add_entropy64(Uint64) {}
		void vRNG::add_entropy_N(const void *_data, size_t length) {
			const Uint8 *data = (const Uint8*)_data;
			for (unsigned long i = 0; i < length; i++) add_entropy8(data[i]);
		}
		bool vRNG::add_entropy_automatically(int milliseconds) {
			return PractRand::Internals::add_entropy_automatically(this, milliseconds);
		}
	}
	void self_test_PractRand() {
		RNGs::Raw::mt19937::self_test();
		RNGs::Raw::hc256::self_test();
		RNGs::Raw::isaac32x256::self_test();
		//RNGs::Raw::trivium::self_test();//failing validation atm
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
			std::fprintf(stderr, "PractRand - endianness configured incorrectly\n");
			issue_error("PractRand - endianness configured incorrectly");
		}
		//RNGs::Raw::rarns16::self_test();
		//RNGs::Raw::rarns32::self_test();
		//RNGs::Raw::rarns64::self_test();
	}
}
