class ILogger {
public:
    virtual ~ILogger(){};

    virtual void info(string message) = 0;
    virtual void error(string message) = 0;
    virtual void warning(string message) = 0;
};
