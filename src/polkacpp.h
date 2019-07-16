// clang-format off
#include <functional>
#include <memory>

using namespace std;

// Include structs
#include "structs/responsebase.h"
#include "structs/systeminfo.h"
#include "structs/blockhash.h"
#include "structs/metadataV0.h"
#include "structs/metadataV5.h"
#include "structs/metadata.h"
#include "structs/runtimeversion.h"
#include "structs/common.h"

// Include application layer interface
#include "interfaces/iapplication.h"
// clang-format on

namespace polkadot {

class api {
    class impl;
    unique_ptr<impl> pImpl;
    api();
    static shared_ptr<api> _instance;

public:
    ~api();
    static shared_ptr<api> getInstance();
    IApplication *app();
};

} // namespace polkadot
