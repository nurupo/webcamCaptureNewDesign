#ifndef WINAPI_SHARED_UNIQUE_ID_H
#define WINAPI_SHARED_UNIQUE_ID_H

#include <unique_id.h>

#include <string>

#include <windows.h>

namespace webcam_capture {

class WinapiShared_UniqueId : public UniqueId
{
public:
    WinapiShared_UniqueId(const WCHAR *uniqueId, BackendImplementation implementation);
    ~WinapiShared_UniqueId();

    const std::wstring &getId() const;

protected:
    bool equals(const UniqueId &other) const override;

private:    
    std::wstring uniqueId;
};

} // namespace webcam_capture

#endif // WINAPI_SHARED_UNIQUE_ID_H
