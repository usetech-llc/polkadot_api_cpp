class IWebSocketMessageObserver {
public:
    virtual void handleWsMessage(const Json &message) = 0;
};
