#include "winapi_shared_unique_id.h"

namespace webcam_capture {

WinapiShared_UniqueId::WinapiShared_UniqueId(const WCHAR *uniqueId, BackendImplementation implementation)
    :   UniqueId(implementation),
        uniqueId(uniqueId)
{
}

WinapiShared_UniqueId::~WinapiShared_UniqueId()
{
}

bool WinapiShared_UniqueId::equals(const UniqueId &other) const
{
    // "other" must be a UniqueId of the same backend implementation in order to proceed
    if (!UniqueId::equals(other)) {
        return false;
    }

    //need to compare string without this "{...}" value.
    const WinapiShared_UniqueId &otherUniqueId = static_cast<const WinapiShared_UniqueId &>(other);
    size_t thisPos = uniqueId.find_first_of(L'{');
    size_t otherPos = otherUniqueId.uniqueId.find_first_of(L'{');

    if (thisPos != otherPos || thisPos == std::string::npos) {
        return false;
    }

    return uniqueId.compare(0, thisPos, otherUniqueId.uniqueId, 0, otherPos) == 0;
}

const std::wstring &WinapiShared_UniqueId::getId() const
{
    return uniqueId;
}

}// namespace webcam_capture

