/**
 * Copyright (c) 2014-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once
#include "RNYoga-internal.h"
#include "RNYoga.h"

struct RNYGStyle {
    RNYGDirection direction;
    RNYGFlexDirection flexDirection;
    RNYGJustify justifyContent;
    RNYGAlign alignContent;
    RNYGAlign alignItems;
    RNYGAlign alignSelf;
    RNYGPositionType positionType;
    RNYGWrap flexWrap;
    RNYGOverflow overflow;
    RNYGDisplay display;
  float flex;
  float flexGrow;
  float flexShrink;
    RNYGValue flexBasis;
  std::array<RNYGValue, RNYGEdgeCount> margin;
  std::array<RNYGValue, RNYGEdgeCount> position;
  std::array<RNYGValue, RNYGEdgeCount> padding;
  std::array<RNYGValue, RNYGEdgeCount> border;
  std::array<RNYGValue, 2> dimensions;
  std::array<RNYGValue, 2> minDimensions;
  std::array<RNYGValue, 2> maxDimensions;
  float aspectRatio;

    RNYGStyle();
  // Yoga specific properties, not compatible with flexbox specification
  bool operator==(const RNYGStyle& style);

  bool operator!=(RNYGStyle style);
  ~RNYGStyle();
};
