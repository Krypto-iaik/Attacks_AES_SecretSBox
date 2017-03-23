#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "AES_common.h"
/**The user can choose the SBox!
NOTE: It MUST BE BIJECTIVE!!!*/
#include "AES_sbox.h"
#include "multiplication.h"
#include "subspace_checks.h"

#define N_Round 3
#define NUMBER_CP 256

word8 play[NUMBER_CP][16], cipher[NUMBER_CP][16];

word8 randomByte(){
  return (word8) randomInRange(0, 255);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**Check if the sbox is a bijection!*/

int checkSboxBijection()
{
    int v[256], i, n;

    for(i = 0; i<256; i++)
    {
        v[i] = 0;
    }

    for(i = 0; i<256; i++)
    {
        n = (int) sBox[i];

        v[n]++;
    }

    for(i = 0; i<256; i++)
    {
        if(v[i]!=1)
           return -1;
    }

    return 0;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**The function finds one byte of the key, precisely k_{0,0} + k_{1,1}, using the strategy described in the paper.
It also checks that it is the correct one.
Note that the idea is that for a wrong key, AES behaves as a random function.
NOTE: it is not necessary to find any information of the S-Box to find the key.*/

int foundKey(word8 key[][4])
{
    int dddelta, i, flag, j, k;
    unsigned char temp[4][4], temp2[4][4], delta, verify[16];


    for(dddelta = 0; dddelta < 256; dddelta++)
    {
        delta = (word8) dddelta;

        /**Generation of the plaintexts*/

        for(i=0; i<4; i++)
        {
            for(j=0;j<4;j++)
            {
                temp[i][j] = randomByte();
            }
        }

        for(k=0;k<NUMBER_CP;k++)
        {
            for(i=0; i<4; i++)
            {
                for(j=0;j<4;j++)
                {
                    play[k][i+4*j] = temp[i][j];
                }
            }
        }

        for(k=0;k<256;k++)
        {
            play[k][0] = (word8) k;
            play[k][5] = play[k][0] ^ delta;
        }

        /**compute the ciphertext*/

        for(k=0;k<NUMBER_CP;k++)
        {

            for(i=0; i<4; i++)
            {
                for(j=0;j<4;j++)
                {
                    temp[i][j] = play[k][i+4*j];
                }
            }

             encryption(temp, key, &(temp2[0][0]));

            for(i=0; i<4; i++)
            {
                for(j=0;j<4;j++)
                {
                    cipher[k][i+4*j] = temp2[i][j];
                }
            }

        }

        /**Check the guess key -> check if the ciphertexts belong to ID_0,1,3!!!*/

        //note the mixcolumns is omitted, so we use ID_0,1,3.
        //If there is the mixColumn, then use M_0,1,3, or apply MC^-1 to the ciphertexts!

        flag = 0;

        for(i=0; i<16; i++)
        {
            verify[i] = cipher[0][i];
        }

        for(k=1; k<NUMBER_CP; k++)
        {
            for(i=0; i<16; i++)
            {
                verify[i] = verify[i] ^ cipher[k][i];
            }
        }

        for(i=0; i<16; i++)
        {
            if(verify[i]!=0)
            {
                flag = 1;
                break;
            }
        }

        if(flag == 0)
        {
            printf("Possible difference between the key bytes k[0][0] and k[1][1]: %x\n", delta);
            printf("Real difference: %x ", key[0][0] ^ key[1][1]);

            if((key[0][0] ^ key[1][1]) == delta)
                printf("- FOUND KEY!");

            printf("\n\n");
        }

    }

    return 0;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**The program finds the secret key (actually 1 byte of it) without using any information about the S-Box.
The attack used is a square attack. Note: it is not necessary find any information of the S-Box to find the key! */

int main()
{
    //Secret key
    word8 key[4][4] = {
        0x00, 0x44, 0x88, 0xcc,
        0x11, 0x55, 0x99, 0xdd,
        0x22, 0x66, 0xaa, 0xee,
        0x33, 0x77, 0xbb, 0xff
    };

    int r;

    srand(time(NULL));

    r = checkSboxBijection();

    printf("SQUARE ATTACK on 3-rounds AES with SECRET S-Box.\n\n");

    printf("The program finds the secret key (actually 1 byte of it) without using any information about the S-Box.\n");
    printf("The attack used is a square attack. Note: it is not necessary find any information of the S-Box to find the key!\n\n");

    printf("If you'd like, you can change the S-Box!\n\n");

    if(r == -1)
    {
        printf("The SBox is NOT bijective!!\n");
        return -1;
    }

    printf("The (secret) SBox is bijective: we can find the key without using it!!\n\n");

    foundKey(key);

    return 0;
}


