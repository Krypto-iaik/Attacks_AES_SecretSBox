CFLAGS=-O3 -Wall

all: AES SecretAES_3RoundAttack SecretAES_3RoundSQUAREAttack SecretAES_4RoundAttack SecretAES_4RoundAttack_Unoptimized

multiplication: multiplication.h multiplication.c multiplication_smallScale.c
	$(CC) $(CFLAGS) -c -o multiplication.o multiplication.c
	$(CC) $(CFLAGS) -c -o multiplication_smallScale.o multiplication_smallScale.c
	
subspace_checks: subspace_checks.h subspace_checks.c
	$(CC) $(CFLAGS) -c -o subspace_checks.o subspace_checks.c

aes_common: AES_common.h AES_common.c
	$(CC) $(CFLAGS) -c -o AES_common10.o AES_common.c -DN_Round=10
	$(CC) $(CFLAGS) -c -o AES_common2.o AES_common.c -DN_Round=2
	$(CC) $(CFLAGS) -c -o AES_common3.o AES_common.c -DN_Round=3
	$(CC) $(CFLAGS) -c -o AES_common4.o AES_common.c -DN_Round=4


AES: aes_common multiplication AES.c
	$(CC) $(CFLAGS) -o AES AES.c AES_common10.o multiplication.o

AES_smallScale: aes_common multiplication AES_smallScaleVersion.c
	$(CC) $(CFLAGS) -o AES_smallScale AES_smallScaleVersion.c AES_common10.o multiplication_smallScale.o

SecretAES_3RoundAttack: aes_common multiplication subspace_checks SecretAES_3RoundAttack.c
	$(CC) $(CFLAGS) -o SecretAES_3RoundAttack SecretAES_3RoundAttack.c AES_common3.o subspace_checks.o multiplication.o

SecretAES_3RoundSQUAREAttack: aes_common multiplication subspace_checks SecretAES_3RoundSQUAREAttack.c
	$(CC) $(CFLAGS) -o SecretAES_3RoundSQUAREAttack SecretAES_3RoundSQUAREAttack.c AES_common3.o subspace_checks.o multiplication.o

SecretAES_4RoundAttack: aes_common multiplication subspace_checks SecretAES_4RoundAttack.c
	$(CC) $(CFLAGS) -o SecretAES_4RoundAttack SecretAES_4RoundAttack.c AES_common4.o subspace_checks.o multiplication.o

SecretAES_4RoundAttack_Unoptimized: aes_common multiplication subspace_checks SecretAES_4RoundAttack_Unoptimized.c
	$(CC) $(CFLAGS) -o SecretAES_4RoundAttack_Unoptimized SecretAES_4RoundAttack_Unoptimized.c AES_common4.o subspace_checks.o multiplication.o

clean:
	$(RM) -f AES_common10.o AES_common2.o AES_common3.o AES_common4.o 
	$(RM) -f multiplication.o multiplication_smallScale.o 
	$(RM) -f subspace_checks.o
	$(RM) -f AES
	$(RM) -f SecretAES_3RoundAttack 
	$(RM) -f SecretAES_3RoundSQUAREAttack 
	$(RM) -f SecretAES_4RoundAttack
	$(RM) -f SecretAES_4RoundAttack_Unoptimized
	
.PHONY: clean
