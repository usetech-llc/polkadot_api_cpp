class IWebSocketMessageObserver {
public:
    virtual void handleWsMessage(const int subscriptionId, const Json &message) = 0;
};
