/*
 * FILE:	sha2.h
 * AUTHOR:	Aaron D. Gifford - http://www.aarongifford.com/
 * 
 * Copyright (c) 2000-2001, Aaron D. Gifford
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTOR(S) ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTOR(S) BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id: sha2.h,v 1.1 2001/11/08 00:02:01 adg Exp adg $
 */

/*
Changes made to Aaron's code in this file:
	all symbols moved in to PractRand namespace
	some macro constant converted to enum constants
	integer types changed to use PractRands config.h
*/

#ifndef __SHA2_H__
#define __SHA2_H__

#ifndef __PRACTRAND_CONFIG_H__
#include "PractRand/config.h"
#endif //__PRACTRAND_CONFIG_H__

namespace PractRand {
	namespace Crypto {
		class SHA2_512 {
			typedef Uint64 Word;
			Word state[8];
			Uint64 length;
			union InputBlock {
				Word as_word[16];
				Uint8 as_byte[16 * 8];
			};
			InputBlock input_buffer;
			unsigned long leftover_input_bytes;
			void process_block();
			void process_final_block();
			Word endianness_word(Word);
			void endianness_input();
			void endianness_state();
		public:
			enum {RESULT_LENGTH = 64};
			void reset();
			void handle_input(const Uint8 *input, unsigned long length);
			void finish(Uint8 destination[RESULT_LENGTH]);
			SHA2_512() {reset();}
		};
	}//Crypto
}//PractRand

#endif /* __SHA2_H__ */

