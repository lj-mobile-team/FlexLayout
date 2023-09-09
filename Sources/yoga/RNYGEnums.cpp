/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "RNYGEnums.h"

const char *RNYGAlignToString(const RNYGAlign value){
  switch(value){
    case RNYGAlignAuto:
      return "auto";
    case RNYGAlignFlexStart:
      return "flex-start";
    case RNYGAlignCenter:
      return "center";
    case RNYGAlignFlexEnd:
      return "flex-end";
    case RNYGAlignStretch:
      return "stretch";
    case RNYGAlignBaseline:
      return "baseline";
    case RNYGAlignSpaceBetween:
      return "space-between";
    case RNYGAlignSpaceAround:
      return "space-around";
  }
  return "unknown";
}

const char *RNYGDimensionToString(const RNYGDimension value){
  switch(value){
    case RNYGDimensionWidth:
      return "width";
    case RNYGDimensionHeight:
      return "height";
  }
  return "unknown";
}

const char *RNYGDirectionToString(const RNYGDirection value){
  switch(value){
    case RNYGDirectionInherit:
      return "inherit";
    case RNYGDirectionLTR:
      return "ltr";
    case RNYGDirectionRTL:
      return "rtl";
  }
  return "unknown";
}

const char *RNYGDisplayToString(const RNYGDisplay value){
  switch(value){
    case RNYGDisplayFlex:
      return "flex";
    case RNYGDisplayNone:
      return "none";
  }
  return "unknown";
}

const char *RNYGEdgeToString(const RNYGEdge value){
  switch(value){
    case RNYGEdgeLeft:
      return "left";
    case RNYGEdgeTop:
      return "top";
    case RNYGEdgeRight:
      return "right";
    case RNYGEdgeBottom:
      return "bottom";
    case RNYGEdgeStart:
      return "start";
    case RNYGEdgeEnd:
      return "end";
    case RNYGEdgeHorizontal:
      return "horizontal";
    case RNYGEdgeVertical:
      return "vertical";
    case RNYGEdgeAll:
      return "all";
  }
  return "unknown";
}

const char *RNYGExperimentalFeatureToString(const RNYGExperimentalFeature value){
  switch(value){
    case RNYGExperimentalFeatureWebFlexBasis:
      return "web-flex-basis";
      default: return "unknown";
  }
  return "unknown";
}

const char *RNYGFlexDirectionToString(const RNYGFlexDirection value){
  switch(value){
    case RNYGFlexDirectionColumn:
      return "column";
    case RNYGFlexDirectionColumnReverse:
      return "column-reverse";
    case RNYGFlexDirectionRow:
      return "row";
    case RNYGFlexDirectionRowReverse:
      return "row-reverse";
  }
  return "unknown";
}

const char *RNYGJustifyToString(const RNYGJustify value){
  switch(value){
    case RNYGJustifyFlexStart:
      return "flex-start";
    case RNYGJustifyCenter:
      return "center";
    case RNYGJustifyFlexEnd:
      return "flex-end";
    case RNYGJustifySpaceBetween:
      return "space-between";
    case RNYGJustifySpaceAround:
      return "space-around";
    case RNYGJustifySpaceEvenly:
      return "space-evenly";
  }
  return "unknown";
}

const char *RNYGLogLevelToString(const RNYGLogLevel value){
  switch(value){
    case RNYGLogLevelError:
      return "error";
    case RNYGLogLevelWarn:
      return "warn";
    case RNYGLogLevelInfo:
      return "info";
    case RNYGLogLevelDebug:
      return "debug";
    case RNYGLogLevelVerbose:
      return "verbose";
    case RNYGLogLevelFatal:
      return "fatal";
  }
  return "unknown";
}

const char *RNYGMeasureModeToString(const RNYGMeasureMode value){
  switch(value){
    case RNYGMeasureModeUndefined:
      return "undefined";
    case RNYGMeasureModeExactly:
      return "exactly";
    case RNYGMeasureModeAtMost:
      return "at-most";
  }
  return "unknown";
}

const char *RNYGNodeTypeToString(const RNYGNodeType value){
  switch(value){
    case RNYGNodeTypeDefault:
      return "default";
    case RNYGNodeTypeText:
      return "text";
  }
  return "unknown";
}

const char *RNYGOverflowToString(const RNYGOverflow value){
  switch(value){
    case RNYGOverflowVisible:
      return "visible";
    case RNYGOverflowHidden:
      return "hidden";
    case RNYGOverflowScroll:
      return "scroll";
  }
  return "unknown";
}

const char *RNYGPositionTypeToString(const RNYGPositionType value){
  switch(value){
    case RNYGPositionTypeRelative:
      return "relative";
    case RNYGPositionTypeAbsolute:
      return "absolute";
    default: return "unknown";
  }
  return "unknown";
}

const char *RNYGPrintOptionsToString(const RNYGPrintOptions value){
  switch(value){
    case RNYGPrintOptionsLayout:
      return "layout";
    case RNYGPrintOptionsStyle:
      return "style";
    case RNYGPrintOptionsChildren:
      return "children";
  }
  return "unknown";
}

const char *RNYGUnitToString(const RNYGUnit value){
  switch(value){
    case RNYGUnitUndefined:
      return "undefined";
    case RNYGUnitPoint:
      return "point";
    case RNYGUnitPercent:
      return "percent";
    case RNYGUnitAuto:
      return "auto";
  }
  return "unknown";
}

const char *RNYGWrapToString(const RNYGWrap value){
  switch(value){
    case RNYGWrapNoWrap:
      return "no-wrap";
    case RNYGWrapWrap:
      return "wrap";
    case RNYGWrapWrapReverse:
      return "wrap-reverse";
  }
  return "unknown";
}
