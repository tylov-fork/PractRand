
#include <deque>
//RNGs in the "other" directory are not intended for real world use
//only for research; as such they may get pretty sloppy in some areas
//and are usually not optimized
namespace PractRand {
	namespace RNGs {
		namespace Polymorphic {
			namespace NotRecommended {
				class Transform64 : public vRNG64 {
				public:
					vRNG64 *base_rng;
					void seed(Uint64 seed);
					Uint64 get_flags() const;
					Transform64(vRNG64 *rng) : base_rng(rng) {}
					~Transform64();
				};
				class Transform32 : public vRNG32 {
				public:
					vRNG32 *base_rng;
					void seed(Uint64 seed);
					Uint64 get_flags() const;
					Transform32(vRNG32 *rng) : base_rng(rng) {}
					~Transform32();
				};
				class Transform16 : public vRNG16 {
				public:
					vRNG16 *base_rng;
					void seed(Uint64 seed);
					Uint64 get_flags() const;
					Transform16(vRNG16 *rng) : base_rng(rng) {}
					~Transform16();
				};
				class Transform8 : public vRNG8 {
				public:
					vRNG8 *base_rng;
					void seed(Uint64 seed);
					Uint64 get_flags() const;
					Transform8(vRNG8 *rng) : base_rng(rng) {}
					~Transform8();
				};

				class GeneralizedTableTransform : public vRNG8 {//written for self-shrinking-generators, but also useful for other transforms
				public:
					struct Entry {
						Uint8 data;
						Uint8 count;
					};
					//the transform table
					const Entry *table;

					//for buffering fractional bytes of output
					Uint32 buf_data;
					Uint32 buf_count;

					//for buffering full bytes of output
					std::deque<Uint8> finished_bytes;

					std::string name;
					vRNG *base_rng;
					void seed(Uint64 seed);
					Uint64 get_flags() const;
					std::string get_name() const ;
					GeneralizedTableTransform(vRNG *rng, const Entry *table_, std::string name_) : base_rng(rng), table(table_), buf_count(0), buf_data(0), name(name_) {}
					~GeneralizedTableTransform();
					void walk_state(StateWalkingObject *);
					Uint8 raw8();
				};
				vRNG *apply_SelfShrinkTransform(vRNG *base_rng);
				//vRNG *apply_SimpleShrinkTransform(vRNG *base_rng);

				class BaysDurhamShuffle64 : public Transform64 {
					Uint64 table[256];
					Uint8 prev;
					Uint8 index_mask;
					Uint8 index_shift;
				public:
					Uint64 raw64();
					void seed(Uint64 s);
					void walk_state(StateWalkingObject *);
					std::string get_name() const ;
					BaysDurhamShuffle64(vRNG64 *rng, int table_size_L2, int shift=0) 
						: Transform64(rng), index_mask((1<<table_size_L2)-1), index_shift(shift) {}
				};
				class BaysDurhamShuffle32 : public Transform32 {
					Uint32 table[256];
					Uint8 prev;
					Uint8 index_mask;
					Uint8 index_shift;
				public:
					Uint32 raw32();
					void seed(Uint64 s);
					void walk_state(StateWalkingObject *);
					std::string get_name() const ;
					BaysDurhamShuffle32(vRNG32 *rng, int table_size_L2, int shift=0) 
						: Transform32(rng), index_mask((1<<table_size_L2)-1), index_shift(shift) {}
				};
				class BaysDurhamShuffle16 : public Transform16 {
					Uint16 table[256];
					Uint8 prev;
					Uint8 index_mask;
					Uint8 index_shift;
				public:
					Uint16 raw16();
					void seed(Uint64 s);
					void walk_state(StateWalkingObject *);
					std::string get_name() const ;
					BaysDurhamShuffle16(vRNG16 *rng, int table_size_L2, int shift=0) 
						: Transform16(rng), index_mask((1<<table_size_L2)-1), index_shift(shift) {}
				};
				class BaysDurhamShuffle8 : public Transform8 {
					Uint8 table[256];
					Uint8 prev;
					Uint8 index_mask;
					Uint8 index_shift;
				public:
					Uint8 raw8();
					void seed(Uint64 s);
					void walk_state(StateWalkingObject *);
					std::string get_name() const ;
					BaysDurhamShuffle8(vRNG8 *rng, int table_size_L2, int shift=0) 
						: Transform8(rng), index_mask((1<<table_size_L2)-1), index_shift(shift) {}
				};
				vRNG *apply_BaysDurhamShuffle(vRNG *base_rng, int table_size_L2=8, int shift=-1);
			}
		}
	}
}
