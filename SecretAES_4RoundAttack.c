#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N_Round 4
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

typedef unsigned char word8;

/**The user can choose the SBox!
NOTE: It MUST BE BIJECTIVE!!!*/

//S-box
const unsigned char sBox[256] = {
  0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
  0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
  0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
  0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
  0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
  0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
  0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
  0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
  0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
  0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
  0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
  0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
  0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
  0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
  0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
  0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

word8 play[NUMBER_CP][16], cipher[NUMBER_CP][16], cipher2copy[NUMBER_CP][16];

int randomInRange(int min, int max){

  int range = max - min + 1;
  int a, b, c, d;

  a = rand() % range;
  b = rand() % range;
  c = rand() % range;
  d = (a*b) % range;
  d = (d+c) % range;

  return (min + d);

}

word8 randomByte(){
  return (word8) randomInRange(0, 255);
}

//int logarithm2(long int n)
//{
//    long int i = 1;
//    int j = 1;
//
//    if (n<=0)
//        return -1;
//
//    if (n == 1)
//        return 0;
//
//    while(i<n)
//    {
//        i = i * 2;
//        j++;
//    }
//
//    return j;
//
//}
//
//long int pow2(int n)
//{
//    long int i = 1;
//    int j;
//
//    if(n == 0)
//        return 1;
//
//    for(j=0; j<n;j++)
//    {
//        i = i * 2;
//    }
//
//    return i;
//}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*Multiplication byte per x*/

word8 multiplicationX(word8 byte){

  word8 bitTemp;

  bitTemp=byte>>7;
  byte=byte<<1;

  if(bitTemp==0)
    return byte;
  else
    return (byte^0x1B);

}

/*Multiplication byte per x^n*/

word8 multiplicationXN(word8 byte, int n){

  int i;

  for(i=0;i<n;i++)
    byte=multiplicationX(byte);

  return byte;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*Initialization state*/

void inizialization(word8 *p, unsigned char initialMessage[][4]){

  int i, j;

  for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      *(p+j+4*i) = initialMessage[i][j];
    }
  }

}

void inizialization2(word8 *p, unsigned char initialMessage[][4]){

  int i, j;

  for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      *(p+(N_Round+1)*j+(N_Round+1)*4*i) = initialMessage[i][j];
    }
  }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*MixColumn*/

void mixColumn(word8 *p){

  int i, j;
  word8 colonna[4], nuovaColonna[4];

  //separo le colonne e calcolo le nuove
  for(i=0;i<4;i++){

    //prendo la colonna i-sima
    for(j=0;j<4;j++){
      colonna[j]=*(p + i + 4*j);
    }

    //calcolo nuova colonna
    nuovaColonna[0]= multiplicationX(colonna[0]) ^ multiplicationX(colonna[1]) ^ colonna[1] ^ colonna[2] ^ colonna[3];
    nuovaColonna[1]= colonna[0] ^ multiplicationX(colonna[1]) ^ multiplicationX(colonna[2]) ^ colonna[2] ^ colonna[3];
    nuovaColonna[2]= colonna[0] ^ colonna[1] ^ multiplicationX(colonna[2]) ^ multiplicationX(colonna[3]) ^ colonna[3];
    nuovaColonna[3]= multiplicationX(colonna[0]) ^ colonna[0] ^ colonna[1] ^ colonna[2] ^ multiplicationX(colonna[3]);

    //reinserisco colonna
    for(j=0;j<4;j++){
      *(p + i + 4*j)=nuovaColonna[j];
    }

  }

}

/*Inverse MixColumn*/

void inverseMixColumn(word8 *p){

  int i, j;
  word8 colonna[4], nuovaColonna[4];

  //separo le colonne e calcolo le nuove
  for(i=0;i<4;i++){

    //prendo la colonna i-sima
    for(j=0;j<4;j++){
      colonna[j]=*(p + i + 4*j);
    }

    //calcolo nuova colonna
    nuovaColonna[0]= multiplicationXN(colonna[0], 3) ^ multiplicationXN(colonna[0], 2) ^ multiplicationX(colonna[0]) ^
                     multiplicationXN(colonna[1], 3) ^ multiplicationX(colonna[1]) ^ colonna[1] ^ multiplicationXN(colonna[2], 3) ^
                     multiplicationXN(colonna[2], 2) ^ colonna[2] ^ multiplicationXN(colonna[3], 3) ^ colonna[3];

    nuovaColonna[1]= multiplicationXN(colonna[0], 3) ^ colonna[0] ^ multiplicationXN(colonna[1], 3) ^ multiplicationXN(colonna[1], 2) ^
                     multiplicationX(colonna[1]) ^ multiplicationXN(colonna[2], 3) ^ multiplicationX(colonna[2]) ^ colonna[2] ^
                     multiplicationXN(colonna[3], 3) ^ multiplicationXN(colonna[3], 2) ^ colonna[3];

    nuovaColonna[2]= multiplicationXN(colonna[0], 3) ^ multiplicationXN(colonna[0], 2) ^ colonna[0] ^ multiplicationXN(colonna[1], 3) ^
                    colonna[1] ^ multiplicationXN(colonna[2], 3) ^ multiplicationXN(colonna[2], 2) ^ multiplicationX(colonna[2]) ^
                    multiplicationXN(colonna[3], 3)^multiplicationX(colonna[3]) ^ colonna[3];

    nuovaColonna[3]= multiplicationXN(colonna[0], 3)^ multiplicationX(colonna[0]) ^ colonna[0] ^ multiplicationXN(colonna[1], 3) ^
                    multiplicationXN(colonna[1], 2)^colonna[1] ^ multiplicationXN(colonna[2], 3)^colonna[2] ^ multiplicationXN(colonna[3], 3)^
                    multiplicationXN(colonna[3], 2)^multiplicationX(colonna[3]);

    //reinserisco colonna
    for(j=0;j<4;j++){
      *(p + i + 4*j)=nuovaColonna[j];
    }

  }

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*Add Round Key*/

void addRoundKey(word8 *p, word8 key[][4]){

  int i, j;

  for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      *(p+j+4*i) ^= key[i][j];
    }
  }

}

void addRoundKey2(word8 *p, word8 key[][4][N_Round+1], int costante){

  int i, j;

  for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      *(p+j+4*i) ^= key[i][j][costante];
    }
  }

}

/*Inverse Add round key*/

void invAddRoundKey(word8 *p, word8 key[][4][N_Round+1], int costante){

  word8 keytemp[4][4];

  int i, j;

  for(i=0;i<4;i++){
    for(j=0;j<4;j++)
      keytemp[i][j]=key[i][j][costante];
  }

  inverseMixColumn(&(keytemp[0][0]));

   for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      *(p+j+4*i) ^= keytemp[i][j];
    }
  }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*Print*/

void printState(word8 file[][4]){

  int i, j;

  for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      printf("0x%x, ", file[i][j]);
    }
    printf("\n");
  }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*Byte sub transformation with S-box*/

word8 byteTransformation(word8 byte){

  return sBox[byte];

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*ByteTransformation*/

void byteSubTransformation(word8 *puntatore){

  int i, j;

  for(i=0; i<4; i++){
    for(j=0; j<4; j++)
      *(puntatore+j+4*i)=byteTransformation(*(puntatore+j+4*i));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*Generation Round key*/

/*Calculation third column of the key*/
void nuovaColonna(word8 *pColonna, int numeroRound){

  word8 temp, rCostante, colonnaTemp[4];
  int i;

  //rotazione degli elementi
  temp=*pColonna;
  for(i=0;i<3;i++){
    colonnaTemp[i]=*(pColonna+i+1);
  }
  colonnaTemp[3]=temp;

  //S-box
  for(i=0;i<4;i++)
    colonnaTemp[i]=byteTransformation(colonnaTemp[i]);

  //ultimoStep
  if(numeroRound==0)
    rCostante=0x01;
  else{
    rCostante=0x02;
    for(i=1;i<numeroRound;i++)
       rCostante=multiplicationX(rCostante);
  }

  colonnaTemp[0]^=rCostante;

  //return colonna
  for(i=0;i<4;i++){
    *(pColonna+i)=colonnaTemp[i];
  }

}

void generationRoundKey(word8 *pKey, int numeroRound){

  int i, j;

  word8 colonnaTemp[4];

  //third column
  for(i=0;i<4;i++)
    colonnaTemp[i]=*(pKey + 3 + 4*i);

  nuovaColonna(&(colonnaTemp[0]), numeroRound);

  //New key//

  //first column
  for(i=0;i<4;i++)
    *(pKey+4*i)=*(pKey+4*i)^colonnaTemp[i];

  //other columns
  for(i=1;i<4;i++){

    for(j=0;j<4;j++){
      *(pKey+i+4*j)=*(pKey+i+4*j)^*(pKey+i+4*j-1);
    }

  }

}

void generationRoundKey2(word8 *pKey, int numeroRound, word8 *pKeyPrecedente){

  int i, j;

  word8 colonnaTemp[4];

  numeroRound--;

  //third column
  for(i=0;i<4;i++)
    colonnaTemp[i]=*(pKeyPrecedente + 3*(N_Round+1) + 4*i*(N_Round+1));

  nuovaColonna(&(colonnaTemp[0]), numeroRound);

  //New Key//

  //first column
  for(i=0;i<4;i++)
    *(pKey+4*(N_Round+1)*i)=*(pKeyPrecedente+4*(N_Round+1)*i)^colonnaTemp[i];

  //other columns
  for(i=1;i<4;i++){

    for(j=0;j<4;j++){
      *(pKey+i*(N_Round+1)+4*(1+N_Round)*j)=*(pKeyPrecedente+i*(N_Round+1)+4*(1+N_Round)*j)^*(pKey+(i-1)*(1+N_Round)+4*(1+N_Round)*j);
    }

  }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*shift rows*/

void shiftRows(word8 *p){

  word8 temp[3];
  int i, j;

  for(i=1;i<4;i++){
    for(j=0;j<i;j++)
      temp[j]=*(p+4*i+j);

    for(j=0;j<(4-i);j++)
      *(p+4*i+j)=*(p+4*i+j+i);

    for(j=(4-i);j<4;j++)
      *(p+4*i+j)=temp[j-4+i];
  }

}

/*inverse shift rows*/

void inverseShiftRows(word8 *p){

  word8 temp[3];
  int i, j;

  for(i=1;i<4;i++){
    for(j=3;j>(3-i);j--)
      temp[j-1]=*(p+4*i+j);

    for(j=3;j>i-1;j--)
      *(p+4*i+j)=*(p+4*i+j-i);

    for(j=0;j<i;j++)
      *(p+4*i+j)=temp[3-i+j];
  }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*Encryption*/

void encryption(word8 initialMessage[][4], word8 initialKey[][4], word8 *messaggioCifrato){

  int i, j;

  //inizialization - state
  unsigned char state[4][4];
  inizialization(&(state[0][0]), initialMessage);

  //inizialization - key
  unsigned char key[4][4];
  inizialization(&(key[0][0]), initialKey);

  //Initial Round
  addRoundKey(&(state[0][0]), key);

  //Round
  for(i=0; i<N_Round-1; i++){
    generationRoundKey(&(key[0][0]), i);
    byteSubTransformation(&(state[0][0]));
    shiftRows(&(state[0][0]));
    mixColumn(&(state[0][0]));
    addRoundKey(&(state[0][0]), key);

  }

  //Final Round
  generationRoundKey(&(key[0][0]), N_Round-1);
  byteSubTransformation(&(state[0][0]));
  shiftRows(&(state[0][0]));
  addRoundKey(&(state[0][0]), key);

  //store the message
  for(i=0; i<4; i++){
    for(j=0; j<4; j++)
      *(messaggioCifrato+j+4*i)=state[i][j];
  }

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*Suppose that p = p1 \xor p2, that is the sum of two plaintexts, and consider W of dimension 12.
It returns:
0 - not belong;
1 - belong to subspace of dim 12.
*/

int belongToW(word8 p[][4])
{
    /*NOTE: I consider for simplicity MC^-1(W) and I apply MC^-1 directly on the texts!*/

    if ((p[0][0] == 0) && (p[3][1] == 0) && (p[2][2] == 0) && (p[1][3] == 0))
       return 1;

    if ((p[1][0] == 0) && (p[0][1] == 0) && (p[3][2] == 0) && (p[2][3] == 0))
       return 1;

    if ((p[2][0] == 0) && (p[1][1] == 0) && (p[0][2] == 0) && (p[3][3] == 0))
       return 1;

    if ((p[3][0] == 0) && (p[2][1] == 0) && (p[1][2] == 0) && (p[0][3] == 0))
       return 1;

    return 0;
}

int belongToW1(word8 p[][4])
{
    /*Consider MC^-1(M_0) since no mixcolumns at the end!*/

    if ((p[0][0] == 0) && (p[3][1] == 0) && (p[2][2] == 0) && (p[1][3] == 0))
       return 1;

    return 0;
}

int belongToW2(word8 p[][4])
{
    /*Consider MC^-1(M_1) since no mixcolumns at the end!*/

   if ((p[1][0] == 0) && (p[0][1] == 0) && (p[3][2] == 0) && (p[2][3] == 0))
       return 1;

   return 0;
}

int belongToW3(word8 p[][4])
{
    /*Consider MC^-1(M_2) since no mixcolumns at the end!*/

    if ((p[2][0] == 0) && (p[1][1] == 0) && (p[0][2] == 0) && (p[3][3] == 0))
       return 1;

   return 0;
}

int belongToW4(word8 p[][4])
{
    /*Consider MC^-1(M_3) since no mixcolumns at the end!*/

    if ((p[3][0] == 0) && (p[2][1] == 0) && (p[1][2] == 0) && (p[0][3] == 0))
       return 1;

    return 0;
}

int belongToW_2(word8 p[][4], int coset)
{
    if(coset == 0)
        return belongToW1(p);

    if(coset == 1)
        return belongToW2(p);

    if(coset == 2)
        return belongToW3(p);

    return belongToW4(p);

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

///**This function works only when the number of texts is a power of 2.*/
//void sort2(int coset)
//{
//    int log, i, j, division, high, low, middle, a, b, c;
//    word8 t1[16], t2[16];
//
//    log = logarithm2(NUMBER_CP);
//
//    for(i=0; i<NUMBER_CP; i = i+2)
//    {
//        for(j=0;j<16;j++)
//        {
//            t1[j] = cipher[i][j];
//            t2[j] = cipher[i+1][j];
//        }
//
//        if(lessOrEqual(t1, t2, coset) == 1)
//        {
//            for(j=0;j<16;j++)
//            {
//                cipher[i][j] = t2[j];
//                cipher[i+1][j] = t1[j];
//            }
//        }
//    }
//
//    for(i = 2; i < log; i++)
//    {
//        a = pow2(i);
//        b = a/2;
//        division = NUMBER_CP / a;
//
//        for(j = 0; j < division; j++)
//        {
//            high = a * (j+1);
//            low = a * j;
//            middle = low + b;
//
//            merging2(low, middle, high, coset);
//
//        }
//    }
//
//}


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


