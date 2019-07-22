#include "polkadot.h"

string CConstants::parity_node_url("wss://alex.unfrastructure.io/public/ws");
const string CConstants::local_node_url("ws://localhost:0000/");
const string CConstants::certificate_file("ca-chain.cert.pem");

// delay for system_health call (mircoseconds)
const long CConstants::health_check_delay_time = 15000000;

const string CConstants::config_file("config.json");
