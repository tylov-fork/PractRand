#define PRACTRAND__RANDF_IMPLEMENTATION(RNG)  {return  float(((RNG).raw32() & ((PractRand::Uint32(1) << 24)-1)) *  float(1.0/16777216.0));}
#define PRACTRAND__RANDLF_IMPLEMENTATION(RNG)  {return double(((RNG).raw64() & ((PractRand::Uint64(1) << 53)-1)) * double(1.0/9007199254740992.0));}

#define PRACTRAND__RANDI_IMPLEMENTATION(max)     \
	Uint32 mask, tmp;\
	max -= 1;\
	mask = max;\
	mask |= mask >> 1; mask |= mask >>  2; mask |= mask >> 4;\
	mask |= mask >> 8; mask |= mask >> 16;\
	do {\
		tmp = raw32() & mask;\
	} while (tmp > max);\
	return tmp;
#define PRACTRAND__RANDLI_IMPLEMENTATION(max)     \
	Uint64 mask, tmp;\
	max -= 1;\
	mask = max;\
	mask |= mask >> 1; mask |= mask >>  2; mask |= mask >>  4;\
	mask |= mask >> 8; mask |= mask >> 16; mask |= mask >> 32;\
	do {\
		tmp = raw64() & mask;\
	} while (tmp > max);\
	return tmp;

#define PRACTRAND__POLYMORPHIC_RNG_BASICS_C8(RNG) \
	Uint8  PractRand::RNGs::Polymorphic:: RNG ::raw8 () {return implementation.raw8();}\
	Uint16 PractRand::RNGs::Polymorphic:: RNG ::raw16() {Uint16 r = implementation.raw8(); return (Uint16(implementation.raw8()) << 8) | r;}\
	Uint32 PractRand::RNGs::Polymorphic:: RNG ::raw32() {\
		Uint32 r = implementation.raw8();\
		r = r | (Uint32(implementation.raw8()) << 8);\
		r = r | (Uint32(implementation.raw8()) << 16);\
		return r | (Uint32(implementation.raw8()) << 24);\
	}\
	Uint64 PractRand::RNGs::Polymorphic:: RNG ::raw64() {\
		Uint64 r = raw32();\
		return r | (Uint64(raw32()) << 32);\
	}\
	std::string PractRand::RNGs::Polymorphic:: RNG ::get_name() const {return # RNG ;}\
	Uint64 PractRand::RNGs::Polymorphic:: RNG ::get_flags() const {return implementation.FLAGS;}\
	void PractRand::RNGs::Polymorphic:: RNG ::walk_state(StateWalkingObject *walker) {\
		implementation.walk_state(walker);\
	}
#define PRACTRAND__POLYMORPHIC_RNG_BASICS_C16(RNG) \
	Uint8  PractRand::RNGs::Polymorphic:: RNG ::raw8 () {return Uint8(implementation.raw16());}\
	Uint16 PractRand::RNGs::Polymorphic:: RNG ::raw16() {return implementation.raw16();}\
	Uint32 PractRand::RNGs::Polymorphic:: RNG ::raw32() {\
		Uint16 r = implementation.raw16();\
		return r | (Uint32(implementation.raw16()) << 16);\
	}\
	Uint64 PractRand::RNGs::Polymorphic:: RNG ::raw64() {\
		Uint64 r = implementation.raw16();\
		r = r | (Uint32(implementation.raw16()) << 16);\
		r = r | (Uint64(implementation.raw16()) << 32);\
		return r | (Uint64(implementation.raw16()) << 48);\
	}\
	std::string PractRand::RNGs::Polymorphic:: RNG ::get_name() const {return # RNG ;}\
	Uint64 PractRand::RNGs::Polymorphic:: RNG ::get_flags() const {return implementation.FLAGS;}\
	void PractRand::RNGs::Polymorphic:: RNG ::walk_state(StateWalkingObject *walker) {\
		implementation.walk_state(walker);\
	}
#define PRACTRAND__POLYMORPHIC_RNG_BASICS_C32(RNG) \
	Uint8  PractRand::RNGs::Polymorphic:: RNG ::raw8 () {return Uint8 (implementation.raw32());}\
	Uint16 PractRand::RNGs::Polymorphic:: RNG ::raw16() {return Uint16(implementation.raw32());}\
	Uint32 PractRand::RNGs::Polymorphic:: RNG ::raw32() {return Uint32(implementation.raw32());}\
	Uint64 PractRand::RNGs::Polymorphic:: RNG ::raw64() {\
		Uint64 r = implementation.raw32();\
		return (r << 32) | implementation.raw32();\
	}\
	std::string PractRand::RNGs::Polymorphic:: RNG ::get_name() const {return # RNG ;}\
	Uint64 PractRand::RNGs::Polymorphic:: RNG ::get_flags() const {return implementation.FLAGS;}\
	void PractRand::RNGs::Polymorphic:: RNG ::walk_state(StateWalkingObject *walker) {\
		implementation.walk_state(walker);\
	}
#define PRACTRAND__POLYMORPHIC_RNG_BASICS_C64(RNG) \
	Uint8  PractRand::RNGs::Polymorphic:: RNG ::raw8 () {return Uint8 (implementation.raw64());}\
	Uint16 PractRand::RNGs::Polymorphic:: RNG ::raw16() {return Uint16(implementation.raw64());}\
	Uint32 PractRand::RNGs::Polymorphic:: RNG ::raw32() {return Uint32(implementation.raw64());}\
	Uint64 PractRand::RNGs::Polymorphic:: RNG ::raw64() {return implementation.raw64();}\
	std::string PractRand::RNGs::Polymorphic:: RNG ::get_name() const {return # RNG ;}\
	Uint64 PractRand::RNGs::Polymorphic:: RNG ::get_flags() const {return implementation.FLAGS;}\
	void PractRand::RNGs::Polymorphic:: RNG ::walk_state(StateWalkingObject *walker) {\
		implementation.walk_state(walker);\
	}



namespace PractRand {
	namespace RNGs {
		class vRNG;
	}
	namespace Internals {
		void add_entropy_automatically( PractRand::RNGs::vRNG *rng, int milliseconds=0 );
	}
}