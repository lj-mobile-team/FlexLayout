/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

// Not defined in MSVC++
#ifndef NAN
static const unsigned long __nan[2] = {0xffffffff, 0x7fffffff};
#define NAN (*(const float *) __nan)
#endif

#define RNYGUndefined NAN

#include "RNYGEnums.h"
#include "RNYGMacros.h"

RNYG_EXTERN_C_BEGIN

typedef struct RNYGSize {
  float width;
  float height;
} RNYGSize;

typedef struct RNYGValue {
  float value;
  RNYGUnit unit;
} RNYGValue;

extern const RNYGValue RNYGValueUndefined;
extern const RNYGValue RNYGValueAuto;

typedef struct RNYGConfig *RNYGConfigRef;

typedef struct RNYGNode* RNYGNodeRef;

typedef RNYGSize (*RNYGMeasureFunc)(RNYGNodeRef node,
                                float width,
                                    RNYGMeasureMode widthMode,
                                float height,
                                    RNYGMeasureMode heightMode);
typedef float (*RNYGBaselineFunc)(RNYGNodeRef node, const float width, const float height);
typedef void (*RNYGDirtiedFunc)(RNYGNodeRef node);
typedef void (*RNYGPrintFunc)(RNYGNodeRef node);
typedef int (*RNYGLogger)(const RNYGConfigRef config,
                        const RNYGNodeRef node,
                          RNYGLogLevel level,
                        const char *format,
                        va_list args);
typedef void (*RNYGNodeClonedFunc)(RNYGNodeRef oldNode,
                                 RNYGNodeRef newNode,
                                 RNYGNodeRef parent,
                                 int childIndex);

// YGNode
WIN_EXPORT RNYGNodeRef RNYGNodeNew(void);
WIN_EXPORT RNYGNodeRef RNYGNodeNewWithConfig(const RNYGConfigRef config);
WIN_EXPORT RNYGNodeRef RNYGNodeClone(const RNYGNodeRef node);
WIN_EXPORT void RNYGNodeFree(const RNYGNodeRef node);
WIN_EXPORT void RNYGNodeFreeRecursive(const RNYGNodeRef node);
WIN_EXPORT void RNYGNodeReset(const RNYGNodeRef node);
WIN_EXPORT int32_t RNYGNodeGetInstanceCount(void);

WIN_EXPORT void RNYGNodeInsertChild(const RNYGNodeRef node,
                                  const RNYGNodeRef child,
                                  const uint32_t index);
WIN_EXPORT void RNYGNodeRemoveChild(const RNYGNodeRef node, const RNYGNodeRef child);
WIN_EXPORT void RNYGNodeRemoveAllChildren(const RNYGNodeRef node);
WIN_EXPORT RNYGNodeRef RNYGNodeGetChild(const RNYGNodeRef node, const uint32_t index);
WIN_EXPORT RNYGNodeRef RNYGNodeGetParent(const RNYGNodeRef node);
WIN_EXPORT uint32_t RNYGNodeGetChildCount(const RNYGNodeRef node);

WIN_EXPORT void RNYGNodeCalculateLayout(const RNYGNodeRef node,
                                      const float availableWidth,
                                      const float availableHeight,
                                      const RNYGDirection parentDirection);

// Mark a node as dirty. Only valid for nodes with a custom measure function
// set.
// YG knows when to mark all other nodes as dirty but because nodes with
// measure functions
// depends on information not known to YG they must perform this dirty
// marking manually.
WIN_EXPORT void RNYGNodeMarkDirty(const RNYGNodeRef node);

// This function marks the current node and all its descendants as dirty. This function is added to test yoga benchmarks.
// This function is not expected to be used in production as calling `YGCalculateLayout` will cause the recalculation of each and every node.
WIN_EXPORT void RNYGNodeMarkDirtyAndPropogateToDescendants(const RNYGNodeRef node);

WIN_EXPORT void RNYGNodePrint(const RNYGNodeRef node, const RNYGPrintOptions options);

WIN_EXPORT bool RNYGFloatIsUndefined(const float value);

WIN_EXPORT bool RNYGNodeCanUseCachedMeasurement(const RNYGMeasureMode widthMode,
                                              const float width,
                                              const RNYGMeasureMode heightMode,
                                              const float height,
                                              const RNYGMeasureMode lastWidthMode,
                                              const float lastWidth,
                                              const RNYGMeasureMode lastHeightMode,
                                              const float lastHeight,
                                              const float lastComputedWidth,
                                              const float lastComputedHeight,
                                              const float marginRow,
                                              const float marginColumn,
                                              const RNYGConfigRef config);

WIN_EXPORT void RNYGNodeCopyStyle(const RNYGNodeRef dstNode, const RNYGNodeRef srcNode);

#define RNYG_NODE_PROPERTY(type, name, paramName)                          \
  WIN_EXPORT void RNYGNodeSet##name(const RNYGNodeRef node, type paramName); \
  WIN_EXPORT type RNYGNodeGet##name(const RNYGNodeRef node);

#define RNYG_NODE_STYLE_PROPERTY(type, name, paramName)                               \
  WIN_EXPORT void RNYGNodeStyleSet##name(const RNYGNodeRef node, const type paramName); \
  WIN_EXPORT type RNYGNodeStyleGet##name(const RNYGNodeRef node);

#define RNYG_NODE_STYLE_PROPERTY_UNIT(type, name, paramName)                                    \
  WIN_EXPORT void RNYGNodeStyleSet##name(const RNYGNodeRef node, const float paramName);          \
  WIN_EXPORT void RNYGNodeStyleSet##name##Percent(const RNYGNodeRef node, const float paramName); \
  WIN_EXPORT type RNYGNodeStyleGet##name(const RNYGNodeRef node);

#define RNYG_NODE_STYLE_PROPERTY_UNIT_AUTO(type, name, paramName) \
  RNYG_NODE_STYLE_PROPERTY_UNIT(type, name, paramName)            \
  WIN_EXPORT void RNYGNodeStyleSet##name##Auto(const RNYGNodeRef node);

#define RNYG_NODE_STYLE_EDGE_PROPERTY(type, name, paramName)    \
  WIN_EXPORT void RNYGNodeStyleSet##name(const RNYGNodeRef node,  \
                                       const RNYGEdge edge,     \
                                       const type paramName); \
  WIN_EXPORT type RNYGNodeStyleGet##name(const RNYGNodeRef node, const RNYGEdge edge);

#define RNYG_NODE_STYLE_EDGE_PROPERTY_UNIT(type, name, paramName)         \
  WIN_EXPORT void RNYGNodeStyleSet##name(const RNYGNodeRef node,            \
                                       const RNYGEdge edge,               \
                                       const float paramName);          \
  WIN_EXPORT void RNYGNodeStyleSet##name##Percent(const RNYGNodeRef node,   \
                                                const RNYGEdge edge,      \
                                                const float paramName); \
  WIN_EXPORT WIN_STRUCT(type) RNYGNodeStyleGet##name(const RNYGNodeRef node, const RNYGEdge edge);

#define RNYG_NODE_STYLE_EDGE_PROPERTY_UNIT_AUTO(type, name) \
  WIN_EXPORT void RNYGNodeStyleSet##name##Auto(const RNYGNodeRef node, const RNYGEdge edge);

#define RNYG_NODE_LAYOUT_PROPERTY(type, name) \
  WIN_EXPORT type RNYGNodeLayoutGet##name(const RNYGNodeRef node);

#define RNYG_NODE_LAYOUT_EDGE_PROPERTY(type, name) \
  WIN_EXPORT type RNYGNodeLayoutGet##name(const RNYGNodeRef node, const RNYGEdge edge);

void* RNYGNodeGetContext(RNYGNodeRef node);
void RNYGNodeSetContext(RNYGNodeRef node, void* context);
RNYGMeasureFunc RNYGNodeGetMeasureFunc(RNYGNodeRef node);
void RNYGNodeSetMeasureFunc(RNYGNodeRef node, RNYGMeasureFunc measureFunc);
RNYGBaselineFunc RNYGNodeGetBaselineFunc(RNYGNodeRef node);
void RNYGNodeSetBaselineFunc(RNYGNodeRef node, RNYGBaselineFunc baselineFunc);
RNYGDirtiedFunc RNYGNodeGetDirtiedFunc(RNYGNodeRef node);
void RNYGNodeSetDirtiedFunc(RNYGNodeRef node, RNYGDirtiedFunc dirtiedFunc);
RNYGPrintFunc RNYGNodeGetPrintFunc(RNYGNodeRef node);
void RNYGNodeSetPrintFunc(RNYGNodeRef node, RNYGPrintFunc printFunc);
bool RNYGNodeGetHasNewLayout(RNYGNodeRef node);
void RNYGNodeSetHasNewLayout(RNYGNodeRef node, bool hasNewLayout);
RNYGNodeType RNYGNodeGetNodeType(RNYGNodeRef node);
void RNYGNodeSetNodeType(RNYGNodeRef node, RNYGNodeType nodeType);
bool RNYGNodeIsDirty(RNYGNodeRef node);
bool RNYGNodeLayoutGetDidUseLegacyFlag(const RNYGNodeRef node);

RNYG_NODE_STYLE_PROPERTY(RNYGDirection, Direction, direction);
RNYG_NODE_STYLE_PROPERTY(RNYGFlexDirection, FlexDirection, flexDirection);
RNYG_NODE_STYLE_PROPERTY(RNYGJustify, JustifyContent, justifyContent);
RNYG_NODE_STYLE_PROPERTY(RNYGAlign, AlignContent, alignContent);
RNYG_NODE_STYLE_PROPERTY(RNYGAlign, AlignItems, alignItems);
RNYG_NODE_STYLE_PROPERTY(RNYGAlign, AlignSelf, alignSelf);
RNYG_NODE_STYLE_PROPERTY(RNYGPositionType, PositionType, positionType);
RNYG_NODE_STYLE_PROPERTY(RNYGWrap, FlexWrap, flexWrap);
RNYG_NODE_STYLE_PROPERTY(RNYGOverflow, Overflow, overflow);
RNYG_NODE_STYLE_PROPERTY(RNYGDisplay, Display, display);

RNYG_NODE_STYLE_PROPERTY(float, Flex, flex);
RNYG_NODE_STYLE_PROPERTY(float, FlexGrow, flexGrow);
RNYG_NODE_STYLE_PROPERTY(float, FlexShrink, flexShrink);
RNYG_NODE_STYLE_PROPERTY_UNIT_AUTO(RNYGValue, FlexBasis, flexBasis);

RNYG_NODE_STYLE_EDGE_PROPERTY_UNIT(RNYGValue, Position, position);
RNYG_NODE_STYLE_EDGE_PROPERTY_UNIT(RNYGValue, Margin, margin);
RNYG_NODE_STYLE_EDGE_PROPERTY_UNIT_AUTO(RNYGValue, Margin);
RNYG_NODE_STYLE_EDGE_PROPERTY_UNIT(RNYGValue, Padding, padding);
RNYG_NODE_STYLE_EDGE_PROPERTY(float, Border, border);

RNYG_NODE_STYLE_PROPERTY_UNIT_AUTO(RNYGValue, Width, width);
RNYG_NODE_STYLE_PROPERTY_UNIT_AUTO(RNYGValue, Height, height);
RNYG_NODE_STYLE_PROPERTY_UNIT(RNYGValue, MinWidth, minWidth);
RNYG_NODE_STYLE_PROPERTY_UNIT(RNYGValue, MinHeight, minHeight);
RNYG_NODE_STYLE_PROPERTY_UNIT(RNYGValue, MaxWidth, maxWidth);
RNYG_NODE_STYLE_PROPERTY_UNIT(RNYGValue, MaxHeight, maxHeight);

// Yoga specific properties, not compatible with flexbox specification
// Aspect ratio control the size of the undefined dimension of a node.
// Aspect ratio is encoded as a floating point value width/height. e.g. A value of 2 leads to a node
// with a width twice the size of its height while a value of 0.5 gives the opposite effect.
//
// - On a node with a set width/height aspect ratio control the size of the unset dimension
// - On a node with a set flex basis aspect ratio controls the size of the node in the cross axis if
// unset
// - On a node with a measure function aspect ratio works as though the measure function measures
// the flex basis
// - On a node with flex grow/shrink aspect ratio controls the size of the node in the cross axis if
// unset
// - Aspect ratio takes min/max dimensions into account
RNYG_NODE_STYLE_PROPERTY(float, AspectRatio, aspectRatio);

RNYG_NODE_LAYOUT_PROPERTY(float, Left);
RNYG_NODE_LAYOUT_PROPERTY(float, Top);
RNYG_NODE_LAYOUT_PROPERTY(float, Right);
RNYG_NODE_LAYOUT_PROPERTY(float, Bottom);
RNYG_NODE_LAYOUT_PROPERTY(float, Width);
RNYG_NODE_LAYOUT_PROPERTY(float, Height);
RNYG_NODE_LAYOUT_PROPERTY(RNYGDirection, Direction);
RNYG_NODE_LAYOUT_PROPERTY(bool, HadOverflow);

// Get the computed values for these nodes after performing layout. If they were set using
// point values then the returned value will be the same as YGNodeStyleGetXXX. However if
// they were set using a percentage value then the returned value is the computed value used
// during layout.
RNYG_NODE_LAYOUT_EDGE_PROPERTY(float, Margin);
RNYG_NODE_LAYOUT_EDGE_PROPERTY(float, Border);
RNYG_NODE_LAYOUT_EDGE_PROPERTY(float, Padding);

WIN_EXPORT void RNYGConfigSetLogger(const RNYGConfigRef config, RNYGLogger logger);
WIN_EXPORT void RNYGLog(const RNYGNodeRef node, RNYGLogLevel level, const char *message, ...);
WIN_EXPORT void RNYGLogWithConfig(const RNYGConfigRef config, RNYGLogLevel level, const char *format, ...);
WIN_EXPORT void RNYGAssert(const bool condition, const char *message);
WIN_EXPORT void RNYGAssertWithNode(const RNYGNodeRef node, const bool condition, const char *message);
WIN_EXPORT void RNYGAssertWithConfig(const RNYGConfigRef config,
                                   const bool condition,
                                   const char *message);

// Set this to number of pixels in 1 point to round calculation results
// If you want to avoid rounding - set PointScaleFactor to 0
WIN_EXPORT void RNYGConfigSetPointScaleFactor(const RNYGConfigRef config, const float pixelsInPoint);

// Yoga previously had an error where containers would take the maximum space possible instead of
// the minimum
// like they are supposed to. In practice this resulted in implicit behaviour similar to align-self:
// stretch;
// Because this was such a long-standing bug we must allow legacy users to switch back to this
// behaviour.
WIN_EXPORT void RNYGConfigSetUseLegacyStretchBehaviour(const RNYGConfigRef config,
                                                     const bool useLegacyStretchBehaviour);

// YGConfig
WIN_EXPORT RNYGConfigRef RNYGConfigNew(void);
WIN_EXPORT void RNYGConfigFree(const RNYGConfigRef config);
WIN_EXPORT void RNYGConfigCopy(const RNYGConfigRef dest, const RNYGConfigRef src);
WIN_EXPORT int32_t RNYGConfigGetInstanceCount(void);

WIN_EXPORT void RNYGConfigSetExperimentalFeatureEnabled(const RNYGConfigRef config,
                                                      const RNYGExperimentalFeature feature,
                                                      const bool enabled);
WIN_EXPORT bool RNYGConfigIsExperimentalFeatureEnabled(const RNYGConfigRef config,
                                                     const RNYGExperimentalFeature feature);

// Using the web defaults is the prefered configuration for new projects.
// Usage of non web defaults should be considered as legacy.
WIN_EXPORT void RNYGConfigSetUseWebDefaults(const RNYGConfigRef config, const bool enabled);
WIN_EXPORT bool RNYGConfigGetUseWebDefaults(const RNYGConfigRef config);

WIN_EXPORT void RNYGConfigSetNodeClonedFunc(const RNYGConfigRef config,
                                          const RNYGNodeClonedFunc callback);

// Export only for C#
WIN_EXPORT RNYGConfigRef RNYGConfigGetDefault(void);

WIN_EXPORT void RNYGConfigSetContext(const RNYGConfigRef config, void *context);
WIN_EXPORT void *RNYGConfigGetContext(const RNYGConfigRef config);

WIN_EXPORT float RNYGRoundValueToPixelGrid(
    const float value,
    const float pointScaleFactor,
    const bool forceCeil,
    const bool forceFloor);

RNYG_EXTERN_C_END
