class IMessageObserver {
public:
    /**
     *  Do not call API in message handler thread
     */
    virtual void handleMessage(const string &payload) = 0;
};
