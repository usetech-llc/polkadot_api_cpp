#define MAX_LOG_MSG_LENGTH 500

class EasyLogger : public ILogger {
private:
    mutex _mtx;

public:
    virtual ~EasyLogger() {}
    virtual void info(string message);
    virtual void error(string message);
    virtual void warning(string message);
};
