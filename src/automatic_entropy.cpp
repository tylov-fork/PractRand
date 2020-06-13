#include <string>
#include <stdlib.h>
#include <time.h>

#include "PractRand/config.h"
#include "PractRand/rng_basics.h"
#include "PractRand/rng_helpers.h"
#include "PractRand/rng_internals.h"

#ifdef _WIN32
#include <windows.h>
#endif

/*
TO DO: improve entropy gathering code; it kinda sucks now
ideas:
	try using the intended platform method
		RtlGenRandom on windows?
		/dev/random and/or /dev/urandom on unix?
	improve RDTSC
		add implementations for gcc and/or intel-C
		check x86 generation to prevent use or RDTSC on 386/486 targets?
*/

using namespace PractRand;

void PractRand::Internals::add_entropy_automatically( PractRand::RNGs::vRNG *rng, int milliseconds ) {
#if 1
	{//libc
		//not much entropy, but we take what we can get
		rng->add_entropy64((Uint64)::time(NULL));
		rng->add_entropy64((Uint64)::clock());
		rng->add_entropy64((Uint64)rng);
		void *p = malloc(1);
		rng->add_entropy64((Uint64)p);
		free(p);
	}
#endif
#if (defined _WIN32) && 1
	{//win32 crypto PRNG
		HMODULE hLib=LoadLibraryA("ADVAPI32.DLL");
		if (hLib) {
			BOOLEAN (APIENTRY *get_random_bytes)(void*, ULONG) =
				(BOOLEAN (APIENTRY *)(void*,ULONG))GetProcAddress(hLib,"SystemFunction036");
			if (get_random_bytes) {
				enum {NUM_RANDOM_U64s = 4};
				Uint64 buf[NUM_RANDOM_U64s];
				if(get_random_bytes(buf,NUM_RANDOM_U64s*sizeof(buf[0]))) {
					for (int i = 0; i < NUM_RANDOM_U64s; i++) rng->add_entropy64(buf[i]);
				}
			}
			FreeLibrary(hLib);
		}
	}
#endif
#if (defined _WIN32) && 1
	{//win32 (intended to work on win2k and later)
		::LARGE_INTEGER qt;
		QueryPerformanceCounter(&qt);
		rng->add_entropy64(qt.QuadPart);
		rng->add_entropy32(::GetCurrentProcessId());
		rng->add_entropy64((Uint64)::GetCurrentProcess());
		rng->add_entropy32(::GetCurrentThreadId());
		//vrng->add_entropy32(::GetCurrentProcessorNumber());//may require Vista?
		SYSTEMTIME st;
		::GetSystemTime(&st);
		rng->add_entropy_N(&st, sizeof(st));
		MEMORYSTATUS mem;
		GlobalMemoryStatus(&mem);
		rng->add_entropy_N(&mem, sizeof(mem));
		DWORD t = GetCurrentTime();
		rng->add_entropy32(t);
	}
#endif
#if defined _MSC_VER && ((defined _M_IX86 && _M_IX86 >= 500))
// || defined _M_X64 || defined _M_AMD64)
	{
		Uint32 t;
		__asm {
			RDTSC
			mov [t], eax
		}
		rng->add_entropy32(t);
	}
#endif

#if (defined _WIN32) && 0 //DISABLED
	{//WINDOWS REGISTRY
		//Not a true entropy source, but an accumulator across multiple runs, 
		//  which we need more than we need another entropy source.  
		//Probably ought to be a transaction,
		//  but that would require Vista as a minimum Windows version, 
		//  and I want to support 2k and XP also
		HKEY key = NULL;
		for (int tries = 0; tries < 3; tries++) {
			//printf("attempt %d\n", tries);
			if (key) {
				RegCloseKey(key);
				key = NULL;
			}
			DWORD d;
			long r;
			r = RegCreateKeyExA(
				HKEY_CURRENT_USER,
				"Software\\PractRand",
				0,
				NULL,
				REG_OPTION_NON_VOLATILE,
				KEY_QUERY_VALUE | KEY_SET_VALUE,
				NULL,
				&key,
				&d
			);
			if (r != ERROR_SUCCESS) continue;
			//printf("%s\n", (d==REG_CREATED_NEW_KEY) ? "failed to find key, creating" : "found key");
			enum {TARGET_SIZE = 128, BUFFER_SIZE=512};
			Uint8 buffer[BUFFER_SIZE];
			DWORD size = BUFFER_SIZE;
			DWORD type;
			//to do: use different seed names for different EntropyPool algorithms
			r = RegQueryValueExA(key, "seed", NULL, &type, buffer, &size);
			if (r == ERROR_MORE_DATA || type != REG_BINARY) {
				r = ERROR_FILE_NOT_FOUND;
				d = REG_CREATED_NEW_KEY;
				//printf("seed invalid\n");
			}
			if (r == ERROR_FILE_NOT_FOUND && (d == REG_CREATED_NEW_KEY || (tries > 0)) ) {
				//printf("resetting seed (either invalid or failed to read multiple times)\n");
				rng->flush_buffers();
				for (int i = 0; i < TARGET_SIZE; i++) buffer[i] = rng->raw8();
				r = RegSetValueExA(key, "seed", 0, REG_BINARY, buffer, TARGET_SIZE);
			}
			if (r != ERROR_SUCCESS) continue;
			//printf("using seed (%02x%02x)\n", buffer[1], buffer[0]);
			rng->add_entropy_N(buffer, size);
			if (size > TARGET_SIZE) size = TARGET_SIZE;
			rng->flush_buffers();
			//printf("buffers flushed\n");
			for (unsigned int i = 0; i < size; i++) buffer[i] ^= rng->raw8();
			for (unsigned int i = size; i < TARGET_SIZE; i++) buffer[i] = rng->raw8();
			//printf("attempting to set new seed (%02x%02x)\n", buffer[1], buffer[0]);
			r = RegSetValueExA(key, "seed", 0, REG_BINARY, buffer, TARGET_SIZE);
			if (r != ERROR_SUCCESS) continue;
			//printf("successfully set new seed\n");
			break;
		}
		if (key) {
			RegCloseKey(key);
			key = NULL;
		}
	}
#endif //_WIN32 (registry)

#if (defined _WIN32) && 0
	if (milliseconds) {//accumulate entropy over time (does a VERY bad job)
		LARGE_INTEGER qt, qt2;
		QueryPerformanceCounter(&qt);
		DWORD start_time = GetTickCount();
		DWORD t = start_time;
		rng->add_entropy32(t);
		long count=0;
		while (t < start_time + milliseconds) {
			if (QueryPerformanceCounter(&qt2)) {
				if (qt2.QuadPart != qt.QuadPart) {
					rng->add_entropy32(qt2.LowPart);
					qt.QuadPart = qt2.QuadPart;
				}
			}
			count++;
			DWORD t2 = GetTickCount();
			if (t != t2) {
				rng->add_entropy32(t2 + count);
				t = t2;
			}
		}
		rng->add_entropy16(Uint16(count));
		milliseconds = 0;
	}
#endif

	rng->flush_buffers();
}
