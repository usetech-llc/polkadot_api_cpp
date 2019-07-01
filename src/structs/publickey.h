#define PUBLIC_KEY_LENGTH 16
#define ADDRESS_LENGTH 48

typedef struct PublicKey {
    unsigned char bytes[PUBLIC_KEY_LENGTH];
} PublicKey;

typedef struct Address {
    unsigned char symbols[ADDRESS_LENGTH];
} Address;
