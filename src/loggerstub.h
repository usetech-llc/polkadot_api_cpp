class LoggerStub : public ILogger // logger stub
{
private:
    /* data */
public:
    void info(string message);
    void error(string message);
    void warning(string message);
};
