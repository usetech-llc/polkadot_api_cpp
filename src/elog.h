class EasyLogger : public ILogger {
public:
    virtual ~EasyLogger() {}
    virtual void info(string message);
    virtual void error(string message);
    virtual void warning(string message);
};
