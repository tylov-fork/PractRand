#include <string>
#include <sstream>
#include "PractRand/config.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/rng_internals.h"

#include "PractRand/RNGs/other/simple.h"

namespace PractRand {
	namespace RNGs {
		namespace Polymorphic {
			namespace NotRecommended {
				Uint32 xorshift32::raw32() {
					a ^= a << 13;
					a ^= a >> 17;
					a ^= a << 5;
					return a;
				}
				std::string xorshift32::get_name() const {return "xorshift32";}
				void xorshift32::walk_state(StateWalkingObject *walker) {
					walker->handle(a);
					if (!a) a = 1;
				}
				Uint64 xorshift64::raw64() {
					a ^= a << 13;
					a ^= a >> 7;
					a ^= a << 17;
					return a;
				}
				std::string xorshift64::get_name() const {return "xorshift64";}
				void xorshift64::walk_state(StateWalkingObject *walker) {
					walker->handle(a);
					if (!a) a = 1;
				}
				void xorshift128_64::xrs(int bits) {
					if (bits < 64) {
						low ^= low >> bits;
						low ^= high << (64-bits);
						high ^= high >> bits;
					}
					else if (bits > 64) low ^= high >> (bits - 64);
					else low ^= high;
				}
				void xorshift128_64::xls(int bits) {
					if (bits < 64) {
						high ^= high << bits;
						high ^= low >> (64-bits);
						low ^= low << bits;
					}
					else if (bits > 64) low ^= high >> (bits - 64);
					else low ^= high;
				}
				Uint64 xorshift128_64::raw64() {
					xls(16);
					xrs(53);
					xls(47);
					return low;
				}
				std::string xorshift128_64::get_name() const {return "xorshift128_64";}
				void xorshift128_64::walk_state(StateWalkingObject *walker) {
					walker->handle(low);walker->handle(high);
					if (!high && !low) low = 1;
				}

				std::string xorshift128_32::get_name() const {return "xorshift128_32";}
				void xorshift128_32::walk_state(StateWalkingObject *walker) {impl.walk_state(walker);}
				std::string xorshift64_32::get_name() const {return "xorshift64_32";}
				void xorshift64_32::walk_state(StateWalkingObject *walker) {impl.walk_state(walker);}
				std::string xorshift32_16::get_name() const {return "xorshift32_16";}
				void xorshift32_16::walk_state(StateWalkingObject *walker) {impl.walk_state(walker);}

				Uint32 xorshift32x4::raw32() {
					Uint32 tmp = x ^ (x << 15);
					x = y;
					y = z;
					z = w;
					tmp ^= tmp >> 4;
					w ^= (w >> 21) ^ tmp;
					return w;
				}
				std::string xorshift32x4::get_name() const {return "xorshift32x4";}
				void xorshift32x4::walk_state(StateWalkingObject *walker) {
					walker->handle(x);
					walker->handle(y);
					walker->handle(z);
					walker->handle(w);
					if (!(x || y || z || w)) x = 1;
				}
				Uint32 xorwow32x5::raw32() {
					Uint32 tmp = x;
					x = y;
					y = z;
					z = w ^ (w << 1);
					w = v ^ (v >> 7);
					v ^= (v << 4) ^ tmp;
					d += 362437;
					return v+d;
				}
				std::string xorwow32x5::get_name() const {return "xorwow32x5";}
				void xorwow32x5::walk_state(StateWalkingObject *walker) {
					walker->handle(x);
					walker->handle(y);
					walker->handle(z);
					walker->handle(w);
					walker->handle(v);
					walker->handle(d);
					if (!(x || y || z || w || v)) x = 1;
				}

				Uint32 sapparot::raw32() {
					Uint32 tmp;
					tmp = a + 0x9e3779b9;
					tmp = (tmp<<7)|(tmp>>25);
					a = b ^ (~tmp)^(tmp<<3);
					a=(a<<7)|(a>>25);
					b = tmp;
					return a ^ b;
				}
				std::string sapparot::get_name() const {return "sapparot";}
				void sapparot::walk_state(StateWalkingObject *walker) {
					walker->handle(a);
					walker->handle(b);
				}

				Uint16 sap48_16::raw16() {
					Uint16 tmp;
					tmp = a + 0x79b9 + c;
					tmp = (tmp<<5)|(tmp>>11);
					a = b ^ (~tmp)^(tmp<<3);
					a=(a<<5)|(a>>11);
					b = tmp;
					c = (c + a) ^ b;
					return b;
				}
				std::string sap48_16::get_name() const {return "sap48_16";}
				void sap48_16::walk_state(StateWalkingObject *walker) {
					walker->handle(a);
					walker->handle(b);
					walker->handle(c);
				}
				Uint32 sap96_32::raw32() {
					Uint32 tmp;
					tmp = a + 0x9e3779b9 + c;
					tmp = (tmp<<7)|(tmp>>25);
					a = b ^ (~tmp)^(tmp<<3);
					a=(a<<7)|(a>>25);
					b = tmp;
					c = (c + a) ^ b;
					return b;
				}
				std::string sap96_32::get_name() const {return "sap96_32";}
				void sap96_32::walk_state(StateWalkingObject *walker) {
					walker->handle(a);
					walker->handle(b);
					walker->handle(c);
				}

				Uint32 flea32x1::raw32() {
					enum {SHIFT1=15, SHIFT2=27};
					Uint32 e = a[d % SIZE];
					a[d % SIZE] = ((b<<SHIFT1)|(b>>(32-SHIFT1)));
					b = c + ((d<<SHIFT2)|(d>>(32-SHIFT2)));
					c = d + a[i++ % SIZE];
					d = e + c;
					return b;
				}
				std::string flea32x1::get_name() const {return "flea32x1";}
				void flea32x1::walk_state(StateWalkingObject *walker) {
					for (int z = 0; z < SIZE; z++) walker->handle(a[z]);
					walker->handle(b);
					walker->handle(c);
					walker->handle(d);
					walker->handle(i);
				}

				/*
				sfc version 1:
					tmp = a ^ counter++;
					a = b + (b << SHIFT1);
					b = ((b << SHIFT2) | (b >> (WORD_BITS - SHIFT2))) + tmp;
					steps:
						load a			load b			load counter
					*	b<<SHIFT1		b<<<SHIFT2		counter + 1		a ^ counter
						+ b				+ a ^ counter	store counter	
						store a			store b
					overall:
						quality suffers, but it's fast
						note that (b + (b << SHIFT1)) can be a single LEA on x86 for small shifts
					constants:
						16: 2,5 ?
						32: 5,12 ?
						64: 7,41 ?
				sfc version 2:
					code:
						tmp = a ^ b;
						a = b + (b << SHIFT1);
						b = ((b << SHIFT2) | (b >> (WORD_BITS - SHIFT2))) + tmp + counter++;
					steps:
						load a			load b				load counter
					*	b << SHIFT1		b <<< SHIFT2		a ^ b			counter + 1
						+ b				+((a^b)+counter)***
						store a			store b
					suggested values for SHIFT1,SHIFT2 by output avalanche: 
						16 bit: *2,5 - 2,6 - 3,5 - 3,6
						32 bit: 4,9 - 5,9 - *5,12 - 6,10 - 9,13
						64 bit: 6,40 - 7,17 - *7,41 - 10,24
					overall:
						does okay @ 32 & 64 bit, but poorly at 16 bit
						note that (b + (b << SHIFT1)) can be a single LEA on x86 for small shifts
				sfc version 3:
					code
						tmp = a + b + counter++;
						a = b ^ (b >> SHIFT1);
						b = ((b << SHIFT2) | (b >> (WORD_BITS - SHIFT2))) + tmp;
					steps:
						load a			load b			load counter
						b >> SHIFT1		b <<< SHIFT2	a+b+counter***		counter + 1
					*	^ b				+(a+b+counter)	store counter
						store a			store b
					overall:
						good statistical properties
						slower than earlier versions on my CPU, but still fast
					refence values for SHIFT1,SHIFT2:
						16 bit: 2,5
						32 bit: 5,12
						64 bit: 7,41
				candidate for version 4 of sfc:
					code
						Word old = a + b + counter++;
						a = b ^ (b >> SHIFT2);
						b = c + (c << SHIFT3);
						c = old + rotate(c,SHIFT1);
						return old;
					steps:
						load a			load b			load c				load counter
					*	b >> SHIFT2		b << SHIFT2		c <<< SHIFT1		counter + 1		a+b+counter***
						^ b				+ b				+(a+b+counter)		store counter
						store a			store b			store c				
					overall:
						very good behavior on statistical tests
						uses an extra word / register
						adequate speed
					refence values for SHIFT1,SHIFT2,SHIFT3:
						8 bit:  3,2,2
						16 bit: 7,3,2
						32 bit: 13,5,3
						64 bit: 29,9,3
				*/
				Uint16 sfc_v1_16::raw16() {
					Uint16 tmp = a ^ counter++;
					a = b + (b << 2);
					enum {BARREL_SHIFT = 5};
					b = ((b << BARREL_SHIFT) | (b >> (16-BARREL_SHIFT))) + tmp;
					return a;
				}
				std::string sfc_v1_16::get_name() const {return "sfc_v1_16";}
				void sfc_v1_16::walk_state(StateWalkingObject *walker) {
					walker->handle(a);
					walker->handle(b);
					walker->handle(counter);
				}
				Uint32 sfc_v1_32::raw32() {
					Uint32 tmp = a ^ counter++;
					a = b + (b << 5);
					enum {BARREL_SHIFT = 12};
					b = ((b << BARREL_SHIFT) | (b >> (32-BARREL_SHIFT))) + tmp;
					return a;
				}
				std::string sfc_v1_32::get_name() const {return "sfc_v1_32";}
				void sfc_v1_32::walk_state(StateWalkingObject *walker) {
					walker->handle(a);
					walker->handle(b);
					walker->handle(counter);
				}
				Uint16 sfc_v2_16::raw16() {
					Uint16 tmp = a ^ b;
					a = b + (b << 2);
					enum {BARREL_SHIFT = 5};
					b = ((b << BARREL_SHIFT) | (b >> (16-BARREL_SHIFT))) + tmp + counter++;
					return tmp;
				}
				std::string sfc_v2_16::get_name() const {return "sfc_v2_16";}
				void sfc_v2_16::walk_state(StateWalkingObject *walker) {
					walker->handle(a);
					walker->handle(b);
					walker->handle(counter);
				}
				Uint32 sfc_v2_32::raw32() {
					Uint32 tmp = a ^ b;
					a = b + (b << 5);
					enum {BARREL_SHIFT = 12};
					b = ((b << BARREL_SHIFT) | (b >> (32-BARREL_SHIFT))) + tmp + counter++;
					return tmp;
				}
				std::string sfc_v2_32::get_name() const {return "sfc_v2_32";}
				void sfc_v2_32::walk_state(StateWalkingObject *walker) {
					walker->handle(a);
					walker->handle(b);
					walker->handle(counter);
				}
				Uint16 sfc_v3_16::raw16() {
					Uint16 tmp = a ^ b;
					a = b + (b >> 2);
					enum {BARREL_SHIFT = 5};
					b = ((b << BARREL_SHIFT) | (b >> (16-BARREL_SHIFT))) + tmp + counter++;
					return tmp;
				}
				std::string sfc_v3_16::get_name() const {return "sfc_v3_16";}
				void sfc_v3_16::walk_state(StateWalkingObject *walker) {
					walker->handle(a);
					walker->handle(b);
					walker->handle(counter);
				}
				Uint32 sfc_v3_32::raw32() {
					Uint32 tmp = a ^ b;
					a = b + (b >> 5);
					enum {BARREL_SHIFT = 12};
					b = ((b << BARREL_SHIFT) | (b >> (32-BARREL_SHIFT))) + tmp + counter++;
					return tmp;
				}
				std::string sfc_v3_32::get_name() const {return "sfc_v3_32";}
				void sfc_v3_32::walk_state(StateWalkingObject *walker) {
					walker->handle(a);
					walker->handle(b);
					walker->handle(counter);
				}

				Uint32 simpleA::raw32() {
					enum {BARREL_SHIFT1 = 19};
					Uint32 tmp = b ^ ((a << BARREL_SHIFT1) | (a >> (32-BARREL_SHIFT1)));
					a = ~b + c;
					b = c;
					c += tmp;
					return b;
				}
				std::string simpleA::get_name() const {return "simpleA";}
				void simpleA::walk_state(StateWalkingObject *walker) {
					walker->handle(a);
					walker->handle(b);
					walker->handle(c);
				}
				Uint16 simpleB::raw16() {
					enum {BARREL_SHIFT1 = 3, BARREL_SHIFT2 = 5};
					Uint16 tmp = ((a << BARREL_SHIFT1) | (a >> (16-BARREL_SHIFT1))) ^ ~b;
					a = b + c;
					b = c;
					c = tmp + ((c << BARREL_SHIFT2) | (c >> (16-BARREL_SHIFT2)));
					return tmp;
				}
				std::string simpleB::get_name() const {return "simpleB";}
				void simpleB::walk_state(StateWalkingObject *walker) {
					walker->handle(a);
					walker->handle(b);
					walker->handle(c);
				}
				Uint16 simpleC::raw16() {
					enum {BARREL_SHIFT1 = 3, BARREL_SHIFT2 = 5};
					Uint16 tmp = ((a << BARREL_SHIFT1) | (a >> (16-BARREL_SHIFT1))) ^ ~b;
					a = b + ((c << BARREL_SHIFT2) | (c >> (16-BARREL_SHIFT2)));
					b = c ^ (c >> 2);
					c += tmp;
					return tmp;
				}
				std::string simpleC::get_name() const {return "simpleC";}
				void simpleC::walk_state(StateWalkingObject *walker) {
					walker->handle(a);
					walker->handle(b);
					walker->handle(c);
				}
				Uint32 simpleD::raw32() {
					enum {BARREL_SHIFT1 = 19};
					Uint32 old = a;
					Uint32 tmp = b ^ ((a << BARREL_SHIFT1) | (a >> (32-BARREL_SHIFT1)));
					a = b + c;
					b = c ^ old;
					c = old + tmp;
					return tmp;
				}
				std::string simpleD::get_name() const {return "simpleD";}
				void simpleD::walk_state(StateWalkingObject *walker) {
					walker->handle(a);
					walker->handle(b);
					walker->handle(c);
				}
				Uint32 simpleE::raw32() {
					Uint32 old = a + b;
					a = b ^ c;
					b = c + old;
					c = old + ((c << 13) | (c >> 19));
					return old;
				}
				std::string simpleE::get_name() const {return "simpleE";}
				void simpleE::walk_state(StateWalkingObject *walker) {
					walker->handle(a);
					walker->handle(b);
					walker->handle(c);
				}
			}
		}
	}
}
