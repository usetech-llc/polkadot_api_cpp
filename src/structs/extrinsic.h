struct Method {
    uint8_t moduleIndex;
    uint8_t methodIndex;
};

struct Extrinsic {
    uint128 length;
    Signature signature;
};

struct GenericMethod : public Method {
    string methodBytes;
};

struct GenericExtrinsic : public Extrinsic {
    GenericMethod method;
    string signerAddress;
};
