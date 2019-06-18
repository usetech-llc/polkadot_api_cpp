class IMessageObserver {
public:
    virtual void handleMessage(const string &payload) = 0;
};
