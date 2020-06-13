
/*
RNGs in the mediocre directory are not intended for real world use
only for research; as such they may get pretty sloppy in some areas

This set is of RNGs that:
1. use an array with repetitive access patterns - generally a Fibonacci-style cyclic buffer
2. don't use much flow control, variable shifts, etc
3. are likely to have easily detectable bias
*/

namespace PractRand {
	namespace RNGs {
		namespace Polymorphic {
			namespace NotRecommended {
				//Mitchell-Moore: LFib32(Uint32, 55, 24, ADD)
				class mm32 : public vRNG32 {
					Uint32 cbuf[55];
					Uint8 index1, index2;
				public:
					Uint32 raw32();
					std::string get_name() const;
					void walk_state(StateWalkingObject *);
				};
				//Mitchell-Moore modified: LFib16(Uint32, 55, 24, ADD) >> 16
				class mm32_16 : public vRNG16 {
					Uint32 cbuf[55];
					Uint8 index1, index2;
				public:
					Uint16 raw16();
					std::string get_name() const;
					void walk_state(StateWalkingObject *);
				};
				//Mitchell-Moore modified: LFib32(Uint32, 55, 24, ADC)
				class mm32_awc : public vRNG32 {
					Uint32 cbuf[55];
					Uint8 index1, index2, carry;
				public:
					Uint32 raw32();
					std::string get_name() const;
					void walk_state(StateWalkingObject *);
				};

				//proposed by Marsaglia
				class mwc4691 : public vRNG32 {
					Uint32 cbuf[4691];
					int index, carry;
				public:
					Uint32 raw32();
					std::string get_name() const;
					void walk_state(StateWalkingObject *);
				};
				//proposed by Marsaglia
				//class cwsb4288;

				class cbuf_accum : public vRNG32 {
					enum {L = 32};
					Uint32 cbuf[L], accum;
					Uint8 index;
				public:
					Uint32 raw32();
					std::string get_name() const;
					void walk_state(StateWalkingObject *);
				};
				class dual_cbuf : public vRNG32 {
					enum {L1 = 3, L2 = 5};
					Uint32 cbuf1[L1], cbuf2[L2];
					Uint8 index1, index2;
				public:
					Uint32 raw32();
					std::string get_name() const;
					void walk_state(StateWalkingObject *);
				};
				class dual_cbuf_accum : public vRNG32 {
					enum {L1 = 4, L2 = 5};
					Uint32 cbuf1[L1], cbuf2[L2], accum;
					Uint8 index1, index2;
				public:
					Uint32 raw32();
					std::string get_name() const;
					void walk_state(StateWalkingObject *);
				};

				class green_hammer : public vRNG8 {
					enum {SIZE = 71, LAG = 6};
					Uint8 table[SIZE];
					static Uint8 mash(Uint8 *ptr);
					void mix();
					void _seed(Uint8 *s, int len);
				public:
					Uint8 raw8();
					std::string get_name() const;
					void walk_state(StateWalkingObject *);
				};
			}
		}
	}
}
