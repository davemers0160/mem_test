#define _CRT_SECURE_NO_WARNINGS

#if defined(_WIN32) | defined(__WIN32__) | defined(__WIN32) | defined(_WIN64) | defined(__WIN64)

#include <windows.h>

#else
//#include <dlfcn.h>
typedef void* HINSTANCE;

#endif

// C/C++ includes
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <string>
#include <vector>
#include <thread>
#include <random>

#if defined(_WIN32) | defined(__WIN32__) | defined(__WIN32) | defined(_WIN64) | defined(__WIN64)

#include <intrin.h>

#else
    //For gcc / clang / icc targeting x86 / x86 - 64 use x86intrin.h
    //For gcc / clang / armcc targeting ARM with NEON use arm_neon.h
    //For gcc / clang / armcc targeting ARM with WMMX use mmintrin.h
    //For gcc / clang / xlcc targeting PowerPC with VMX(aka Altivec) and /or VSX use altivec.h
    //For gcc / clang targeting PowerPC with SPE use spe.h
    
    // X86
    #if defined(__x86_64__) || defined (__i386__)
    #include <x86intrin.h>
    // ARM
    #elif defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__)
    #include <arm_neon.h>
    #include <sys/time.h>

    #endif

#endif

// -------------------------------GLOBALS--------------------------------------
// Store 64-bit integer a into memory using a non-temporal hint to minimize
// cache pollution. If the cache line containing address mem_addr is already in
// the cache, the cache will be updated.
// https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#text=_mm_stream_si64
#if defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__)
inline void _mm_stream_si64(int64_t *p, int64_t a)
{
    vst1_s64(p, vdup_n_s64(a));
}
#endif

// ----------------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    uint32_t idx, jdx;
    int bp = 0;

    typedef std::chrono::microseconds us;
    auto start_time = std::chrono::high_resolution_clock::now();
    auto stop_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<us>(stop_time - start_time);

    //----------------------------------------------------------------------------------------
    // variables    
//    int64_t mem_data[4096] = { 0 };
    int64_t dummy_value = 0xFFFFFFFFFFFFFFFF;

    uint32_t buffer_size = 256;
    int32_t alignment_size = 16;

#if defined(_WIN32) | defined(__WIN32__) | defined(__WIN32) | defined(_WIN64) | defined(__WIN64)
    int64_t *mem_data = (int64_t *)_aligned_malloc(16 * buffer_size * sizeof(int64_t), alignment_size);
    int64_t *mem_address;
#else
    long long *mem_data = (long long *)std::aligned_alloc(alignment_size, 16 * buffer_size * sizeof(long long));
    long long *mem_address;

#endif


    std::vector<int16_t> frame_preamble = {1, 0, 1, 0, 1, 0, 1, 0};

    std::vector<int16_t> frame_data = {0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0 ,0 ,0 ,0, 1, 0, 0, 1, 1};

    std::vector<int16_t> frame_bits = frame_preamble;
    frame_bits.insert(frame_bits.end(), frame_data.begin(), frame_data.end());

    // bit time
    std::chrono::microseconds bit_time(10000);

    while(1)
    {
        std::cout << "starting frame..." << std::endl;
        
        for (idx = 0; idx < frame_bits.size(); ++idx)
        {
            mem_address = mem_data;
            
            if(frame_bits[idx] == 1)
            {
                start_time = std::chrono::high_resolution_clock::now();
                stop_time = std::chrono::high_resolution_clock::now();                
                
                while (std::chrono::duration_cast<us>(stop_time - start_time) < bit_time)
                {
                    mem_address = mem_data; 
                    for (jdx = 0; jdx < buffer_size; ++jdx)
                    {
#if defined(_WIN32) | defined(__WIN32__) | defined(__WIN32) | defined(_WIN64) | defined(__WIN64)
                        _mm_stream_si64x(mem_address, dummy_value);
#else
                        _mm_stream_si64(mem_address, dummy_value);
#endif
                        mem_address += alignment_size;
                    }
                    stop_time = std::chrono::high_resolution_clock::now();
                }

                std::this_thread::sleep_for(bit_time);

            }
            else
            {
                std::this_thread::sleep_for(bit_time);

                start_time = std::chrono::high_resolution_clock::now();
                stop_time = std::chrono::high_resolution_clock::now();

                while (std::chrono::duration_cast<us>(stop_time - start_time) < bit_time)
                {
                    mem_address = mem_data;
                    for (jdx = 0; jdx < buffer_size; ++jdx)
                    {
#if defined(_WIN32) | defined(__WIN32__) | defined(__WIN32) | defined(_WIN64) | defined(__WIN64)
                        _mm_stream_si64x(mem_address, dummy_value);
#else
                        _mm_stream_si64(mem_address, dummy_value);
#endif
                        mem_address += alignment_size;
                    }

                    stop_time = std::chrono::high_resolution_clock::now();
                }
            }

        }

    }

    bp = 2;
    std::cin.ignore();
	return 0;

}	// end of main
