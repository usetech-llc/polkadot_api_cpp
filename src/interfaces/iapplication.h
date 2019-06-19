class IApplication {
public:
    virtual ~IApplication() {}

    /**
     *  Connects to WebSocket
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
