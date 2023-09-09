/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once
#include "RNYGNode.h"
#include "Yoga-internal.h"

// This struct is an helper model to hold the data for step 4 of flexbox
// algo, which is collecting the flex items in a line.
//
// - itemsOnLine: Number of items which can fit in a line considering the
// available Inner dimension, the flex items computed flexbasis and their
// margin. It may be different than the difference between start and end
// indicates because we skip over absolute-positioned items.
//
// - sizeConsumedOnCurrentLine: It is accumulation of the dimensions and margin
// of all the children on the current line. This will be used in order to either
// set the dimensions of the node if none already exist or to compute the
// remaining space left for the flexible children.
//
// - totalFlexGrowFactors: total flex grow factors of flex items which are to be
// layed in the current line
//
// - totalFlexShrinkFactors: total flex shrink factors of flex items which are
// to be layed in the current line
//
// - endOfLineIndex: Its the end index of the last flex item which was examined
// and it may or may not be part of the current line(as it may be absolutely
// positioned or inculding it may have caused to overshoot availableInnerDim)
//
// - relativeChildren: Maintain a vector of the child nodes that can shrink
// and/or grow.

struct RNYGCollectFlexItemsRowValues {
  uint32_t itemsOnLine;
  float sizeConsumedOnCurrentLine;
  float totalFlexGrowFactors;
  float totalFlexShrinkScaledFactors;
  float endOfLineIndex;
  std::vector<RNYGNodeRef> relativeChildren;
  float remainingFreeSpace;
  // The size of the mainDim for the row after considering size, padding, margin
  // and border of flex items. This is used to calculate maxLineDim after going
  // through all the rows to decide on the main axis size of parent.
  float mainDim;
  // The size of the crossDim for the row after considering size, padding,
  // margin and border of flex items. Used for calculating containers crossSize.
  float crossDim;
};

bool RNYGValueEqual(const RNYGValue a, const RNYGValue b);

RNYGFlexDirection RNYGFlexDirectionCross(
    const RNYGFlexDirection flexDirection,
    const RNYGDirection direction);

inline bool RNYGFlexDirectionIsRow(const RNYGFlexDirection flexDirection) {
  return flexDirection == RNYGFlexDirectionRow ||
      flexDirection == RNYGFlexDirectionRowReverse;
}

inline float RNYGResolveValue(const RNYGValue value, const float parentSize) {
  switch (value.unit) {
    case RNYGUnitUndefined:
    case RNYGUnitAuto:
      return RNYGUndefined;
    case RNYGUnitPoint:
      return value.value;
    case RNYGUnitPercent:
      return value.value * parentSize / 100.0f;
  }
  return RNYGUndefined;
}

inline bool RNYGFlexDirectionIsColumn(const RNYGFlexDirection flexDirection) {
  return flexDirection == RNYGFlexDirectionColumn ||
      flexDirection == RNYGFlexDirectionColumnReverse;
}

inline RNYGFlexDirection RNYGResolveFlexDirection(
    const RNYGFlexDirection flexDirection,
    const RNYGDirection direction) {
  if (direction == RNYGDirectionRTL) {
    if (flexDirection == RNYGFlexDirectionRow) {
      return RNYGFlexDirectionRowReverse;
    } else if (flexDirection == RNYGFlexDirectionRowReverse) {
      return RNYGFlexDirectionRow;
    }
  }

  return flexDirection;
}

static inline float RNYGResolveValueMargin(
    const RNYGValue value,
    const float parentSize) {
  return value.unit == RNYGUnitAuto ? 0 : RNYGResolveValue(value, parentSize);
}
