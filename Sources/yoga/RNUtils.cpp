/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "RNUtils.h"

RNYGFlexDirection RNYGFlexDirectionCross(
    const RNYGFlexDirection flexDirection,
    const RNYGDirection direction) {
  return RNYGFlexDirectionIsColumn(flexDirection)
      ? RNYGResolveFlexDirection(RNYGFlexDirectionRow, direction)
      : RNYGFlexDirectionColumn;
}

bool RNYGValueEqual(const RNYGValue a, const RNYGValue b) {
  if (a.unit != b.unit) {
    return false;
  }

  if (a.unit == RNYGUnitUndefined ||
      (std::isnan(a.value) && std::isnan(b.value))) {
    return true;
  }

  return fabs(a.value - b.value) < 0.0001f;
}
