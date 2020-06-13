#include <string>
#include <sstream>
#include <vector>
#include "PractRand/config.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/rng_internals.h"
#include "PractRand/tests.h"

#include "PractRand/RNGs/other/transform.h"

namespace PractRand {
	namespace RNGs {
		namespace Polymorphic {
			namespace NotRecommended {
				void Transform64::seed(Uint64 s) {base_rng->seed(s);}
				Uint64 Transform64::get_flags() const {return base_rng->get_flags() | FLAG::USES_INDIRECTION;}
				void Transform64::walk_state(StateWalkingObject *walker) {base_rng->walk_state(walker);}
				Transform64::~Transform64() {delete base_rng;}
				void Transform32::seed(Uint64 s) {base_rng->seed(s);}
				Uint64 Transform32::get_flags() const {return base_rng->get_flags() | FLAG::USES_INDIRECTION;}
				void Transform32::walk_state(StateWalkingObject *walker) {base_rng->walk_state(walker);}
				Transform32::~Transform32() {delete base_rng;}
				void Transform16::seed(Uint64 s) {base_rng->seed(s);}
				Uint64 Transform16::get_flags() const {return base_rng->get_flags() | FLAG::USES_INDIRECTION;}
				void Transform16::walk_state(StateWalkingObject *walker) {base_rng->walk_state(walker);}
				Transform16::~Transform16() {delete base_rng;}
				void Transform8::seed(Uint64 s) {base_rng->seed(s);}
				Uint64 Transform8::get_flags() const {return base_rng->get_flags() | FLAG::USES_INDIRECTION;}
				void Transform8::walk_state(StateWalkingObject *walker) {base_rng->walk_state(walker);}
				Transform8::~Transform8() {delete base_rng;}
				void MultiplexTransformRNG::refill() { index = 0; }
				Uint8 MultiplexTransformRNG::raw8() {
					if (index >= Tests::TestBlock::SIZE) {
						refill();
						index = 1;
						return buffer->as8[0];
					}
					return buffer->as8[index++];
				}
				Uint16 MultiplexTransformRNG::raw16() {
					index += 3; index &= ~1;//round up to force alignment, and also increment position
					if (index > Tests::TestBlock::SIZE) {
						refill();
						index = 2;
						return buffer->as16[0];
					}
					Uint16 rv = *reinterpret_cast<Uint16*>(&buffer->as8[index - 2]);//read 16 aligned bits
					return rv;
				}
				Uint32 MultiplexTransformRNG::raw32() {
					index += 7; index &= ~3;
					if (index > Tests::TestBlock::SIZE) {
						refill();
						index = 4;
						return buffer->as32[0];
					}
					Uint32 rv = *reinterpret_cast<Uint32*>(&buffer->as8[index - 4]);
					return rv;
				}
				Uint64 MultiplexTransformRNG::raw64() {
					index += 15; index &= ~7;
					if (index > Tests::TestBlock::SIZE) {
						refill();
						index = 8;
						return buffer->as64[0];
					}
					Uint64 rv = *reinterpret_cast<Uint64*>(&buffer->as8[index - 8]);
					return rv;
				}
				void MultiplexTransformRNG::seed(Uint64 seedval) {
					index = 999999;
					for (std::vector<vRNG*>::iterator it = source_rngs.begin(); it != source_rngs.end(); it++) {
						vRNG *vrng = *it;
						vrng->seed(seedval);
					}
				}
				void MultiplexTransformRNG::seed(vRNG *seeder) {
					index = 999999;
					for (std::vector<vRNG*>::iterator it = source_rngs.begin(); it != source_rngs.end(); it++) {
						vRNG *vrng = *it;
						vrng->seed(seeder);
					}
					/*static bool first = true;
					if (first) {
						std::printf("\n{\n");
						for (std::vector<vRNG*>::iterator it = source_rngs.begin(); it != source_rngs.end(); it++) std::printf("\t{%s:%s}\n", (*it)->get_name().c_str(), (*it)->print_state().c_str());
						std::printf("}\n");
						first = false;
					}*/
				}
				int MultiplexTransformRNG::get_native_output_size() const {
					int lowest = 9999, highest = -1;
					for (std::vector<vRNG*>::const_iterator it = source_rngs.begin(); it != source_rngs.end(); it++) {
						int ls = (*it)->get_native_output_size();
						if (ls < lowest) lowest = ls;
						if (ls > highest) highest = ls;
					}
					if (lowest == highest) return lowest;
					return -1;
				}
				Uint64 MultiplexTransformRNG::get_flags() const {
					Uint64 anded_bits = Uint64(-1);
					Uint64 ored_bits = Uint64(0);
					for (std::vector<vRNG*>::const_iterator it = source_rngs.begin(); it != source_rngs.end(); it++) {
						Uint64 lf = (*it)->get_flags();
						anded_bits &= lf;
						ored_bits |= lf;
					}
					using namespace PractRand::RNGs::FLAG;
					return (OUTPUT_IS_BUFFERED | STATE_UNAVAILABLE) |
						(anded_bits & (SUPPORTS_FASTFORWARD | SUPPORTS_ENTROPY_ACCUMULATION | USES_SPECIFIED | ENDIAN_SAFE)) |
						(ored_bits & (CRYPTOGRAPHIC_SECURITY | USES_MULTIPLICATION | USES_COMPLEX_INSTRUCTIONS | USES_VARIABLE_SHIFTS | USES_INDIRECTION | USES_CYCLIC_BUFFER | USES_FLOW_CONTROL | USES_BIT_SCANS | USES_OTHER_WORD_SIZES | OUTPUT_IS_HASHED));
				}
				void MultiplexTransformRNG::walk_state(StateWalkingObject *walker) {
					for (std::vector<vRNG*>::const_iterator it = source_rngs.begin(); it != source_rngs.end(); it++) {
						(*it)->walk_state(walker);
					}
					if (!walker->is_read_only()) index = 999999;
				}
				MultiplexTransformRNG::MultiplexTransformRNG(const std::vector<vRNG*> &sources) {
					buffer = new Tests::TestBlock;
					index = 999999;
					source_rngs = sources;
				}
				MultiplexTransformRNG::~MultiplexTransformRNG() { delete buffer; buffer = nullptr; }


				ReinterpretAsUnknown::ReinterpretAsUnknown( vRNG *rng ) : Transform8(rng) {
					PractRand::Tests::TestBlock *block = new PractRand::Tests::TestBlock;
					buffer = &block->as8[0];
					index = 8192 / OUTPUT_BITS;
				}
				ReinterpretAsUnknown::~ReinterpretAsUnknown() {
					PractRand::Tests::TestBlock *block = (PractRand::Tests::TestBlock*) buffer;
					delete block;
				}
				void ReinterpretAsUnknown::refill() {
					PractRand::Tests::TestBlock *block = (PractRand::Tests::TestBlock*) buffer;
					block->fill(base_rng);
					index = 0;
				}
				Uint8 ReinterpretAsUnknown::raw8() {
					if (index >= 8192 / OUTPUT_BITS) refill();
					return buffer[index++];
				}
				std::string ReinterpretAsUnknown::get_name() const {return std::string("AsUnknown(") + base_rng->get_name() + ")";}

				ReinterpretAs8::ReinterpretAs8( vRNG *rng ) : Transform8(rng) {
					PractRand::Tests::TestBlock *block = new PractRand::Tests::TestBlock;
					buffer = &block->as8[0];
					index = 8192 / OUTPUT_BITS;
				}
				ReinterpretAs8::~ReinterpretAs8() {
					PractRand::Tests::TestBlock *block = (PractRand::Tests::TestBlock*) buffer;
					delete block;
				}
				void ReinterpretAs8::refill() {
					PractRand::Tests::TestBlock *block = (PractRand::Tests::TestBlock*) buffer;
					block->fill(base_rng);
					index = 0;
				}
				Uint8 ReinterpretAs8::raw8() {
					if (index >= 8192 / OUTPUT_BITS) refill();
					return buffer[index++];
				}
				std::string ReinterpretAs8::get_name() const {return std::string("As8(") + base_rng->get_name() + ")";}

				ReinterpretAs16::ReinterpretAs16( vRNG *rng ) : Transform16(rng) {
					PractRand::Tests::TestBlock *block = new PractRand::Tests::TestBlock;
					buffer = &block->as16[0];
					index = 8192 / OUTPUT_BITS;
				}
				ReinterpretAs16::~ReinterpretAs16() {
					PractRand::Tests::TestBlock *block = (PractRand::Tests::TestBlock*) buffer;
					delete block;
				}
				void ReinterpretAs16::refill() {
					PractRand::Tests::TestBlock *block = (PractRand::Tests::TestBlock*) buffer;
					block->fill(base_rng);
					index = 0;
				}
				Uint16 ReinterpretAs16::raw16() {
					if (index >= 8192 / OUTPUT_BITS) refill();
					return buffer[index++];
				}
				std::string ReinterpretAs16::get_name() const {return std::string("As16(") + base_rng->get_name() + ")";}

				ReinterpretAs32::ReinterpretAs32( vRNG *rng ) : Transform32(rng) {
					PractRand::Tests::TestBlock *block = new PractRand::Tests::TestBlock;
					buffer = &block->as32[0];
					index = 8192 / OUTPUT_BITS;
				}
				ReinterpretAs32::~ReinterpretAs32() {
					PractRand::Tests::TestBlock *block = (PractRand::Tests::TestBlock*) buffer;
					delete block;
				}
				void ReinterpretAs32::refill() {
					PractRand::Tests::TestBlock *block = (PractRand::Tests::TestBlock*) buffer;
					block->fill(base_rng);
					index = 0;
				}
				Uint32 ReinterpretAs32::raw32() {
					if (index >= 8192 / OUTPUT_BITS) refill();
					return buffer[index++];
				}
				std::string ReinterpretAs32::get_name() const {return std::string("As32(") + base_rng->get_name() + ")";}

				ReinterpretAs64::ReinterpretAs64( vRNG *rng ) : Transform64(rng) {
					PractRand::Tests::TestBlock *block = new PractRand::Tests::TestBlock;
					buffer = &block->as64[0];
					index = 8192 / OUTPUT_BITS;
				}
				ReinterpretAs64::~ReinterpretAs64() {
					PractRand::Tests::TestBlock *block = (PractRand::Tests::TestBlock*) buffer;
					delete block;
				}
				void ReinterpretAs64::refill() {
					PractRand::Tests::TestBlock *block = (PractRand::Tests::TestBlock*) buffer;
					block->fill(base_rng);
					index = 0;
				}
				Uint64 ReinterpretAs64::raw64() {
					if (index >= 8192 / OUTPUT_BITS) refill();
					return buffer[index++];
				}
				std::string ReinterpretAs64::get_name() const {return std::string("As64(") + base_rng->get_name() + ")";}

				void Xor::refill() {
					MultiplexTransformRNG::refill();
					PractRand::Tests::TestBlock tmp;
					buffer->fill(source_rngs[0]);
					for (int sri = 1; sri < source_rngs.size(); sri++) {
						tmp.fill(source_rngs[sri]);
						for (int i = 0; i < PractRand::Tests::TestBlock::SIZE / 8; i++) buffer->as64[i] ^= tmp.as64[i];
					}
				}
				std::string Xor::get_name() const {
					std::string rv = "xor(";
					for (int sri = 0; sri < source_rngs.size(); sri++) {
						if (sri) rv += ",";
						rv += source_rngs[sri]->get_name();
					}
					rv += ")";
					return rv;
				}

				Discard16to8::Discard16to8(vRNG *base_rng_) : Transform8(base_rng_) {
					//if (base_rng_->get_native_output_size() != INPUT_BITS) std::cerr << "* warning: Discard16to8 using incorrect input size?\n";
					PractRand::Tests::TestBlock *block = new PractRand::Tests::TestBlock;
					buffer = &block->as16[0];
					index = 8192 / INPUT_BITS;
				}
				void Discard16to8::refill() { PractRand::Tests::TestBlock *block = (PractRand::Tests::TestBlock*) buffer; block->fill(base_rng); index = 0; }
				std::string Discard16to8::get_name() const { return std::string("Discard16to8(") + base_rng->get_name() + ")"; }
				Uint8 Discard16to8::raw8() {
					if (index >= 8192 / INPUT_BITS) refill();
					return OutWord(buffer[index++]);
				}
				Discard32to8::Discard32to8(vRNG *base_rng_) : Transform8(base_rng_) {
					//if (base_rng_->get_native_output_size() != INPUT_BITS) std::cerr << "* warning: Discard16to8 using incorrect input size?\n";
					PractRand::Tests::TestBlock *block = new PractRand::Tests::TestBlock;
					buffer = &block->as32[0];
					index = 8192 / 32;
				}
				std::string Discard32to8::get_name() const { return std::string("Discard32to8(") + base_rng->get_name() + ")"; }
				void Discard32to8::refill() { PractRand::Tests::TestBlock *block = (PractRand::Tests::TestBlock*) buffer; block->fill(base_rng); index = 0; }
				Uint8 Discard32to8::raw8() {
					if (index >= 8192 / INPUT_BITS) refill();
					return OutWord(buffer[index++]);
				}
				Discard64to8::Discard64to8(vRNG *base_rng_) : Transform8(base_rng_) {
					//if (base_rng_->get_native_output_size() != INPUT_BITS) std::cerr << "* warning: Discard16to8 using incorrect input size?\n";
					PractRand::Tests::TestBlock *block = new PractRand::Tests::TestBlock;
					buffer = &block->as64[0];
					index = 8192 / INPUT_BITS;
				}
				std::string Discard64to8::get_name() const { return std::string("Discard64to8(") + base_rng->get_name() + ")"; }
				void Discard64to8::refill() {
					PractRand::Tests::TestBlock *block = (PractRand::Tests::TestBlock*) buffer; block->fill(base_rng); index = 0;
				}
				Uint8 Discard64to8::raw8() {
					if (index >= 8192 / INPUT_BITS) refill();
					return OutWord(buffer[index++]);
				}
				Discard32to16::Discard32to16(vRNG *base_rng_) : Transform16(base_rng_) {
					//if (base_rng_->get_native_output_size() != INPUT_BITS) std::cerr << "* warning: Discard16to8 using incorrect input size?\n";
					PractRand::Tests::TestBlock *block = new PractRand::Tests::TestBlock;
					buffer = &block->as32[0];
					index = 8192 / INPUT_BITS;
				}
				std::string Discard32to16::get_name() const { return std::string("Discard32to16(") + base_rng->get_name() + ")"; }
				void Discard32to16::refill() { PractRand::Tests::TestBlock *block = (PractRand::Tests::TestBlock*) buffer; block->fill(base_rng); index = 0; }
				Uint16 Discard32to16::raw16() {
					if (index >= 8192 / INPUT_BITS) refill();
					return OutWord(buffer[index++]);
				}
				Discard64to16::Discard64to16(vRNG *base_rng_) : Transform16(base_rng_) {
					//if (base_rng_->get_native_output_size() != INPUT_BITS) std::cerr << "* warning: Discard16to8 using incorrect input size?\n";
					PractRand::Tests::TestBlock *block = new PractRand::Tests::TestBlock;
					buffer = &block->as64[0];
					index = 8192 / INPUT_BITS;
				}
				std::string Discard64to16::get_name() const { return std::string("Discard64to16(") + base_rng->get_name() + ")"; }
				void Discard64to16::refill() { PractRand::Tests::TestBlock *block = (PractRand::Tests::TestBlock*) buffer; block->fill(base_rng); index = 0; }
				Uint16 Discard64to16::raw16() {
					if (index >= 8192 / INPUT_BITS) refill();
					return OutWord(buffer[index++]);
				}
				Discard64to32::Discard64to32(vRNG *base_rng_) : Transform32(base_rng_) {
					//if (base_rng_->get_native_output_size() != INPUT_BITS) std::cerr << "* warning: Discard16to8 using incorrect input size?\n";
					PractRand::Tests::TestBlock *block = new PractRand::Tests::TestBlock;
					buffer = &block->as64[0];
					index = 8192 / INPUT_BITS;
				}
				std::string Discard64to32::get_name() const { return std::string("Discard64to32(") + base_rng->get_name() + ")"; }
				void Discard64to32::refill() { PractRand::Tests::TestBlock *block = (PractRand::Tests::TestBlock*) buffer; block->fill(base_rng); index = 0; }
				Uint32 Discard64to32::raw32() {
					if (index >= 8192 / INPUT_BITS) refill();
					return OutWord(buffer[index++]);
				}

				void GeneralizedTableTransform::seed(Uint64 s) {base_rng->seed(s);}
				Uint64 GeneralizedTableTransform::get_flags() const {
					return base_rng->get_flags() | FLAG::USES_FLOW_CONTROL | FLAG::STATE_UNAVAILABLE;//not exactly, but close enough
				}
				GeneralizedTableTransform::~GeneralizedTableTransform() {delete base_rng;}
				void GeneralizedTableTransform::walk_state(StateWalkingObject *walker) {
					base_rng->walk_state(walker);
					buf_data = 0;
					buf_count = 0;
					finished_bytes.clear();
				}
				Uint8 GeneralizedTableTransform::raw8() {
					while (true) {
						if (!finished_bytes.empty()) {
							Uint8 rv = finished_bytes.front();
							finished_bytes.pop_front();
							return rv;
						}
						Uint64 in = base_rng->raw64();
						for (int i = 0; i < 8; i++) {
							const Entry &e = table[in & 255];
							in >>= 8;
							buf_data |= Uint32(e.data) << buf_count;
							buf_count += e.count;
							if (buf_count >= 8) {
								finished_bytes.push_back(buf_data & 255);
								buf_count -= 8;
								buf_data >>= 8;
							}
						}
					}
				}
				static const GeneralizedTableTransform::Entry self_shrinking_table11[256] = {
					{ 0, 0 },{ 0, 0 },{ 0, 1 },{ 1, 1 },{ 0, 0 },{ 0, 0 },{ 0, 1 },{ 1, 1 },
					{ 0, 1 },{ 0, 1 },{ 0, 2 },{ 1, 2 },{ 1, 1 },{ 1, 1 },{ 2, 2 },{ 3, 2 },
					{ 0, 0 },{ 0, 0 },{ 0, 1 },{ 1, 1 },{ 0, 0 },{ 0, 0 },{ 0, 1 },{ 1, 1 },
					{ 0, 1 },{ 0, 1 },{ 0, 2 },{ 1, 2 },{ 1, 1 },{ 1, 1 },{ 2, 2 },{ 3, 2 },
					{ 0, 1 },{ 0, 1 },{ 0, 2 },{ 1, 2 },{ 0, 1 },{ 0, 1 },{ 0, 2 },{ 1, 2 },
					{ 0, 2 },{ 0, 2 },{ 0, 3 },{ 1, 3 },{ 1, 2 },{ 1, 2 },{ 2, 3 },{ 3, 3 },
					{ 1, 1 },{ 1, 1 },{ 2, 2 },{ 3, 2 },{ 1, 1 },{ 1, 1 },{ 2, 2 },{ 3, 2 },
					{ 2, 2 },{ 2, 2 },{ 4, 3 },{ 5, 3 },{ 3, 2 },{ 3, 2 },{ 6, 3 },{ 7, 3 },
					{ 0, 0 },{ 0, 0 },{ 0, 1 },{ 1, 1 },{ 0, 0 },{ 0, 0 },{ 0, 1 },{ 1, 1 },
					{ 0, 1 },{ 0, 1 },{ 0, 2 },{ 1, 2 },{ 1, 1 },{ 1, 1 },{ 2, 2 },{ 3, 2 },
					{ 0, 0 },{ 0, 0 },{ 0, 1 },{ 1, 1 },{ 0, 0 },{ 0, 0 },{ 0, 1 },{ 1, 1 },
					{ 0, 1 },{ 0, 1 },{ 0, 2 },{ 1, 2 },{ 1, 1 },{ 1, 1 },{ 2, 2 },{ 3, 2 },
					{ 0, 1 },{ 0, 1 },{ 0, 2 },{ 1, 2 },{ 0, 1 },{ 0, 1 },{ 0, 2 },{ 1, 2 },
					{ 0, 2 },{ 0, 2 },{ 0, 3 },{ 1, 3 },{ 1, 2 },{ 1, 2 },{ 2, 3 },{ 3, 3 },
					{ 1, 1 },{ 1, 1 },{ 2, 2 },{ 3, 2 },{ 1, 1 },{ 1, 1 },{ 2, 2 },{ 3, 2 },
					{ 2, 2 },{ 2, 2 },{ 4, 3 },{ 5, 3 },{ 3, 2 },{ 3, 2 },{ 6, 3 },{ 7, 3 },
					{ 0, 1 },{ 0, 1 },{ 0, 2 },{ 1, 2 },{ 0, 1 },{ 0, 1 },{ 0, 2 },{ 1, 2 },
					{ 0, 2 },{ 0, 2 },{ 0, 3 },{ 1, 3 },{ 1, 2 },{ 1, 2 },{ 2, 3 },{ 3, 3 },
					{ 0, 1 },{ 0, 1 },{ 0, 2 },{ 1, 2 },{ 0, 1 },{ 0, 1 },{ 0, 2 },{ 1, 2 },
					{ 0, 2 },{ 0, 2 },{ 0, 3 },{ 1, 3 },{ 1, 2 },{ 1, 2 },{ 2, 3 },{ 3, 3 },
					{ 0, 2 },{ 0, 2 },{ 0, 3 },{ 1, 3 },{ 0, 2 },{ 0, 2 },{ 0, 3 },{ 1, 3 },
					{ 0, 3 },{ 0, 3 },{ 0, 4 },{ 1, 4 },{ 1, 3 },{ 1, 3 },{ 2, 4 },{ 3, 4 },
					{ 1, 2 },{ 1, 2 },{ 2, 3 },{ 3, 3 },{ 1, 2 },{ 1, 2 },{ 2, 3 },{ 3, 3 },
					{ 2, 3 },{ 2, 3 },{ 4, 4 },{ 5, 4 },{ 3, 3 },{ 3, 3 },{ 6, 4 },{ 7, 4 },
					{ 1, 1 },{ 1, 1 },{ 2, 2 },{ 3, 2 },{ 1, 1 },{ 1, 1 },{ 2, 2 },{ 3, 2 },
					{ 2, 2 },{ 2, 2 },{ 4, 3 },{ 5, 3 },{ 3, 2 },{ 3, 2 },{ 6, 3 },{ 7, 3 },
					{ 1, 1 },{ 1, 1 },{ 2, 2 },{ 3, 2 },{ 1, 1 },{ 1, 1 },{ 2, 2 },{ 3, 2 },
					{ 2, 2 },{ 2, 2 },{ 4, 3 },{ 5, 3 },{ 3, 2 },{ 3, 2 },{ 6, 3 },{ 7, 3 },
					{ 2, 2 },{ 2, 2 },{ 4, 3 },{ 5, 3 },{ 2, 2 },{ 2, 2 },{ 4, 3 },{ 5, 3 },
					{ 4, 3 },{ 4, 3 },{ 8, 4 },{ 9, 4 },{ 5, 3 },{ 5, 3 },{10, 4 },{11, 4 },
					{ 3, 2 },{ 3, 2 },{ 6, 3 },{ 7, 3 },{ 3, 2 },{ 3, 2 },{ 6, 3 },{ 7, 3 },
					{ 6, 3 },{ 6, 3 },{12, 4 },{13, 4 },{ 7, 3 },{ 7, 3 },{14, 4 },{15, 4 }
				};
				std::string GeneralizedTableTransform::get_name() const {return name;}
				vRNG *apply_SelfShrinkTransform(vRNG *base_rng) {
					return new GeneralizedTableTransform (base_rng, self_shrinking_table11, std::string("[SShrink]") + base_rng->get_name() );
				}
				//vRNG *apply_SimpleShrinkTransform(vRNG *base_rng) {
				//	return new GeneralizedTableTransform (base_rng, NULL, std::string("[Shrink3of4]") + base_rng->get_name() );
				//}



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
					if (!(walker->get_properties() & walker->FLAG_CLUMSY)) {
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
					if (!(walker->get_properties() & walker->FLAG_CLUMSY)) {
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
					if (!(walker->get_properties() & walker->FLAG_CLUMSY)) {
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
					if (!(walker->get_properties() & walker->FLAG_CLUMSY)) {
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
