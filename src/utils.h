#ifndef UTILS_H
#define UTILS_H

#ifdef WEBCAM_CAPTURE_DEBUG
#include <iostream>
#define DEBUG_PRINT(x) std::cerr << x << std::endl << std::flush
#else
#define DEBUG_PRINT(x)
#endif

#define FPS_FROM_RATIONAL(x, y) (static_cast<float>(x)/(y))
#define FPS_EQUAL(x, y) ((((x) + 0.008) > (y)) && (((x) - 0.008) < (y)))

#endif // UTILS_H
