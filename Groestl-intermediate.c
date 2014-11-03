/* Groestl-intermediate.c     January 2011
 *                              Updated 2014
 *
 * Prints intermediate state variables. To turn off printing, define
 * the macro TURN_OFF_PRINTING.
 *
 * Authors: Soeren S. Thomsen
 *          Krystian Matusiewicz
 *
 * Modifications by: Sergio Gonzalez
 *
 * This code is placed in the public domain
 */

#include <string.h>
#include "Groestl-intermediate.h"

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define RESET "\033[0m"

//#define TURN_OFF_PRINTING //Desactivar la impresion de datos generales.
//#define TURN_OFF_PRINTING_ROUNDS //Desactivar la impresion de datos por ronda P/Q

#define NO_PRINT_DIFF_MATRIX_EMT //Desactivar la impresion de salida EMT
//#define NO_PRINT_DIFF_BINARY_MATRIX //Desactivar la impresion de matriz binaria
//#define NO_PRINT_DIFF_MATRIX //Desactivar la salida de la matriz de variacion

//#define NO_COMPARE_BLOCK //Desactivar la impresion de comparado de bloque
//#define NO_COMPARE_PQ_ROUNDS //Desactivar el comparado de rondas de P/Q
#define COMPARE_PQ_SUBROUNDS //Desactivar el comparado de subrondas de P/Q
//#define NO_COMPARE_PQ //Desactivar el comparado de inicio/fin de datos de P/Q

//#define STATE_HEX_VALUES //Imprimir el valor de HEX

//#define TEST_RIJNDAEL_PQ_BOXES
#define TEST_GROSTL_ENCODING

//assumes little endian
void printBits(size_t const size, void const * const ptr)
{
	unsigned char *b = (unsigned char*) ptr;
	unsigned char byte;
	int i, j;
	printf(KWHT);
	for (i=size-1;i>=0;i--)
	{
		for (j=7;j>=0;j--)
		{
			byte = b[i] & (1<<j);
			byte >>= j;
			printf("%u", byte);
		}
	}
	printf(RESET);
	printf(" ");
}

//assumes little endian
float diffBits(size_t const size, void const * const ptr1, 
		void const * const ptr2, int print)
{
	unsigned char *b1 = (unsigned char*) ptr1;
 	unsigned char *b2 = (unsigned char*) ptr2;
	unsigned char byte1, byte2;
	int i, j;

	float diff = 0;
	if (print)
		printf(KWHT);

	for (i=size-1;i>=0;i--)
	{
		for (j=7;j>=0;j--)
		{
			byte1 = b1[i] & (1<<j);
			byte1 >>= j;
			byte2 = b2[i] & (1<<j);
			byte2 >>= j;
			if(byte1==byte2){
					if (print)
						printf(KGRN "%u", byte2);
			}	else {
					if (print)
						printf(KRED "%u", byte2);
				diff++;
			}
		}
	}
	if (print){
		printf(RESET);
		printf(" ");
	}
	diff = diff / (size*8);
	return diff;
}


/* this definition turns off printing of intermediate variables */
/* #define TURN_OFF_PRINTING */

/* S-box */
u8 S[256] = {
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
  0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
  0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
  0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
  0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
  0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
  0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
  0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
  0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
  0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
  0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
  0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
  0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
  0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
  0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
  0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
  0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

/* Shift values for short/long variants */
int Shift[2][2][ROWS] = {
  {
    {0,1,2,3,4,5,6,7},
    {1,3,5,7,0,2,4,6}
  },
  {
    {0,1,2,3,4,5,6,11}, 
    {1,3,5,11,0,2,4,6}
  }
};


/* AddRoundConstant xors a round-dependent constant to the state */
void AddRoundConstant(u8 x[ROWS][COLS1024], int columns, u8 round, Variant v) {
  int i,j;
  switch (v&1) {
  case 0 :
    for (i = 0; i < columns; i++) x[0][i] ^= (i<<4)^round;
    break;
  case 1 :
    for (i = 0; i < columns; i++) 
      for (j = 0; j < ROWS-1; j++)
	x[j][i] ^= 0xff;
    for (i = 0; i < columns; i++) x[ROWS-1][i] ^= (i<<4)^0xff^round;
    break;
  }
}

/* SubBytes replaces each byte by a value from the S-box */
void SubBytes(u8 x[ROWS][COLS1024], int columns) {
  int i, j;

  for (i = 0; i < ROWS; i++)
    for (j = 0; j < columns; j++)
      x[i][j] = S[x[i][j]];
}

/* ShiftBytes cyclically shifts each row to the left by a number of
   positions */
void ShiftBytes(u8 x[ROWS][COLS1024], int columns, Variant v) {
  int *R = Shift[v/2][v&1];
  int i, j;
  u8 temp[COLS1024];

  for (i = 0; i < ROWS; i++) {
    for (j = 0; j < columns; j++) {
      temp[j] = x[i][(j+R[i])%columns];
    }
    for (j = 0; j < columns; j++) {
      x[i][j] = temp[j];
    }
  }
}

/* MixBytes reversibly mixes the bytes within a column */
void MixBytes(u8 x[ROWS][COLS1024], int columns) {
  int i, j;
  u8 temp[ROWS];

  for (i = 0; i < columns; i++) {
    for (j = 0; j < ROWS; j++) {
      temp[j] = 
	mul2(x[(j+0)%ROWS][i])^
	mul2(x[(j+1)%ROWS][i])^
	mul3(x[(j+2)%ROWS][i])^
	mul4(x[(j+3)%ROWS][i])^
	mul5(x[(j+4)%ROWS][i])^
	mul3(x[(j+5)%ROWS][i])^
	mul5(x[(j+6)%ROWS][i])^
	mul7(x[(j+7)%ROWS][i]);
    }
    for (j = 0; j < ROWS; j++) {
      x[j][i] = temp[j];
    }
  }
}

/* apply P-permutation to x */
void P(hashState *ctx, u8 x[ROWS][COLS1024]) {
#if defined(COMPARE_PQ_SUBROUNDS) && defined(NO_COMPARE_PQ_ROUNDS)
#warning COMPARE_PQ_SUBROUNDS disabled, as it is incompatible with NO_COMPARE_PQ_ROUNDS
#endif
  u8 i;
  Variant v = ctx->columns==8?P512:P1024;
#ifndef TURN_OFF_PRINTING
  printf(":: BEGIN P\n");
  printf("Input:\n");
  PrintState(ctx, x);
#endif
#ifndef NO_COMPARE_PQ
	u8 y[ROWS][COLS1024];
	int k = 0;
	int l = 0;
	//Let's copy the value of x, so we can compare it later.
	for (k = 0; k < ROWS; k++) {
		for (l = 0; l < ctx->columns; l++) {
			y[k][l] = x[k][l];
		}
	}
#endif
  for (i = 0; i < ctx->rounds; i++) {
#ifndef NO_COMPARE_PQ_ROUNDS 
	u8 z[ROWS][COLS1024];
	int k = 0;
	int l = 0;
	//Let's copy the value of x, so we can compare it later.
	for (k = 0; k < ROWS; k++) {
		for (l = 0; l < ctx->columns; l++) {
			z[k][l] = x[k][l];
		}
	}
#endif
    AddRoundConstant(x, ctx->columns, i, v);
#ifndef TURN_OFF_PRINTING_ROUNDS
    printf(KMAG "\n\nt=%d (AddRoundConstant):\n" RESET, i);
    PrintState(ctx, x);
#endif
#if !defined(NO_PRINT_DIFF_MATRIX_EMT) && defined(COMPARE_PQ_SUBROUNDS) && !defined(NO_COMPARE_PQ_ROUNDS)
	printf("%% ** P - Round %i - AddRoundConstant (vs IV/LastRound)\n" , i);
#endif
#if defined(COMPARE_PQ_SUBROUNDS) && !defined(NO_COMPARE_PQ_ROUNDS)
	PrintDiff(ctx,x,z);
#endif
    SubBytes(x, ctx->columns);
#ifndef TURN_OFF_PRINTING_ROUNDS
    printf(KMAG "\n\nt=%d (SubBytes):\n" RESET, i);
    PrintState(ctx, x);
#endif
#if !defined(NO_PRINT_DIFF_MATRIX_EMT) && defined(COMPARE_PQ_SUBROUNDS) && !defined(NO_COMPARE_PQ_ROUNDS)
	printf("%% ** P - Round %i - SubBytes (vs IV/LastRound)\n" , i);
#endif
#if defined(COMPARE_PQ_SUBROUNDS) && !defined(NO_COMPARE_PQ_ROUNDS)
	PrintDiff(ctx,x,z);
#endif
    ShiftBytes(x, ctx->columns, v);
#ifndef TURN_OFF_PRINTING_ROUNDS
    printf(KMAG "\n\nt=%d (ShiftBytes):\n" RESET, i);
    PrintState(ctx, x);
#endif
#if !defined(NO_PRINT_DIFF_MATRIX_EMT) && defined(COMPARE_PQ_SUBROUNDS) && !defined(NO_COMPARE_PQ_ROUNDS)
	printf("%% ** P - Round %i - ShiftBytes (vs IV/LastRound)\n" , i);
#endif
#if defined(COMPARE_PQ_SUBROUNDS) && !defined(NO_COMPARE_PQ_ROUNDS)
	PrintDiff(ctx,x,z);
#endif
    MixBytes(x, ctx->columns);
#ifndef TURN_OFF_PRINTING_ROUNDS
    printf(KMAG "\n\nt=%d (MixBytes):\n" RESET, i);
    PrintState(ctx, x);
#endif
#if !defined(NO_PRINT_DIFF_MATRIX_EMT) && defined(COMPARE_PQ_SUBROUNDS) && !defined(NO_COMPARE_PQ_ROUNDS)
	printf("%% ** P - Round %i - MixBytes (vs IV/LastRound)\n" , i);
#endif
#if defined(COMPARE_PQ_SUBROUNDS) && !defined(NO_COMPARE_PQ_ROUNDS)
	PrintDiff(ctx,x,z);
#endif
#ifndef NO_COMPARE_PQ_ROUNDS
#ifndef TURN_OFF_PRINTING
  	printf(KMAG "\n\n======================================\n" RESET);
	printf(KMAG "  Round %i -Compare vs Round Input\n" RESET, i);
	printf(KMAG "======================================\n" RESET);
#endif
#if !defined(NO_PRINT_DIFF_MATRIX_EMT) && defined(COMPARE_PQ_SUBROUNDS) && !defined(NO_COMPARE_PQ_ROUNDS)
	printf("%% ** P - Round %i - Compare vs Round Input\n" , i);
#endif
	PrintDiff(ctx,x,z);
#endif

  }
#ifndef TURN_OFF_PRINTING
  printf("Output:\n");
	PrintState(ctx, x);
#endif
#ifndef NO_COMPARE_PQ
#ifndef TURN_OFF_PRINTING
  printf(KMAG "\n\n======================================\n" RESET);
	printf(KMAG "  Compare vs Original Input\n" RESET);
	printf(KMAG "======================================\n" RESET);
#endif
#if !defined(NO_PRINT_DIFF_MATRIX_EMT) && defined(COMPARE_PQ_SUBROUNDS) && !defined(NO_COMPARE_PQ_ROUNDS)
	printf("%% ** P - Compare vs Original Input\n");
#endif
	PrintDiff(ctx,x,y);
#endif
#ifndef TURN_OFF_PRINTING
 printf(":: END P\n\n");
#endif
}

/* apply Q-permutation to x */
void Q(hashState *ctx, u8 x[ROWS][COLS1024]) {
#if defined(COMPARE_PQ_SUBROUNDS) && defined(NO_COMPARE_PQ_ROUNDS)
#warning COMPARE_PQ_SUBROUNDS disabled, incompatible with NO_COMPARE_PQ_ROUNDS
#endif
  u8 i;
  Variant v = ctx->columns==8?Q512:Q1024;
#ifndef TURN_OFF_PRINTING
  printf(":: BEGIN Q\n");
  printf("Input:\n");
  PrintState(ctx, x);
#endif
#ifndef NO_COMPARE_PQ
	u8 y[ROWS][COLS1024];
	int k = 0;
	int l = 0;
	//Let's copy the value of x, so we can compare it later.
	for (k = 0; k < ROWS; k++) {
		for (l = 0; l < ctx->columns; l++) {
			y[k][l] = x[k][l];
		}
	}
#endif
  for (i = 0; i < ctx->rounds; i++) {
#ifndef NO_COMPARE_PQ_ROUNDS
	u8 z[ROWS][COLS1024];
	int k = 0;
	int l = 0;
	//Let's copy the value of x, so we can compare it later.
	for (k = 0; k < ROWS; k++) {
		for (l = 0; l < ctx->columns; l++) {
			z[k][l] = x[k][l];
		}
	}
#endif
    AddRoundConstant(x, ctx->columns, i, v);
#ifndef TURN_OFF_PRINTING_ROUNDS
    printf(KMAG "\n\nt=%d (AddRoundConstant):\n" RESET, i);
    PrintState(ctx, x);
#endif
#if !defined(NO_PRINT_DIFF_MATRIX_EMT) && defined(COMPARE_PQ_SUBROUNDS) && !defined(NO_COMPARE_PQ_ROUNDS)
	printf("%% ** Q - Round %i - AddRoundConstant (vs IV/LastRound)\n" , i);
#endif
#if defined(COMPARE_PQ_SUBROUNDS) && !defined(NO_COMPARE_PQ_ROUNDS)
	PrintDiff(ctx,x,z);
#endif
    SubBytes(x, ctx->columns);
#ifndef TURN_OFF_PRINTING_ROUNDS
    printf(KMAG "\n\nt=%d (SubBytes):\n" RESET, i);
    PrintState(ctx, x);
#endif
#if !defined(NO_PRINT_DIFF_MATRIX_EMT) && defined(COMPARE_PQ_SUBROUNDS) && !defined(NO_COMPARE_PQ_ROUNDS)
	printf("%% ** Q - Round %i - SubBytes (vs IV/LastRound)\n" , i);
#endif
#if defined(COMPARE_PQ_SUBROUNDS) && !defined(NO_COMPARE_PQ_ROUNDS)
	PrintDiff(ctx,x,z);
#endif
    ShiftBytes(x, ctx->columns, v);
#ifndef TURN_OFF_PRINTING_ROUNDS
    printf(KMAG "\n\nt=%d (ShiftBytes):\n" RESET, i);
    PrintState(ctx, x);
#endif
#if !defined(NO_PRINT_DIFF_MATRIX_EMT) && defined(COMPARE_PQ_SUBROUNDS) && !defined(NO_COMPARE_PQ_ROUNDS)
	printf("%% ** Q - Round %i - ShiftBytes (vs IV/LastRound)\n" , i);
#endif
#if defined(COMPARE_PQ_SUBROUNDS) && !defined(NO_COMPARE_PQ_ROUNDS)
	PrintDiff(ctx,x,z);
#endif
    MixBytes(x, ctx->columns);
#ifndef TURN_OFF_PRINTING_ROUNDS
    printf(KMAG "\n\nt=%d (MixBytes):\n" RESET, i);
    PrintState(ctx, x);
#endif
#if !defined(NO_PRINT_DIFF_MATRIX_EMT) && defined(COMPARE_PQ_SUBROUNDS) && !defined(NO_COMPARE_PQ_ROUNDS)
	printf("%% ** Q - Round %i - ShiftBytes (vs IV/LastRound)\n" , i);
#endif
#if defined(COMPARE_PQ_SUBROUNDS) && !defined(NO_COMPARE_PQ_ROUNDS)
	PrintDiff(ctx,x,z);
#endif
#ifndef NO_COMPARE_PQ_ROUNDS
#ifndef TURN_OFF_PRINTING
  	printf(KMAG "\n\n======================================\n" RESET);
	printf(KMAG "  Round %i -Compare vs Round Input\n" RESET, i);
	printf(KMAG "======================================\n" RESET);
#endif
#if !defined(NO_PRINT_DIFF_MATRIX_EMT) && defined(COMPARE_PQ_SUBROUNDS) && !defined(NO_COMPARE_PQ_ROUNDS)
	printf("%% ** Q - Round %i - Compare vs Round Input\n" , i);
#endif
	PrintDiff(ctx,x,z);
#endif
  }
#ifndef TURN_OFF_PRINTING
  printf("Output:\n");
	PrintState(ctx, x);
#endif
#ifndef NO_COMPARE_PQ
#ifndef TURN_OFF_PRINTING
  	printf(KMAG "\n\n======================================\n" RESET);
	printf(KMAG "  Compare vs Original Input\n" RESET);
	printf(KMAG "======================================\n" RESET);
#endif
#if !defined(NO_PRINT_DIFF_MATRIX_EMT) && defined(COMPARE_PQ_SUBROUNDS) && !defined(NO_COMPARE_PQ_ROUNDS)
	printf("%% ** Q - Compare vs Original Input\n");
#endif
	PrintDiff(ctx,x,y);
#endif
#ifndef TURN_OFF_PRINTING
 printf(":: END Q\n\n");
#endif
}

/* digest (up to) msglen bytes */
void Transform(hashState* ctx, 
	       const BitSequence *input, 
	       u32 msglen) { 
  int i, j;
  u8 temp1[ROWS][COLS1024], temp2[ROWS][COLS1024];

  /* digest one message block at the time */
  for (; msglen >= ctx->statesize; 
       msglen -= ctx->statesize, input += ctx->statesize) {
    /* store message block (m) in temp2, and xor of chaining (h) and
       message block in temp1 */
    for (i = 0; i < ROWS; i++) {
      for (j = 0; j < ctx->columns; j++) {
	temp1[i][j] = ctx->chaining[i][j]^input[j*ROWS+i];
	temp2[i][j] = input[j*ROWS+i];
      }
    }

#ifndef TURN_OFF_PRINTING
    printf("========================================\n\n");
    printf("Block Contents:\n");
    PrintState(ctx, temp2);
    printf("\n");
#endif
#ifndef NO_COMPARE_BLOCK
		u8 y[ROWS][COLS1024];
		int k = 0;
		int l = 0;
		//Let's copy the value of x, so we can compare it later.
		for (k = 0; k < ROWS; k++) {
			for (l = 0; l < ctx->columns; l++) {
				y[k][l] = temp2[k][l];
			}
		}
#endif
    P(ctx, temp1); /* P(h+m) */
    Q(ctx, temp2); /* Q(m) */

    /* xor P(h+m) and Q(m) onto chaining, yielding P(h+m)+Q(m)+h */
    for (i = 0; i < ROWS; i++) {
      for (j = 0; j < ctx->columns; j++) {
	ctx->chaining[i][j] ^= temp1[i][j]^temp2[i][j];
      }
    }
#ifndef TURN_OFF_PRINTING
    printf("P(h+m) + Q(m) + h =\n");
    PrintState(ctx, ctx->chaining);
    printf("\n");
#endif
#ifndef NO_COMPARE_BLOCK
		PrintDiff(ctx,ctx->chaining,y);
#endif
    /* increment block counter */
    ctx->block_counter++;
  }
}

/* do output transformation, P(h)+h */
void OutputTransformation(hashState *ctx) {
  int i, j;
  u8 temp[ROWS][COLS1024];

  /* store chaining ("h") in temp */
  for (i = 0; i < ROWS; i++) {
    for (j = 0; j < ctx->columns; j++) {
      temp[i][j] = ctx->chaining[i][j];
    }
  }

#ifndef TURN_OFF_PRINTING
    printf("========================================\n\n");
  printf("Output transformation:\n\n");
#endif

  /* compute P(temp) = P(h) */
  P(ctx, temp);

  /* feed chaining forward, yielding P(h)+h */
  for (i = 0; i < ROWS; i++) {
    for (j = 0; j < ctx->columns; j++) {
      ctx->chaining[i][j] ^= temp[i][j];
    }
  }
#ifndef TURN_OFF_PRINTING
    printf("P(h) + h =\n");
    PrintState(ctx, ctx->chaining);
#endif
}


/* initialise context */
HashReturn Init(hashState* ctx,
		int hashbitlen) {
  int i, j;

  if (hashbitlen <= 0 || (hashbitlen%8) || hashbitlen > 512)
    return BAD_HASHLEN;

  if (hashbitlen <= 256) {
    ctx->rounds = ROUNDS512;
    ctx->columns = COLS512;
    ctx->statesize = SIZE512;
  }
  else {
    ctx->rounds = ROUNDS1024;
    ctx->columns = COLS1024;
    ctx->statesize = SIZE1024;
  }

  /* zeroise chaining variable */
  for (i = 0; i < ROWS; i++) {
    for (j = 0; j < ctx->columns; j++) {
      ctx->chaining[i][j] = 0;
    }
  }

  /* store hashbitlen and set initial value */
  ctx->hashbitlen = hashbitlen;
  for (i = ROWS-sizeof(int); i < ROWS; i++) {
    ctx->chaining[i][ctx->columns-1] = (u8)(hashbitlen>>(8*(7-i)));
  }

  /* initialise other variables */
  ctx->buf_ptr = 0;
  ctx->block_counter = 0;
  ctx->bits_in_last_byte = 0;

  return SUCCESS;
}

HashReturn Update(hashState* ctx,
		  const BitSequence* input,
		  DataLength databitlen) {
  int index = 0;
  DataLength msglen = databitlen/8; /* no. of (full) bytes supplied */
  DataLength rem = databitlen%8;    /* no. of additional bits */

  if (ctx->bits_in_last_byte) return FAIL;

  /* if the buffer contains data that still needs to be digested */
  if (ctx->buf_ptr) {
    /* copy data into buffer until buffer is full, or there is no more
       data */
    for (index = 0; ctx->buf_ptr < ctx->statesize && index < msglen; 
	 index++, ctx->buf_ptr++) {
      ctx->buffer[ctx->buf_ptr] = input[index];
    }

    if (ctx->buf_ptr < ctx->statesize) {
      /* this chunk of message does not fill the buffer */
      if (rem) {
	/* if there are additional bits, add them to the buffer */
	ctx->bits_in_last_byte = rem;
	ctx->buffer[ctx->buf_ptr++] = input[index];
      }
      return SUCCESS;
    }

    /* the buffer is full, digest */
    ctx->buf_ptr = 0;
    Transform(ctx, ctx->buffer, ctx->statesize);
  }

  /* digest remainder of data modulo the block size */
  Transform(ctx, input+index, msglen-index);
  index += ((msglen-index)/ctx->statesize)*ctx->statesize;

  /* copy remaining data to buffer */
  for (; index < msglen; index++, ctx->buf_ptr++) {
    ctx->buffer[ctx->buf_ptr] = input[index];
  }
      
  if (rem) {
    ctx->bits_in_last_byte = rem;
    ctx->buffer[ctx->buf_ptr++] = input[index];
  }
  return SUCCESS;
}


#define BILB ctx->bits_in_last_byte
HashReturn Final(hashState* ctx,
		 BitSequence* output) {
  int i, j, hashbytelen = ctx->hashbitlen/8;

  /* 100... padding */
  if (ctx->bits_in_last_byte) {
    ctx->buffer[ctx->buf_ptr-1] &= ((1<<BILB)-1)<<(8-BILB);
    ctx->buffer[ctx->buf_ptr-1] ^= 0x1<<(7-BILB);
  }
  else ctx->buffer[ctx->buf_ptr++] = 0x80;

  if (ctx->buf_ptr > ctx->statesize-LENGTHFIELDLEN) {
    /* padding requires two blocks */
    while (ctx->buf_ptr < ctx->statesize) {
      ctx->buffer[ctx->buf_ptr++] = 0;
    }
    Transform(ctx, ctx->buffer, ctx->statesize);
    ctx->buf_ptr = 0;
  }
  while (ctx->buf_ptr < ctx->statesize-LENGTHFIELDLEN) {
    ctx->buffer[ctx->buf_ptr++] = 0;
  }

  /* length padding */
  ctx->block_counter++;
  ctx->buf_ptr = ctx->statesize;
  while (ctx->buf_ptr > ctx->statesize-LENGTHFIELDLEN) {
    ctx->buffer[--ctx->buf_ptr] = (u8)ctx->block_counter;
    ctx->block_counter >>= 8;
  }

  /* digest (last) padding block */
  Transform(ctx, ctx->buffer, ctx->statesize);
  /* output transformation */
  OutputTransformation(ctx);

  /* store hash output */
  j = 0;
  for (i = ctx->statesize-hashbytelen; i < ctx->statesize; i++,j++) {
    output[j] = ctx->chaining[i%ROWS][i/ROWS];
  }

  /* zeroise */
  for (i = 0; i < ROWS; i++) {
    for (j = 0; j < ctx->columns; j++) {
      ctx->chaining[i][j] = 0;
    }
  }
  for (i = 0; i < ctx->statesize; i++) {
    ctx->buffer[i] = 0;
  }

  return SUCCESS;
}

/* hash bit sequence */
HashReturn Hash(int hashbitlen,
		const BitSequence* data, 
		DataLength databitlen,
		BitSequence* hashval) {
  HashReturn ret;
  hashState ctx;

  /* initialise */
  if ((ret = Init(&ctx, hashbitlen)))
    return ret;

#ifndef TURN_OFF_PRINTING
  char str[1+(databitlen+7)/8];
  str[(databitlen+7)/8] = 0;
  memcpy(str, data, (databitlen+7)/8);
  printf("########################################\n\n");
  printf("Groestl\n");
  printf("  Message Digest Length = %d\n\n", hashbitlen);
  printf("########################################\n\n\n");
  printf("%d-Block Message Sample\n\n", 
	 (int)(1+(databitlen+8*LENGTHFIELDLEN)/(8*ctx.statesize)));
  printf("  Input Message = \"%s\"\n\n", str);
  printf("========================================\n\n");
  printf("Initial state:\n");
  PrintState(&ctx, ctx.chaining);
  printf("\n");
#endif

  /* process message */
  if ((ret = Update(&ctx, data, databitlen)))
    return ret;

  /* finalise */
  ret = Final(&ctx, hashval);

#ifndef TURN_OFF_PRINTING
  printf("\n----------------------------------------\n\n");
  printf("Message Digest is\n");
  PrintHash(hashval, hashbitlen);
#endif

  return ret;
}

/* print hash result */
void PrintHash(BitSequence *hashval, int hashbitlen) {
  int i;
  for (i = 0; i < (hashbitlen+7)/8; i++) 
    printf("%02x", hashval[i]);
  printf("\n");
}

void PrintDiff(const hashState *ctx, u8 x[ROWS][COLS1024], 
		u8 y[ROWS][COLS1024]) {
  int i, j;

	//printf("=======================================\n");
	//printf("  *           Differences           *\n");
	//printf("=======================================\n");
	//printf("Input Y:\n");
	//PrintState(ctx,y);
	//printf("Output X:\n");
	//PrintState(ctx,x);
#ifndef TURN_OFF_PRINTING
	printf("DiffMap:\n");
#endif

	float diffPct[ROWS][ctx->columns];

#ifndef NO_PRINT_DIFF_BINARY_MATRIX
  for (i = 0; i < ROWS; i++) {
    for (j = 0; j < ctx->columns; j++) {
			char buffer[8];      
			diffPct[i][j] = diffBits(1, &y[i][j], &x[i][j], 1);
		}
    printf("\n");
  }
	printf("\n");
#else
  for (i = 0; i < ROWS; i++) {
    for (j = 0; j < ctx->columns; j++) {
			char buffer[8];      
			diffPct[i][j] = diffBits(1, &y[i][j], &x[i][j], 0);
		}
  }
#endif

#ifndef NO_PRINT_DIFF_MATRIX
	printf(KCYN);
	for (i = 0; i < ROWS; i++) {
    for (j = 0; j < ctx->columns; j++) {
#ifdef STATE_HEX_VALUES
			printf("%02x ", x[i][j]);
#endif
			printf("%.2f ", diffPct[i][j]);
		}
    printf("\n");
  }
	printf(RESET);
	printf("\n");
#endif

#ifndef NO_PRINT_DIFF_MATRIX_EMT
	printf(KYEL);
  printf(">A:=");
	printf("[");
	for (i = 0; i < ROWS; i++) {
    for (j = 0; j < ctx->columns; j++) {
			printf("%.3f", diffPct[i][j]);
			if (j != ctx->columns-1)
				printf(",");
		}
		if (i != ROWS-1)
    printf("; ");
  }
	printf("];");
	printf("... \n>B:=mean(A) | dev(A) | mean(A') | dev(A') ; writetable(B,labc=[\"mean_row\",\"dev_row\",\"mean_col\",\"dev_col\"]),\n>boxplot(A):\n>boxplot(A'):");
	printf(RESET);
	printf("\n");
#endif

}

void PrintState(const hashState *ctx, u8 x[ROWS][COLS1024]) {
  int i, j;
  for (i = 0; i < ROWS; i++) {
    for (j = 0; j < ctx->columns; j++) {
	char buffer[128];      
		//itoa(x[i][j],buffer,2);
		//printf("%08s ", buffer);

#ifdef STATE_HEX_VALUES
		printf("%02x ", x[i][j]);
#endif
		
		printBits(1, &x[i][j]);
    }
    printf("\n");
  }
}

#ifdef TEST_GROSTL_ENCODING
int main (int argc, const char* argv[]){
	BitSequence data[]  = {'a','b','c'};
	DataLength dataLng = sizeof(data) * 8;
	int i = 512;
	HashReturn retVal;
	for (;i <= 512; i = i+8){
		BitSequence hash[i];
		retVal = Hash(i,data,dataLng,hash);
		if (retVal == FAIL)
			printf("Error!\n");
		else if (retVal == BAD_HASHLEN)
			printf("Bad Hashlen!\n");
	}
}
#endif

#ifdef TEST_RIJNDAEL_PQ_BOXES
int main (int argc, const char* argv[]){
	int hashbitlen = 512;
	int databitlen = 1024;
  HashReturn ret;
  hashState ctx;
  
#ifndef TURN_OFF_PRINTING
  printf("Initializing Rijndael test...\n");
#endif
  
  int roundNbr = 14;
  
  for (; roundNbr >= 0; roundNbr--){
  
#ifndef TURN_OFF_PRINTING
  printf(KMAG "\n\n\n===========================================================\n");
  printf("BEGIN %i Rounds for P/Q autotest!\n",roundNbr);
  printf("===========================================================\n" RESET);
#endif
  
  int i, j;

  if (hashbitlen <= 0 || (hashbitlen%8) || hashbitlen > 512)
    return BAD_HASHLEN;

  if (hashbitlen <= 256) {
    ctx.rounds = roundNbr;
    ctx.columns = COLS512;
    ctx.statesize = SIZE512;
  }
  else {
    ctx.rounds = roundNbr;
    ctx.columns = COLS1024;
    ctx.statesize = SIZE1024;
  }

  /* zeroise chaining variable */
  for (i = 0; i < ROWS; i++) {
    for (j = 0; j < ctx.columns; j++) {
      ctx.chaining[i][j] = 0;
    }
  }

  /* initialise other variables */
  ctx.buf_ptr = 0;
  ctx.block_counter = 0;
  ctx.bits_in_last_byte = 0;

#ifndef TURN_OFF_PRINTING
  printf("Block:\n");
  PrintState(&ctx,ctx.chaining);
#endif

		u8 matrixP[ROWS][COLS1024], matrixQ[ROWS][COLS1024];
		int k = 0;
		int l = 0;
		for (k = 0; k < ROWS; k++) {
			for (l = 0; l < ctx.columns; l++) {
				matrixP[k][l] = ctx.chaining[k][l];
				matrixQ[k][l] = ctx.chaining[k][l];
			}
		}
#ifndef TURN_OFF_PRINTING
		printf(KMAG "\nTesting P for all 0s matrix....\n" RESET);
#endif
#ifndef NO_PRINT_DIFF_MATRIX_EMT
		printf("%%** #Rounds: %i - P for all 0s matrix\n", roundNbr);
#endif
    P(&ctx, matrixP);
#ifndef NO_PRINT_DIFF_MATRIX_EMT
    PrintDiff(&ctx, matrixP, ctx.chaining);
#endif
#ifndef TURN_OFF_PRINTING
    printf(KMAG "\nTesting Q for all 0s matrix....\n" RESET);
#endif
#ifndef NO_PRINT_DIFF_MATRIX_EMT
		printf("%%** #Rounds: %i - Q for all 0s matrix\n", roundNbr);
#endif
    Q(&ctx, matrixQ);
#ifndef NO_PRINT_DIFF_MATRIX_EMT
    PrintDiff(&ctx, matrixQ, ctx.chaining);
#endif
    
    
    ctx.chaining[0][0] = 1;
      
    u8 matrixP2[ROWS][COLS1024], matrixQ2[ROWS][COLS1024];
		for (k = 0; k < ROWS; k++) {
			for (l = 0; l < ctx.columns; l++) {
				matrixP2[k][l] = ctx.chaining[k][l];
				matrixQ2[k][l] = ctx.chaining[k][l];
			}
		}
#ifndef TURN_OFF_PRINTING
		printf(KMAG "\nTesting P for one 1 matrix....\n" RESET);
#endif
#ifndef NO_PRINT_DIFF_MATRIX_EMT
		printf("%%** #Rounds: %i - Testing P for one 1 matrix\n", roundNbr);
#endif
    P(&ctx, matrixP2);
#ifndef NO_PRINT_DIFF_MATRIX_EMT
    PrintDiff(&ctx, matrixP, ctx.chaining);
#endif
#ifndef TURN_OFF_PRINTING
    printf(KMAG "\nTesting Q for one 1 matrix....\n" RESET);
#endif
#ifndef NO_PRINT_DIFF_MATRIX_EMT
		printf("%%** #Rounds: %i - Testing Q for one 1 matrix\n", roundNbr);
#endif
    Q(&ctx, matrixQ2);
#ifndef NO_PRINT_DIFF_MATRIX_EMT
    PrintDiff(&ctx, matrixQ, ctx.chaining);
#endif
    
#ifndef TURN_OFF_PRINTING

		printf(KMAG "\nTesting P0 vs P1....\n" RESET);
#endif
#ifndef NO_PRINT_DIFF_MATRIX_EMT
		printf("%%** #Rounds: %i - Testing P0 vs P1 \n", roundNbr);
#endif
    PrintDiff(&ctx, matrixP2, matrixP);
#ifndef TURN_OFF_PRINTING
    printf(KMAG "\nTesting Q0 vs Q1....\n" RESET);
#endif
#ifndef NO_PRINT_DIFF_MATRIX_EMT
		printf("%%** #Rounds: %i - Testing Q0 vs Q1\n", roundNbr);
#endif
    PrintDiff(&ctx, matrixQ2, matrixQ);

#ifndef TURN_OFF_PRINTING
  printf(KMAG "===========================================================\n");
  printf("END %i Rounds for P/Q autotest!\n",roundNbr);
  printf("===========================================================\n\n\n" RESET);
#endif
	}
}
#endif

