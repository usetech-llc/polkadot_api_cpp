#include "polkadot.h"

string CConstants::parity_node_url("wss://poc3-rpc.polkadot.io:443/");
const string CConstants::local_node_url("wss://localhost:0000/");
const string CConstants::certificate_file("ca-chain.cert.pem");
// lastLengthChange storage subscription hash
const string CConstants::lastLengthChangeSubcription("0xe781aa1e06ea53e01a4e129e0496764e");
// sessionLenth storage subscription hash
const string CConstants::sessionLengthSubcription("0xd9c94b41dc87728ebf0a966d2e9ad9c0");
// currentEra storage subscription hash
const string CConstants::currentEraSubcription("0x579ab55d37b1220812be3c3df29d4858");
// sessionsPerEra storage subscription hash
const string CConstants::sessionsPerEraSubcription("0xb7b6ec0f25eb1ed8b91d05f697d7a874");
// currentIndex storage subscription hash
const string CConstants::currentIndexSubcription("0xb8f48a8c01f629d6dc877f64892bed49");
// delay for system_health call (mircoseconds)
const long CConstants::delayTime = 300000000;