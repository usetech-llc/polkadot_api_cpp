#include "../src/polkadot.h"

int main(int argc, char *argv[]) {
    JsonRpcParams params;
    params.jsonrpcVersion = "2.0";

    EasyLogger logger;
    CJsonRpc jsonRpc(CWebSocketClient::getInstance(&logger), &logger, params);

    CPolkaApi app(&logger, &jsonRpc);

    string blockHash = "0x37096ff58d1831c2ee64b026f8b70afab1942119c022d1dcfdbdc1558ebf63fa";

    app.connect();
    unique_ptr<GetMetadataParams> par2(new GetMetadataParams);
    strcpy(par2->blockHash, blockHash.c_str());
    auto resp2 = app.getMetadata(move(par2));
    app.disconnect();

    assert(resp2->metadataV0 != nullptr || resp2->metadataV5 != nullptr);
    cout << endl << "--- Received metadata ---" << endl;
    if (resp2->metadataV0) {
        cout << "OuterEventWrapperV0.name: " << resp2->metadataV0->oew->name << endl;
        cout << "ModuleV0[0].prefix: " << resp2->metadataV0->module[0]->prefix << endl;
        cout << "ModuleV0[1].prefix: " << resp2->metadataV0->module[1]->prefix << endl;
        cout << "..." << endl;
    }
    if (resp2->metadataV5) {
        cout << "ModuleV5[0].prefix: " << resp2->metadataV5->module[0]->prefix << endl;
        cout << "ModuleV5[1].prefix: " << resp2->metadataV5->module[1]->prefix << endl;
        cout << "..." << endl;
    }
    cout << endl;

    cout << "success" << endl;

    delete CWebSocketClient::getInstance(nullptr);
    return 0;
}
