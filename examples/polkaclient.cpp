#include "../src/polkadot.h"

std::string helpText = 
" Available commands:\n"
" get [command]:\n"
"   blockhash [BlockHash]  - Get the block hash for a specific block\n"
"   runtimeversion         - Get run-time version\n"
"   metadata [BlockHash]   - Returns the runtime metadata\n"
"   properties             - Get a custom set of properties\n"
"   chainid                - Retrivies the chain\n"
"   chainname              - Retrivies the node name\n"
"   chainversion           - Retrivies the version of the node\n"
" balance\n"
" subscribe\n"
;

int main(int argc, char *argv[])
{
    if (argc == 1)
        cout << helpText;

    if (argc > 1)
    {
        if (strcmp(argv[1], "--help") == 0)
        {
            cout << helpText;
            return 0;
        }

        if (strcmp(argv[1], "get") == 0)
        {
            if (strcmp(argv[2], "blockhash") == 0)
            {
                cout << "blockhash called with parameter " << argv[3] << "\n";
                return 0;
            }

            if (strcmp(argv[2], "runtimeversion") == 0)
            {
                cout << "runtimeversion called\n";
                return 0;
            }

            if (strcmp(argv[2], "metadata") == 0)
            {
                cout << "metadata called with parameter " << argv[3] << "\n";
                return 0;
            }

            if (strcmp(argv[2], "properties") == 0)
            {
                cout << "properties called with parameter\n";
                return 0;
            }

            if (strcmp(argv[2], "chainid") == 0)
            {
                cout << "chainid called with parameter\n";
                return 0;
            }

            if (strcmp(argv[2], "chainname") == 0)
            {
                cout << "chainname called with parameter\n";
                return 0;
            }

            if (strcmp(argv[2], "chainversion") == 0)
            {
                cout << "chainversion called with parameter\n";
                return 0;
            }
        }

        if (strcmp(argv[1], "balance") == 0)
        {
            cout << "balance called\n";
            return 0;
        }

        if (strcmp(argv[1], "subscribe") == 0)
        {
            cout << "subscribe called\n";
            return 0;
        }     

        cout << "Unknown command: " << argv[1];
    }

    return 0;
}
