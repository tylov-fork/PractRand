
/*
RNGs in the mediocre directory are not intended for real world use
only for research; as such they may get pretty sloppy in some areas

This set is of RNGs that:
1. use an array with dynamic access patterns
2. don't use complex flow control or advanced math functions (sin/cos, log/exp, etc)
3. are likely to have detectable bias
*/

namespace PractRand {
	namespace RNGs {
		namespace Polymorphic {
			namespace NotRecommended {
				//the classic cryptographic RNG
				class rc4 : public vRNG8 {
					Uint8 arr[256];
					Uint8 a, b;
				public:
					Uint8 raw8();
					std::string get_name() const;
					//void seed (Uint64 s);
					void walk_state(StateWalkingObject *);
				};

				//weaker variant of the classic cryptographic RNG
				//(same state transitions, different output)
				class rc4_weakened : public vRNG8 {
					Uint8 arr[256];
					Uint8 a, b;
				public:
					Uint8 raw8();
					std::string get_name() const;
					//void seed (Uint64 s);
					void walk_state(StateWalkingObject *);
				};


				//based upon the IBAA algorithm by Robert Jenkins
				//but reduced strength via smaller table sizes and smaller integer types
				class ibaa8 : public vRNG8 {
					int table_size_L2;
					Uint8 *table;
					Uint8 a, b, left;
				public:
					Uint8 raw8();
					std::string get_name() const;
					void walk_state(StateWalkingObject *);
					ibaa8(int table_size_L2_);
					~ibaa8();
				};
				class ibaa16 : public vRNG16 {
					int table_size_L2;
					Uint16 *table;
					Uint16 a, b, left;
				public:
					Uint16 raw16();
					std::string get_name() const;
					void walk_state(StateWalkingObject *);
					ibaa16(int table_size_L2_);
					~ibaa16();
				};
				class ibaa32 : public vRNG32 {
					int table_size_L2;
					Uint32 *table;
					Uint32 a, b, left;
				public:
					Uint32 raw32();
					std::string get_name() const;
					void walk_state(StateWalkingObject *);
					ibaa32(int table_size_L2_);
					~ibaa32();
				};
			}
		}
	}
}
