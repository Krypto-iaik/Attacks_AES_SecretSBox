#Attacks on AES-128 with a Secret S-Box

Attacks for AES-128 with a single secret S-Box up to 4 rounds. The attacks are set up using the Subspace Trail.

For a complete discussion about these attacks using the Subspace Trail, we refer to the following paper:

Title: "Subspace Trail Cryptanalysis and its Applications to AES" Authors: Lorenzo Grassi, Christian Rechberger and Sondre Ronjom Available at: http://eprint.iacr.org/

Programs:

0) AES.c

Attacks:

1) SecretAES_3RoundAttack.c

2) SecretAES_3RoundSQUAREAttack.c

3) SecretAES_4RoundAttack.c
3a) SecretAES_4RoundAttack_Unoptimized.c

The programs should run with almost C-compilers (we have used gcc version 4.8.1). None of these programs is speed-optimized, they are for verification purposes only.

In all the programs, the main function is always the last one.

The first program contains our implementation of AES (encryption and decryption). It has been verified against test-vectors.

The other four programs are the ATTACKs up to four rounds on AES with Secret-Box.

In all the programs, it is possible to CHANGE the S-Box, with the only condition that the new one is bijective.
The attacks are able to find the right key without using/discovering any information of the S-Box.

For each program, the user can choose the secret key (which is defined in the main fuction). 

Only for simplicity, the programs don't try to find the complete secret key, but only part of it. We think that this is sufficient, since in all the cases the remaing part of the key can be found using the same implementation.

We implement our technique on 2 different attacks:

- truncated differential;
- square attacks;

but we remember that it is also possible to use it for impossible differential attack.


Detailed description of each of the previous programs (for all the following programs, it is possible to change the secret key defined in the main part, which is always the last one, and the S-Box, which can be found at the top of the code):

1) the first program implements the truncated differntial attack on 3 rounds of AES with secret S-Box. Given two plaintexts as described in the texts, they belong to the same coset of D_{0,1,3} after one round (and so in the same coset of M_{0,1,3} after 3 rounds) only if the guess value of the key is the right one.
Using this property it is possible to deduce one byte of the key, and in a similar way all the key.

2) the second program implements the square attack on 3 rounds of AES. Even if it is worse than the previous one, it shows that it is possible to use the same technique also for the square attack.

3/3a) the third programs implement the truncated differential attack on 4 rounds of AES with secret S-Box. While in the previous case it is possible to work on two consecutive-diagonal bytes, in this case it is necessary to work with two different pairs of two consecutive-diagonal bytes (explanation given in the text).
For these programs, it is possible to change the number of chosen plaintexts. We advise to use at least 2^13 texts, for a maximum of 2^16. To change these number, use NUMBER_CP1 and NUMBER_CP2 on line 6 and 7 (DON'T change the definition of NUMBER_CP).

For this last case, we add both the versions, that is 3) the optimized one and 3a) the unoptimized one.
The difference is the following:
- in the first one, before to count the number of collisions, the texts are ordered with respect to the partial order <= defined in the paper;
then the program considers only the consecutive texts in order to find the total number of collisions;
- in the second one, the texts are not ordered and all the possible pairs are constructed.
We have decided to add both the programs in order to better appreciate and highlight the different computational costs.

More information about these last two programs.
In these two programs, one is looking for delta0 = key[0][0]^key[1][1] and simultaneously to delta1 = key[1][2] ^ key[2][3].
Thus:
- in program 3a), the second byte (i.e. delta1) is fixed and equal to the right value (since the program is very slow);
- in program 3), it is possible to choose to find both values or only one of them (in this case, the other one is fixed and equal to the right value).
To do this, use the option SETTING on line 9 ->
 SETTING = 1: looks ONLY for delta0;
 SETTING = 2: looks for both.
	
TIME of execution (NUMBER_CP = 2^14):
 SETTING = 1 -> approximately less than 15 sec
 SETTING = 2 -> approximately less than 1 hour

- in program 3) it is possible to choose to print all the intermediate values or only the final one, using PRINT_RESULT on line 17.


FINAL Considerations:

ALL these programs show that it is possible to find the key without necessarly found the secret S-Box.
Only for completeness, we remember that when such key is found, it is possible to deduce the secret S-Box as described in the paper.
