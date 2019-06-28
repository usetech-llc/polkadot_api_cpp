class EasyLogger : public ILogger {
private:
    mutex _mtx;

public:
    virtual ~EasyLogger() {}
    virtual void info(string message);
    virtual void error(string message);
    virtual void warning(string message);
};
