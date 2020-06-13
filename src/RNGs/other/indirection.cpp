#include <string>
#include <sstream>
#include "PractRand/config.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/rng_internals.h"

#include "PractRand/RNGs/other/indirection.h"
#include "PractRand/test_helpers.h"

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
					walker->handle(a);walker->handle(b);
					for (int i = 0; i < 256; i++) walker->handle(arr[i]);
				}

				Uint8 rc4_weakened::raw8() {
					b += arr[a];
					Uint8 tmp = arr[b];
					arr[b] = arr[a];
					arr[a] = tmp;
					return tmp;// + b;
				}
				std::string rc4_weakened::get_name() const {return "rc4_weakened";}
				void rc4_weakened::walk_state(StateWalkingObject *walker) {
					walker->handle(a);walker->handle(b);
					for (int i = 0; i < 256; i++) walker->handle(arr[i]);
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
					for (int i = 0; i < table_size; i++) walker->handle(table[i]);
					if (left >= table_size) left = 0;
				}
				ibaa8::ibaa8(int table_size_L2_) : table_size_L2(table_size_L2_) {
					table = new Uint8(1 << table_size_L2);
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
					for (int i = 0; i < table_size; i++) walker->handle(table[i]);
					if (left >= table_size) left = 0;
				}
				ibaa16::ibaa16(int table_size_L2_) : table_size_L2(table_size_L2_) {
					table = new Uint16(1 << table_size_L2);
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
					int table_size = 1<<table_size_L2;
					for (int i = 0; i < table_size; i++) walker->handle(table[i]);
					if (left >= table_size) left = 0;
				}
				ibaa32::ibaa32(int table_size_L2_) : table_size_L2(table_size_L2_) {
					table = new Uint32(1 << table_size_L2);
				}
				ibaa32::~ibaa32() {delete[] table;}
			}
		}
	}
}
