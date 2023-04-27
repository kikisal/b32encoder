#include <stdio.h>
#include <cstdlib>
#include <unistd.h>
#include <inttypes.h>
#include <cstring>

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

#define GROUP_BITS_COUNT 5

u8 table[] = {
	'a', 'b', 'c', 'd',
	'W', 'X', 'Y', 'Z',
	'0', '1', '2', '3',
	'E', 'F', 'G', 'H',
	'A', 'B', 'C', 'D',
	'4', '5', '6', '7',
	'8', '9', 'w', 'x',
	'y', 'z', 'j', 'k'
};

/* returns fractional part of x */
float fract(float x) {
	return x - (int)x;
}

int minimum_bytes(int bytes, float byte_group_count) {
	float groups = (float)bytes/byte_group_count;
	int dgroups = (int)groups;
	// dgroups map  directly to bytes count.
	return fract(groups) > 0 ? dgroups + 1 : dgroups;
}

int truncate(float x) { return (int)x; }

u8 load_byte(size_t bit_offset, u8 const* buff, size_t buff_size) {
	u8 out{0};
	short bits_in_a_byte = 8;

	int index = truncate(bit_offset/8.0f);
	if (index >= buff_size)
		index = buff_size - 2; // not important if parameters are constrained to the right range.

	int offset_in_byte = bit_offset - index*8; // from 0 - 7
	if (offset_in_byte == 0)
		return buff[index];
	else {
		if (index + 1 >= buff_size)
			return buff[index] << offset_in_byte;
		else
			return (buff[index] << offset_in_byte) | (buff[index + 1] >> (bits_in_a_byte - offset_in_byte));
	}
/*
buff:
01110100
01100101
01110011
01110100

10000000 |
00111001
10111001

load_byte(0*5, buff, 4): [01110100]
load_byte(1*5, buff, 4): [10001100]
load_byte(2*5, buff, 4): [10010101]
load_byte(3*5, buff, 4): [10111001]
load_byte(4*5, buff, 4): [00110111]
load_byte(5*5, buff, 4): [11101000]
load_byte(6*5, buff, 4): [00000000]
*/
}

bool encode(u8 const* buff, size_t size, u8*& obuff) {
	size_t out_len = minimum_bytes(size, (float)GROUP_BITS_COUNT/8.0f);
	obuff = (u8*)malloc(out_len + 1);
	if (!obuff)
		return false;

	for (int i = 0; i < out_len; i++)
		obuff[i] = table[load_byte(i*GROUP_BITS_COUNT, buff, size) >> 3];

	obuff[out_len] = 0;
	return true;
}

int main() {
	// random 8 bits data.
	u8* buff = (u8*)"aaaaaa";
	size_t buff_len = strlen((char const*)buff);


	printf("Input buffer(%ld): \"%s\"\n", buff_len, buff);

	u8* str {nullptr};

	encode(buff, buff_len, str);

	printf("Encoded buffer(%ld): \"%s\"\n", strlen((const char*)str), str);


	free(str);

	return 0;
}
