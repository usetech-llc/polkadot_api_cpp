#include "polkadot.h"

using easywsclient::WebSocket;

CWebSocketClient::CWebSocketClient(string nodeUrl) :
    _nodeUrl(nodeUrl)
{
}

CWebSocketClient::CWebSocketClient()
{
}

//{"jsonrpc":"2.0","method":"state_storage","params":{"result":{"block":"0x4a10c0e96359e56170dee4ab0163d254a0adbf92dcaae242d4cdb391a42fb4be","changes":[["0xcc956bdb7605e3547539f321ac2bc95c","0x080000000000000000010000000000"]]},"subscription":3227192}}

int CWebSocketClient::connect()
{
    WebSocket::pointer ws = WebSocket::from_url(_nodeUrl);

    if (!ws) {
        cout << "ws is null" << endl;
        return -1;
    } else {
        //while (true) {
            ws->poll();
            ws->send("hello");
            //ws->dispatch(handle_message);
            // ...do more stuff...
        //}
        delete ws;
    }

    return 0;
}
