/**
 * Copyright (c) 2014-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */
#include "RNYGStyle.h"

const RNYGValue kYGValueUndefined = {RNYGUndefined, RNYGUnitUndefined};

const RNYGValue kYGValueAuto = {RNYGUndefined, RNYGUnitAuto};

const std::array<RNYGValue, RNYGEdgeCount> kYGDefaultEdgeValuesUnit = {
    {kYGValueUndefined,
     kYGValueUndefined,
     kYGValueUndefined,
     kYGValueUndefined,
     kYGValueUndefined,
     kYGValueUndefined,
     kYGValueUndefined,
     kYGValueUndefined,
     kYGValueUndefined}};

const std::array<RNYGValue, 2> kYGDefaultDimensionValuesAutoUnit = {
    {kYGValueAuto, kYGValueAuto}};

const std::array<RNYGValue, 2> kYGDefaultDimensionValuesUnit = {
    {kYGValueUndefined, kYGValueUndefined}};

RNYGStyle::RNYGStyle()
    : direction(RNYGDirectionInherit),
      flexDirection(RNYGFlexDirectionColumn),
      justifyContent(RNYGJustifyFlexStart),
      alignContent(RNYGAlignFlexStart),
      alignItems(RNYGAlignStretch),
      alignSelf(RNYGAlignAuto),
      positionType(RNYGPositionTypeRelative),
      flexWrap(RNYGWrapNoWrap),
      overflow(RNYGOverflowVisible),
      display(RNYGDisplayFlex),
      flex(RNYGUndefined),
      flexGrow(RNYGUndefined),
      flexShrink(RNYGUndefined),
      flexBasis(kYGValueAuto),
      margin(kYGDefaultEdgeValuesUnit),
      position(kYGDefaultEdgeValuesUnit),
      padding(kYGDefaultEdgeValuesUnit),
      border(kYGDefaultEdgeValuesUnit),
      dimensions(kYGDefaultDimensionValuesAutoUnit),
      minDimensions(kYGDefaultDimensionValuesUnit),
      maxDimensions(kYGDefaultDimensionValuesUnit),
      aspectRatio(RNYGUndefined) {}

// Yoga specific properties, not compatible with flexbox specification
bool RNYGStyle::operator==(const RNYGStyle& style) {
  bool areNonFloatValuesEqual = direction == style.direction &&
      flexDirection == style.flexDirection &&
      justifyContent == style.justifyContent &&
      alignContent == style.alignContent && alignItems == style.alignItems &&
      alignSelf == style.alignSelf && positionType == style.positionType &&
      flexWrap == style.flexWrap && overflow == style.overflow &&
      display == style.display && RNYGValueEqual(flexBasis, style.flexBasis) &&
    RNYGValueArrayEqual(margin, style.margin) &&
    RNYGValueArrayEqual(position, style.position) &&
    RNYGValueArrayEqual(padding, style.padding) &&
    RNYGValueArrayEqual(border, style.border) &&
    RNYGValueArrayEqual(dimensions, style.dimensions) &&
    RNYGValueArrayEqual(minDimensions, style.minDimensions) &&
    RNYGValueArrayEqual(maxDimensions, style.maxDimensions);

  if (!(std::isnan(flex) && std::isnan(style.flex))) {
    areNonFloatValuesEqual = areNonFloatValuesEqual && flex == style.flex;
  }

  if (!(std::isnan(flexGrow) && std::isnan(style.flexGrow))) {
    areNonFloatValuesEqual =
        areNonFloatValuesEqual && flexGrow == style.flexGrow;
  }

  if (!(std::isnan(flexShrink) && std::isnan(style.flexShrink))) {
    areNonFloatValuesEqual =
        areNonFloatValuesEqual && flexShrink == style.flexShrink;
  }

  if (!(std::isnan(aspectRatio) && std::isnan(style.aspectRatio))) {
    areNonFloatValuesEqual =
        areNonFloatValuesEqual && aspectRatio == style.aspectRatio;
  }

  return areNonFloatValuesEqual;
}

bool RNYGStyle::operator!=(RNYGStyle style) {
  return !(*this == style);
}

RNYGStyle::~RNYGStyle() {}
