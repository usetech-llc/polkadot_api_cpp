class MetadataFactory {
private:
    string _buffer;
    int _version;
    ILogger *_logger;


public:
    MetadataFactory(ILogger* logger);
    void setInputData(string hexString);
    int getVersion();
    unique_ptr<MDV0> getMetadataV0();
    unique_ptr<MDV5> getMetadataV5();
};