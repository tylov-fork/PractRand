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
						16 bit: 2,5 - 2,6 - 3,5 - 3,6
						32 bit: 4,9 - 5,9 - 5,12 - 6,10 - 9,13
						64 bit: 6,40 - 7,17 - 7,41 - 10,24
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
				Uint64 sfc_v1_64::raw64() {
					Uint64 tmp = a ^ counter++;
					a = b + (b << 7);
					enum {BARREL_SHIFT = 41};
					b = ((b << BARREL_SHIFT) | (b >> (64-BARREL_SHIFT))) + tmp;
					return a;

				}
				std::string sfc_v1_64::get_name() const {return "sfc64";}
				void sfc_v1_64::walk_state(StateWalkingObject *walker) {
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
			}
		}
	}
}
