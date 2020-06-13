#include <string>
#include <sstream>
#include <cstdlib>
#include "PractRand/config.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/rng_internals.h"

#include "PractRand/RNGs/other/indirection.h"
#include "PractRand/RNGs/arbee.h"
//#include "PractRand/test_helpers.h"

namespace PractRand {
	namespace RNGs {
		namespace Polymorphic {
			namespace NotRecommended {
				Uint8 rc4::raw8() {
					b += arr[a];
					Uint8 tmp = arr[b];
					arr[b] = arr[a];
					arr[a] = tmp;
					return arr[Uint8(arr[a++] + arr[b])];
				}
				std::string rc4::get_name() const {return "rc4";}
				void rc4::walk_state(StateWalkingObject *walker) {
					walker->handle(a);
					walker->handle(b);
					if (walker->is_clumsy() && !walker->is_read_only()) {
						Uint64 seed;
						walker->handle(seed);
						PractRand::RNGs::Raw::arbee seeder(seed);
						for (int i = 0; i < 256; i++) arr[i] = i;
						for (int i = 0; i < 256; i++) {
							Uint8 ai = i, bi = seeder.raw8();
							Uint8 tmp = arr[ai]; arr[ai] = arr[bi]; arr[bi] = tmp;
						}
					}
					else {
						for (int i = 0; i < 256; i++) walker->handle(arr[i]);
					}
				}

				Uint8 rc4_weakened::raw8() {
					b += arr[a];
					Uint8 tmp = arr[b];
					arr[b] = arr[a];
					arr[a++] = tmp;
					return tmp;
				}
				std::string rc4_weakened::get_name() const {return "rc4_weakened";}
				void rc4_weakened::walk_state(StateWalkingObject *walker) {
					walker->handle(a);
					walker->handle(b);
					if (walker->is_clumsy() && !walker->is_read_only()) {
						Uint64 seed;
						walker->handle(seed);
						PractRand::RNGs::Raw::arbee seeder(seed);
						seeder.walk_state(walker);
						for (int i = 0; i < 256; i++) arr[i] = i;
						for (int i = 0; i < 256; i++) {
							Uint8 ai = i, bi = seeder.raw8();
							Uint8 tmp = arr[ai]; arr[ai] = arr[bi]; arr[bi] = tmp;
						}
					}
					else {
						for (int i = 0; i < 256; i++) walker->handle(arr[i]);
					}
				}

				Uint8 ibaa8::raw8() {
					if (left) {
						return table[--left];
					}
					const int half_size = 1<<(table_size_L2-1);
					const int mask = (1<<table_size_L2)-1;
					Uint8 *base = &table[mask+1];
					for (int i = 0; i <= mask; i++) {
						Uint8 x, y;
						x = base[i];
						a = ((a << 5) | (a >> 3)) + base[(i+half_size) & mask];
						y = base[x & mask] + a + b;
						base[i] = y;
						b = base[(y >> table_size_L2) & mask] + x;
						table[i] = b;
					}
					left = mask;
					return table[mask];
				}
				std::string ibaa8::get_name() const {
					std::ostringstream tmp;
					int table_size = 1<<table_size_L2;
					tmp << "ibaa8x" << table_size;
					return tmp.str();
				}
				void ibaa8::walk_state(StateWalkingObject *walker) {
					walker->handle(a);walker->handle(b);
					int table_size = 1<<table_size_L2;
					for (int i = 0; i < table_size * 2; i++) walker->handle(table[i]);
					walker->handle(left);
					if (left >= table_size) left = 0;
				}
				ibaa8::ibaa8(int table_size_L2_) : table_size_L2(table_size_L2_) {
					table = new Uint8[2 << table_size_L2];
				}
				ibaa8::~ibaa8() {delete[] table;}

				Uint16 ibaa16::raw16() {
					if (left) {
						return table[--left];
					}
					const int half_size = 1<<(table_size_L2-1);
					const int mask = (1<<table_size_L2)-1;
					Uint16 *base = &table[mask+1];
					for (int i = 0; i <= mask; i++) {
						Uint16 x, y;
						x = base[i];
						a = ((a << 11) | (a >> 5)) + base[(i+half_size) & mask];
						y = base[x & mask] + a + b;
						base[i] = y;
						b = base[(y >> table_size_L2) & mask] + x;
						table[i] = b;
					}
					left = mask;
					return table[mask];
				}
				std::string ibaa16::get_name() const {
					std::ostringstream tmp;
					int table_size = 1<<table_size_L2;
					tmp << "ibaa16x" << table_size;
					return tmp.str();
				}
				void ibaa16::walk_state(StateWalkingObject *walker) {
					walker->handle(a);walker->handle(b);
					int table_size = 1<<table_size_L2;
					for (int i = 0; i < table_size * 2; i++) walker->handle(table[i]);
					walker->handle(left);
					if (left >= table_size) left = 0;
				}
				ibaa16::ibaa16(int table_size_L2_) : table_size_L2(table_size_L2_) {
					table = new Uint16[2 << table_size_L2];
				}
				ibaa16::~ibaa16() {delete[] table;}

				Uint32 ibaa32::raw32() {
					if (left) {
						return table[--left];
					}
					const int half_size = 1<<(table_size_L2-1);
					const int mask = (1<<table_size_L2)-1;
					Uint32 *base = &table[mask+1];
					for (int i = 0; i <= mask; i++) {
						Uint32 x, y;
						x = base[i];
						a = ((a << 19) | (a >> 13)) + base[(i+half_size) & mask];
						y = base[x & mask] + a + b;
						base[i] = y;
						b = base[(y >> table_size_L2) & mask] + x;
						table[i] = b;
					}
					left = mask;
					return table[mask];
				}
				std::string ibaa32::get_name() const {
					std::ostringstream tmp;
					int table_size = 1<<table_size_L2;
					tmp << "ibaa32x" << table_size;
					return tmp.str();
				}
				void ibaa32::walk_state(StateWalkingObject *walker) {
					walker->handle(a);walker->handle(b);
					unsigned long table_size = 1<<table_size_L2;
					for (unsigned long i = 0; i < table_size * 2; i++) walker->handle(table[i]);
					walker->handle(left);
					if (left >= table_size) left = 0;
				}
				ibaa32::ibaa32(int table_size_L2_) : table_size_L2(table_size_L2_) {
					table = new Uint32[2 << table_size_L2];
				}
				ibaa32::~ibaa32() {delete[] table;}

				
				#define ind32(mm,x)  (*(Uint32 *)((Uint8 *)(mm) + ((x) & ((MASK)<<2))))
				#define rngstep32(mix,a,b,mm,m,m2,r,x) \
				{ \
				  x = *m;  \
				  a = (a^(mix)) + *(m2++); \
				  *(m++) = y = ind32(mm,x) + a + b; \
				  *(r++) = b = ind32(mm,y>>table_size_L2) + x; \
				}
				Uint32 isaac32_small::raw32() {
					if (left) {
						return table[--left];
					}
					const int HALF_SIZE = 1<<(table_size_L2-1);
					const int MASK = (1<<table_size_L2)-1;
					Uint32 *base = &table[MASK+1];
					Uint32 *m, *m2, *mend, *r;
					Uint32 x, y;
					m = base;
					r = table;
					b += ++c;
					if (table_size_L2 != 2) {
						for (m = base, mend = m2 = m+HALF_SIZE; m<mend; )
						{
							rngstep32( a<<13, a, b, base, m, m2, r, x);
							rngstep32( a>>6 , a, b, base, m, m2, r, x);
							rngstep32( a<<2 , a, b, base, m, m2, r, x);
							rngstep32( a>>16, a, b, base, m, m2, r, x);
						}
						for (m2 = base; m2<mend; )
						{
							rngstep32( a<<13, a, b, base, m, m2, r, x);
							rngstep32( a>>6 , a, b, base, m, m2, r, x);
							rngstep32( a<<2 , a, b, base, m, m2, r, x);
							rngstep32( a>>16, a, b, base, m, m2, r, x);
						}
					}
					else {
						for (m = base, mend = m2 = m+HALF_SIZE; m<mend; )
						{
							rngstep32( a<<13, a, b, base, m, m2, r, x);
							rngstep32( a>>6 , a, b, base, m, m2, r, x);
						}
						for (m2 = base; m2<mend; )
						{
							rngstep32( a<<2 , a, b, base, m, m2, r, x);
							rngstep32( a>>16, a, b, base, m, m2, r, x);
						}
					}
					left = MASK;
					return table[left];
				}
				std::string isaac32_small::get_name() const {
					std::ostringstream tmp;
					int table_size = 1<<table_size_L2;
					tmp << "isaac32x" << table_size;
					return tmp.str();
				}
				void isaac32_small::walk_state(StateWalkingObject *walker) {
					walker->handle(a);walker->handle(b);walker->handle(c);
					unsigned long table_size = 1<<table_size_L2;
					for (unsigned long i = 0; i < table_size; i++) walker->handle(table[i]);
					walker->handle(left);
					if (left >= table_size) left = 0;
				}
				isaac32_small::isaac32_small(int table_size_L2_) : table_size_L2(table_size_L2_) {
					if (table_size_L2 < 2) PractRand::issue_error("invalid table size for isaac32_small");
					table = new Uint32(2 << table_size_L2);
				}
				isaac32_small::~isaac32_small() {delete[] table;}
				#undef ind32
				#undef rngstep32
			}
		}
	}
}
