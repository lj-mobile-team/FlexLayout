/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "RNYGMacros.h"

RNYG_EXTERN_C_BEGIN

#define RNYGAlignCount 8
typedef RNYG_ENUM_BEGIN(RNYGAlign) {
    RNYGAlignAuto,
    RNYGAlignFlexStart,
    RNYGAlignCenter,
    RNYGAlignFlexEnd,
    RNYGAlignStretch,
    RNYGAlignBaseline,
    RNYGAlignSpaceBetween,
    RNYGAlignSpaceAround,
} RNYG_ENUM_END(RNYGAlign);
WIN_EXPORT const char *RNYGAlignToString(const RNYGAlign value);

#define RNYGDimensionCount 2
typedef RNYG_ENUM_BEGIN(RNYGDimension) {
    RNYGDimensionWidth,
    RNYGDimensionHeight,
} RNYG_ENUM_END(RNYGDimension);
WIN_EXPORT const char *RNYGDimensionToString(const RNYGDimension value);

#define RNYGDirectionCount 3
typedef RNYG_ENUM_BEGIN(RNYGDirection) {
    RNYGDirectionInherit,
    RNYGDirectionLTR,
    RNYGDirectionRTL,
} RNYG_ENUM_END(RNYGDirection);
WIN_EXPORT const char *RNYGDirectionToString(const RNYGDirection value);

#define RNYGDisplayCount 2
typedef RNYG_ENUM_BEGIN(RNYGDisplay) {
    RNYGDisplayFlex,
    RNYGDisplayNone,
} RNYG_ENUM_END(RNYGDisplay);
WIN_EXPORT const char *RNYGDisplayToString(const RNYGDisplay value);

#define RNYGEdgeCount 9
typedef RNYG_ENUM_BEGIN(RNYGEdge) {
    RNYGEdgeLeft,
    RNYGEdgeTop,
    RNYGEdgeRight,
    RNYGEdgeBottom,
    RNYGEdgeStart,
    RNYGEdgeEnd,
    RNYGEdgeHorizontal,
    RNYGEdgeVertical,
    RNYGEdgeAll,
} RNYG_ENUM_END(RNYGEdge);
WIN_EXPORT const char *RNYGEdgeToString(const RNYGEdge value);

#define RNYGExperimentalFeatureCount 1
typedef RNYG_ENUM_BEGIN(RNYGExperimentalFeature) {
    RNYGExperimentalFeatureWebFlexBasis,
} RNYG_ENUM_END(RNYGExperimentalFeature);
WIN_EXPORT const char *RNYGExperimentalFeatureToString(const RNYGExperimentalFeature value);

#define RNYGFlexDirectionCount 4
typedef RNYG_ENUM_BEGIN(RNYGFlexDirection) {
    RNYGFlexDirectionColumn,
    RNYGFlexDirectionColumnReverse,
    RNYGFlexDirectionRow,
    RNYGFlexDirectionRowReverse,
} RNYG_ENUM_END(RNYGFlexDirection);
WIN_EXPORT const char *RNYGFlexDirectionToString(const RNYGFlexDirection value);

#define RNYGJustifyCount 6
typedef RNYG_ENUM_BEGIN(RNYGJustify){
    RNYGJustifyFlexStart,
    RNYGJustifyCenter,
    RNYGJustifyFlexEnd,
    RNYGJustifySpaceBetween,
    RNYGJustifySpaceAround,
    RNYGJustifySpaceEvenly,
} RNYG_ENUM_END(RNYGJustify);
WIN_EXPORT const char *RNYGJustifyToString(const RNYGJustify value);

#define RNYGLogLevelCount 6
typedef RNYG_ENUM_BEGIN(RNYGLogLevel) {
    RNYGLogLevelError,
    RNYGLogLevelWarn,
    RNYGLogLevelInfo,
    RNYGLogLevelDebug,
    RNYGLogLevelVerbose,
    RNYGLogLevelFatal,
} RNYG_ENUM_END(RNYGLogLevel);
WIN_EXPORT const char *RNYGLogLevelToString(const RNYGLogLevel value);

#define RNYGMeasureModeCount 3
typedef RNYG_ENUM_BEGIN(RNYGMeasureMode) {
    RNYGMeasureModeUndefined,
    RNYGMeasureModeExactly,
    RNYGMeasureModeAtMost,
} RNYG_ENUM_END(RNYGMeasureMode);
WIN_EXPORT const char *RNYGMeasureModeToString(const RNYGMeasureMode value);

#define RNYGNodeTypeCount 2
typedef RNYG_ENUM_BEGIN(RNYGNodeType) {
    RNYGNodeTypeDefault,
    RNYGNodeTypeText,
} RNYG_ENUM_END(RNYGNodeType);
WIN_EXPORT const char *RNYGNodeTypeToString(const RNYGNodeType value);

#define RNYGOverflowCount 3
typedef RNYG_ENUM_BEGIN(RNYGOverflow) {
    RNYGOverflowVisible,
    RNYGOverflowHidden,
    RNYGOverflowScroll,
} RNYG_ENUM_END(RNYGOverflow);
WIN_EXPORT const char *RNYGOverflowToString(const RNYGOverflow value);

#define RNYGPositionTypeCount 2
typedef RNYG_ENUM_BEGIN(RNYGPositionType) {
    RNYGPositionTypeRelative,
    RNYGPositionTypeAbsolute,
} RNYG_ENUM_END(RNYGPositionType);
WIN_EXPORT const char *RNYGPositionTypeToString(const RNYGPositionType value);

#define RNYGPrintOptionsCount 3
typedef RNYG_ENUM_BEGIN(RNYGPrintOptions) {
    RNYGPrintOptionsLayout = 1,
    RNYGPrintOptionsStyle = 2,
    RNYGPrintOptionsChildren = 4,
} RNYG_ENUM_END(RNYGPrintOptions);
WIN_EXPORT const char *RNYGPrintOptionsToString(const RNYGPrintOptions value);

#define RNYGUnitCount 4
typedef RNYG_ENUM_BEGIN(RNYGUnit) {
    RNYGUnitUndefined,
    RNYGUnitPoint,
    RNYGUnitPercent,
    RNYGUnitAuto,
} RNYG_ENUM_END(RNYGUnit);
WIN_EXPORT const char *RNYGUnitToString(const RNYGUnit value);

#define RNYGWrapCount 3
typedef RNYG_ENUM_BEGIN(RNYGWrap) {
    RNYGWrapNoWrap,
    RNYGWrapWrap,
    RNYGWrapWrapReverse,
} RNYG_ENUM_END(RNYGWrap);
WIN_EXPORT const char *RNYGWrapToString(const RNYGWrap value);

RNYG_EXTERN_C_END
