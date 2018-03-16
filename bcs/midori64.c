#include <stdio.h>
#include <stdint.h>

static uint8_t s_box[16] = {0xc,0xa,0xd,0x3,0xe,0xb,0xf,0x7,0x8,0x9,0x1,0x5,0x0,0x2,0x4,0x6};
static uint8_t const_key[15][16] = {
	{0,0,0,1,0,1,0,1,1,0,1,1,0,0,1,1},{0,1,1,1,1,0,0,0,1,1,0,0,0,0,0,0},
	{1,0,1,0,0,1,0,0,0,0,1,1,0,1,0,1},{0,1,1,0,0,0,1,0,0,0,0,1,0,0,1,1},
	{0,0,0,1,0,0,0,0,0,1,0,0,1,1,1,1},{1,1,0,1,0,0,0,1,0,1,1,1,0,0,0,0},
	{0,0,0,0,0,0,1,0,0,1,1,0,0,1,1,0},{0,0,0,0,1,0,1,1,1,1,0,0,1,1,0,0},
	{1,0,0,1,0,1,0,0,1,0,0,0,0,0,0,1},{0,1,0,0,0,0,0,0,1,0,1,1,1,0,0,0},
	{0,1,1,1,0,0,0,1,1,0,0,1,0,1,1,1},{0,0,1,0,0,0,1,0,1,0,0,0,1,1,1,0},
	{0,1,0,1,0,0,0,1,0,0,1,1,0,0,0,0},{1,1,1,1,1,0,0,0,1,1,0,0,1,0,1,0},
	{1,1,0,1,1,1,1,1,1,0,0,1,0,0,0,0}
};

/*
 * SubCell function of the midori64 algorithm.
 * It applies the Sbox to all 16 4-bits cells of the state.
 */
void SubCell(uint8_t*state){
	int i;
	for (i = 0; i <= 15; i++)
		state[i] = s_box[state[i]];
}

/*
 * ShuffleCell function of the midori64 algorithm.
 * It uses a temporary state to shuffle each cells of the given state as described
 * by the midori64 algorithm.
 */
void ShuffleCell(uint8_t*state){
	int i;
	uint8_t temp[16];
	temp[0] = state[0], temp[1] = state[10], temp[2] = state[5], temp[3] = state[15],
	temp[4] = state[14], temp[5] = state[4], temp[6] = state[11], temp[7] = state[1],
	temp[8] = state[9], temp[9] = state[3], temp[10] = state[12], temp[11] = state[6],
	temp[12] = state[7], temp[13] = state[13], temp[14] = state[2], temp[15] = state[8];
	for (i = 0; i <= 15; i++)
		state[i] = temp[i];
}

/*
 * MixColumn function of the midori64 algorithm.
 * It uses the property of the M matrix as described in the midori64 algorithm
 * to substitute the matrix calculation with some simple XOR calculations to get
 * better performances.
 */
void MixColumn(uint8_t*state){
	int i;
	uint8_t temp[16];
	for (i = 0; i <= 3; i++) {
		temp[4 * i + 0] = state[4 * i + 1] ^ state[4 * i + 2] ^ state[4 * i + 3];
		temp[4 * i + 1] = state[4 * i + 0] ^ state[4 * i + 2] ^ state[4 * i + 3];
		temp[4 * i + 2] = state[4 * i + 0] ^ state[4 * i + 1] ^ state[4 * i + 3];
		temp[4 * i + 3] = state[4 * i + 0] ^ state[4 * i + 1] ^ state[4 * i + 2];
	}
	for (i = 0; i <= 15; i++)
		state[i] = temp[i];
}

/*
 * KeyAdd function of the midori64 algorithm.
 * It XORs each state with the corresponding bit of the key K and the alpha it
 * is supposed to use at that time.
 */
void Round_KeyAdd(int r,uint8_t*state, uint8_t*K){
	int i;
	if (r % 2 == 0)
		for (i = 0; i <= 15; i++)
			state[i] = state[i] ^ K[i] ^ const_key[r][i];
	else
		for (i = 0; i <= 15; i++)
			state[i] = state[i] ^ K[i + 16] ^ const_key[r][i];
}

/*
 * Encrypt function of the midori64 algorithm.
 * It represents the core of the midori64 algorithm.
 */
void Encrypt(uint8_t*state, uint8_t*K){
	int i;
	for (i = 0; i <= 15; i++)
		state[i] = state[i] ^ K[i] ^ K[i + 16];
	for (i = 0; i <= 14; i++) {
		SubCell(state);
		ShuffleCell(state);
		MixColumn(state);
		Round_KeyAdd(i,state,K);
	}
	SubCell(state);
	for (i = 0; i <= 15; i++)
		state[i] = state[i] ^ K[i] ^ K[i + 16];
}

/*
 * main function.
 * It gives the message and the key in hexadecimal to the Encrypt function.
 */
int main(){
	int i;
	uint8_t in[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	uint8_t K[32] = { 0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0
		          ,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0 };

	Encrypt(in, K);

	for (i = 0; i <= 15; i++)
		printf("%x", in[i]);
	printf("\n");
}
