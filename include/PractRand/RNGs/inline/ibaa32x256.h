class inline_RNG_ibaa32x256 {
	enum {SIZE_L2 = 8, SIZE = 256};
	Uint32 table[SIZE], results[SIZE], accum, old, index;
public:
	//raw random bits:
	Uint32 raw32() {
		if (index < SIZE) return results[index++];
		Uint32 x, y;
		unsigned long i;
		for (i = 0; i < SIZE/2; i++) {
			x = table[i];
			accum = ((accum << 19) | (accum >> 13)) + 
				table[i + SIZE/2];
			table[i] = y = table[x & (SIZE-1)] + accum + old;
			results[i] = old = table[(y >> SIZE_L2) & (SIZE-1)] + x;
		}
		for (; i < SIZE; i++) {
			x = table[i];
			accum = ((accum << 19) | (accum >> 13)) + 
				table[i - SIZE/2];
			table[i] = y = table[x & (SIZE-1)] + accum + old;
			results[i] = old = table[(y >> SIZE_L2) & (SIZE-1)] + x;
		}
		index = 1;
		return results[0];
	}
	Uint8  raw8()  {return (Uint8) raw32();}
	Uint16 raw16() {return (Uint16)raw32();}
	Uint64 raw64() {Uint32 r = raw32(); return (Uint64(r)<<32) | raw32();}

	//random integers:
	Uint32 randi ( Uint32 max ) {
		Uint32 mask, tmp;
		max -= 1;
		mask = max;
		mask |= mask >> 1; mask |= mask >>  2; mask |= mask >> 4;
		mask |= mask >> 8; mask |= mask >> 16;
		while (1) {
			tmp = raw32() & mask;
			if (tmp <= max) return tmp;
		}
	}
	Uint32 randi ( Uint32 min, Uint32 max ) {return randi(max-min) + min;}

	Uint64 randli ( Uint64 max ) {
		Uint64 mask, tmp;
		max -= 1;
		mask = max;
		mask |= mask >> 1; mask |= mask >>  2; mask |= mask >>  4;
		mask |= mask >> 8; mask |= mask >> 16; mask |= mask >> 32;
		while (1) {
			tmp = raw64() & mask;
			if (tmp <= max) return tmp;
		}
	}
	Uint64 randli ( Uint64 min, Uint64 max ) {return randli(max-min) + min;}

	//this method introduces bias, but is much faster than 
	Uint32 randi_fast ( Uint32 max ) {return (Uint32)((raw32() * (Uint64)max) >> 32);}
	Uint32 randi_fast ( Uint32 min, Uint32 max ) {return randi_fast(max-min) + min;}

	//random floating point numbers:
	float randf ( ) { return float(raw32() * (1.0 / 4294967296.0)); }
	float randf ( float m ) { return randf() * m; }
	float randf ( float min, float max ) { return randf() * (max-min) + min; }

	double randlf ( ) { return ((double)raw64()) * (double)(1.0 / 18446744073709551616.0); }
	double randlf ( double m ) { return randlf() * m; }
	double randlf ( double min, double max ) { return randlf() * (max-min) + min; }

	//seeding functions
	void seed ( Uint64 s ) {//a fairly weak seeding function
		accum = (Uint32)s;
		old = Uint32(s>>32);
		for (unsigned int i = 0; i < SIZE; i++) state[i] = i;
		index = SIZE;
		for (unsigned int i = 0; i < SIZE; i++) raw32();
		accum ^= Uint32(s>>32);
		old ^= (Uint32)s;
		if (!accum) accum = 1;
	}
};
