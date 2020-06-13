class inline_RNG_jsf32 {
	Uint32 a, b, c, d;
public:
	//raw random bits:
	Uint32 raw32() {
		Uint32 e = a - ((b << 27) | (b >> 5));
		a = b ^ ((c << 17) | (c >> 15));
		b = c + d;
		c = d + e;
		d = e + a;
		return b;
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

	void seed ( Uint64 s ) {
		a = (Uint32)s; b = Uint32(s>>32); c = a&b; d = 71; 
		for (int i = 0; i < 4; i++) raw32();
	}
};
