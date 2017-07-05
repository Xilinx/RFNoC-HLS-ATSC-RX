/* Include file to configure the RS codec for character symbols
 *
 * Copyright 2002, Phil Karn, KA9Q
 * May be used under the terms of the GNU General Public License (GPL)
 *
 * Modified by Sachin Bharadwaj Sundramurthy and Andrew Lanez (Team Rabbit Ears)
 * for RFNoC and Vivado HLS Challenge
 */
#ifndef INCLUDED_RS_CHAR_H
#define INCLUDED_RS_CHAR_H

#define DTYPE unsigned char

/* Reed-Solomon codec control block */
struct rs {
  unsigned int mm;              /* Bits per symbol */
  unsigned int nn;              /* Symbols per block (= (1<<mm)-1) */
  unsigned char *alpha_to;      /* log lookup table */
  unsigned char *index_of;      /* Antilog lookup table */
  unsigned char *genpoly;       /* Generator polynomial */
  unsigned int nroots;     /* Number of generator roots = number of parity symbols */
  unsigned char fcr;        /* First consecutive root, index form */
  unsigned char prim;       /* Primitive element, index form */
  unsigned char iprim;      /* prim-th root of 1, index form */
  unsigned char *modnn_table;         /* modnn lookup table, 512 entries */
};

static inline unsigned int modnn(struct rs *rs, unsigned int x){
	for(int i = 0; i < 4; i++) {
		if(x >= 255)
			x-=255;
		x = (x >> 8) + (x & 255);
	}
  return x;
}
#define MODNN(x) modnn(rs,x)

#define MM 8
#define NN 255
#define ALPHA_TO (rs->alpha_to)
#define INDEX_OF (rs->index_of)
#define GENPOLY (rs->genpoly)
#define NROOTS 20
#define FCR 0
#define PRIM 1
#define IPRIM 1
#define A0 (255)

#define ENCODE_RS encode_rs_char
#define DECODE_RS decode_rs_char
#define INIT_RS init_rs_char
#define FREE_RS free_rs_char

void ENCODE_RS(void *p,DTYPE *data,DTYPE *parity);
int DECODE_RS(rs *p,DTYPE *data,int *eras_pos,int no_eras);
void *INIT_RS(unsigned int symsize,unsigned int gfpoly,unsigned int fcr,
		   unsigned int prim,unsigned int nroots);
void FREE_RS(void *p);


#endif

