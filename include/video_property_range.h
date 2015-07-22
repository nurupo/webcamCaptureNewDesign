#ifndef VIDEO_PROPERTY_RANGE_H
#define VIDEO_PROPERTY_RANGE_H

#include <webcam_capture_export.h>

namespace webcam_capture {
/**
 * Provides range information of a video property.
 */
class WEBCAM_CAPTURE_EXPORT VideoPropertyRange
{
public:
    VideoPropertyRange():
        minimum(0),
        maximum(0),
        stepSize(0),
        defaultValue(0) {}

    /**
     * @param min Minimum video property value.
     * @param max Maximum video property value.
     * @param step Step size.
     * @param defaultValue Default video property value.
     */
    VideoPropertyRange(int min, int max, int step, int defaultValue) :
        minimum(min),
        maximum(max),
        stepSize(step),
        defaultValue(defaultValue) {}

    /**
     * @return Minimum video property value.
     */
    int getMinimum() const
    {
        return minimum;
    }

    /**
     * @return Maximum video property value.
     */
    int getMaximum() const
    {
        return maximum;
    }

    /**
     * @return Step size.
     */
    int getStepSize() const
    {
        return stepSize;
    }

    /**
     * @return Default video property value.
     */
    int getDefault() const
    {
        return defaultValue;
    }

private:
    int minimum;
    int maximum;
    int stepSize;
    int defaultValue;
};

} // namespace webcam_capture

#endif // VIDEO_PROPERTY_RANGE_H

