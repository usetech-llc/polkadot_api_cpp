#include "../src/polkadot.h"
#include "helpers/mockjsonrpc.h"
#undef NDEBUG
#include <cassert>

class CMockJsonRpcMissingField : public CMockJsonRpcMD0 {
private:
    virtual Json getMetadata() {

    string testValue;
    string line;
	ifstream myfile("test/metadata/exampleMetadataV0.txt");
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			testValue += line;
		}
		myfile.close();
	}

        string err;
        return Json("0x" + testValue);
    }
};

int main(int argc, char *argv[]) {
    LoggerStub log;
    CMockJsonRpcMissingField mockRpc;

    CPolkaApi app(&log, &mockRpc);
    app.connect();

    string blockHash = "0x37096ff58d1831c2ee64b026f8b70afab1942119c022d1dcfdbdc1558ebf63fa";
    
    unique_ptr<GetMetadataParams> par2(new GetMetadataParams);
    strcpy(par2->blockHash, blockHash.c_str());
    auto resp2 = app.getMetadata(move(par2));

    cout << endl << endl << endl;

    if (resp2->metadataV0 != nullptr)
        cout << "V0 first attr - " << resp2->metadataV0->module[0]->module.name << endl;
        
    if (resp2->metadataV5 != nullptr)    
        cout << "V5 first attr - " << resp2->metadataV5->module[0]->name << endl;

    // metadataV0 parsed successfully, in other case it would be nullptr
    assert(resp2->metadataV0 != nullptr);

    // metadataV5 is not set here
    assert(resp2->metadataV5 == nullptr);

    app.disconnect();

    cout << "success" << endl;

    return 0;
}
