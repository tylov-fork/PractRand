#include <string>
#include <sstream>
#include "PractRand/config.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/rng_internals.h"

#include "PractRand/RNGs/other/fibonacci.h"

namespace PractRand {
	namespace RNGs {
		namespace Polymorphic {
			namespace NotRecommended {
				//Mitchell-Moore: LFib32(Uint32, 55, 24, ADD)
				Uint32 mm32::raw32() {
					Uint32 tmp;
					tmp = cbuf[index1] += cbuf[index2];
					if ( ++index1 == 55 ) index1 = 0;
					if ( ++index2 == 55 ) index2 = 0;
					return tmp;
				}
				std::string mm32::get_name() const {return "mm32";}
				void mm32::walk_state(StateWalkingObject *walker) {
					walker->handle(index1);
					for (int i = 0; i < 55; i++) walker->handle(cbuf[i]);
					if (index1 >= 55) index1 %= 55;
					index2 = index1 - 24;
					if (index2 >= 55) index2 += 55;
				}
				//Mitchell-Moore modified: LFib16(Uint32, 55, 24, ADD) >> 16
				Uint16 mm32_16::raw16() {
					Uint32 tmp;
					tmp = cbuf[index1] += cbuf[index2];
					if ( ++index1 == 55 ) index1 = 0;
					if ( ++index2 == 55 ) index2 = 0;
					return tmp >> 16;
				}
				std::string mm32_16::get_name() const {return "mm32_16";}
				void mm32_16::walk_state(StateWalkingObject *walker) {
					walker->handle(index1);
					for (int i = 0; i < 55; i++) walker->handle(cbuf[i]);
					if (index1 >= 55) index1 %= 55;
					index2 = index1 - 24;
					if (index2 >= 55) index2 += 55;
				}
				//Mitchell-Moore modified: LFib32(Uint32, 55, 24, ADD) >> 16
				Uint32 mm32_awc::raw32() {
					Uint32 tmp1, tmp2, tmp3;
					tmp1 = cbuf[index1];
					tmp2 = cbuf[index2];
					tmp3 = tmp1 + tmp2 + carry;
					cbuf[index1] = tmp3;
					carry = (tmp3 - tmp1) >> 31;
					if ((tmp3 == tmp1) && tmp2) carry = 1;
					if ( ++index1 == 55 ) index1 = 0;
					if ( ++index2 == 55 ) index2 = 0;
					return tmp3;
				}
				std::string mm32_awc::get_name() const {return "mm32_awc";}
				void mm32_awc::walk_state(StateWalkingObject *walker) {
					walker->handle(carry);
					walker->handle(index1);
					for (int i = 0; i < 55; i++) walker->handle(cbuf[i]);
					if (index1 >= 55) index1 %= 55;
					index2 = index1 - 24;
					if (index2 >= 55) index2 += 55;
					carry &= 1;
				}

				//used by Marsaglia in KISS4691 (2010)
				Uint32 mwc4691::raw32() {
					index = (index < 4691-1) ? index + 1 : 0;
					Uint32 x, t;
					x = cbuf[index];
					t = (x << 13) + carry + x;
					carry = (x>>19) + (t<=x);
					if (!t && !x) carry--;
					cbuf[index] = t;
					return t;
				}
				std::string mwc4691::get_name() const {return "mwc4691";}
				void mwc4691::walk_state(StateWalkingObject *walker) {
					walker->handle(index);
					walker->handle(carry);
					for (int i = 0; i < 4691; i++) walker->handle(cbuf[i]);
				}
				
				//
				Uint32 cbuf_accum::raw32() {
					Uint32 tmp = cbuf[--index];
					accum = ((accum << 11) | (accum >> 21)) + ~tmp;
					cbuf[index] = accum;
					if ( !index ) index = L;
					return accum;
				}
				std::string cbuf_accum::get_name() const {return "cbuf_accum";}
				void cbuf_accum::walk_state(StateWalkingObject *walker) {
					walker->handle(index);
					walker->handle(accum);
					for (int i = 0; i < L; i++) walker->handle(cbuf[i]);
					if (index >= L) index %= L;
				}
				Uint32 dual_cbuf::raw32() {
					Uint32 tmp1, tmp2;
					tmp1 = cbuf1[--index1];
					tmp2 = cbuf2[--index2];
					cbuf1[index1] = tmp1 + tmp2;
					cbuf2[index2] = ((tmp1 << 11) | (tmp1 >> 21)) ^ tmp2;
					if ( !index1 ) index1 = L1;
					if ( !index2 ) index2 = L2;
					return tmp1 + tmp2;
				}
				std::string dual_cbuf::get_name() const {return "dual_cbuf";}
				void dual_cbuf::walk_state(StateWalkingObject *walker) {
					walker->handle(index1);
					walker->handle(index2);
					for (int i = 0; i < L1; i++) walker->handle(cbuf1[i]);
					for (int i = 0; i < L2; i++) walker->handle(cbuf2[i]);
					if (index1 >= L1) index1 %= L1;
					if (index2 >= L2) index2 %= L2;
				}
				Uint32 dual_cbuf_accum::raw32() {
					Uint32 tmp1, tmp2;
					tmp1 = cbuf1[--index1];
					tmp2 = cbuf2[--index2];
					accum = ((accum << 11) | (accum >> 21)) + tmp1;
					cbuf1[index1] = tmp1 ^ tmp2;
					cbuf2[index2] = accum;
					if ( !index1 ) index1 = L1;
					if ( !index2 ) index2 = L2;
					return accum;
				}
				std::string dual_cbuf_accum::get_name() const {return "dual_cbuf_accum";}
				void dual_cbuf_accum::walk_state(StateWalkingObject *walker) {
					walker->handle(index1);
					walker->handle(index2);
					walker->handle(accum);
					for (int i = 0; i < L1; i++) walker->handle(cbuf1[i]);
					for (int i = 0; i < L2; i++) walker->handle(cbuf2[i]);
					if (index1 >= L1) index1 %= L1;
					if (index2 >= L2) index2 %= L2;
				}



				Uint8 green_hammer::mash( Uint8 *ptr ) {
					Uint32 tmp = 0;
					for (int i = 0; i < LAG; i++) {
						tmp = (tmp << 1) + *ptr++;
					}
					return Uint8(tmp+(tmp>>8));
				}
				void green_hammer::mix() {
					int i;
					Uint8 tmp = table[SIZE-1];
					for(i=0; i<(SIZE-LAG); i++) table[i+LAG] += table[i];
					for(   ; i<SIZE; i++) table[i-(SIZE-LAG)] += table[i];
				}
				void green_hammer::_seed(Uint8 *s, int len) {
					for(int i=0; i<SIZE; i++) table[i] = 0;
					for(int i=0; i<len; i++) {
						Uint8 tmp = *s++;
						for (int j = 0; j < 8; j++) {
							table[j] += tmp;
							tmp >>= 1;
						}
						mix();
					}
					for(int i=0; i<50; i++) mix();
				}
				Uint8 green_hammer::raw8() {
					mix();
					enum {SP2M = (SIZE >= 64 + LAG) ? 63 : ((SIZE >= 32 + LAG) ? 31 : ((SIZE >= 16 + LAG) ? 15 : ((SIZE >= 8 + LAG) ? 7 : ((SIZE >= 4 + LAG) ? 3 : ((SIZE >= 2 + LAG) ? 1 : -1)))))};
				//	static int assertion[SP2M == 63 ? 1 : -1];
					//return mash(&table[mash(&table[SIZE-LAG])&SP2M]);
					return table[mash(&table[SIZE-LAG])&SP2M];
				}
				std::string green_hammer::get_name() const {return "green_hammer";}
				void green_hammer::walk_state(StateWalkingObject *walker) {
					for (int i = 0; i < SIZE; i++) walker->handle(table[i]);
				}

			}
		}
	}
}
