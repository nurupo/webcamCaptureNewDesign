#ifndef VIDEO_PROPERTY_RANGE_H
#define VIDEO_PROPERTY_RANGE_H

namespace webcam_capture {
    /**
     * Provides range information of a property.
     */
    class VideoPropertyRange {
        public:
            VideoPropertyRange():
                minValue(0),
                maxValue(0),
                stepValue(0),
                defaultValue(0) {}

           /**
            * @param min Minimum video property value.
            * @param max Maximum video property value.
            * @param step Step size.
            * @param defaultValue Default video property value.
            */
           VideoPropertyRange (int min, int max, int step, int defaultValue) :
               minValue(min),
               maxValue(max),
               stepValue(step),
               defaultValue(defaultValue) {}

           ~VideoPropertyRange() {}

           /**
            * @return Minimum video property value.
            */
           int getMinValue() const { return minValue; }
           /**
            * @return Maximum video property value.
            */
           int getMaxValue() const { return maxValue; }
           /**
            * @return Step size value.
            */
           int getStepValue() const { return stepValue; }
           /**
            * @return Default video property value.
            */
           int getDefaultValue() const { return defaultValue; }

           /**
            * @param minVal Set minimum video property value.
            */
           void setMinValue(const int minVal) { minValue = minVal; }

           /**
            * @param maxVal Set maximum video property value.
            */
           void setMaxValue(const int maxVal) { maxValue = maxVal; }

           /**
            * @param stepVal Set step size value.
            */
           void setStepValue(const int stepVal) { stepValue = stepVal; }

           /**
            * @param defaultVal Set defuaut video property value.
            */
           void setDefaultValue(const int defaultVal) { defaultValue = defaultVal; }

        private:
           int minValue;
           int maxValue;
           int stepValue;
           int defaultValue;
    }; //FIXME(nurupo): make immutable.

} // namespace webcam_capture

#endif // VIDEO_PROPERTY_RANGE_H

