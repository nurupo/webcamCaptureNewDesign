#include <unique_id.h>

namespace webcam_capture {

UniqueId::UniqueId(BackendImplementation implementation) : implementation(implementation)
{
    //empty
}

UniqueId::~UniqueId()
{
    //empty
}

bool UniqueId::operator==(const UniqueId &other)
{
    return equals(other);
}

bool UniqueId::operator!=(const UniqueId &other)
{
    return !equals(other);
}

bool UniqueId::equals(const UniqueId &other) const
{
    return implementation == other.implementation;
}

}
