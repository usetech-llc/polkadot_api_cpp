#define RESPONSE_TIMEOUT_S 10

class IJsonRpc {
public:
    virtual ~IJsonRpc() {}

    /**
     *  Connects to WebSocket
     *
     * @param node_url - Node URL to connect to
     * @return operation result
     */
    virtual int connect(string node_url = "") = 0;

    /**
     *  Disconnects from WebSocket
     */
    virtual void disconnect() = 0;

    /**
     *  Synchronously send request and wait for result
     *
     * @param jsonMap JSON object with command parameters
     * @param timeout_s - timeout of response in seconds
     * @return JSON object that contains parsed node response
     */
    virtual Json request(Json jsonMap, long timeout_s = RESPONSE_TIMEOUT_S) = 0;

    /**
     *  Send a command to subscribe to websocket updates, e.g. state_subscribeStorage
     *
     * @param jsonMap JSON object with command parameters
     * @param observer The observer that will be notified of updates
     * @return subscription ID
     */
    virtual int subscribeWs(Json jsonMap, IWebSocketMessageObserver *observer) = 0;

    /**
     *  Send a command to unsubscribe from websocket updates, e.g. state_unsubscribeStorage
     *
     * @param subscriptionId Id of subscription to unsubscribe from
     * @return command execution result
     */
    virtual int unsubscribeWs(int subscriptionId) = 0;
};
