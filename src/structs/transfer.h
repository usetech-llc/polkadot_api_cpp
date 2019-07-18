
struct TransferMethod : public Method {
    uint8_t receiverPublicKey[SR25519_PUBLIC_SIZE];
    uint128 amount;
};

struct TransferExtrinsic : public Extrinsic {
    TransferMethod method;

    long serializeMethodBinary(uint8_t *buf) {
        // Compact-encode amount
        auto compactAmount = scale::encodeCompactInteger(method.amount);

        int writtenLength = 0;

        // Module + Method
        buf[writtenLength++] = method.moduleIndex;
        buf[writtenLength++] = method.methodIndex;

        // Address separator
        buf[writtenLength++] = ADDRESS_SEPARATOR;

        // Receiving address public key
        memcpy(buf + writtenLength, method.receiverPublicKey, SR25519_PUBLIC_SIZE);
        writtenLength += SR25519_PUBLIC_SIZE;

        // Amount
        writtenLength += scale::writeCompactToBuf(compactAmount, buf + writtenLength);

        return writtenLength;
    }

    long serializeBinary(uint8_t *buf) {
        // Compact-encode amount
        auto compactAmount = scale::encodeCompactInteger(method.amount);

        // Compact-encode nonce
        auto compactNonce = scale::encodeCompactInteger(signature.nonce);

        // Calculate total extrinsic length
        length = 134 + compactAmount.length + compactNonce.length;
        auto compactLength = scale::encodeCompactInteger(length);

        /////////////////////////////////////////
        // Serialize and write to buffer

        int writtenLength = 0;

        // Length
        writtenLength += scale::writeCompactToBuf(compactLength, buf + writtenLength);

        // Signature version
        buf[writtenLength++] = signature.version;

        // Address separator
        buf[writtenLength++] = ADDRESS_SEPARATOR;

        // Signer public key
        memcpy(buf + writtenLength, signature.signerPublicKey, SR25519_PUBLIC_SIZE);
        writtenLength += SR25519_PUBLIC_SIZE;

        // SR25519 Signature
        memcpy(buf + writtenLength, signature.sr25519Signature, SR25519_SIGNATURE_SIZE);
        writtenLength += SR25519_SIGNATURE_SIZE;

        // Nonce
        writtenLength += scale::writeCompactToBuf(compactNonce, buf + writtenLength);

        // Extrinsic Era
        buf[writtenLength++] = signature.era;

        // Method
        writtenLength += serializeMethodBinary(buf + writtenLength);

        return writtenLength;
    }
};
