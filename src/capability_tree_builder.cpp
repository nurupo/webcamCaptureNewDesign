#include "capability_tree_builder.h"

#include "utils.h"

namespace webcam_capture {

const size_t CapabilityTreeBuilder::FORMAT_MAP_INITIAL_BUCKET_COUNT = 5;
const size_t CapabilityTreeBuilder::RESOLUTION_MAP_INITIAL_BUCKET_COUNT = 7;
const size_t CapabilityTreeBuilder::FPS_MAP_INITIAL_BUCKET_COUNT = 4;

CapabilityTreeBuilder::CapabilityTreeBuilder() :
    formatMap(FORMAT_MAP_INITIAL_BUCKET_COUNT, formatHash)
{
}

size_t CapabilityTreeBuilder::resolutionHash(const std::pair<int, int> &p)
{
    return std::hash<int>()(p.first + p.second);
}

size_t CapabilityTreeBuilder::formatHash(const PixelFormat &f)
{
    return static_cast<std::size_t>(f);
}

bool CapabilityTreeBuilder::resolutionEquals(const std::pair<int, int> &p, const std::pair<int, int> &q)
{
    return p.first == q.first && p.second == q.second;
}

bool CapabilityTreeBuilder::fpsEquals(const float &p, const float &q)
{
    return FPS_EQUAL(p, q);
}

void CapabilityTreeBuilder::addCapability(PixelFormat pixelFormat, int width, int height, std::vector<float> fps)
{
    // because we want our maps to use custom hash/equals functions,
    // we have to manually check if they already exist and if not -- construct them with the right functions

    // construct resolution map if not exist
    auto formatMapItem = formatMap.find(pixelFormat);

    if (formatMapItem == formatMap.end()) {
        // no resolution map for such format found, explicitly insert it.
        formatMap[pixelFormat] = ResolutionMap(RESOLUTION_MAP_INITIAL_BUCKET_COUNT, resolutionHash, resolutionEquals);
    }

    ResolutionMap &resolutionMap = formatMap[pixelFormat];

    std::pair<int, int> resolution(width, height);

    // construct fps map if not exist
    auto resolutionMapItem = resolutionMap.find(resolution);

    if (resolutionMapItem == resolutionMap.end()) {
        // no fps map for such resolution found, explicitly insert it.
        resolutionMap[resolution] = FpsMap(3, std::hash<float>(), fpsEquals);
    }

    FpsMap &fpsMap = resolutionMap[resolution];

    for (auto && f : fps) {
        fpsMap[f] = true;
    }
}

std::vector<CapabilityFormat> CapabilityTreeBuilder::build() const
{
    std::vector<CapabilityFormat> capFormat;
    capFormat.reserve(formatMap.size());

    for (auto formatMapItem : formatMap) {
        const PixelFormat &format               = formatMapItem.first;
        const ResolutionMap &resolutionMap = formatMapItem.second;

        std::vector<CapabilityResolution> capResolutions;
        capResolutions.reserve(resolutionMap.size());

        for (auto resolutionMapItem : resolutionMap) {
            const std::pair<int, int> &resolution = resolutionMapItem.first;
            const FpsMap &fpsMap                  = resolutionMapItem.second;

            std::vector<CapabilityFps> capFps;
            capFps.reserve(fpsMap.size());

            for (auto fpsMapItem : fpsMap) {
                capFps.push_back(CapabilityFps(fpsMapItem.first));
            }

            capResolutions.push_back(CapabilityResolution(resolution.first, resolution.second, std::move(capFps)));
        }

        capFormat.push_back(CapabilityFormat(format, std::move(capResolutions)));
    }

    return capFormat;
}

} // namespace webcam_capture
