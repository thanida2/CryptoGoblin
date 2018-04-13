/*
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
  *
  * Additional permission under GNU GPL version 3 section 7
  *
  * If you modify this Program, or any covered work, by linking or combining
  * it with OpenSSL (or a modified version of that library), containing parts
  * covered by the terms of OpenSSL License and SSLeay License, the licensors
  * of this Program grant you additional permission to convey the resulting work.
  *
  */

/*
 * The orginal author of this AES implementation is Karl Malbrain.
 */

#ifdef __GNUC__
# ifdef _WIN64
#  include <intrin.h>
# else
#  include <x86intrin.h>
# endif
#else
# include <intrin.h>
#endif // __GNUC__

#include <inttypes.h>

#define TABLE_ALIGN     64

#define sb_data(w) {\
    w(0x63), w(0x7c), w(0x77), w(0x7b), w(0xf2), w(0x6b), w(0x6f), w(0xc5),\
    w(0x30), w(0x01), w(0x67), w(0x2b), w(0xfe), w(0xd7), w(0xab), w(0x76),\
    w(0xca), w(0x82), w(0xc9), w(0x7d), w(0xfa), w(0x59), w(0x47), w(0xf0),\
    w(0xad), w(0xd4), w(0xa2), w(0xaf), w(0x9c), w(0xa4), w(0x72), w(0xc0),\
    w(0xb7), w(0xfd), w(0x93), w(0x26), w(0x36), w(0x3f), w(0xf7), w(0xcc),\
    w(0x34), w(0xa5), w(0xe5), w(0xf1), w(0x71), w(0xd8), w(0x31), w(0x15),\
    w(0x04), w(0xc7), w(0x23), w(0xc3), w(0x18), w(0x96), w(0x05), w(0x9a),\
    w(0x07), w(0x12), w(0x80), w(0xe2), w(0xeb), w(0x27), w(0xb2), w(0x75),\
    w(0x09), w(0x83), w(0x2c), w(0x1a), w(0x1b), w(0x6e), w(0x5a), w(0xa0),\
    w(0x52), w(0x3b), w(0xd6), w(0xb3), w(0x29), w(0xe3), w(0x2f), w(0x84),\
    w(0x53), w(0xd1), w(0x00), w(0xed), w(0x20), w(0xfc), w(0xb1), w(0x5b),\
    w(0x6a), w(0xcb), w(0xbe), w(0x39), w(0x4a), w(0x4c), w(0x58), w(0xcf),\
    w(0xd0), w(0xef), w(0xaa), w(0xfb), w(0x43), w(0x4d), w(0x33), w(0x85),\
    w(0x45), w(0xf9), w(0x02), w(0x7f), w(0x50), w(0x3c), w(0x9f), w(0xa8),\
    w(0x51), w(0xa3), w(0x40), w(0x8f), w(0x92), w(0x9d), w(0x38), w(0xf5),\
    w(0xbc), w(0xb6), w(0xda), w(0x21), w(0x10), w(0xff), w(0xf3), w(0xd2),\
    w(0xcd), w(0x0c), w(0x13), w(0xec), w(0x5f), w(0x97), w(0x44), w(0x17),\
    w(0xc4), w(0xa7), w(0x7e), w(0x3d), w(0x64), w(0x5d), w(0x19), w(0x73),\
    w(0x60), w(0x81), w(0x4f), w(0xdc), w(0x22), w(0x2a), w(0x90), w(0x88),\
    w(0x46), w(0xee), w(0xb8), w(0x14), w(0xde), w(0x5e), w(0x0b), w(0xdb),\
    w(0xe0), w(0x32), w(0x3a), w(0x0a), w(0x49), w(0x06), w(0x24), w(0x5c),\
    w(0xc2), w(0xd3), w(0xac), w(0x62), w(0x91), w(0x95), w(0xe4), w(0x79),\
    w(0xe7), w(0xc8), w(0x37), w(0x6d), w(0x8d), w(0xd5), w(0x4e), w(0xa9),\
    w(0x6c), w(0x56), w(0xf4), w(0xea), w(0x65), w(0x7a), w(0xae), w(0x08),\
    w(0xba), w(0x78), w(0x25), w(0x2e), w(0x1c), w(0xa6), w(0xb4), w(0xc6),\
    w(0xe8), w(0xdd), w(0x74), w(0x1f), w(0x4b), w(0xbd), w(0x8b), w(0x8a),\
    w(0x70), w(0x3e), w(0xb5), w(0x66), w(0x48), w(0x03), w(0xf6), w(0x0e),\
    w(0x61), w(0x35), w(0x57), w(0xb9), w(0x86), w(0xc1), w(0x1d), w(0x9e),\
    w(0xe1), w(0xf8), w(0x98), w(0x11), w(0x69), w(0xd9), w(0x8e), w(0x94),\
    w(0x9b), w(0x1e), w(0x87), w(0xe9), w(0xce), w(0x55), w(0x28), w(0xdf),\
    w(0x8c), w(0xa1), w(0x89), w(0x0d), w(0xbf), w(0xe6), w(0x42), w(0x68),\
    w(0x41), w(0x99), w(0x2d), w(0x0f), w(0xb0), w(0x54), w(0xbb), w(0x16) }

#define WPOLY           0x011b
#define to_byte(x) ((x) & 0xff)
#define bval(x,n) to_byte((x) >> (8 * (n)))

#define bytes2word(b0, b1, b2, b3) (((uint32_t)(b3) << 24) | \
                                    ((uint32_t)(b2) << 16) | \
                                    ((uint32_t)(b1) << 8) | \
                                    (b0))

#define u0(p)   bytes2word(f2(p), p, p, f3(p))
#define u1(p)   bytes2word(f3(p), f2(p), p, p)
#define u2(p)   bytes2word(p, f3(p), f2(p), p)
#define u3(p)   bytes2word(p, p, f3(p), f2(p))

#define h0(x) (x)
#define f2(x)   ((x<<1) ^ (((x>>7) & 1) * WPOLY))
#define f3(x)   (f2(x) ^ x)

alignas(TABLE_ALIGN) const uint32_t t_fn[4][256] = { sb_data(u0), sb_data(u1), sb_data(u2), sb_data(u3) };
alignas(TABLE_ALIGN) const uint8_t aes_sbox[256] = sb_data(h0);

ALWAYS_INLINE FLATTEN static inline void soft_aes_4round(__m128i* __restrict__ key, __m128i* __restrict__ in1, __m128i* __restrict__ in2, __m128i* __restrict__ in3, __m128i* __restrict__ in4) {
    uint32_t a0, a1, a2, a3, b0, b1, b2, b3, c0, c1, c2, c3, d0, d1, d2, d3;

    a0 = _mm_cvtsi128_si32(*in1);
    a1 = _mm_cvtsi128_si32(_mm_shuffle_epi32(*in1, 0x55));
    a2 = _mm_cvtsi128_si32(_mm_shuffle_epi32(*in1, 0xAA));
    a3 = _mm_cvtsi128_si32(_mm_shuffle_epi32(*in1, 0xFF));

    b0 = _mm_cvtsi128_si32(*in2);
    b1 = _mm_cvtsi128_si32(_mm_shuffle_epi32(*in2, 0x55));
    b2 = _mm_cvtsi128_si32(_mm_shuffle_epi32(*in2, 0xAA));
    b3 = _mm_cvtsi128_si32(_mm_shuffle_epi32(*in2, 0xFF));

    __m128i out1 = _mm_set_epi32(
        (t_fn[0][a3 & 0xff] ^ t_fn[1][(a0 >> 8) & 0xff] ^ t_fn[2][(a1 >> 16) & 0xff] ^ t_fn[3][a2 >> 24]),
        (t_fn[0][a2 & 0xff] ^ t_fn[1][(a3 >> 8) & 0xff] ^ t_fn[2][(a0 >> 16) & 0xff] ^ t_fn[3][a1 >> 24]),
        (t_fn[0][a1 & 0xff] ^ t_fn[1][(a2 >> 8) & 0xff] ^ t_fn[2][(a3 >> 16) & 0xff] ^ t_fn[3][a0 >> 24]),
        (t_fn[0][a0 & 0xff] ^ t_fn[1][(a1 >> 8) & 0xff] ^ t_fn[2][(a2 >> 16) & 0xff] ^ t_fn[3][a3 >> 24]));

    c0 = _mm_cvtsi128_si32(*in3);
    c1 = _mm_cvtsi128_si32(_mm_shuffle_epi32(*in3, 0x55));
    c2 = _mm_cvtsi128_si32(_mm_shuffle_epi32(*in3, 0xAA));
    c3 = _mm_cvtsi128_si32(_mm_shuffle_epi32(*in3, 0xFF));

    *in1 = _mm_xor_si128(out1, *key);

    __m128i out2 = _mm_set_epi32(
        (t_fn[0][b3 & 0xff] ^ t_fn[1][(b0 >> 8) & 0xff] ^ t_fn[2][(b1 >> 16) & 0xff] ^ t_fn[3][b2 >> 24]),
        (t_fn[0][b2 & 0xff] ^ t_fn[1][(b3 >> 8) & 0xff] ^ t_fn[2][(b0 >> 16) & 0xff] ^ t_fn[3][b1 >> 24]),
        (t_fn[0][b1 & 0xff] ^ t_fn[1][(b2 >> 8) & 0xff] ^ t_fn[2][(b3 >> 16) & 0xff] ^ t_fn[3][b0 >> 24]),
        (t_fn[0][b0 & 0xff] ^ t_fn[1][(b1 >> 8) & 0xff] ^ t_fn[2][(b2 >> 16) & 0xff] ^ t_fn[3][b3 >> 24]));

    d0 = _mm_cvtsi128_si32(*in4);
    d1 = _mm_cvtsi128_si32(_mm_shuffle_epi32(*in4, 0x55));
    d2 = _mm_cvtsi128_si32(_mm_shuffle_epi32(*in4, 0xAA));
    d3 = _mm_cvtsi128_si32(_mm_shuffle_epi32(*in4, 0xFF));

    *in2 = _mm_xor_si128(out2, *key);

    __m128i out3 = _mm_set_epi32(
        (t_fn[0][c3 & 0xff] ^ t_fn[1][(c0 >> 8) & 0xff] ^ t_fn[2][(c1 >> 16) & 0xff] ^ t_fn[3][c2 >> 24]),
        (t_fn[0][c2 & 0xff] ^ t_fn[1][(c3 >> 8) & 0xff] ^ t_fn[2][(c0 >> 16) & 0xff] ^ t_fn[3][c1 >> 24]),
        (t_fn[0][c1 & 0xff] ^ t_fn[1][(c2 >> 8) & 0xff] ^ t_fn[2][(c3 >> 16) & 0xff] ^ t_fn[3][c0 >> 24]),
        (t_fn[0][c0 & 0xff] ^ t_fn[1][(c1 >> 8) & 0xff] ^ t_fn[2][(c2 >> 16) & 0xff] ^ t_fn[3][c3 >> 24]));

    __m128i out4 = _mm_set_epi32(
        (t_fn[0][d3 & 0xff] ^ t_fn[1][(d0 >> 8) & 0xff] ^ t_fn[2][(d1 >> 16) & 0xff] ^ t_fn[3][d2 >> 24]),
        (t_fn[0][d2 & 0xff] ^ t_fn[1][(d3 >> 8) & 0xff] ^ t_fn[2][(d0 >> 16) & 0xff] ^ t_fn[3][d1 >> 24]),
        (t_fn[0][d1 & 0xff] ^ t_fn[1][(d2 >> 8) & 0xff] ^ t_fn[2][(d3 >> 16) & 0xff] ^ t_fn[3][d0 >> 24]),
        (t_fn[0][d0 & 0xff] ^ t_fn[1][(d1 >> 8) & 0xff] ^ t_fn[2][(d2 >> 16) & 0xff] ^ t_fn[3][d3 >> 24]));


    *in3 = _mm_xor_si128(out3, *key);
    *in4 = _mm_xor_si128(out4, *key);
}

ALWAYS_INLINE FLATTEN static inline __m128i soft_aesenc(__m128i in, __m128i key) {
    uint32_t x0, x1, x2, x3;
    x0 = _mm_cvtsi128_si32(in);
    x1 = _mm_cvtsi128_si32(_mm_shuffle_epi32(in, 0x55));
    x2 = _mm_cvtsi128_si32(_mm_shuffle_epi32(in, 0xAA));
    x3 = _mm_cvtsi128_si32(_mm_shuffle_epi32(in, 0xFF));

    __m128i out = _mm_set_epi32(
        (t_fn[0][x3 & 0xff] ^ t_fn[1][(x0 >> 8) & 0xff] ^ t_fn[2][(x1 >> 16) & 0xff] ^ t_fn[3][x2 >> 24]),
        (t_fn[0][x2 & 0xff] ^ t_fn[1][(x3 >> 8) & 0xff] ^ t_fn[2][(x0 >> 16) & 0xff] ^ t_fn[3][x1 >> 24]),
        (t_fn[0][x1 & 0xff] ^ t_fn[1][(x2 >> 8) & 0xff] ^ t_fn[2][(x3 >> 16) & 0xff] ^ t_fn[3][x0 >> 24]),
        (t_fn[0][x0 & 0xff] ^ t_fn[1][(x1 >> 8) & 0xff] ^ t_fn[2][(x2 >> 16) & 0xff] ^ t_fn[3][x3 >> 24]));

    return _mm_xor_si128(out, key);
}

static inline void sub_word(uint8_t* key)
{
    key[0] = aes_sbox[key[0]];
    key[1] = aes_sbox[key[1]];
    key[2] = aes_sbox[key[2]];
    key[3] = aes_sbox[key[3]];
}

#ifdef __clang__
static inline uint32_t _rotr(uint32_t value, uint32_t amount)
{
    return (value >> amount) | (value << ((32 - amount) & 31));
}
#endif

__m128i soft_aeskeygenassist(__m128i key, uint8_t rcon)
{
    uint32_t X1 = _mm_cvtsi128_si32(_mm_shuffle_epi32(key, 0x55));
    uint32_t X3 = _mm_cvtsi128_si32(_mm_shuffle_epi32(key, 0xFF));
    sub_word((uint8_t*)&X1);
    sub_word((uint8_t*)&X3);
    return _mm_set_epi32(_rotr(X3, 8) ^ rcon, X3,_rotr(X1, 8) ^ rcon, X1);
}
