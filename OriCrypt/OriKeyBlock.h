#ifndef KeyBlockH
#define KeyBlockH

#define ORI_KEY_CREATE_8BIT(name, key)    \
    const char name[] = { key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7], 0x00 };

#define ORI_KEY_CREATE_16BIT(name, key)         \
    const char name[] = { key[0], key[1], key[2],  key[3],  key[4],  key[5],  key[6],  key[7],   \
                          key[8], key[9], key[10], key[11], key[12], key[13], key[14], key[15], 0x00 };

#define ORI_KEY_CREATE_32BIT(name, key)         \
    const char name[] = { key[0],  key[1],  key[2],  key[3],  key[4],  key[5],  key[6],  key[7],  \
                          key[8],  key[9],  key[10], key[11], key[12], key[13], key[14], key[15], \
						  key[16], key[17], key[18], key[19], key[20], key[21], key[22], key[23], \
                          key[24], key[25], key[26], key[27], key[28], key[29], key[30], key[31], 0x00 };

#define ORI_KEY_CREATE_64BIT(name, key)         \
    const char name[] = { key[0],  key[1],  key[2] , key[3],  key[4],  key[5],  key[6],  key[7],  \
                          key[8],  key[9],  key[10], key[11], key[12], key[13], key[14], key[15], \
						  key[16], key[17], key[18], key[19], key[20], key[21], key[22], key[23], \
                          key[24], key[25], key[26], key[27], key[28], key[29], key[30], key[31], \
					      key[32], key[33], key[34], key[35], key[36], key[37], key[38], key[39], \
                          key[40], key[41], key[42], key[43], key[44], key[45], key[46], key[47], \
						  key[48], key[49], key[50], key[51], key[52], key[53], key[54], key[55], \
                          key[56], key[57], key[58], key[59], key[60], key[61], key[62], key[63], 0x00 };


#define ORI_DKEY_CREATE(name, key)       \
ORI_KEY_CREATE_64BIT(StaticKey, key);    \
ORI_DYNAMIC_KEY_CREATE(name, StaticKey); 

#define ORI_DYNAMIC_KEY_CREATE(name, static_key) \
           char key[65]    = { 0x00 };           \
		   SHA256Crypt(static_key, key);         \
		   const char name[] =                   \
					    { key[0],  key[1],  key[2] , key[3],  key[4],  key[5],  key[6],  key[7],  \
                          key[8],  key[9],  key[10], key[11], key[12], key[13], key[14], key[15], \
						  key[16], key[17], key[18], key[19], key[20], key[21], key[22], key[23], \
                          key[24], key[25], key[26], key[27], key[28], key[29], key[30], key[31], \
					      key[32], key[33], key[34], key[35], key[36], key[37], key[38], key[39], \
                          key[40], key[41], key[42], key[43], key[44], key[45], key[46], key[47], \
						  key[48], key[49], key[50], key[51], key[52], key[53], key[54], key[55], \
                          key[56], key[57], key[58], key[59], key[60], key[61], key[62], key[63], 0x00 };

#define ORI_KEY_RESULT(name) (char *)name
              
#endif
