#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "AES_common.h"
/**The user can choose the SBox!
NOTE: It MUST BE BIJECTIVE!!!*/
#include "AES_sbox.h"
#include "multiplication.h"
#include "subspace_checks.h"

#define NUMBER_CP1 256
#define NUMBER_CP2 64
#define NUMBER_CP NUMBER_CP1*NUMBER_CP2 //SHOULD BE GREATER (or EQUAL) THAN 2^13 - 2^14
#define SETTING 1
/*There are TWO possible settings. Remember that we are looking for delta_0 and delta_1.
- SETTING = 1 means that delta_1 is fixed and equal to the right value; the programs looks only for delta_0;
- SETTING = 2 means that the program looks for both delta.

Time of execution (NUMBER_CP = 2^14):
- Setting 1 -> approximately less than 15 sec
- Setting 2 -> approximately less than 1 hour*/
#define PRINT_RESULT 1
/*PRINT_RESULT = 1 -> all the intermediate results are print;
PRINT_RESULT = 0 -> only the final result is print. */

word8 play[NUMBER_CP][16], cipher[NUMBER_CP][16], cipher2copy[NUMBER_CP][16];

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**MERGE-SORT algorithm */

/*given two ciphertexts a and b, it return 0 if a<=b, 1 otherwise */
int lessOrEqual(word8 a[], word8 b[], int coset)
{
    int i, temp, aaa, bbb;

    for(i=0;i<4;i++)
    {
        temp = 4*coset - 3 * i;

        if(temp < 0)
            temp = temp + 16;

        aaa = (int) a[temp];
        bbb = (int) b[temp];

        if(aaa>bbb)
            return 1;

        if(aaa<bbb)
            return 0;
    }

    return 0;
}

//void merging2(int low, int mid, int high, int coset) {
//
//    int l1, l2, i, j;
//    word8 text1[16], text2[16];
//
//    l1 = low;
//    l2 = mid;
//
//    for(i = low; ((l1 < mid) && (l2 < high)); i++)
//    {
//        for(j = 0; j<16; j++)
//        {
//            text1[j] = cipher[l1][j];
//            text2[j] = cipher[l2][j];
//        }
//
//        if(lessOrEqual(text1, text2, coset) == 0)
//        {
//            for(j = 0; j<16; j++)
//            {
//                cipher2copy[i][j] = text1[j];
//            }
//            l1++;
//        }
//        else
//        {
//            for(j = 0; j<16; j++)
//            {
//                cipher2copy[i][j] = text2[j];
//            }
//            l2++;
//        }
//    }
//
//    while(l1 < mid)
//    {
//       for(j = 0; j<16; j++)
//        {
//            cipher2copy[i][j] = cipher[l1][j];
//        }
//        i++;
//        l1++;
//    }
//
//    while(l2 < high)
//    {
//       for(j = 0; j<16; j++)
//        {
//            cipher2copy[i][j] = cipher[l2][j];
//        }
//        i++;
//        l2++;
//    }
//
//    for(i = low; i < high; i++)
//    {
//        for(j = 0; j<16; j++)
//        {
//            cipher[i][j] = cipher2copy[i][j];
//        }
//    }
//}

void merging3(int low, int mid, int high, int coset) {

    int l1, l2, i, j;
    word8 text1[16], text2[16];

    l1 = low;
    l2 = mid + 1;

    for(i = low; ((l1 <= mid) && (l2 <= high)); i++)
    {
        for(j = 0; j<16; j++)
        {
            text1[j] = cipher[l1][j];
            text2[j] = cipher[l2][j];
        }

        if(lessOrEqual(text1, text2, coset) == 0)
        {
            for(j = 0; j<16; j++)
            {
                cipher2copy[i][j] = text1[j];
            }
            l1++;
        }
        else
        {
            for(j = 0; j<16; j++)
            {
                cipher2copy[i][j] = text2[j];
            }
            l2++;
        }
    }

    while(l1 <= mid)
    {
       for(j = 0; j<16; j++)
        {
            cipher2copy[i][j] = cipher[l1][j];
        }
        i++;
        l1++;
    }

    while(l2 <= high)
    {
       for(j = 0; j<16; j++)
        {
            cipher2copy[i][j] = cipher[l2][j];
        }
        i++;
        l2++;

    }

    for(i = low; i <= high; i++)
    {
        for(j = 0; j<16; j++)
        {
            cipher[i][j] = cipher2copy[i][j];
        }
    }

}

/**This function works only when the number of texts is a power of 2.*/
/*void sort2(int coset)
{
    int log, i, j, division, high, low, middle, a, b, c;
    word8 t1[16], t2[16];

    log = logarithm2(NUMBER_CP);

    for(i=0; i<NUMBER_CP; i = i+2)
    {
        for(j=0;j<16;j++)
        {
            t1[j] = cipher[i][j];
            t2[j] = cipher[i+1][j];
        }

        if(lessOrEqual(t1, t2, coset) == 1)
        {
            for(j=0;j<16;j++)
            {
                cipher[i][j] = t2[j];
                cipher[i+1][j] = t1[j];
            }
        }
    }

    for(i = 2; i < log; i++)
    {
        a = pow2(i);
        b = a/2;
        division = NUMBER_CP / a;

        for(j = 0; j < division; j++)
        {
            high = a * (j+1);
            low = a * j;
            middle = low + b;

            merging2(low, middle, high, coset);

        }
    }

}*/


void sort3(int low, int high, int coset) {
   int mid;

   long int m;

   if(low < high) {
      m = (long int) low + (long int) high;
      m = m/2;
      mid = (int) m;
//      mid = (low + high)/2;
      sort3(low, mid, coset);
      sort3(mid+1, high, coset);
      merging3(low, mid, high, coset);
   }else {
      return;
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**The function finds two bytes of the key, precisely k_{0,0} + k_{1,1} and k_{1,2} + k_{2,3}, using
 the strategy described in the paper.
It also checks that it is the correct one.
Note that the idea is that for a wrong key, AES behaves as a random function.
NOTE: it is not necessary to find any information of the S-Box to find the key.*/

int foundKey2(word8 key[][4])
{
    int dddelta, i, flag, j, k, l, max, numberCollision, kkk, rTemp, coset, dddelta2;
    unsigned char temp[4][4], temp2[4][4], delta, delta1, verify[4][4], superdelta, superdelta2;

    long int contSup;

    max = -1;

    //delta2 = key[1][2] ^ key[2][3];

    for(dddelta2 = 0; dddelta2 < 256; dddelta2++)
    {
        delta1 = (word8) dddelta2;

        for(dddelta = 0; dddelta < 256; dddelta++)
        {

            delta = (word8) dddelta;

            if(PRINT_RESULT == 1)
                printf("0x%x - 0x%x \t", delta1, delta);

            /**Generation of the plaintexts*/

            numberCollision = 0;

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

            for(kkk = 0; kkk < NUMBER_CP1; kkk++)
            {
                for(k=0;k<NUMBER_CP2;k++)
                {
                    play[k + NUMBER_CP2 * kkk][9] = (word8) kkk;
                    play[k + NUMBER_CP2 * kkk][14] = play[k + NUMBER_CP2 * kkk][9] ^ delta1;
                    play[k + NUMBER_CP2 * kkk][0] = (word8) k;
                    play[k + NUMBER_CP2 * kkk][5] = play[k + NUMBER_CP2 * kkk][0] ^ delta;
                }
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

            numberCollision = 0;

            //Four times!

            for(coset = 0; coset < 4; coset++)
            {
                //re-order the ciphertexts!
                sort3(0, NUMBER_CP-1, coset);
                //sort2(coset);

                //count the number of collision
                i = 0;
                rTemp = 0;

                while(i < (NUMBER_CP-1))
                {
                    rTemp = 0;
                    j = i;
                    flag = 0;

                    do
                    {
                        flag = 0;

                        for(k = 0; k<4; k++)
                        {
                            for(l = 0; l<4; l++)
                            {
                                temp[k][l] = cipher[j+1][k + l*4] ^ cipher[j][k + l*4];
                            }
                        }

                        if(belongToW_2(temp, coset) == 1)
                        {
                            rTemp = rTemp + 1;
                            flag = 1;
                            j = j + 1;
                        }
                    }while(flag == 1);

                    i = j + 1;
                    rTemp = rTemp * (rTemp + 1);
                    numberCollision = numberCollision + (rTemp/ 2);
                }
            }


            /*for(k=0; k<NUMBER_CP; k++)
            {
                for(i=k+1; i<NUMBER_CP; i++)
                {
                    for(j=0;j<4;j++)
                    {
                        for(l=0;l<4;l++)
                        {
                            verify[j][l] = cipher[k][j+4*l] ^ cipher[i][j+4*l];
                        }
                    }

                    numberCollision += belongToW(verify);
                }
            }*/


            if(numberCollision > max)
            {
                max = numberCollision;
                superdelta = delta;
                superdelta2 = delta1;
            }

            if(PRINT_RESULT == 1)
                printf("%d \n", numberCollision);

        }
    }

    printf("Possible difference between the key bytes k[0][0] and k[1][1]: %x\n", superdelta);
    printf("Real difference: %x\n", key[0][0] ^ key[1][1]);

    printf("Possible difference between the key bytes k[1][2] and k[2][3]: %x\n", superdelta2);
    printf("Real difference: %x\n ", key[1][2] ^ key[2][3]);

    if(((key[0][0] ^ key[1][1]) == superdelta)&&((key[1][2] ^ key[2][3]) == superdelta2))
        printf("\t - FOUND KEY!");

    printf("\n\n");

    return 0;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**The function finds two bytes of the key, precisely delta_0 = k_{0,0} + k_{1,1} and delta_1 = k_{1,2} + k_{2,3}, using
 the strategy described in the paper.
THE SECOND ONE (delta_1) IS FIXED and EQUAL to the RIGHT ONE.
It also checks that it is the correct one.
Note that the idea is that for a wrong key, AES behaves as a random function.
NOTE: it is not necessary to find any information of the S-Box to find the key.*/

int foundKey(word8 key[][4])
{
    int dddelta, i, flag, j, k, l, max, numberCollision, kkk, rTemp, coset;
    unsigned char temp[4][4], temp2[4][4], delta, delta1, verify[4][4], superdelta;

    long int contSup;

    max = -1;

    delta1 = key[1][2] ^ key[2][3];

    for(dddelta = 0; dddelta < 256; dddelta++)
    {

        delta = (word8) dddelta;

        if(PRINT_RESULT == 1)
            printf("0x%x \t", delta);

        /**Generation of the plaintexts*/

        numberCollision = 0;

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

        for(kkk = 0; kkk < NUMBER_CP1; kkk++)
        {
            for(k=0;k<NUMBER_CP2;k++)
            {
                play[k + NUMBER_CP2 * kkk][9] = (word8) kkk;
                play[k + NUMBER_CP2 * kkk][14] = play[k + NUMBER_CP2 * kkk][9] ^ delta1;
                play[k + NUMBER_CP2 * kkk][0] = (word8) k;
                play[k + NUMBER_CP2 * kkk][5] = play[k + NUMBER_CP2 * kkk][0] ^ delta;
            }
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

        numberCollision = 0;

        //Four times!

        for(coset = 0; coset < 4; coset++)
        {
            //re-order the ciphertexts!
            sort3(0, NUMBER_CP-1, coset);
            //sort2(coset);

            //count the number of collision
            i = 0;
            rTemp = 0;

            while(i < (NUMBER_CP-1))
            {
                rTemp = 0;
                j = i;
                flag = 0;

                do
                {
                    flag = 0;

                    for(k = 0; k<4; k++)
                    {
                        for(l = 0; l<4; l++)
                        {
                            temp[k][l] = cipher[j+1][k + l*4] ^ cipher[j][k + l*4];
                        }
                    }

                    if(belongToW_2(temp, coset) == 1)
                    {
                        rTemp = rTemp + 1;
                        flag = 1;
                        j = j + 1;
                    }
                }while(flag == 1);

                i = j + 1;
                rTemp = rTemp * (rTemp + 1);
                numberCollision = numberCollision + (rTemp/ 2);
            }
        }


        /*for(k=0; k<NUMBER_CP; k++)
        {
            for(i=k+1; i<NUMBER_CP; i++)
            {
                for(j=0;j<4;j++)
                {
                    for(l=0;l<4;l++)
                    {
                        verify[j][l] = cipher[k][j+4*l] ^ cipher[i][j+4*l];
                    }
                }

                numberCollision += belongToW(verify);
            }
        }*/


        if(numberCollision > max)
        {
            max = numberCollision;
            superdelta = delta;
        }

        if(PRINT_RESULT == 1)
            printf("%d \n", numberCollision);

    }

    printf("Possible difference between the key bytes k[0][0] and k[1][1]: %x\n", superdelta);
    printf("Real difference: %x ", key[0][0] ^ key[1][1]);

    if((key[0][0] ^ key[1][1]) == superdelta)
        printf("- FOUND KEY!");

    printf("\n\n");

    return 0;

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**The program finds the secret key (actually 1 byte of it) without using any information about the S-Box.
The attack used is a 4-rounds truncated differential attack. Note: it is not necessary find any information of the S-Box to find the key! */

int main()
{
    //Secret key
    word8 key[4][4] = {
        0x00, 0x44, 0x88, 0xcc,
        0x11, 0x05, 0x99, 0xdd,
        0x22, 0x66, 0xaa, 0xee,
        0x33, 0x77, 0xbb, 0xff
    };

    int r;

    srand(time(NULL));

    r = checkSboxBijection();

    printf("ATTACK on 4-rounds AES with SECRET S-Box.\n\n");

    printf("The program finds the secret key (actually 1 byte of it) without using any information about the S-Box.\n\n");
    printf("The attack used is a 4-rounds truncated differential attack. Note: it is not necessary find any information of the S-Box to find the key!\n\n");

    printf("If you'd like, you can change the S-Box!\n\n");

    if(r == -1)
    {
        printf("The SBox is NOT bijective!!\n");
        return -1;
    }

    printf("The (secret) SBox is bijective: we can find the key without using it!!\n");

    /**SETTING = 1 -> we are looking for 2 delta: one of them is fixed and equal to the right one!*/
    if(SETTING == 1)
    {
        foundKey(key);
    }
    else
    {
        foundKey2(key);
    }

    return 0;
}


