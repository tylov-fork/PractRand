#ifndef RNG_from_name_h
#define RNG_from_name_h

namespace Special_RNGs {
	class RNG_stdin : public PractRand::RNGs::vRNG {
		static void read_failed() {
			std::fprintf(stderr, "error reading input\n");
			std::exit(0);
		}
		virtual PractRand::Uint8 raw8() {
			PractRand::Uint8 rv;
			if (1 == std::fread(&rv, sizeof(rv), 1, stdin)) return rv; else read_failed();
			return 0;
		}
		virtual PractRand::Uint16 raw16() {
			PractRand::Uint16 rv;
			if (1 == std::fread(&rv, sizeof(rv), 1, stdin)) return rv; else read_failed();
			return 0;
		}
		virtual PractRand::Uint32 raw32() {
			PractRand::Uint32 rv;
			if (1 == std::fread(&rv, sizeof(rv), 1, stdin)) return rv; else read_failed();
			return 0;
		}
		virtual PractRand::Uint64 raw64() {
			PractRand::Uint64 rv;
			if (1 == std::fread(&rv, sizeof(rv), 1, stdin)) return rv; else read_failed();
			return 0;
		}
		virtual std::string get_name() const {return "RNG_stdin";}
		virtual int get_native_output_size() const {return -1;}
		virtual void walk_state(PractRand::StateWalkingObject *) {}
	};
	class RNG_stdin8 : public PractRand::RNGs::vRNG8 {
		static void read_failed() {
			std::fprintf(stderr, "error reading input\n");
			std::exit(0);
		}
		virtual PractRand::Uint8 raw8() {
			PractRand::Uint8 rv;
			if (1 == std::fread(&rv, sizeof(rv), 1, stdin)) return rv; else read_failed();
			return 0;
		}
		virtual std::string get_name() const {return "RNG_stdin8";}
		virtual void walk_state(PractRand::StateWalkingObject *) {}
	};
	class RNG_stdin16 : public PractRand::RNGs::vRNG16 {
		static void read_failed() {
			std::fprintf(stderr, "error reading input\n");
			std::exit(0);
		}
		virtual PractRand::Uint16 raw16() {
			PractRand::Uint16 rv;
			if (1 == std::fread(&rv, sizeof(rv), 1, stdin)) return rv; else read_failed();
			return 0;
		}
		virtual std::string get_name() const {return "RNG_stdin16";}
		virtual void walk_state(PractRand::StateWalkingObject *) {}
	};
	class RNG_stdin32 : public PractRand::RNGs::vRNG32 {
		static void read_failed() {
			std::fprintf(stderr, "error reading input\n");
			std::exit(0);
		}
		virtual PractRand::Uint32 raw32() {
			PractRand::Uint32 rv;
			if (1 == std::fread(&rv, sizeof(rv), 1, stdin)) return rv; else read_failed();
			return 0;
		}
		virtual std::string get_name() const {return "RNG_stdin32";}
		virtual void walk_state(PractRand::StateWalkingObject *) {}
	};
	class RNG_stdin64 : public PractRand::RNGs::vRNG64 {
		static void read_failed() {
			std::fprintf(stderr, "error reading input\n");
			std::exit(0);
		}
		virtual PractRand::Uint64 raw64() {
			PractRand::Uint64 rv;
			if (1 == std::fread(&rv, sizeof(rv), 1, stdin)) return rv; else read_failed();
			return 0;
		}
		virtual std::string get_name() const {return "RNG_stdin64";}
		virtual void walk_state(PractRand::StateWalkingObject *) {}
	};
}

namespace RNG_Factories {
	std::map<std::string, PractRand::RNGs::vRNG *(*)(std::vector<std::string> &params)> RNG_factory_index;

	bool parse_argument_list(const std::string &raw, std::string &name, std::vector<std::string> &parameters) {
		// "BDS(BDS(chacha(8),4),8)" -> "SS", "BDS(chacha(8),4)", "8"
		parameters.clear();
		const char *base = raw.c_str();
		const char *tmp = strpbrk(base, "(,)");
		if (!tmp) {
			name = raw;
			return true;
		}
		if (*tmp != '(') return false;
		if (tmp == base) return false;//zero length name not allowed
		std::size_t checkpoint_pos = tmp-base+1;
		name = raw.substr(0, checkpoint_pos-1);
		int level = 1;
		while (level) {
			tmp = strpbrk(tmp+1, "(,)");
			if (!tmp) return false;
			else if (*tmp == '(') level++;
			else if (*tmp == ')' || *tmp == ',') {
				if (level == 1) {
					std::size_t new_checkpoint = tmp-base+1;
					if (new_checkpoint == checkpoint_pos + 1) return false;//parameter = empty string
					parameters.push_back(raw.substr(checkpoint_pos,new_checkpoint-checkpoint_pos-1));
					checkpoint_pos = new_checkpoint;
				}
				if (*tmp == ')') level--;
			}
		}
		return checkpoint_pos == raw.size();
	}
	PractRand::RNGs::vRNG *create_rng(const std::string &raw) {
		std::string rng_name;
		std::vector<std::string> parameters;
		if (!parse_argument_list(raw, rng_name, parameters)) return NULL;
		std::map<std::string, PractRand::RNGs::vRNG *(*)(std::vector<std::string> &params)>::iterator it = RNG_factory_index.find(rng_name);
		if (it == RNG_factory_index.end()) return NULL;
		return it->second(parameters);
	}

	template<class RNG>
	PractRand::RNGs::vRNG *_generic_recommended_RNG_factory(std::vector<std::string> &params) {
		if (params.size() != 0) return NULL;
		return new RNG(PractRand::SEED_NONE);
	}
	template<class RNG>
	PractRand::RNGs::vRNG *_generic_notrecommended_RNG_factory(std::vector<std::string> &params) {
		if (params.size() != 0) return NULL;
		return new RNG();
	}
	PractRand::RNGs::vRNG *chacha_factory(std::vector<std::string> &params) {
		if (params.size() > 1) {params.push_back("too many parameters for chacha");return NULL;}
		if (params.size() == 0) return new PractRand::RNGs::Polymorphic::chacha(PractRand::SEED_NONE);
		std::string &param = params[0];
		int value = 0;
		for (unsigned long i = 0; i < param.size(); i++) {
			if (param[i] < '0' || param[i] > '9') {params.push_back("parameter must be an integer between 1 and 255, inclusive");return NULL;}
			value = value * 10 + (param[i] - '0');
			if (value > 255) {params.push_back("number of rounds too high for chacha");return NULL;}
		}
		if (value < 1) {params.push_back("number of rounds too low for chacha");return NULL;}
		PractRand::RNGs::Polymorphic::chacha *rng = new PractRand::RNGs::Polymorphic::chacha(PractRand::SEED_NONE);
		rng->set_rounds(value);
		return rng;
	}
	PractRand::RNGs::vRNG *salsa_factory(std::vector<std::string> &params) {
		if (params.size() > 1) {params.push_back("too many parameters for salsa");return NULL;}
		if (params.size() == 0) return new PractRand::RNGs::Polymorphic::salsa(PractRand::SEED_NONE);
		std::string &param = params[0];
		int value = 0;
		for (unsigned long i = 0; i < param.size(); i++) {
			if (param[i] < '0' || param[i] > '9') {params.push_back("parameter must be an integer between 1 and 255, inclusive");return NULL;}
			value = value * 10 + (param[i] - '0');
			if (value > 255) {params.push_back("number of rounds too high for salsa");return NULL;}
		}
		if (value < 1) {params.push_back("number of rounds too low for salsa");return NULL;}
		PractRand::RNGs::Polymorphic::salsa *rng = new PractRand::RNGs::Polymorphic::salsa(PractRand::SEED_NONE);
		rng->set_rounds(value);
		return rng;
	}
	PractRand::RNGs::vRNG *SelfShrink_factory(std::vector<std::string> &params) {
		//SShrink(BaseRNG)
		if (params.size() != 1) return NULL;
		PractRand::RNGs::vRNG *rng = create_rng(params[0]);
		if (!rng) return NULL;
		return PractRand::RNGs::Polymorphic::NotRecommended::apply_SelfShrinkTransform(rng);
	}
	PractRand::RNGs::vRNG *BDS_factory(std::vector<std::string> &params) {
		//BDS(BaseRNG,log2_of_table_size)
		if (params.size() < 2 || params.size() > 3) return NULL;
		int L2 = std::atoi(params[1].c_str());
		if (L2 < 1 || L2 > 16) return NULL;
		int shift = -1;
		if (params.size() == 3) shift = std::atoi(params[2].c_str());
		PractRand::RNGs::vRNG *rng = create_rng(params[0]);
		if (!rng) return NULL;
		return PractRand::RNGs::Polymorphic::NotRecommended::apply_BaysDurhamShuffle(rng, L2, shift);
	}
	template<class RNG>
	PractRand::RNGs::vRNG *_generic_single_parameter_RNG_factory(std::vector<std::string> &params) {
		if (params.size() != 1) return NULL;
		return new RNG(atoi(params[0].c_str()));
	}
	void register_recommended_RNGs() {
#define REGISTER_RNG_0(RNG) RNG_Factories::RNG_factory_index[ #RNG ] = _generic_recommended_RNG_factory<PractRand::RNGs::Polymorphic:: RNG>;
		REGISTER_RNG_0( xsm32 )
		REGISTER_RNG_0( xsm64 )
		REGISTER_RNG_0( hc256 )
		REGISTER_RNG_0( isaac32x256 )
		REGISTER_RNG_0( isaac64x256 )
		REGISTER_RNG_0( efiix8x384 )
		REGISTER_RNG_0( efiix16x384 )
		REGISTER_RNG_0( efiix32x384 )
		REGISTER_RNG_0( efiix64x384 )
		REGISTER_RNG_0( mt19937 )
		REGISTER_RNG_0( jsf32 )
		REGISTER_RNG_0( jsf64 )
		REGISTER_RNG_0( sfc16 )
		REGISTER_RNG_0( sfc32 )
		REGISTER_RNG_0( sfc64 )
		REGISTER_RNG_0( arbee )
		REGISTER_RNG_0( trivium )
		REGISTER_RNG_0( sha2_based_pool )
		RNG_factory_index["chacha"] = chacha_factory;
		RNG_factory_index["salsa"] = salsa_factory;
#undef REGISTER_RNG_0
	}
	void register_input_RNGs() {
#define REGISTER_RNG_0(RNG) RNG_Factories::RNG_factory_index[ #RNG ] = _generic_notrecommended_RNG_factory<  Special_RNGs:: RNG_ ## RNG>;
		REGISTER_RNG_0(stdin)
		REGISTER_RNG_0(stdin8)
		REGISTER_RNG_0(stdin16)
		REGISTER_RNG_0(stdin32)
		REGISTER_RNG_0(stdin64)
#undef REGISTER_RNG_0
	}
	void register_nonrecommended_RNGs() {
#define REGISTER_RNG_0(RNG) RNG_Factories::RNG_factory_index[ #RNG ] = _generic_notrecommended_RNG_factory<  PractRand::RNGs::Polymorphic::NotRecommended:: RNG>;
#define REGISTER_RNG_1(RNG) RNG_Factories::RNG_factory_index[ #RNG ] = _generic_single_parameter_RNG_factory<PractRand::RNGs::Polymorphic::NotRecommended:: RNG>;
		// include/PractRand/RNGs/other/simple.h
		REGISTER_RNG_0(xsalta16x3)
		REGISTER_RNG_0(xsaltb16x3)
		REGISTER_RNG_0(xsaltc16x3)
		REGISTER_RNG_0(xorshift32)
		REGISTER_RNG_0(xorshift64)
		REGISTER_RNG_0(xorshift128_32)
		REGISTER_RNG_0(xorshift128_64)
		REGISTER_RNG_0(xorshift32_16)
		REGISTER_RNG_0(xorshift64_32)
		REGISTER_RNG_0(xorshift32x4)
		REGISTER_RNG_0(sapparot)
		REGISTER_RNG_0(sap48_16)
		REGISTER_RNG_0(sap96_32)
		REGISTER_RNG_0(flea32x1)
		REGISTER_RNG_0(sfc_v1_16)
		REGISTER_RNG_0(sfc_v1_32)
		REGISTER_RNG_0(sfc_v2_16)
		REGISTER_RNG_0(sfc_v2_32)
		REGISTER_RNG_0(sfc_v3_16)
		REGISTER_RNG_0(sfc_v3_32)
		REGISTER_RNG_0(simpleA)
		REGISTER_RNG_0(simpleB)
		REGISTER_RNG_0(simpleC)
		REGISTER_RNG_0(simpleD)
		REGISTER_RNG_0(simpleE)
		REGISTER_RNG_0(simpleF)
		REGISTER_RNG_0(simpleG)
		REGISTER_RNG_0(mo_Lesr32)
		REGISTER_RNG_0(mo_ResrRers32)
		REGISTER_RNG_0(mo_Rers64_32)
		REGISTER_RNG_0(mo_Resr64_32)
		REGISTER_RNG_0(mo_Resdra64_32)

		// include/PractRand/RNGs/other/mult.h
		REGISTER_RNG_0(lcg32_16)
		REGISTER_RNG_0(lcg32)
		REGISTER_RNG_0(lcg32_16_extended)
		REGISTER_RNG_0(lcg32_extended)
		REGISTER_RNG_0(garthy16)
		REGISTER_RNG_0(garthy32)
		REGISTER_RNG_0(binarymult16)
		REGISTER_RNG_0(binarymult32)
		REGISTER_RNG_0(rxmult16)
		REGISTER_RNG_0(multish2x64)
		REGISTER_RNG_0(multish3x32)
		REGISTER_RNG_0(multish4x16)
		REGISTER_RNG_0(old_mwlac16)
		REGISTER_RNG_0(mo_Cmfr32)
		REGISTER_RNG_0(mo_Cmr32)
		REGISTER_RNG_1(clcg96_8_varqual)
		REGISTER_RNG_1(clcg96_16_varqual)
		REGISTER_RNG_1(clcg96_32_varqual)
		REGISTER_RNG_1(cxlcg96_8_varqual)
		REGISTER_RNG_1(cxlcg96_16_varqual)
		REGISTER_RNG_1(cxlcg96_32_varqual)
		REGISTER_RNG_1(lcg64_8_varqual)
		REGISTER_RNG_1(lcg64_16_varqual)
		REGISTER_RNG_1(lcg64_32_varqual)
		REGISTER_RNG_1(xlcg64_8_varqual)
		REGISTER_RNG_1(xlcg64_16_varqual)
		REGISTER_RNG_1(xlcg64_32_varqual)

		// include/PractRand/RNGs/other/fibonacci.h
		REGISTER_RNG_0(lfsr_medium)
		REGISTER_RNG_0(mm32)
		REGISTER_RNG_0(mm32_16)
		REGISTER_RNG_0(mm32_awc)
		REGISTER_RNG_0(mwc4691)
		REGISTER_RNG_0(cbuf_accum)
		REGISTER_RNG_0(dual_cbuf)
		REGISTER_RNG_0(dual_cbuf_accum)
		REGISTER_RNG_0(ranrot32)
		REGISTER_RNG_0(fibmul32_16)
		REGISTER_RNG_0(ranrot32)
		REGISTER_RNG_0(ranrot3tap32)

		// include/PractRand/RNGs/other/indirection.h
		REGISTER_RNG_0(rc4)
		REGISTER_RNG_0(rc4_weakened)
		REGISTER_RNG_1(ibaa8)
		REGISTER_RNG_1(ibaa16)
		REGISTER_RNG_1(ibaa32)
		REGISTER_RNG_1(isaac32_small)

		// include/PractRand/RNGs/other/transform.h
		RNG_Factories::RNG_factory_index["BDS"] = BDS_factory;
		RNG_Factories::RNG_factory_index["SShrink"] = SelfShrink_factory;
#undef REGISTER_RNG_0
#undef REGISTER_RNG_1
	}
}

#endif//RNG_from_name_h