#define SIGNATURE_VERSION 0x81
#define ADDRESS_SEPARATOR 0xFF
#define BLOCK_HASH_SIZE 32

typedef enum { IMMORTAL_ERA = 0, MORTAL_ERA = 1 } ExtrinsicEra;

typedef struct {
    uint8_t version;
    uint8_t signerPublicKey[SR25519_PUBLIC_SIZE];
    uint8_t sr25519Signature[SR25519_SIGNATURE_SIZE];
    unsigned __int128 nonce;
    ExtrinsicEra era;
} Signature;

typedef struct {
    unsigned __int128 nonce;
    uint8_t methodBytesLength;
    uint8_t *methodBytes;
    ExtrinsicEra era;
    uint8_t authoringBlockHash[BLOCK_HASH_SIZE];

    long serializeBinary(uint8_t *buf) {
        int writtenLength = 0;

        // Nonce
        auto compactNonce = scale::encodeCompactInteger(nonce);
        writtenLength += scale::writeCompactToBuf(compactNonce, buf + writtenLength);

        // Method
        memcpy(buf + writtenLength, methodBytes, methodBytesLength);
        writtenLength += methodBytesLength;

        // Extrinsic Era
        buf[writtenLength++] = era;

        // Authoring Block Hash
        memcpy(buf + writtenLength, authoringBlockHash, BLOCK_HASH_SIZE);
        writtenLength += BLOCK_HASH_SIZE;

        return writtenLength;
    }
} SignaturePayload;
