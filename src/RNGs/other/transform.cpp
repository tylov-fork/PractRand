#include <string>
#include <sstream>
#include "PractRand/config.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/rng_internals.h"

#include "PractRand/RNGs/other/transform.h"

namespace PractRand {
	namespace RNGs {
		namespace Polymorphic {
			namespace NotRecommended {
				void Transform64::seed(Uint64 s) {base_rng->seed(s);}
				Uint64 Transform64::get_flags() const {return base_rng->get_flags() | FLAG::USES_INDIRECTION;}
				Transform64::~Transform64() {delete base_rng;}
				void Transform32::seed(Uint64 s) {base_rng->seed(s);}
				Uint64 Transform32::get_flags() const {return base_rng->get_flags() | FLAG::USES_INDIRECTION;}
				Transform32::~Transform32() {delete base_rng;}
				void Transform16::seed(Uint64 s) {base_rng->seed(s);}
				Uint64 Transform16::get_flags() const {return base_rng->get_flags() | FLAG::USES_INDIRECTION;}
				Transform16::~Transform16() {delete base_rng;}
				void Transform8::seed(Uint64 s) {base_rng->seed(s);}
				Uint64 Transform8::get_flags() const {return base_rng->get_flags() | FLAG::USES_INDIRECTION;}
				Transform8::~Transform8() {delete base_rng;}



				Uint64 BaysDurhamShuffle64::raw64() {
					Uint64 &storage = table[prev];
					Uint64 rv = storage;
					storage = base_rng->raw64();
					prev = Uint8(storage >> index_shift) & index_mask;
					return rv;
				}
				void BaysDurhamShuffle64::seed(Uint64 s) {
					base_rng->seed(s);
					for (int i = 0; i <= index_mask; i++) 
						table[i] = base_rng->raw64();
					for (int i = 0; i <= index_mask; i++) {raw64();raw64();}
				}
				void BaysDurhamShuffle64::walk_state(StateWalkingObject *walker) {
					base_rng->walk_state(walker);
					if (!(walker->get_properties() && walker->FLAG_CLUMSY)) {
						walker->handle(index_mask);
						walker->handle(index_shift);
					}
					for (int i = 0; i <= index_mask; i++) walker->handle(table[i]);
					walker->handle(prev);
					prev &= index_mask;
				}
				std::string BaysDurhamShuffle64::get_name() const {
					std::ostringstream tmp;
					tmp << "[BDS" << (1+int(index_mask)) << "]" << base_rng->get_name();
					return tmp.str();
				}

				Uint32 BaysDurhamShuffle32::raw32() {
					Uint32 &storage = table[prev];
					Uint32 rv = storage;
					storage = base_rng->raw32();
					prev = Uint8(storage >> index_shift) & index_mask;
					return rv;
				}
				void BaysDurhamShuffle32::seed(Uint64 s) {
					base_rng->seed(s);
					for (int i = 0; i <= index_mask; i++) 
						table[i] = base_rng->raw32();
					for (int i = 0; i <= index_mask; i++)  {raw32();raw32();}
				}
				void BaysDurhamShuffle32::walk_state(StateWalkingObject *walker) {
					base_rng->walk_state(walker);
					if (!(walker->get_properties() && walker->FLAG_CLUMSY)) {
						walker->handle(index_mask);
						walker->handle(index_shift);
					}
					for (int i = 0; i <= index_mask; i++) walker->handle(table[i]);
					walker->handle(prev);
					prev &= index_mask;
				}
				std::string BaysDurhamShuffle32::get_name() const {
					std::ostringstream tmp;
					tmp << "[BDS" << (1+int(index_mask)) << "]" << base_rng->get_name();
					return tmp.str();
				}

				Uint16 BaysDurhamShuffle16::raw16() {
					Uint16 &storage = table[prev];
					Uint16 rv = storage;
					storage = base_rng->raw32();
					prev = Uint8(storage >> index_shift) & index_mask;
					return rv;
				}
				void BaysDurhamShuffle16::seed(Uint64 s) {
					base_rng->seed(s);
					for (int i = 0; i <= index_mask; i++) 
						table[i] = base_rng->raw32();
					for (int i = 0; i <= index_mask; i++)  {raw16();raw16();}
				}
				void BaysDurhamShuffle16::walk_state(StateWalkingObject *walker) {
					base_rng->walk_state(walker);
					if (!(walker->get_properties() && walker->FLAG_CLUMSY)) {
						walker->handle(index_mask);
						walker->handle(index_shift);
					}
					for (int i = 0; i <= index_mask; i++) walker->handle(table[i]);
					walker->handle(prev);
					prev &= index_mask;
				}
				std::string BaysDurhamShuffle16::get_name() const {
					std::ostringstream tmp;
					tmp << "[BDS" << (1+int(index_mask)) << "]" << base_rng->get_name();
					return tmp.str();
				}

				Uint8 BaysDurhamShuffle8::raw8() {
					Uint8 &storage = table[prev];
					Uint8 rv = storage;
					storage = base_rng->raw32();
					prev = Uint8(storage >> index_shift) & index_mask;
					return rv;
				}
				void BaysDurhamShuffle8::seed(Uint64 s) {
					base_rng->seed(s);
					for (int i = 0; i <= index_mask; i++) 
						table[i] = base_rng->raw32();
					for (int i = 0; i <= index_mask; i++)  {raw8();raw8();}
				}
				void BaysDurhamShuffle8::walk_state(StateWalkingObject *walker) {
					base_rng->walk_state(walker);
					if (!(walker->get_properties() && walker->FLAG_CLUMSY)) {
						walker->handle(index_mask);
						walker->handle(index_shift);
					}
					for (int i = 0; i <= index_mask; i++) walker->handle(table[i]);
					walker->handle(prev);
					prev &= index_mask;
				}
				std::string BaysDurhamShuffle8::get_name() const {
					std::ostringstream tmp;
					tmp << "[BDS" << (1+int(index_mask)) << "]" << base_rng->get_name();
					return tmp.str();
				}
				vRNG *apply_BaysDurhamShuffle(vRNG *base_rng, int table_size_L2, int shift) {
					vRNG8 *tmp8 = dynamic_cast<vRNG8*>(base_rng);
					if (tmp8)  return new BaysDurhamShuffle8 (tmp8, table_size_L2, shift >= 0 ? shift : 8-table_size_L2);
					vRNG16 *tmp16 = dynamic_cast<vRNG16*>(base_rng);
					if (tmp16) return new BaysDurhamShuffle16(tmp16, table_size_L2, shift >= 0 ? shift : 16-table_size_L2);
					vRNG32 *tmp32 = dynamic_cast<vRNG32*>(base_rng);
					if (tmp32) return new BaysDurhamShuffle32(tmp32, table_size_L2, shift >= 0 ? shift : 32-table_size_L2);
					vRNG64 *tmp64 = dynamic_cast<vRNG64*>(base_rng);
					if (tmp64) return new BaysDurhamShuffle64(tmp64, table_size_L2, shift >= 0 ? shift : 64-table_size_L2);
					issue_error();
					return NULL;//just to quiet the warnings
				}
			}
		}
	}
}
