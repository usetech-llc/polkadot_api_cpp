#include <fstream>
#include <iostream>
#include <stdio.h>
#include <streambuf>
#include <string.h>
#include <unistd.h>

using namespace std;

#include <polkacpp/polkacpp.h>

std::string helpText = " Available commands:\n"
                       " connect                  - Connect to a node and display basic information\n"
                       " balance <address>\n"
                       " trasnfer <from address> <to address> <amount> <private key file>\n";

shared_ptr<polkadot::api> papi;
IApplication *app;
void connect() {
    cout << "Connecting to polkadot node... " << endl;
    papi = polkadot::api::getInstance();
    app = papi->app();
    app->connect();
}

void displayInfo() {
    auto resp = app->getSystemInfo();
    cout << "System Information: " << endl
         << "  Chain ID       : " << resp->chainId << endl
         << "  Chain Name     : " << resp->chainName << endl
         << "  Token Decimals : " << resp->tokenDecimals << endl
         << "  Token Symbol   : " << resp->tokenSymbol << endl;
}

void disconnect() { app->disconnect(); }

uint128 atoi128(const char *s);

int main(int argc, char *argv[]) {
    if (argc == 1)
        cout << helpText;

    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0) {
            cout << helpText;
            return 0;
        }

        if (strcmp(argv[1], "connect") == 0) {
            connect();
            displayInfo();
            disconnect();
            return 0;
        }

        if (strcmp(argv[1], "balance") == 0) {
            if (argc < 3) {
                cout << helpText;
                return 0;
            }
            string address = argv[2];

            connect();

            // Load balance from Polkadot API
            bool done = false;
            app->subscribeBalance(address, [&](uint128 balance) {
                balance /= 1000000;
                long balLong = (long)balance;

                char balanceStr[1024];
                double balanceD = (double)balLong / 1000000000.;
                if (balanceD >= 1.)
                    sprintf(balanceStr, "%f DOT", balanceD);
                else
                    sprintf(balanceStr, "%f mDOT", balanceD * 1000);
                cout << endl << "Balance: " << balanceStr << endl << endl;
                done = true;
            });
            while (!done)
                usleep(100000);
            app->unsubscribeBalance(address);
            disconnect();

            return 0;
        }

        if (strcmp(argv[1], "transfer") == 0) {
            if (argc < 6) {
                cout << helpText;
                return 0;
            }
            string addressFrom = argv[2];
            string addressTo = argv[3];
            uint128 amount = atoi128(argv[4]);
            string privateFile = argv[5];

            std::ifstream t(privateFile);
            std::string senderPrivateKeyStr((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

            connect();

            // Transfer and wait for updates
            bool done = false;
            app->signAndSendTransfer(addressFrom, senderPrivateKeyStr, addressTo, amount, [&](string result) {
                if (result == "ready")
                    cout << endl
                         << endl
                         << "   ---=== Transaction was registered in network ===--- " << endl
                         << endl
                         << endl;
                if (result == "finalized") {
                    cout << endl << endl << "   ---=== Transaction was mined! ===--- " << endl << endl << endl;
                    done = true;
                }
            });

            // Wait until transaction is mined
            while (!done)
                usleep(10000);

            disconnect();

            return 0;
        }

        cout << "Unknown command: " << argv[1];
    }

    return 0;
}
