#ifndef _practrand_rng_basics_h
#define _practrand_rng_basics_h

#ifndef __PRACTRAND_CONFIG_H__
#include "PractRand/config.h"
#endif //__PRACTRAND_CONFIG_H__

namespace PractRand {
	void initialize_PractRand();
	void self_test_PractRand();
	void issue_error(const char *msg = 0);

	class StateWalkingObject;

	class SEED_AUTO_TYPE {};
	class SEED_NONE_TYPE {};
	static SEED_AUTO_TYPE SEED_AUTO;
	static SEED_NONE_TYPE SEED_NONE;

	namespace RNGs {
		class vRNG {
		public:
		//constuctors, destructors, seeding, serialization, & low level state manipulation:
			//vRNG(Uint64 seed_) {seed_64(seed_);}
			//vRNG(vRNG *rng_) {seed(rng_);}
			//vRNG(_dummy_SeedingTypeAuto *) {autoseed();}
			//vRNG(_dummy_SeedingTypeNone *) {}
			virtual ~vRNG();
			virtual void seed(Uint64 seed);
			void seed(vRNG *rng);
			void autoseed();
			long serialize( char *buffer, long buffer_size );//returns serialized size, or zero on failure
			char *serialize( size_t *size );//returns malloced block, or NULL on error, sets *size to size of block
			bool deserialize( const char *buffer, long size );//returns true on success, false on failure
			virtual void walk_state(StateWalkingObject *) = 0;


			//raw random bits
			virtual Uint8  raw8 () = 0;
			virtual Uint16 raw16() = 0;
			virtual Uint32 raw32() = 0;
			virtual Uint64 raw64() = 0;
			//virtual void raw_N(Uint8 *, size_t length) = 0;

		//uniform distributions
			Uint32 randi(Uint32 max);
			Uint32 randi(Uint32 min, Uint32 max) {return randi(max-min)+min;}
			Uint32 randi_fast(Uint32 max);
			Uint32 randi_fast(Uint32 min, Uint32 max) {return randi_fast(max-min)+min;}
			Uint64 randli(Uint64 max);
			Uint64 randli(Uint64 min, Uint64 max) {return randli(max-min)+min;}
			float randf();
			float randf(float max) {return randf() * max;}
			float randf(float min, float max) {return randf() * (max-min) + min;}
			double randlf();
			double randlf(double max) {return randlf() * max;}
			double randlf(double min, double max) {return randlf() * (max-min) + min;}

		//metadata functions
			virtual Uint64 get_flags() const;
			virtual std::string get_name() const = 0;
			virtual int get_native_output_size() const = 0;
			//virtual Uint32 get_property(int index) const;
			//virtual bool set_parameter(const char *) const;

		//exotic methods (not supported by many implementations):
		//exotic methods 1: random access
			virtual void seek_forward128 (Uint64 how_far_low64, Uint64 how_far_high64);
			virtual void seek_backward128(Uint64 how_far_low64, Uint64 how_far_high64);
			void seek_forward (Uint64 how_far) {seek_forward128 (how_far, 0);}
			void seek_backward(Uint64 how_far) {seek_backward128(how_far, 0);}

		//exotic methods 2: entropy pooling
			virtual void add_entropy8 (Uint8 );
			virtual void add_entropy16(Uint16);
			virtual void add_entropy32(Uint32);
			virtual void add_entropy64(Uint64);
			virtual void add_entropy_N(const void *, size_t length);
			//add_entropy_automatically returns true if a good amount (> 128 bits) of entropy was added
			//the milliseconds parameter is the maximum amount of time it is allowed spend collecting entropy
			virtual bool add_entropy_automatically(int milliseconds = 0);

		//exotic methods 3: misc.
			virtual void flush_buffers();

		//Boost / C++0x TR1 compatibility:
#if defined PRACTRAND_BOOST_COMPATIBILITY
			typedef Uint64 result_type;
			result_type operator()() {return raw64();}
			static const bool has_fixed_value = true;
			static const result_type min_value = 0;
			static const result_type max_value = ~(result_type)0;
			result_type min() const {return min_value;}
			result_type max() const {return max_value;}
#endif
		};
		class vRNG8 : public vRNG {
		public:
			enum {OUTPUT_BITS = 8};
			virtual Uint16 raw16();
			virtual Uint32 raw32();
			virtual Uint64 raw64();
			virtual int get_native_output_size() const;
		};
		class vRNG16 : public vRNG {
		public:
			enum {OUTPUT_BITS = 16};
			virtual Uint8  raw8 ();
			virtual Uint32 raw32();
			virtual Uint64 raw64();
			virtual int get_native_output_size() const;
		};
		class vRNG32 : public vRNG {
		public:
			enum {OUTPUT_BITS = 32};
			virtual Uint8  raw8 ();
			virtual Uint16 raw16();
			virtual Uint64 raw64();
			virtual int get_native_output_size() const;
		};
		class vRNG64 : public vRNG {
		public:
			enum {OUTPUT_BITS = 64};
			virtual Uint8  raw8 ();
			virtual Uint16 raw16();
			virtual Uint32 raw32();
			virtual int get_native_output_size() const;
		};
		namespace OUTPUT_TYPES {enum {
	//		SIMPLE_1 = 0,     //one of 8,16,32,64 as _raw()
			NORMAL_1 = 1,     //one of 8,16,32,64 as raw ## X ()
			NORMAL_ALL = 2,   //all of 8,16,32,64 as raw ## X ()
	//		TEMPLATED_ALL = 3,//all of 8,16,32,64 as raw ## X() AND as _raw<X>()
		};}
//		enum DISTRIBUTIONS_TYPE {
//			DISTRIBUTIONS_TYPE__NONE = 0,
//			DISTRIBUTIONS_TYPE__NORMAL = 1
//		};
		namespace SEEDING_TYPES { enum {
			SEEDING_TYPE_INT = 1,//seed(Uint64)
			SEEDING_TYPE_VRNG = 2//seed(vRNG *)
		};}
//		enum INTERNAL_STATES_VALID {
//			INTERNAL_STATES_VALID__LOW = 0,//don't manually modify state
//			INTERNAL_STATES_VALID__MED = 1,//manually modification of state not recommended, but unlikely to go horribly wrong
//			INTERNAL_STATES_VALID__HIGH = 2,//may manually modify state; don't expect decent results from low entropy states though
//			INTERNAL_STATES_VALID__ALL = 3//all states pretty much equally valid
//		};
//		enum STATE_TRANSITION_TYPE {
//			UNKNOWN = 0,
//			IRREVERSIBLE_MULTI_CYCLIC = 1,
//			REVERSIBLE_MULTI_CYCLIC = 2,
//			REVERSIBLE_SINGLE_CYCLE = 3,
//			IRREVERSIBLE_SINGLE_CYCLE = 4
//		};
		namespace FLAG { enum {
			SUPPORTS_FASTFORWARD = 1<<0,//also includes rewind
			SUPPORTS_ENTROPY_ACCUMULATION = 1<<1,//supports add_entropy*
			CRYPTOGRAPHIC_OUTPUT = 1<<2,
			CRYPTOGRAPHIC_INPUT = 1<<3,//only meaningful with SUPPORTS_ENTROPY_ACCUMULATION
			USES_SPECIFIED = 1<<4,//true if all the other USES_* flags are properly set
			USES_MULTIPLICATION = 1<<5,
			USES_COMPLEX_INSTRUCTIONS = 1<<6,//division, sqrt, exp, log, etc
			USES_VARIABLE_SHIFTS = 1<<7,
			USES_INDIRECTION = 1<<8,
			USES_CYCLIC_BUFFER = 1<<9,
			USES_FLOW_CONTROL = 1<<10,//very simple flow control is not counted
			USES_BIT_SCANS = 1<<11,//bsf & bsr opcodes on x86
			ENDIAN_SAFE = 1<<12,//single flag for output (raw*) and input (add_entropy*)
			OUTPUT_IS_BUFFERED = 1<<13,
			OUTPUT_IS_HASHED = 1<<14,

			NEEDS_GENERIC_SEEDING = 1<<31,
		};}
		typedef vRNG PolymorphicRNG;
		typedef vRNG8 PolymorphicRNG8;
		typedef vRNG16 PolymorphicRNG16;
		typedef vRNG32 PolymorphicRNG32;
		typedef vRNG64 PolymorphicRNG64;
		namespace Polymorphic {
			using PractRand::RNGs::vRNG;
			using PractRand::RNGs::vRNG8;
			using PractRand::RNGs::vRNG16;
			using PractRand::RNGs::vRNG32;
			using PractRand::RNGs::vRNG64;
			typedef vRNG PolymorphicRNG;
			typedef vRNG8 PolymorphicRNG8;
			typedef vRNG16 PolymorphicRNG16;
			typedef vRNG32 PolymorphicRNG32;
			typedef vRNG64 PolymorphicRNG64;
		}
	}//namespace RNGs
}//namespace PractRand
#endif//_practrand_rng_basics_h
