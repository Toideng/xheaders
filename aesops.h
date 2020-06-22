#ifndef AESOPS_H
#define AESOPS_H

#include "xtypes.h"



int
aes128_prepare_key(
	byte roundkeys[0x200], /* will output both encryption and decryption
	                        * round keys: first 11 round keys are for
	                        * encryption, last 11 round keys are for
	                        * decryption (in order of use)
	                        */
	byte key[16]
);

void
aes128_encrypt(
	byte block[16],
	byte roundkeys[0x200]
);

void
aes128_decrypt(
	byte block[16],
	byte roundkeys[0x200]
);










void
aes192_prepare_key(
	byte roundkeys[0x200], /* will output both encryption and decryption
	                        * round keys: first 13 round keys are for
	                        * encryption, last 13 round keys are for
	                        * decryption (in order of use)
	                        */
	byte key[24]
);

void
aes192_encrypt(
	byte block[16],
	byte roundkeys[0x200]
);

void
aes192_decrypt(
	byte block[16],
	byte roundkeys[0x200]
);










int
aes256_prepare_key(
	byte roundkeys[0x200], /* will output both encryption and decryption
	                        * round keys: first 15 round keys are for
	                        * encryption, last 15 round keys are for
	                        * decryption (in order of use)
	                        */
	byte key[32]
);

void
aes256_encrypt(
	byte block[16],
	byte roundkeys[0x200]
);

void
aes256_decrypt(
	byte block[16],
	byte roundkeys[0x200]
);



#endif /* AESOPS_H */
