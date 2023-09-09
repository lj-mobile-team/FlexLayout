/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <vector>
#include "RNYoga.h"

using RNYGVector = std::vector<RNYGNodeRef>;

RNYG_EXTERN_C_BEGIN

WIN_EXPORT float RNYGRoundValueToPixelGrid(const float value,
                                         const float pointScaleFactor,
                                         const bool forceCeil,
                                         const bool forceFloor);

RNYG_EXTERN_C_END

extern const std::array<RNYGEdge, 4> trailing;
extern const std::array<RNYGEdge, 4> leading;
extern bool RNYGValueEqual(const RNYGValue a, const RNYGValue b);
extern const RNYGValue RNYGValueUndefined;
extern const RNYGValue RNYGValueAuto;
extern const RNYGValue RNYGValueZero;

template <std::size_t size>
bool RNYGValueArrayEqual(
    const std::array<RNYGValue, size> val1,
    const std::array<RNYGValue, size> val2) {
  bool areEqual = true;
  for (uint32_t i = 0; i < size && areEqual; ++i) {
    areEqual = RNYGValueEqual(val1[i], val2[i]);
  }
  return areEqual;
}

struct RNYGCachedMeasurement {
  float availableWidth;
  float availableHeight;
    RNYGMeasureMode widthMeasureMode;
    RNYGMeasureMode heightMeasureMode;

  float computedWidth;
  float computedHeight;

    RNYGCachedMeasurement()
      : availableWidth(0),
        availableHeight(0),
        widthMeasureMode((RNYGMeasureMode)-1),
        heightMeasureMode((RNYGMeasureMode)-1),
        computedWidth(-1),
        computedHeight(-1) {}

  bool operator==(RNYGCachedMeasurement measurement) const {
    bool isEqual = widthMeasureMode == measurement.widthMeasureMode &&
        heightMeasureMode == measurement.heightMeasureMode;

    if (!std::isnan(availableWidth) ||
        !std::isnan(measurement.availableWidth)) {
      isEqual = isEqual && availableWidth == measurement.availableWidth;
    }
    if (!std::isnan(availableHeight) ||
        !std::isnan(measurement.availableHeight)) {
      isEqual = isEqual && availableHeight == measurement.availableHeight;
    }
    if (!std::isnan(computedWidth) || !std::isnan(measurement.computedWidth)) {
      isEqual = isEqual && computedWidth == measurement.computedWidth;
    }
    if (!std::isnan(computedHeight) ||
        !std::isnan(measurement.computedHeight)) {
      isEqual = isEqual && computedHeight == measurement.computedHeight;
    }

    return isEqual;
  }
};

// This value was chosen based on empiracle data. Even the most complicated
// layouts should not require more than 16 entries to fit within the cache.
#define RNYG_MAX_CACHED_RESULT_COUNT 16

struct RNYGConfig {
  bool experimentalFeatures[RNYGExperimentalFeatureCount + 1];
  bool useWebDefaults;
  bool useLegacyStretchBehaviour;
  bool shouldDiffLayoutWithoutLegacyStretchBehaviour;
  float pointScaleFactor;
    RNYGLogger logger;
    RNYGNodeClonedFunc cloneNodeCallback;
  void* context;
};

static const float kDefaultFlexGrow = 0.0f;
static const float kDefaultFlexShrink = 0.0f;
static const float kWebDefaultFlexShrink = 1.0f;

extern bool RNYGFloatsEqual(const float a, const float b);
extern bool RNYGValueEqual(const RNYGValue a, const RNYGValue b);
extern const RNYGValue* RNYGComputedEdgeValue(
    const std::array<RNYGValue, RNYGEdgeCount>& edges,
    const RNYGEdge edge,
    const RNYGValue* const defaultValue);
