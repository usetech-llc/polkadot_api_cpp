// clang-format off
#include <functional>
#include <memory>

using namespace std;

#include "libs/int128/int128.h"
typedef absl::uint128 uint128;

#define SR25519_PUBLIC_SIZE 32
#define SR25519_SECRET_SIZE 64
#define SR25519_SIGNATURE_SIZE 64
#define RESPONSE_QUEUE_MAX_SIZE 128

#include "scale.h"
#include "utils/concurrentmapqueue.h"

// Include structs
#include "structs/responsebase.h"
#include "structs/systeminfo.h"
#include "structs/blockhash.h"
#include "structs/metadataV0.h"
#include "structs/metadataV5.h"
#include "structs/metadataV6.h"
#include "structs/metadataV4.h"
#include "structs/metadataV7.h"
#include "structs/metadata.h"
#include "structs/runtimeversion.h"
#include "structs/common.h"
#include "structs/signedblock.h"
#include "structs/storageItem.h"
#include "structs/signaturepayload.h"
#include "structs/extrinsic.h"

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
