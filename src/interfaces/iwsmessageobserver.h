class IWebSocketMessageObserver {
public:
    /**
     *  Do not call API in message handler thread
     */
    virtual void handleWsMessage(const int subscriptionId, const Json &message) = 0;
};
