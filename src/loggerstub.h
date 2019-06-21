class LoggerStub : public ILogger // logger stub
{
private:
    mutex _mtx;
    /* data */
public:
    virtual void info(string message);
    virtual void error(string message);
    virtual void warning(string message);
};
