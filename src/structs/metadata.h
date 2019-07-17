
struct Metadata {

    unique_ptr<MDV0> metadataV0;
    unique_ptr<MDV4> metadataV4;
    unique_ptr<MDV5> metadataV5;
    unique_ptr<MDV6> metadataV6;
};

/*
  params for state_getMetadata
*/
struct GetMetadataParams {
    char blockHash[STRING_SIZE];
};