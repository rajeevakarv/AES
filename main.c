/*
 ============================================================================
 Name        : test_sbox.c
 Author      : Rajeev Verma
 Version     :
 Copyright   : Your copyright notice
 Description : AES SBox table generater
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
unsigned char g_aes_logt[256], g_aes_ilogt[256];

inline unsigned char aes_mul_manual(unsigned char a, unsigned char b)
{
    register unsigned short ac;
    register unsigned char ret;

    ac = a;
    ret = 0;
    while(b) {
        if(b & 0x01)
            ret ^= ac;
        ac <<= 1;
        b >>= 1;
        if(ac & 0x0100)
            ac ^= 0x011b;
    }

    return ret;
}

////////////////////////////////////////////////////////////////////////////////////////

unsigned short quotient1 = 0x00;
unsigned short hold_rem, hold_quo;
unsigned char sum;
void addition(unsigned char a, unsigned char b){

    sum= a^b;

}

unsigned char bit_extract(unsigned char b, int n){
    unsigned char b_bit;
    b_bit = (b>>n)& 1;
    return b_bit;
}


unsigned short multiplication(unsigned short a, unsigned short b) {
        unsigned short temp = 0;

        unsigned short OutOfRange;
        unsigned short f = 0x1b;         /* x^8 + x^4 + x^3 + x + 1 */
        int i;
        for (i = 0; i < 16; i++) {
                if (b & 1)
                        temp = temp ^ a;
                OutOfRange = (a & 0x8000);
                a = a << 1;
                if (OutOfRange)
                        a = a ^ f;
                b = b >> 1;
        }
        return temp;
}
/*
    unsigned char result;
	unsigned char f;
	int n;
	result = bit_extract(b,0);
	result = result * a;

	f = 0x13;

	for(n=1; n<9; n++){
		result = result^((bit_extract(b,n)*(a<<n))^f);
	}
return result;
}
*/

int number_bits(unsigned short a){

    int i, numberofbits = 0;

    for(i = 1; i <= 16; i++){
        if(a & 1){
            numberofbits = i;
        }

        a = a>>1;
    }
    return numberofbits;
}


unsigned short quotient(unsigned short temp){

    unsigned short temp1 = 1 << temp;
    quotient1 = quotient1 ^ temp1;

    return quotient1;
}

unsigned short division(unsigned short a, unsigned short b){

        unsigned short temp;
        unsigned short b1;

        int bits_a = number_bits(a);
        int bits_b = number_bits(b);

        if(bits_a >= bits_b){
          temp = bits_a - bits_b;
          b1 = b << temp;
          a = a ^ b1;

          quotient1 = quotient(temp);
          division(a,b);

     }
        else{
        hold_quo = quotient1;
        quotient1 = 0x00;
        hold_rem = a;
     //   printf("q = %x, r = %x\n\r", hold_quo, hold_rem);
    }
}


unsigned char extend_euclid(unsigned short func, unsigned short aval){
	int i;
    unsigned short first[3], second[3], swap[3];
    unsigned short quo, mul;
    first[0] = 1;
    first[1] = 0;
    first[2] = func;
    second[0] = 0;
    second[1] = 1;
    second[2] = aval;

   // printf("aval = %x\n\r", aval);
  //  printf("second2 = %x\n\r", second[2]);

    if(second[2] == 0){
        printf("No inverse");
    }else{
   //     for(i=0;i<3;i++){
    while(second[2] != 1){
        division(first[2], second[2]);
        quo = hold_quo;

   //     printf("quo = %x\n\r", quo);

        //for(i=0; i<3; i++){
         //   unsigned char mul = multiplication(quo,second[i]);
          //  swap[i] = first[i] - mul;

    //    }

        mul = multiplication(quo,second[0]);
      //  printf("mul = %x\n\r", mul);
        swap[0] = first[0] ^ mul;
        mul = multiplication(quo,second[1]);
      //  printf("mul = %x\n\r", mul);
        swap[1] = first[1] ^ mul;
        mul = multiplication(quo,second[2]);
     //   printf("mul = %x\n\r", mul);
        swap[2] = first[2] ^ mul;
      //  printf("swap2 = %x\n\r", swap[2]);

        first[0] = second[0];
        first[1] = second[1];
        first[2] = second[2];

        second[0] = swap[0];
        second[1] = swap[1];
        second[2] = swap[2];

     //   printf("-------%x %x %x--------\n\r", second[0], second[1], second[2]);

   }
    return second[1];
    }
}


int main(void) {
		int i, loc = 16;
	    unsigned char gen, g_aes_sbox=0;

	    // build logarithm table and it's inverse
	    gen = 1;
	//	for(i = 0; i < 0xff; i++) {
	//		g_aes_logt[gen]  = i;
	//        g_aes_ilogt[i]   = gen;
	//        gen = aes_mul_manual(gen, 0x03);
	//	}
	    // build S-Box and it's inverse
		if(loc == 1)
			g_aes_sbox = 0x7c;
		else{
	        char bi;
	    //    unsigned char inv = ((loc)?extend_euclid(0x11b,0xff-g_aes_logt[(loc)]):0);
	        unsigned char inv = extend_euclid(0x11b,loc);
	        for(bi = 0; bi < 8; bi++) {
	            // based on transformation 5.1
	            // could also be done with a loop based on the matrix
	            g_aes_sbox |= ((inv & (1<<bi)?1:0)
	                        ^ (inv & (1 << ((bi+4) & 7))?1:0)
	                        ^ (inv & (1 << ((bi+5) & 7))?1:0)
	                        ^ (inv & (1 << ((bi+6) & 7))?1:0)
	                        ^ (inv & (1 << ((bi+7) & 7))?1:0)
	                        ^ (0x63 & (1 << bi)?1:0)
	            ) << bi;
	        }

		}
		printf("sbox value = %x", g_aes_sbox);
		return 0;
}
