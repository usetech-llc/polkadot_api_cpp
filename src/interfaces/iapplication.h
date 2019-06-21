class IApplication {
public:
    virtual ~IApplication() {}

    /**
     *  Connects to WebSocket
     *
     * In order to establish successful TLS connection, a root CA certificate needs to be present in pem file configured
     * in CConstants::certificate_file (currently ca-chain.cert.pem). You can put several certificates in this text file
     * one after the other. Currently Polkadot poc-3 server is using DST_Root_CA_X3 CA, so the content of this
     * certificate file is added to ca-chain.cert.pem, but in case if this changes, one can find out the issuer
     * certificate by executing following these instructions:
     *
     *  1. Execute `curl --insecure -v https://poc3-rpc.polkadot.io:443`
     *  2. Find "issuer" line in the output and download issuer certificate from their website
     *  3. Execute `openssl x509 -in <downloaded_cert_file> -noout -issuer`
     *  4. Check folder /usr/share/ca-certificates/mozilla if it has this CA certificate. If it does, copy content to
     *     ca-chain.cert.pem, otherwise return to step 2
     *
     * @return operation result
     */
    virtual int connect() = 0;

    /**
     *  Disconnects from WebSocket
     */
    virtual void disconnect() = 0;

    /**
     *  This method calls rpc bellow and puts it together:
     *  system_chain
     *  system_name
     *  system_version
     *  system_properties
     *
     * @return SystemInfo struct with result
     */
    virtual unique_ptr<SystemInfo> getSystemInfo() = 0;

    /**
     *  Retreives the block hash for specific block
     *
     *  @param struct with blockNumber block number
     *  @return BlockHash struct with result
     */
    virtual unique_ptr<BlockHash> getBlockHash(unique_ptr<GetBlockHashParams> params) = 0;

    /**
     *  Retreives the runtime metadata for specific block
     *
     *  @param struct with blockHash 64 diget number in hex format
     *  @return Metadata struct with result
     */
    virtual unique_ptr<Metadata> getMetadata(unique_ptr<GetMetadataParams> params) = 0;

    /**
     *  Retreives the runtime version information for specific block
     *
     * @param struct with blockHash 64 diget number in hex format
     * @return RuntimeVersion struct with result
     */
    virtual unique_ptr<RuntimeVersion> getRuntimeVersion(unique_ptr<GetRuntimeVersionParams> params) = 0;
};
