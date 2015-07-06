/**
    This header is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */

#ifndef CAMERA_INFORMATION_H
#define CAMERA_INFORMATION_H
#include <string.h>
#include <unique_id.h>

namespace webcam_capture {

    /**
     * Brief description of a camera.
     */
    class CameraInformation {
    public:
       /**
       * @param cameraId Camera Id
       * @param cameraName Camera Name
       */
      CameraInformation(UniqueId * uniqueId, std::string cameraName) : uniqueId(uniqueId), cameraName (cameraName) {}
      ~CameraInformation() {}


      /**
       * @return Unique camera id identifying a camera.
       */
      UniqueId* getUniqueId() const { return uniqueId; }

      /**
       * @return User-friendly camera name in system-specific string encoding.
       */
      std::string getCameraName() const { return cameraName; }

    private:
      std::string cameraName;
      UniqueId * uniqueId;
    }; // FIXME(nurupo): we souldn't use pointer to UniqueId but rather UniqueId itself.
} // namespace webcam_capture

#endif // CAMERA_INFORMATION_H

