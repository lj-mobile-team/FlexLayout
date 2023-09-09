/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "RNYoga.h"
#include <string.h>
#include <algorithm>
#include "RNUtils.h"
#include "RNYGNode.h"
#include "RNYGNodePrint.h"
#include "RNYoga-internal.h"

#ifdef _MSC_VER
#include <float.h>

/* define fmaxf if < VC12 */
#if _MSC_VER < 1800
__forceinline const float fmaxf(const float a, const float b) {
  return (a > b) ? a : b;
}
#endif
#endif

#ifdef ANDROID
static int RNYGAndroidLog(const RNYGConfigRef config,
                        const RNYGNodeRef node,
                          RNYGLogLevel level,
                        const char *format,
                        va_list args);
#else
static int RNYGDefaultLog(const RNYGConfigRef config,
                        const RNYGNodeRef node,
                          RNYGLogLevel level,
                        const char *format,
                        va_list args);
#endif

static RNYGConfig gYGConfigDefaults = {
    .experimentalFeatures =
        {
            [RNYGExperimentalFeatureWebFlexBasis] = false,
        },
    .useWebDefaults = false,
    .useLegacyStretchBehaviour = false,
    .shouldDiffLayoutWithoutLegacyStretchBehaviour = false,
    .pointScaleFactor = 1.0f,
#ifdef ANDROID
    .logger = &YGAndroidLog,
#else
    .logger = &RNYGDefaultLog,
#endif
    .cloneNodeCallback = nullptr,
    .context = nullptr,
};

const RNYGValue RNYGValueZero = {.value = 0, .unit = RNYGUnitPoint};
const RNYGValue RNYGValueUndefined = {RNYGUndefined, RNYGUnitUndefined};
const RNYGValue RNYGValueAuto = {RNYGUndefined, RNYGUnitAuto};

#ifdef ANDROID
#include <android/log.h>
static int RNYGAndroidLog(const RNYGConfigRef config,
                        const RNYGNodeRef node,
                          RNYGLogLevel level,
                        const char *format,
                        va_list args) {
  int androidLevel = RNYGLogLevelDebug;
  switch (level) {
    case RNYGLogLevelFatal:
      androidLevel = ANDROID_LOG_FATAL;
      break;
    case RNYGLogLevelError:
      androidLevel = ANDROID_LOG_ERROR;
      break;
    case RNYGLogLevelWarn:
      androidLevel = ANDROID_LOG_WARN;
      break;
    case RNYGLogLevelInfo:
      androidLevel = ANDROID_LOG_INFO;
      break;
    case RNYGLogLevelDebug:
      androidLevel = ANDROID_LOG_DEBUG;
      break;
    case RNYGLogLevelVerbose:
      androidLevel = ANDROID_LOG_VERBOSE;
      break;
  }
  const int result = __android_log_vprint(androidLevel, "yoga", format, args);
  return result;
}
#else
#define RNYG_UNUSED(x) (void)(x);

static int RNYGDefaultLog(const RNYGConfigRef config,
                        const RNYGNodeRef node,
                          RNYGLogLevel level,
                        const char *format,
                        va_list args) {
    RNYG_UNUSED(config);
    RNYG_UNUSED(node);
  switch (level) {
    case RNYGLogLevelError:
    case RNYGLogLevelFatal:
      return vfprintf(stderr, format, args);
    case RNYGLogLevelWarn:
    case RNYGLogLevelInfo:
    case RNYGLogLevelDebug:
    case RNYGLogLevelVerbose:
    default:
      return vprintf(format, args);
  }
}

#undef RNYG_UNUSED
#endif

bool RNYGFloatIsUndefined(const float value) {
  return std::isnan(value);
}

const RNYGValue* RNYGComputedEdgeValue(
    const std::array<RNYGValue, RNYGEdgeCount>& edges,
    const RNYGEdge edge,
    const RNYGValue* const defaultValue) {
  if (edges[edge].unit != RNYGUnitUndefined) {
    return &edges[edge];
  }

  if ((edge == RNYGEdgeTop || edge == RNYGEdgeBottom) &&
      edges[RNYGEdgeVertical].unit != RNYGUnitUndefined) {
    return &edges[RNYGEdgeVertical];
  }

  if ((edge == RNYGEdgeLeft || edge == RNYGEdgeRight || edge == RNYGEdgeStart || edge == RNYGEdgeEnd) &&
      edges[RNYGEdgeHorizontal].unit != RNYGUnitUndefined) {
    return &edges[RNYGEdgeHorizontal];
  }

  if (edges[RNYGEdgeAll].unit != RNYGUnitUndefined) {
    return &edges[RNYGEdgeAll];
  }

  if (edge == RNYGEdgeStart || edge == RNYGEdgeEnd) {
    return &RNYGValueUndefined;
  }

  return defaultValue;
}

void* RNYGNodeGetContext(RNYGNodeRef node) {
  return node->getContext();
}

void RNYGNodeSetContext(RNYGNodeRef node, void* context) {
  return node->setContext(context);
}

RNYGMeasureFunc RNYGNodeGetMeasureFunc(RNYGNodeRef node) {
  return node->getMeasure();
}

void RNYGNodeSetMeasureFunc(RNYGNodeRef node, RNYGMeasureFunc measureFunc) {
  node->setMeasureFunc(measureFunc);
}

RNYGBaselineFunc RNYGNodeGetBaselineFunc(RNYGNodeRef node) {
  return node->getBaseline();
}

void RNYGNodeSetBaselineFunc(RNYGNodeRef node, RNYGBaselineFunc baselineFunc) {
  node->setBaseLineFunc(baselineFunc);
}

RNYGDirtiedFunc RNYGNodeGetDirtiedFunc(RNYGNodeRef node) {
  return node->getDirtied();
}

void RNYGNodeSetDirtiedFunc(RNYGNodeRef node, RNYGDirtiedFunc dirtiedFunc) {
  node->setDirtiedFunc(dirtiedFunc);
}

RNYGPrintFunc RNYGNodeGetPrintFunc(RNYGNodeRef node) {
  return node->getPrintFunc();
}

void RNYGNodeSetPrintFunc(RNYGNodeRef node, RNYGPrintFunc printFunc) {
  node->setPrintFunc(printFunc);
}

bool RNYGNodeGetHasNewLayout(RNYGNodeRef node) {
  return node->getHasNewLayout();
}

void RNYGNodeSetHasNewLayout(RNYGNodeRef node, bool hasNewLayout) {
  node->setHasNewLayout(hasNewLayout);
}

RNYGNodeType RNYGNodeGetNodeType(RNYGNodeRef node) {
  return node->getNodeType();
}

void RNYGNodeSetNodeType(RNYGNodeRef node, RNYGNodeType nodeType) {
  return node->setNodeType(nodeType);
}

bool RNYGNodeIsDirty(RNYGNodeRef node) {
  return node->isDirty();
}

bool RNYGNodeLayoutGetDidUseLegacyFlag(const RNYGNodeRef node) {
  return node->didUseLegacyFlag();
}

void RNYGNodeMarkDirtyAndPropogateToDescendants(const RNYGNodeRef node) {
  return node->markDirtyAndPropogateDownwards();
}

int32_t gNodeInstanceCount = 0;
int32_t gConfigInstanceCount = 0;

WIN_EXPORT RNYGNodeRef RNYGNodeNewWithConfig(const RNYGConfigRef config) {
  const RNYGNodeRef node = new RNYGNode();
    RNYGAssertWithConfig(
      config, node != nullptr, "Could not allocate memory for node");
  gNodeInstanceCount++;

  if (config->useWebDefaults) {
    node->setStyleFlexDirection(RNYGFlexDirectionRow);
    node->setStyleAlignContent(RNYGAlignStretch);
  }
  node->setConfig(config);
  return node;
}

RNYGNodeRef RNYGNodeNew(void) {
  return RNYGNodeNewWithConfig(&gYGConfigDefaults);
}

RNYGNodeRef RNYGNodeClone(RNYGNodeRef oldNode) {
    RNYGNodeRef node = new RNYGNode(*oldNode);
    RNYGAssertWithConfig(
      oldNode->getConfig(),
      node != nullptr,
      "Could not allocate memory for node");
  gNodeInstanceCount++;
  node->setParent(nullptr);
  return node;
}

static RNYGConfigRef RNYGConfigClone(const RNYGConfig& oldConfig) {
  const RNYGConfigRef config = new RNYGConfig(oldConfig);
    RNYGAssert(config != nullptr, "Could not allocate memory for config");
  if (config == nullptr) {
    abort();
  }
  gConfigInstanceCount++;
  return config;
}

static RNYGNodeRef RNYGNodeDeepClone(RNYGNodeRef oldNode) {
    RNYGNodeRef node = RNYGNodeClone(oldNode);
    RNYGVector vec = RNYGVector();
  vec.reserve(oldNode->getChildren().size());
    RNYGNodeRef childNode = nullptr;
  for (auto& item : oldNode->getChildren()) {
    childNode = RNYGNodeDeepClone(item);
    childNode->setParent(node);
    vec.push_back(childNode);
  }
  node->setChildren(vec);

  if (oldNode->getConfig() != nullptr) {
    node->setConfig(RNYGConfigClone(*(oldNode->getConfig())));
  }

  if (oldNode->getNextChild() != nullptr) {
    node->setNextChild(RNYGNodeDeepClone(oldNode->getNextChild()));
  }

  return node;
}

void RNYGNodeFree(const RNYGNodeRef node) {
  if (node->getParent()) {
    node->getParent()->removeChild(node);
    node->setParent(nullptr);
  }

  const uint32_t childCount = RNYGNodeGetChildCount(node);
  for (uint32_t i = 0; i < childCount; i++) {
    const RNYGNodeRef child = RNYGNodeGetChild(node, i);
    child->setParent(nullptr);
  }

  node->clearChildren();
  delete node;
  gNodeInstanceCount--;
}

static void RNYGConfigFreeRecursive(const RNYGNodeRef root) {
  if (root->getConfig() != nullptr) {
    gConfigInstanceCount--;
    delete root->getConfig();
  }
  // Delete configs recursively for childrens
  for (uint32_t i = 0; i < root->getChildrenCount(); ++i) {
      RNYGConfigFreeRecursive(root->getChild(i));
  }
}

void RNYGNodeFreeRecursive(const RNYGNodeRef root) {
  while (RNYGNodeGetChildCount(root) > 0) {
    const RNYGNodeRef child = RNYGNodeGetChild(root, 0);
    if (child->getParent() != root) {
      // Don't free shared nodes that we don't own.
      break;
    }
      RNYGNodeRemoveChild(root, child);
      RNYGNodeFreeRecursive(child);
  }
    RNYGNodeFree(root);
}

void RNYGNodeReset(const RNYGNodeRef node) {
    RNYGAssertWithNode(node,
                       RNYGNodeGetChildCount(node) == 0,
                   "Cannot reset a node which still has children attached");
    RNYGAssertWithNode(
      node,
      node->getParent() == nullptr,
      "Cannot reset a node still attached to a parent");

  node->clearChildren();

  const RNYGConfigRef config = node->getConfig();
  *node = RNYGNode();
  if (config->useWebDefaults) {
    node->setStyleFlexDirection(RNYGFlexDirectionRow);
    node->setStyleAlignContent(RNYGAlignStretch);
  }
  node->setConfig(config);
}

int32_t RNYGNodeGetInstanceCount(void) {
  return gNodeInstanceCount;
}

int32_t RNYGConfigGetInstanceCount(void) {
  return gConfigInstanceCount;
}

// Export only for C#
RNYGConfigRef RNYGConfigGetDefault() {
  return &gYGConfigDefaults;
}

RNYGConfigRef RNYGConfigNew(void) {
  const RNYGConfigRef config = (const RNYGConfigRef)malloc(sizeof(RNYGConfig));
    RNYGAssert(config != nullptr, "Could not allocate memory for config");
  if (config == nullptr) {
    abort();
  }
  gConfigInstanceCount++;
  memcpy(config, &gYGConfigDefaults, sizeof(RNYGConfig));
  return config;
}

void RNYGConfigFree(const RNYGConfigRef config) {
  free(config);
  gConfigInstanceCount--;
}

void RNYGConfigCopy(const RNYGConfigRef dest, const RNYGConfigRef src) {
  memcpy(dest, src, sizeof(RNYGConfig));
}

void RNYGNodeInsertChild(const RNYGNodeRef node, const RNYGNodeRef child, const uint32_t index) {
    RNYGAssertWithNode(
      node,
      child->getParent() == nullptr,
      "Child already has a parent, it must be removed first.");
    RNYGAssertWithNode(
      node,
      node->getMeasure() == nullptr,
      "Cannot add child: Nodes with measure functions cannot have children.");

  node->cloneChildrenIfNeeded();
  node->insertChild(child, index);
  child->setParent(node);
  node->markDirtyAndPropogate();
}

void RNYGNodeRemoveChild(const RNYGNodeRef parent, const RNYGNodeRef excludedChild) {
  // This algorithm is a forked variant from cloneChildrenIfNeeded in YGNode
  // that excludes a child.
  const uint32_t childCount = RNYGNodeGetChildCount(parent);

  if (childCount == 0) {
    // This is an empty set. Nothing to remove.
    return;
  }
  const RNYGNodeRef firstChild = RNYGNodeGetChild(parent, 0);
  if (firstChild->getParent() == parent) {
    // If the first child has this node as its parent, we assume that it is already unique.
    // We can now try to delete a child in this list.
    if (parent->removeChild(excludedChild)) {
      excludedChild->setLayout(
                               RNYGNode().getLayout()); // layout is no longer valid
      excludedChild->setParent(nullptr);
      parent->markDirtyAndPropogate();
    }
    return;
  }
  // Otherwise we have to clone the node list except for the child we're trying to delete.
  // We don't want to simply clone all children, because then the host will need to free
  // the clone of the child that was just deleted.
  const RNYGNodeClonedFunc cloneNodeCallback =
      parent->getConfig()->cloneNodeCallback;
  uint32_t nextInsertIndex = 0;
  for (uint32_t i = 0; i < childCount; i++) {
    const RNYGNodeRef oldChild = parent->getChild(i);
    if (excludedChild == oldChild) {
      // Ignore the deleted child. Don't reset its layout or parent since it is still valid
      // in the other parent. However, since this parent has now changed, we need to mark it
      // as dirty.
      parent->markDirtyAndPropogate();
      continue;
    }
    const RNYGNodeRef newChild = RNYGNodeClone(oldChild);
    parent->replaceChild(newChild, nextInsertIndex);
    newChild->setParent(parent);
    if (cloneNodeCallback) {
      cloneNodeCallback(oldChild, newChild, parent, nextInsertIndex);
    }
    nextInsertIndex++;
  }
  while (nextInsertIndex < childCount) {
    parent->removeChild(nextInsertIndex);
    nextInsertIndex++;
  }
}

void RNYGNodeRemoveAllChildren(const RNYGNodeRef parent) {
  const uint32_t childCount = RNYGNodeGetChildCount(parent);
  if (childCount == 0) {
    // This is an empty set already. Nothing to do.
    return;
  }
  const RNYGNodeRef firstChild = RNYGNodeGetChild(parent, 0);
  if (firstChild->getParent() == parent) {
    // If the first child has this node as its parent, we assume that this child set is unique.
    for (uint32_t i = 0; i < childCount; i++) {
      const RNYGNodeRef oldChild = RNYGNodeGetChild(parent, i);
      oldChild->setLayout(RNYGNode().getLayout()); // layout is no longer valid
      oldChild->setParent(nullptr);
    }
    parent->clearChildren();
    parent->markDirtyAndPropogate();
    return;
  }
  // Otherwise, we are not the owner of the child set. We don't have to do anything to clear it.
  parent->setChildren(RNYGVector());
  parent->markDirtyAndPropogate();
}

RNYGNodeRef RNYGNodeGetChild(const RNYGNodeRef node, const uint32_t index) {
  if (index < node->getChildren().size()) {
    return node->getChild(index);
  }
  return nullptr;
}

uint32_t RNYGNodeGetChildCount(const RNYGNodeRef node) {
  return static_cast<uint32_t>(node->getChildren().size());
}

RNYGNodeRef RNYGNodeGetParent(const RNYGNodeRef node) {
  return node->getParent();
}

void RNYGNodeMarkDirty(const RNYGNodeRef node) {
    RNYGAssertWithNode(
      node,
      node->getMeasure() != nullptr,
      "Only leaf nodes with custom measure functions"
      "should manually mark themselves as dirty");

  node->markDirtyAndPropogate();
}

void RNYGNodeCopyStyle(const RNYGNodeRef dstNode, const RNYGNodeRef srcNode) {
  if (!(dstNode->getStyle() == srcNode->getStyle())) {
    dstNode->setStyle(srcNode->getStyle());
    dstNode->markDirtyAndPropogate();
  }
}

float RNYGNodeStyleGetFlexGrow(const RNYGNodeRef node) {
  return RNYGFloatIsUndefined(node->getStyle().flexGrow)
      ? kDefaultFlexGrow
      : node->getStyle().flexGrow;
}

float RNYGNodeStyleGetFlexShrink(const RNYGNodeRef node) {
  return RNYGFloatIsUndefined(node->getStyle().flexShrink)
      ? (node->getConfig()->useWebDefaults ? kWebDefaultFlexShrink
                                           : kDefaultFlexShrink)
      : node->getStyle().flexShrink;
}

#define RNYG_NODE_STYLE_PROPERTY_SETTER_IMPL(                               \
    type, name, paramName, instanceName)                                  \
  void RNYGNodeStyleSet##name(const RNYGNodeRef node, const type paramName) { \
    if (node->getStyle().instanceName != paramName) {                     \
RNYGStyle style = node->getStyle();                                   \
      style.instanceName = paramName;                                     \
      node->setStyle(style);                                              \
      node->markDirtyAndPropogate();                                      \
    }                                                                     \
  }

#define RNYG_NODE_STYLE_PROPERTY_SETTER_UNIT_IMPL(                               \
    type, name, paramName, instanceName)                                       \
  void RNYGNodeStyleSet##name(const RNYGNodeRef node, const type paramName) {      \
RNYGValue value = {                                                          \
        .value = paramName,                                                    \
        .unit = RNYGFloatIsUndefined(paramName) ? RNYGUnitUndefined : RNYGUnitPoint, \
    };                                                                         \
    if ((node->getStyle().instanceName.value != value.value &&                 \
         value.unit != RNYGUnitUndefined) ||                                     \
        node->getStyle().instanceName.unit != value.unit) {                    \
RNYGStyle style = node->getStyle();                                        \
      style.instanceName = value;                                              \
      node->setStyle(style);                                                   \
      node->markDirtyAndPropogate();                                           \
    }                                                                          \
  }                                                                            \
                                                                               \
  void RNYGNodeStyleSet##name##Percent(                                          \
      const RNYGNodeRef node, const type paramName) {                            \
RNYGValue value = {                                                          \
        .value = paramName,                                                    \
        .unit =                                                                \
RNYGFloatIsUndefined(paramName) ? RNYGUnitUndefined : RNYGUnitPercent,   \
    };                                                                         \
    if ((node->getStyle().instanceName.value != value.value &&                 \
         value.unit != RNYGUnitUndefined) ||                                     \
        node->getStyle().instanceName.unit != value.unit) {                    \
RNYGStyle style = node->getStyle();                                        \
                                                                               \
      style.instanceName = value;                                              \
      node->setStyle(style);                                                   \
      node->markDirtyAndPropogate();                                           \
    }                                                                          \
  }

#define RNYG_NODE_STYLE_PROPERTY_SETTER_UNIT_AUTO_IMPL(                          \
    type, name, paramName, instanceName)                                       \
  void RNYGNodeStyleSet##name(const RNYGNodeRef node, const type paramName) {      \
RNYGValue value = {                                                          \
        .value = paramName,                                                    \
        .unit = RNYGFloatIsUndefined(paramName) ? RNYGUnitUndefined : RNYGUnitPoint, \
    };                                                                         \
    if ((node->getStyle().instanceName.value != value.value &&                 \
         value.unit != RNYGUnitUndefined) ||                                     \
        node->getStyle().instanceName.unit != value.unit) {                    \
RNYGStyle style = node->getStyle();                                        \
      style.instanceName = value;                                              \
      node->setStyle(style);                                                   \
      node->markDirtyAndPropogate();                                           \
    }                                                                          \
  }                                                                            \
                                                                               \
  void RNYGNodeStyleSet##name##Percent(                                          \
      const RNYGNodeRef node, const type paramName) {                            \
    if (node->getStyle().instanceName.value != paramName ||                    \
        node->getStyle().instanceName.unit != RNYGUnitPercent) {                 \
RNYGStyle style = node->getStyle();                                        \
      style.instanceName.value = paramName;                                    \
      style.instanceName.unit =                                                \
RNYGFloatIsUndefined(paramName) ? RNYGUnitAuto : RNYGUnitPercent;          \
      node->setStyle(style);                                                   \
      node->markDirtyAndPropogate();                                           \
    }                                                                          \
  }                                                                            \
                                                                               \
  void RNYGNodeStyleSet##name##Auto(const RNYGNodeRef node) {                      \
    if (node->getStyle().instanceName.unit != RNYGUnitAuto) {                    \
RNYGStyle style = node->getStyle();                                        \
      style.instanceName.value = RNYGUndefined;                                  \
      style.instanceName.unit = RNYGUnitAuto;                                    \
      node->setStyle(style);                                                   \
      node->markDirtyAndPropogate();                                           \
    }                                                                          \
  }

#define RNYG_NODE_STYLE_PROPERTY_IMPL(type, name, paramName, instanceName)  \
RNYG_NODE_STYLE_PROPERTY_SETTER_IMPL(type, name, paramName, instanceName) \
                                                                          \
  type RNYGNodeStyleGet##name(const RNYGNodeRef node) {                       \
    return node->getStyle().instanceName;                                 \
  }

#define RNYG_NODE_STYLE_PROPERTY_UNIT_IMPL(type, name, paramName, instanceName) \
RNYG_NODE_STYLE_PROPERTY_SETTER_UNIT_IMPL(                                    \
      float, name, paramName, instanceName)                                   \
                                                                              \
  type RNYGNodeStyleGet##name(const RNYGNodeRef node) {                           \
    return node->getStyle().instanceName;                                     \
  }

#define RNYG_NODE_STYLE_PROPERTY_UNIT_AUTO_IMPL(      \
    type, name, paramName, instanceName)            \
RNYG_NODE_STYLE_PROPERTY_SETTER_UNIT_AUTO_IMPL(     \
      float, name, paramName, instanceName)         \
                                                    \
  type RNYGNodeStyleGet##name(const RNYGNodeRef node) { \
    return node->getStyle().instanceName;           \
  }

#define RNYG_NODE_STYLE_EDGE_PROPERTY_UNIT_AUTO_IMPL(type, name, instanceName) \
  void RNYGNodeStyleSet##name##Auto(const RNYGNodeRef node, const RNYGEdge edge) { \
    if (node->getStyle().instanceName[edge].unit != RNYGUnitAuto) {            \
RNYGStyle style = node->getStyle();                                      \
      style.instanceName[edge].value = RNYGUndefined;                          \
      style.instanceName[edge].unit = RNYGUnitAuto;                            \
      node->setStyle(style);                                                 \
      node->markDirtyAndPropogate();                                         \
    }                                                                        \
  }

#define RNYG_NODE_STYLE_EDGE_PROPERTY_UNIT_IMPL(                                 \
    type, name, paramName, instanceName)                                       \
  void RNYGNodeStyleSet##name(                                                   \
      const RNYGNodeRef node, const RNYGEdge edge, const float paramName) {        \
RNYGValue value = {                                                          \
        .value = paramName,                                                    \
        .unit = RNYGFloatIsUndefined(paramName) ? RNYGUnitUndefined : RNYGUnitPoint, \
    };                                                                         \
    if ((node->getStyle().instanceName[edge].value != value.value &&           \
         value.unit != RNYGUnitUndefined) ||                                     \
        node->getStyle().instanceName[edge].unit != value.unit) {              \
RNYGStyle style = node->getStyle();                                        \
      style.instanceName[edge] = value;                                        \
      node->setStyle(style);                                                   \
      node->markDirtyAndPropogate();                                           \
    }                                                                          \
  }                                                                            \
                                                                               \
  void RNYGNodeStyleSet##name##Percent(                                          \
      const RNYGNodeRef node, const RNYGEdge edge, const float paramName) {        \
RNYGValue value = {                                                          \
        .value = paramName,                                                    \
        .unit =                                                                \
RNYGFloatIsUndefined(paramName) ? RNYGUnitUndefined : RNYGUnitPercent,   \
    };                                                                         \
    if ((node->getStyle().instanceName[edge].value != value.value &&           \
         value.unit != RNYGUnitUndefined) ||                                     \
        node->getStyle().instanceName[edge].unit != value.unit) {              \
RNYGStyle style = node->getStyle();                                        \
      style.instanceName[edge] = value;                                        \
      node->setStyle(style);                                                   \
      node->markDirtyAndPropogate();                                           \
    }                                                                          \
  }                                                                            \
                                                                               \
  WIN_STRUCT(type)                                                             \
RNYGNodeStyleGet##name(const RNYGNodeRef node, const RNYGEdge edge) {              \
    return WIN_STRUCT_REF(node->getStyle().instanceName[edge]);                \
  }

#define RNYG_NODE_STYLE_EDGE_PROPERTY_IMPL(type, name, paramName, instanceName)  \
  void RNYGNodeStyleSet##name(                                                   \
      const RNYGNodeRef node, const RNYGEdge edge, const float paramName) {        \
RNYGValue value = {                                                          \
        .value = paramName,                                                    \
        .unit = RNYGFloatIsUndefined(paramName) ? RNYGUnitUndefined : RNYGUnitPoint, \
    };                                                                         \
    if ((node->getStyle().instanceName[edge].value != value.value &&           \
         value.unit != RNYGUnitUndefined) ||                                     \
        node->getStyle().instanceName[edge].unit != value.unit) {              \
RNYGStyle style = node->getStyle();                                        \
      style.instanceName[edge] = value;                                        \
      node->setStyle(style);                                                   \
      node->markDirtyAndPropogate();                                           \
    }                                                                          \
  }                                                                            \
                                                                               \
  float RNYGNodeStyleGet##name(const RNYGNodeRef node, const RNYGEdge edge) {        \
    return node->getStyle().instanceName[edge].value;                          \
  }

#define RNYG_NODE_LAYOUT_PROPERTY_IMPL(type, name, instanceName) \
  type RNYGNodeLayoutGet##name(const RNYGNodeRef node) {           \
    return node->getLayout().instanceName;                     \
  }

#define RNYG_NODE_LAYOUT_RESOLVED_PROPERTY_IMPL(type, name, instanceName) \
  type RNYGNodeLayoutGet##name(const RNYGNodeRef node, const RNYGEdge edge) { \
RNYGAssertWithNode(                                                   \
        node,                                                           \
        edge <= RNYGEdgeEnd,                                              \
        "Cannot get layout properties of multi-edge shorthands");       \
                                                                        \
    if (edge == RNYGEdgeLeft) {                                           \
      if (node->getLayout().direction == RNYGDirectionRTL) {              \
        return node->getLayout().instanceName[RNYGEdgeEnd];               \
      } else {                                                          \
        return node->getLayout().instanceName[RNYGEdgeStart];             \
      }                                                                 \
    }                                                                   \
                                                                        \
    if (edge == RNYGEdgeRight) {                                          \
      if (node->getLayout().direction == RNYGDirectionRTL) {              \
        return node->getLayout().instanceName[RNYGEdgeStart];             \
      } else {                                                          \
        return node->getLayout().instanceName[RNYGEdgeEnd];               \
      }                                                                 \
    }                                                                   \
                                                                        \
    return node->getLayout().instanceName[edge];                        \
  }

// RNYG_NODE_PROPERTY_IMPL(void *, Context, context, context);
// RNYG_NODE_PROPERTY_IMPL(RNYGPrintFunc, PrintFunc, printFunc, print);
// RNYG_NODE_PROPERTY_IMPL(bool, HasNewLayout, hasNewLayout, hasNewLayout);
// RNYG_NODE_PROPERTY_IMPL(RNYGNodeType, NodeType, nodeType, nodeType);

RNYG_NODE_STYLE_PROPERTY_IMPL(RNYGDirection, Direction, direction, direction);
RNYG_NODE_STYLE_PROPERTY_IMPL(RNYGFlexDirection, FlexDirection, flexDirection, flexDirection);
RNYG_NODE_STYLE_PROPERTY_IMPL(RNYGJustify, JustifyContent, justifyContent, justifyContent);
RNYG_NODE_STYLE_PROPERTY_IMPL(RNYGAlign, AlignContent, alignContent, alignContent);
RNYG_NODE_STYLE_PROPERTY_IMPL(RNYGAlign, AlignItems, alignItems, alignItems);
RNYG_NODE_STYLE_PROPERTY_IMPL(RNYGAlign, AlignSelf, alignSelf, alignSelf);
RNYG_NODE_STYLE_PROPERTY_IMPL(RNYGPositionType, PositionType, positionType, positionType);
RNYG_NODE_STYLE_PROPERTY_IMPL(RNYGWrap, FlexWrap, flexWrap, flexWrap);
RNYG_NODE_STYLE_PROPERTY_IMPL(RNYGOverflow, Overflow, overflow, overflow);
RNYG_NODE_STYLE_PROPERTY_IMPL(RNYGDisplay, Display, display, display);

RNYG_NODE_STYLE_PROPERTY_IMPL(float, Flex, flex, flex);
RNYG_NODE_STYLE_PROPERTY_SETTER_IMPL(float, FlexGrow, flexGrow, flexGrow);
RNYG_NODE_STYLE_PROPERTY_SETTER_IMPL(float, FlexShrink, flexShrink, flexShrink);
RNYG_NODE_STYLE_PROPERTY_UNIT_AUTO_IMPL(RNYGValue, FlexBasis, flexBasis, flexBasis);

RNYG_NODE_STYLE_EDGE_PROPERTY_UNIT_IMPL(RNYGValue, Position, position, position);
RNYG_NODE_STYLE_EDGE_PROPERTY_UNIT_IMPL(RNYGValue, Margin, margin, margin);
RNYG_NODE_STYLE_EDGE_PROPERTY_UNIT_AUTO_IMPL(RNYGValue, Margin, margin);
RNYG_NODE_STYLE_EDGE_PROPERTY_UNIT_IMPL(RNYGValue, Padding, padding, padding);
RNYG_NODE_STYLE_EDGE_PROPERTY_IMPL(float, Border, border, border);

RNYG_NODE_STYLE_PROPERTY_UNIT_AUTO_IMPL(RNYGValue, Width, width, dimensions[RNYGDimensionWidth]);
RNYG_NODE_STYLE_PROPERTY_UNIT_AUTO_IMPL(RNYGValue, Height, height, dimensions[RNYGDimensionHeight]);
RNYG_NODE_STYLE_PROPERTY_UNIT_IMPL(RNYGValue, MinWidth, minWidth, minDimensions[RNYGDimensionWidth]);
RNYG_NODE_STYLE_PROPERTY_UNIT_IMPL(RNYGValue, MinHeight, minHeight, minDimensions[RNYGDimensionHeight]);
RNYG_NODE_STYLE_PROPERTY_UNIT_IMPL(RNYGValue, MaxWidth, maxWidth, maxDimensions[RNYGDimensionWidth]);
RNYG_NODE_STYLE_PROPERTY_UNIT_IMPL(RNYGValue, MaxHeight, maxHeight, maxDimensions[RNYGDimensionHeight]);

// Yoga specific properties, not compatible with flexbox specification
RNYG_NODE_STYLE_PROPERTY_IMPL(float, AspectRatio, aspectRatio, aspectRatio);

RNYG_NODE_LAYOUT_PROPERTY_IMPL(float, Left, position[RNYGEdgeLeft]);
RNYG_NODE_LAYOUT_PROPERTY_IMPL(float, Top, position[RNYGEdgeTop]);
RNYG_NODE_LAYOUT_PROPERTY_IMPL(float, Right, position[RNYGEdgeRight]);
RNYG_NODE_LAYOUT_PROPERTY_IMPL(float, Bottom, position[RNYGEdgeBottom]);
RNYG_NODE_LAYOUT_PROPERTY_IMPL(float, Width, dimensions[RNYGDimensionWidth]);
RNYG_NODE_LAYOUT_PROPERTY_IMPL(float, Height, dimensions[RNYGDimensionHeight]);
RNYG_NODE_LAYOUT_PROPERTY_IMPL(RNYGDirection, Direction, direction);
RNYG_NODE_LAYOUT_PROPERTY_IMPL(bool, HadOverflow, hadOverflow);

RNYG_NODE_LAYOUT_RESOLVED_PROPERTY_IMPL(float, Margin, margin);
RNYG_NODE_LAYOUT_RESOLVED_PROPERTY_IMPL(float, Border, border);
RNYG_NODE_LAYOUT_RESOLVED_PROPERTY_IMPL(float, Padding, padding);

uint32_t gCurrentGenerationCount = 0;

bool RNYGLayoutNodeInternal(const RNYGNodeRef node,
                          const float availableWidth,
                          const float availableHeight,
                          const RNYGDirection parentDirection,
                          const RNYGMeasureMode widthMeasureMode,
                          const RNYGMeasureMode heightMeasureMode,
                          const float parentWidth,
                          const float parentHeight,
                          const bool performLayout,
                          const char *reason,
                          const RNYGConfigRef config);

bool RNYGFloatsEqualWithPrecision(const float a, const float b, const float precision) {
  assert(precision > 0);

  if (RNYGFloatIsUndefined(a)) {
    return RNYGFloatIsUndefined(b);
  }
  return fabs(a - b) < precision;
}

bool RNYGFloatsEqual(const float a, const float b) {
  return RNYGFloatsEqualWithPrecision(a, b, 0.0001f);
}

static void RNYGNodePrintInternal(const RNYGNodeRef node,
                                const RNYGPrintOptions options) {
  std::string str;
  facebook::yoga::RNYGNodeToString(&str, node, options, 0);
    RNYGLog(node, RNYGLogLevelDebug, str.c_str());
}

void RNYGNodePrint(const RNYGNodeRef node, const RNYGPrintOptions options) {
    RNYGNodePrintInternal(node, options);
}

const std::array<RNYGEdge, 4> leading = {
    {RNYGEdgeTop, RNYGEdgeBottom, RNYGEdgeLeft, RNYGEdgeRight}};

const std::array<RNYGEdge, 4> trailing = {
    {RNYGEdgeBottom, RNYGEdgeTop, RNYGEdgeRight, RNYGEdgeLeft}};
static const std::array<RNYGEdge, 4> pos = {{
    RNYGEdgeTop,
    RNYGEdgeBottom,
    RNYGEdgeLeft,
    RNYGEdgeRight,
}};

static const std::array<RNYGDimension, 4> dim = {
    {RNYGDimensionHeight, RNYGDimensionHeight, RNYGDimensionWidth, RNYGDimensionWidth}};

static inline float RNYGNodePaddingAndBorderForAxis(const RNYGNodeRef node,
                                                  const RNYGFlexDirection axis,
                                                  const float widthSize) {
  return node->getLeadingPaddingAndBorder(axis, widthSize) +
      node->getTrailingPaddingAndBorder(axis, widthSize);
}

static inline RNYGAlign RNYGNodeAlignItem(const RNYGNodeRef node, const RNYGNodeRef child) {
  const RNYGAlign align = child->getStyle().alignSelf == RNYGAlignAuto
      ? node->getStyle().alignItems
      : child->getStyle().alignSelf;
  if (align == RNYGAlignBaseline &&
      RNYGFlexDirectionIsColumn(node->getStyle().flexDirection)) {
    return RNYGAlignFlexStart;
  }
  return align;
}

static float RNYGBaseline(const RNYGNodeRef node) {
  if (node->getBaseline() != nullptr) {
    const float baseline = node->getBaseline()(
        node,
        node->getLayout().measuredDimensions[RNYGDimensionWidth],
        node->getLayout().measuredDimensions[RNYGDimensionHeight]);
      RNYGAssertWithNode(node,
                     !RNYGFloatIsUndefined(baseline),
                     "Expect custom baseline function to not return NaN");
    return baseline;
  }

    RNYGNodeRef baselineChild = nullptr;
  const uint32_t childCount = RNYGNodeGetChildCount(node);
  for (uint32_t i = 0; i < childCount; i++) {
    const RNYGNodeRef child = RNYGNodeGetChild(node, i);
    if (child->getLineIndex() > 0) {
      break;
    }
    if (child->getStyle().positionType == RNYGPositionTypeAbsolute) {
      continue;
    }
    if (RNYGNodeAlignItem(node, child) == RNYGAlignBaseline) {
      baselineChild = child;
      break;
    }

    if (baselineChild == nullptr) {
      baselineChild = child;
    }
  }

  if (baselineChild == nullptr) {
    return node->getLayout().measuredDimensions[RNYGDimensionHeight];
  }

  const float baseline = RNYGBaseline(baselineChild);
  return baseline + baselineChild->getLayout().position[RNYGEdgeTop];
}

static bool RNYGIsBaselineLayout(const RNYGNodeRef node) {
  if (RNYGFlexDirectionIsColumn(node->getStyle().flexDirection)) {
    return false;
  }
  if (node->getStyle().alignItems == RNYGAlignBaseline) {
    return true;
  }
  const uint32_t childCount = RNYGNodeGetChildCount(node);
  for (uint32_t i = 0; i < childCount; i++) {
    const RNYGNodeRef child = RNYGNodeGetChild(node, i);
    if (child->getStyle().positionType == RNYGPositionTypeRelative &&
        child->getStyle().alignSelf == RNYGAlignBaseline) {
      return true;
    }
  }

  return false;
}

static inline float RNYGNodeDimWithMargin(const RNYGNodeRef node,
                                        const RNYGFlexDirection axis,
                                        const float widthSize) {
  return node->getLayout().measuredDimensions[dim[axis]] +
      node->getLeadingMargin(axis, widthSize) +
      node->getTrailingMargin(axis, widthSize);
}

static inline bool RNYGNodeIsStyleDimDefined(const RNYGNodeRef node,
                                           const RNYGFlexDirection axis,
                                           const float parentSize) {
  return !(
      node->getResolvedDimension(dim[axis]).unit == RNYGUnitAuto ||
      node->getResolvedDimension(dim[axis]).unit == RNYGUnitUndefined ||
      (node->getResolvedDimension(dim[axis]).unit == RNYGUnitPoint &&
       node->getResolvedDimension(dim[axis]).value < 0.0f) ||
      (node->getResolvedDimension(dim[axis]).unit == RNYGUnitPercent &&
       (node->getResolvedDimension(dim[axis]).value < 0.0f ||
        RNYGFloatIsUndefined(parentSize))));
}

static inline bool RNYGNodeIsLayoutDimDefined(const RNYGNodeRef node, const RNYGFlexDirection axis) {
  const float value = node->getLayout().measuredDimensions[dim[axis]];
  return !RNYGFloatIsUndefined(value) && value >= 0.0f;
}

static float RNYGNodeBoundAxisWithinMinAndMax(const RNYGNodeRef node,
                                            const RNYGFlexDirection axis,
                                            const float value,
                                            const float axisSize) {
  float min = RNYGUndefined;
  float max = RNYGUndefined;

  if (RNYGFlexDirectionIsColumn(axis)) {
    min = RNYGResolveValue(
        node->getStyle().minDimensions[RNYGDimensionHeight], axisSize);
    max = RNYGResolveValue(
        node->getStyle().maxDimensions[RNYGDimensionHeight], axisSize);
  } else if (RNYGFlexDirectionIsRow(axis)) {
    min = RNYGResolveValue(
        node->getStyle().minDimensions[RNYGDimensionWidth], axisSize);
    max = RNYGResolveValue(
        node->getStyle().maxDimensions[RNYGDimensionWidth], axisSize);
  }

  float boundValue = value;

  if (!RNYGFloatIsUndefined(max) && max >= 0.0f && boundValue > max) {
    boundValue = max;
  }

  if (!RNYGFloatIsUndefined(min) && min >= 0.0f && boundValue < min) {
    boundValue = min;
  }

  return boundValue;
}

// Like YGNodeBoundAxisWithinMinAndMax but also ensures that the value doesn't go
// below the
// padding and border amount.
static inline float RNYGNodeBoundAxis(const RNYGNodeRef node,
                                    const RNYGFlexDirection axis,
                                    const float value,
                                    const float axisSize,
                                    const float widthSize) {
  return fmaxf(RNYGNodeBoundAxisWithinMinAndMax(node, axis, value, axisSize),
               RNYGNodePaddingAndBorderForAxis(node, axis, widthSize));
}

static void RNYGNodeSetChildTrailingPosition(const RNYGNodeRef node,
                                           const RNYGNodeRef child,
                                           const RNYGFlexDirection axis) {
  const float size = child->getLayout().measuredDimensions[dim[axis]];
  child->setLayoutPosition(
      node->getLayout().measuredDimensions[dim[axis]] - size -
          child->getLayout().position[pos[axis]],
      trailing[axis]);
}

static void RNYGConstrainMaxSizeForMode(const RNYGNodeRef node,
                                      const enum RNYGFlexDirection axis,
                                      const float parentAxisSize,
                                      const float parentWidth,
                                        RNYGMeasureMode *mode,
                                      float *size) {
  const float maxSize =
    RNYGResolveValue(
          node->getStyle().maxDimensions[dim[axis]], parentAxisSize) +
      node->getMarginForAxis(axis, parentWidth);
  switch (*mode) {
    case RNYGMeasureModeExactly:
    case RNYGMeasureModeAtMost:
      *size = (RNYGFloatIsUndefined(maxSize) || *size < maxSize) ? *size : maxSize;
      break;
    case RNYGMeasureModeUndefined:
      if (!RNYGFloatIsUndefined(maxSize)) {
        *mode = RNYGMeasureModeAtMost;
        *size = maxSize;
      }
      break;
  }
}

static void RNYGNodeComputeFlexBasisForChild(const RNYGNodeRef node,
                                           const RNYGNodeRef child,
                                           const float width,
                                           const RNYGMeasureMode widthMode,
                                           const float height,
                                           const float parentWidth,
                                           const float parentHeight,
                                           const RNYGMeasureMode heightMode,
                                           const RNYGDirection direction,
                                           const RNYGConfigRef config) {
  const RNYGFlexDirection mainAxis =
    RNYGResolveFlexDirection(node->getStyle().flexDirection, direction);
  const bool isMainAxisRow = RNYGFlexDirectionIsRow(mainAxis);
  const float mainAxisSize = isMainAxisRow ? width : height;
  const float mainAxisParentSize = isMainAxisRow ? parentWidth : parentHeight;

  float childWidth;
  float childHeight;
    RNYGMeasureMode childWidthMeasureMode;
    RNYGMeasureMode childHeightMeasureMode;

  const float resolvedFlexBasis =
    RNYGResolveValue(child->resolveFlexBasisPtr(), mainAxisParentSize);
  const bool isRowStyleDimDefined = RNYGNodeIsStyleDimDefined(child, RNYGFlexDirectionRow, parentWidth);
  const bool isColumnStyleDimDefined =
    RNYGNodeIsStyleDimDefined(child, RNYGFlexDirectionColumn, parentHeight);

  if (!RNYGFloatIsUndefined(resolvedFlexBasis) && !RNYGFloatIsUndefined(mainAxisSize)) {
    if (RNYGFloatIsUndefined(child->getLayout().computedFlexBasis) ||
        (RNYGConfigIsExperimentalFeatureEnabled(
             child->getConfig(), RNYGExperimentalFeatureWebFlexBasis) &&
         child->getLayout().computedFlexBasisGeneration !=
             gCurrentGenerationCount)) {
      child->setLayoutComputedFlexBasis(fmaxf(
          resolvedFlexBasis,
                                              RNYGNodePaddingAndBorderForAxis(child, mainAxis, parentWidth)));
    }
  } else if (isMainAxisRow && isRowStyleDimDefined) {
    // The width is definite, so use that as the flex basis.
    child->setLayoutComputedFlexBasis(fmaxf(
                                            RNYGResolveValue(
            child->getResolvedDimension(RNYGDimensionWidth), parentWidth),
                                            RNYGNodePaddingAndBorderForAxis(child, RNYGFlexDirectionRow, parentWidth)));
  } else if (!isMainAxisRow && isColumnStyleDimDefined) {
    // The height is definite, so use that as the flex basis.
    child->setLayoutComputedFlexBasis(fmaxf(
                                            RNYGResolveValue(
            child->getResolvedDimension(RNYGDimensionHeight), parentHeight),
                                            RNYGNodePaddingAndBorderForAxis(
            child, RNYGFlexDirectionColumn, parentWidth)));
  } else {
    // Compute the flex basis and hypothetical main size (i.e. the clamped
    // flex basis).
    childWidth = RNYGUndefined;
    childHeight = RNYGUndefined;
    childWidthMeasureMode = RNYGMeasureModeUndefined;
    childHeightMeasureMode = RNYGMeasureModeUndefined;

    const float marginRow =
        child->getMarginForAxis(RNYGFlexDirectionRow, parentWidth);
    const float marginColumn =
        child->getMarginForAxis(RNYGFlexDirectionColumn, parentWidth);

    if (isRowStyleDimDefined) {
      childWidth =
        RNYGResolveValue(
              child->getResolvedDimension(RNYGDimensionWidth), parentWidth) +
          marginRow;
      childWidthMeasureMode = RNYGMeasureModeExactly;
    }
    if (isColumnStyleDimDefined) {
      childHeight =
        RNYGResolveValue(
              child->getResolvedDimension(RNYGDimensionHeight), parentHeight) +
          marginColumn;
      childHeightMeasureMode = RNYGMeasureModeExactly;
    }

    // The W3C spec doesn't say anything about the 'overflow' property,
    // but all major browsers appear to implement the following logic.
    if ((!isMainAxisRow && node->getStyle().overflow == RNYGOverflowScroll) ||
        node->getStyle().overflow != RNYGOverflowScroll) {
      if (RNYGFloatIsUndefined(childWidth) && !RNYGFloatIsUndefined(width)) {
        childWidth = width;
        childWidthMeasureMode = RNYGMeasureModeAtMost;
      }
    }

    if ((isMainAxisRow && node->getStyle().overflow == RNYGOverflowScroll) ||
        node->getStyle().overflow != RNYGOverflowScroll) {
      if (RNYGFloatIsUndefined(childHeight) && !RNYGFloatIsUndefined(height)) {
        childHeight = height;
        childHeightMeasureMode = RNYGMeasureModeAtMost;
      }
    }

    if (!RNYGFloatIsUndefined(child->getStyle().aspectRatio)) {
      if (!isMainAxisRow && childWidthMeasureMode == RNYGMeasureModeExactly) {
        childHeight = marginColumn +
            (childWidth - marginRow) / child->getStyle().aspectRatio;
        childHeightMeasureMode = RNYGMeasureModeExactly;
      } else if (isMainAxisRow && childHeightMeasureMode == RNYGMeasureModeExactly) {
        childWidth = marginRow +
            (childHeight - marginColumn) * child->getStyle().aspectRatio;
        childWidthMeasureMode = RNYGMeasureModeExactly;
      }
    }

    // If child has no defined size in the cross axis and is set to stretch,
    // set the cross
    // axis to be measured exactly with the available inner width

    const bool hasExactWidth = !RNYGFloatIsUndefined(width) && widthMode == RNYGMeasureModeExactly;
    const bool childWidthStretch = RNYGNodeAlignItem(node, child) == RNYGAlignStretch &&
                                   childWidthMeasureMode != RNYGMeasureModeExactly;
    if (!isMainAxisRow && !isRowStyleDimDefined && hasExactWidth && childWidthStretch) {
      childWidth = width;
      childWidthMeasureMode = RNYGMeasureModeExactly;
      if (!RNYGFloatIsUndefined(child->getStyle().aspectRatio)) {
        childHeight = (childWidth - marginRow) / child->getStyle().aspectRatio;
        childHeightMeasureMode = RNYGMeasureModeExactly;
      }
    }

    const bool hasExactHeight = !RNYGFloatIsUndefined(height) && heightMode == RNYGMeasureModeExactly;
    const bool childHeightStretch = RNYGNodeAlignItem(node, child) == RNYGAlignStretch &&
                                    childHeightMeasureMode != RNYGMeasureModeExactly;
    if (isMainAxisRow && !isColumnStyleDimDefined && hasExactHeight && childHeightStretch) {
      childHeight = height;
      childHeightMeasureMode = RNYGMeasureModeExactly;

      if (!RNYGFloatIsUndefined(child->getStyle().aspectRatio)) {
        childWidth =
            (childHeight - marginColumn) * child->getStyle().aspectRatio;
        childWidthMeasureMode = RNYGMeasureModeExactly;
      }
    }

      RNYGConstrainMaxSizeForMode(
        child, RNYGFlexDirectionRow, parentWidth, parentWidth, &childWidthMeasureMode, &childWidth);
      RNYGConstrainMaxSizeForMode(child,
                                  RNYGFlexDirectionColumn,
                              parentHeight,
                              parentWidth,
                              &childHeightMeasureMode,
                              &childHeight);

    // Measure the child
      RNYGLayoutNodeInternal(child,
                         childWidth,
                         childHeight,
                         direction,
                         childWidthMeasureMode,
                         childHeightMeasureMode,
                         parentWidth,
                         parentHeight,
                         false,
                         "measure",
                         config);

    child->setLayoutComputedFlexBasis(fmaxf(
        child->getLayout().measuredDimensions[dim[mainAxis]],
                                            RNYGNodePaddingAndBorderForAxis(child, mainAxis, parentWidth)));
  }
  child->setLayoutComputedFlexBasisGeneration(gCurrentGenerationCount);
}

static void RNYGNodeAbsoluteLayoutChild(const RNYGNodeRef node,
                                      const RNYGNodeRef child,
                                      const float width,
                                      const RNYGMeasureMode widthMode,
                                      const float height,
                                      const RNYGDirection direction,
                                      const RNYGConfigRef config) {
  const RNYGFlexDirection mainAxis =
    RNYGResolveFlexDirection(node->getStyle().flexDirection, direction);
  const RNYGFlexDirection crossAxis = RNYGFlexDirectionCross(mainAxis, direction);
  const bool isMainAxisRow = RNYGFlexDirectionIsRow(mainAxis);

  float childWidth = RNYGUndefined;
  float childHeight = RNYGUndefined;
    RNYGMeasureMode childWidthMeasureMode = RNYGMeasureModeUndefined;
    RNYGMeasureMode childHeightMeasureMode = RNYGMeasureModeUndefined;

  const float marginRow = child->getMarginForAxis(RNYGFlexDirectionRow, width);
  const float marginColumn =
      child->getMarginForAxis(RNYGFlexDirectionColumn, width);

  if (RNYGNodeIsStyleDimDefined(child, RNYGFlexDirectionRow, width)) {
    childWidth =
      RNYGResolveValue(child->getResolvedDimension(RNYGDimensionWidth), width) +
        marginRow;
  } else {
    // If the child doesn't have a specified width, compute the width based
    // on the left/right
    // offsets if they're defined.
    if (child->isLeadingPositionDefined(RNYGFlexDirectionRow) &&
        child->isTrailingPosDefined(RNYGFlexDirectionRow)) {
      childWidth = node->getLayout().measuredDimensions[RNYGDimensionWidth] -
          (node->getLeadingBorder(RNYGFlexDirectionRow) +
           node->getTrailingBorder(RNYGFlexDirectionRow)) -
          (child->getLeadingPosition(RNYGFlexDirectionRow, width) +
           child->getTrailingPosition(RNYGFlexDirectionRow, width));
      childWidth = RNYGNodeBoundAxis(child, RNYGFlexDirectionRow, childWidth, width, width);
    }
  }

  if (RNYGNodeIsStyleDimDefined(child, RNYGFlexDirectionColumn, height)) {
    childHeight =
      RNYGResolveValue(child->getResolvedDimension(RNYGDimensionHeight), height) +
        marginColumn;
  } else {
    // If the child doesn't have a specified height, compute the height
    // based on the top/bottom
    // offsets if they're defined.
    if (child->isLeadingPositionDefined(RNYGFlexDirectionColumn) &&
        child->isTrailingPosDefined(RNYGFlexDirectionColumn)) {
      childHeight = node->getLayout().measuredDimensions[RNYGDimensionHeight] -
          (node->getLeadingBorder(RNYGFlexDirectionColumn) +
           node->getTrailingBorder(RNYGFlexDirectionColumn)) -
          (child->getLeadingPosition(RNYGFlexDirectionColumn, height) +
           child->getTrailingPosition(RNYGFlexDirectionColumn, height));
      childHeight = RNYGNodeBoundAxis(child, RNYGFlexDirectionColumn, childHeight, height, width);
    }
  }

  // Exactly one dimension needs to be defined for us to be able to do aspect ratio
  // calculation. One dimension being the anchor and the other being flexible.
  if (RNYGFloatIsUndefined(childWidth) ^ RNYGFloatIsUndefined(childHeight)) {
    if (!RNYGFloatIsUndefined(child->getStyle().aspectRatio)) {
      if (RNYGFloatIsUndefined(childWidth)) {
        childWidth = marginRow +
            (childHeight - marginColumn) * child->getStyle().aspectRatio;
      } else if (RNYGFloatIsUndefined(childHeight)) {
        childHeight = marginColumn +
            (childWidth - marginRow) / child->getStyle().aspectRatio;
      }
    }
  }

  // If we're still missing one or the other dimension, measure the content.
  if (RNYGFloatIsUndefined(childWidth) || RNYGFloatIsUndefined(childHeight)) {
    childWidthMeasureMode =
      RNYGFloatIsUndefined(childWidth) ? RNYGMeasureModeUndefined : RNYGMeasureModeExactly;
    childHeightMeasureMode =
      RNYGFloatIsUndefined(childHeight) ? RNYGMeasureModeUndefined : RNYGMeasureModeExactly;

    // If the size of the parent is defined then try to constrain the absolute child to that size
    // as well. This allows text within the absolute child to wrap to the size of its parent.
    // This is the same behavior as many browsers implement.
    if (!isMainAxisRow && RNYGFloatIsUndefined(childWidth) && widthMode != RNYGMeasureModeUndefined &&
        width > 0) {
      childWidth = width;
      childWidthMeasureMode = RNYGMeasureModeAtMost;
    }

      RNYGLayoutNodeInternal(child,
                         childWidth,
                         childHeight,
                         direction,
                         childWidthMeasureMode,
                         childHeightMeasureMode,
                         childWidth,
                         childHeight,
                         false,
                         "abs-measure",
                         config);
    childWidth = child->getLayout().measuredDimensions[RNYGDimensionWidth] +
        child->getMarginForAxis(RNYGFlexDirectionRow, width);
    childHeight = child->getLayout().measuredDimensions[RNYGDimensionHeight] +
        child->getMarginForAxis(RNYGFlexDirectionColumn, width);
  }

    RNYGLayoutNodeInternal(child,
                       childWidth,
                       childHeight,
                       direction,
                       RNYGMeasureModeExactly,
                       RNYGMeasureModeExactly,
                       childWidth,
                       childHeight,
                       true,
                       "abs-layout",
                       config);

  if (child->isTrailingPosDefined(mainAxis) &&
      !child->isLeadingPositionDefined(mainAxis)) {
    child->setLayoutPosition(
        node->getLayout().measuredDimensions[dim[mainAxis]] -
            child->getLayout().measuredDimensions[dim[mainAxis]] -
            node->getTrailingBorder(mainAxis) -
            child->getTrailingMargin(mainAxis, width) -
            child->getTrailingPosition(
                mainAxis, isMainAxisRow ? width : height),
        leading[mainAxis]);
  } else if (
      !child->isLeadingPositionDefined(mainAxis) &&
      node->getStyle().justifyContent == RNYGJustifyCenter) {
    child->setLayoutPosition(
        (node->getLayout().measuredDimensions[dim[mainAxis]] -
         child->getLayout().measuredDimensions[dim[mainAxis]]) /
            2.0f,
        leading[mainAxis]);
  } else if (
      !child->isLeadingPositionDefined(mainAxis) &&
      node->getStyle().justifyContent == RNYGJustifyFlexEnd) {
    child->setLayoutPosition(
        (node->getLayout().measuredDimensions[dim[mainAxis]] -
         child->getLayout().measuredDimensions[dim[mainAxis]]),
        leading[mainAxis]);
  }

  if (child->isTrailingPosDefined(crossAxis) &&
      !child->isLeadingPositionDefined(crossAxis)) {
    child->setLayoutPosition(
        node->getLayout().measuredDimensions[dim[crossAxis]] -
            child->getLayout().measuredDimensions[dim[crossAxis]] -
            node->getTrailingBorder(crossAxis) -
            child->getTrailingMargin(crossAxis, width) -
            child->getTrailingPosition(
                crossAxis, isMainAxisRow ? height : width),
        leading[crossAxis]);

  } else if (
      !child->isLeadingPositionDefined(crossAxis) &&
             RNYGNodeAlignItem(node, child) == RNYGAlignCenter) {
    child->setLayoutPosition(
        (node->getLayout().measuredDimensions[dim[crossAxis]] -
         child->getLayout().measuredDimensions[dim[crossAxis]]) /
            2.0f,
        leading[crossAxis]);
  } else if (
      !child->isLeadingPositionDefined(crossAxis) &&
      ((RNYGNodeAlignItem(node, child) == RNYGAlignFlexEnd) ^
       (node->getStyle().flexWrap == RNYGWrapWrapReverse))) {
    child->setLayoutPosition(
        (node->getLayout().measuredDimensions[dim[crossAxis]] -
         child->getLayout().measuredDimensions[dim[crossAxis]]),
        leading[crossAxis]);
  }
}

static void RNYGNodeWithMeasureFuncSetMeasuredDimensions(const RNYGNodeRef node,
                                                       const float availableWidth,
                                                       const float availableHeight,
                                                       const RNYGMeasureMode widthMeasureMode,
                                                       const RNYGMeasureMode heightMeasureMode,
                                                       const float parentWidth,
                                                       const float parentHeight) {
    RNYGAssertWithNode(
      node,
      node->getMeasure() != nullptr,
      "Expected node to have custom measure function");

  const float paddingAndBorderAxisRow =
    RNYGNodePaddingAndBorderForAxis(node, RNYGFlexDirectionRow, availableWidth);
  const float paddingAndBorderAxisColumn =
    RNYGNodePaddingAndBorderForAxis(node, RNYGFlexDirectionColumn, availableWidth);
  const float marginAxisRow =
      node->getMarginForAxis(RNYGFlexDirectionRow, availableWidth);
  const float marginAxisColumn =
      node->getMarginForAxis(RNYGFlexDirectionColumn, availableWidth);

  // We want to make sure we don't call measure with negative size
  const float innerWidth = RNYGFloatIsUndefined(availableWidth)
      ? availableWidth
      : fmaxf(0, availableWidth - marginAxisRow - paddingAndBorderAxisRow);
  const float innerHeight = RNYGFloatIsUndefined(availableHeight)
      ? availableHeight
      : fmaxf(
            0, availableHeight - marginAxisColumn - paddingAndBorderAxisColumn);

  if (widthMeasureMode == RNYGMeasureModeExactly &&
      heightMeasureMode == RNYGMeasureModeExactly) {
    // Don't bother sizing the text if both dimensions are already defined.
    node->setLayoutMeasuredDimension(
        RNYGNodeBoundAxis(
            node,
            RNYGFlexDirectionRow,
            availableWidth - marginAxisRow,
            parentWidth,
            parentWidth),
            RNYGDimensionWidth);
    node->setLayoutMeasuredDimension(
          RNYGNodeBoundAxis(
            node,
            RNYGFlexDirectionColumn,
            availableHeight - marginAxisColumn,
            parentHeight,
            parentWidth),
          RNYGDimensionHeight);
  } else {
    // Measure the text under the current constraints.
    const RNYGSize measuredSize = node->getMeasure()(
        node, innerWidth, widthMeasureMode, innerHeight, heightMeasureMode);

    node->setLayoutMeasuredDimension(
        RNYGNodeBoundAxis(
            node,
                        RNYGFlexDirectionRow,
            (widthMeasureMode == RNYGMeasureModeUndefined ||
             widthMeasureMode == RNYGMeasureModeAtMost)
                ? measuredSize.width + paddingAndBorderAxisRow
                : availableWidth - marginAxisRow,
            parentWidth,
            parentWidth),
        RNYGDimensionWidth);

    node->setLayoutMeasuredDimension(
          RNYGNodeBoundAxis(
            node,
            RNYGFlexDirectionColumn,
            (heightMeasureMode == RNYGMeasureModeUndefined ||
             heightMeasureMode == RNYGMeasureModeAtMost)
                ? measuredSize.height + paddingAndBorderAxisColumn
                : availableHeight - marginAxisColumn,
            parentHeight,
            parentWidth),
       RNYGDimensionHeight);
  }
}

// For nodes with no children, use the available values if they were provided,
// or the minimum size as indicated by the padding and border sizes.
static void RNYGNodeEmptyContainerSetMeasuredDimensions(const RNYGNodeRef node,
                                                      const float availableWidth,
                                                      const float availableHeight,
                                                      const RNYGMeasureMode widthMeasureMode,
                                                      const RNYGMeasureMode heightMeasureMode,
                                                      const float parentWidth,
                                                      const float parentHeight) {
  const float paddingAndBorderAxisRow =
    RNYGNodePaddingAndBorderForAxis(node, RNYGFlexDirectionRow, parentWidth);
  const float paddingAndBorderAxisColumn =
    RNYGNodePaddingAndBorderForAxis(node, RNYGFlexDirectionColumn, parentWidth);
  const float marginAxisRow =
      node->getMarginForAxis(RNYGFlexDirectionRow, parentWidth);
  const float marginAxisColumn =
      node->getMarginForAxis(RNYGFlexDirectionColumn, parentWidth);

  node->setLayoutMeasuredDimension(
       RNYGNodeBoundAxis(
          node,
          RNYGFlexDirectionRow,
          (widthMeasureMode == RNYGMeasureModeUndefined ||
           widthMeasureMode == RNYGMeasureModeAtMost)
              ? paddingAndBorderAxisRow
              : availableWidth - marginAxisRow,
          parentWidth,
          parentWidth),
       RNYGDimensionWidth);

  node->setLayoutMeasuredDimension(
       RNYGNodeBoundAxis(
          node,
          RNYGFlexDirectionColumn,
          (heightMeasureMode == RNYGMeasureModeUndefined ||
           heightMeasureMode == RNYGMeasureModeAtMost)
              ? paddingAndBorderAxisColumn
              : availableHeight - marginAxisColumn,
          parentHeight,
          parentWidth),
       RNYGDimensionHeight);
}

static bool RNYGNodeFixedSizeSetMeasuredDimensions(const RNYGNodeRef node,
                                                 const float availableWidth,
                                                 const float availableHeight,
                                                 const RNYGMeasureMode widthMeasureMode,
                                                 const RNYGMeasureMode heightMeasureMode,
                                                 const float parentWidth,
                                                 const float parentHeight) {
  if ((widthMeasureMode == RNYGMeasureModeAtMost && availableWidth <= 0.0f) ||
      (heightMeasureMode == RNYGMeasureModeAtMost && availableHeight <= 0.0f) ||
      (widthMeasureMode == RNYGMeasureModeExactly && heightMeasureMode == RNYGMeasureModeExactly)) {
    const float marginAxisColumn =
        node->getMarginForAxis(RNYGFlexDirectionColumn, parentWidth);
    const float marginAxisRow =
        node->getMarginForAxis(RNYGFlexDirectionRow, parentWidth);

    node->setLayoutMeasuredDimension(
           RNYGNodeBoundAxis(
            node,
            RNYGFlexDirectionRow,
            RNYGFloatIsUndefined(availableWidth) ||
                    (widthMeasureMode == RNYGMeasureModeAtMost &&
                     availableWidth < 0.0f)
                ? 0.0f
                : availableWidth - marginAxisRow,
            parentWidth,
            parentWidth),
           RNYGDimensionWidth);

    node->setLayoutMeasuredDimension(
         RNYGNodeBoundAxis(
            node,
            RNYGFlexDirectionColumn,
            RNYGFloatIsUndefined(availableHeight) ||
                    (heightMeasureMode == RNYGMeasureModeAtMost &&
                     availableHeight < 0.0f)
                ? 0.0f
                : availableHeight - marginAxisColumn,
            parentHeight,
            parentWidth),
         RNYGDimensionHeight);
    return true;
  }

  return false;
}

static void RNYGZeroOutLayoutRecursivly(const RNYGNodeRef node) {
  memset(&(node->getLayout()), 0, sizeof(RNYGLayout));
  node->setHasNewLayout(true);
  node->cloneChildrenIfNeeded();
  const uint32_t childCount = RNYGNodeGetChildCount(node);
  for (uint32_t i = 0; i < childCount; i++) {
    const RNYGNodeRef child = node->getChild(i);
      RNYGZeroOutLayoutRecursivly(child);
  }
}

static float RNYGNodeCalculateAvailableInnerDim(
    const RNYGNodeRef node,
    RNYGFlexDirection axis,
    float availableDim,
    float parentDim) {
    RNYGFlexDirection direction =
    RNYGFlexDirectionIsRow(axis) ? RNYGFlexDirectionRow : RNYGFlexDirectionColumn;
    RNYGDimension dimension =
    RNYGFlexDirectionIsRow(axis) ? RNYGDimensionWidth : RNYGDimensionHeight;

  const float margin = node->getMarginForAxis(direction, parentDim);
  const float paddingAndBorder =
    RNYGNodePaddingAndBorderForAxis(node, direction, parentDim);

  float availableInnerDim = availableDim - margin - paddingAndBorder;
  // Max dimension overrides predefined dimension value; Min dimension in turn
  // overrides both of the above
  if (!RNYGFloatIsUndefined(availableInnerDim)) {
    // We want to make sure our available height does not violate min and max
    // constraints
    const float minInnerDim =
      RNYGResolveValue(node->getStyle().minDimensions[dimension], parentDim) -
        paddingAndBorder;
    const float maxInnerDim =
      RNYGResolveValue(node->getStyle().maxDimensions[dimension], parentDim) -
        paddingAndBorder;
    availableInnerDim =
        fmaxf(fminf(availableInnerDim, maxInnerDim), minInnerDim);
  }

  return availableInnerDim;
}

static void RNYGNodeComputeFlexBasisForChildren(
    const RNYGNodeRef node,
    const float availableInnerWidth,
    const float availableInnerHeight,
    RNYGMeasureMode widthMeasureMode,
    RNYGMeasureMode heightMeasureMode,
    RNYGDirection direction,
    RNYGFlexDirection mainAxis,
    const RNYGConfigRef config,
    bool performLayout,
    float& totalOuterFlexBasis) {
    RNYGNodeRef singleFlexChild = nullptr;
    RNYGVector children = node->getChildren();
    RNYGMeasureMode measureModeMainDim =
    RNYGFlexDirectionIsRow(mainAxis) ? widthMeasureMode : heightMeasureMode;
  // If there is only one child with flexGrow + flexShrink it means we can set
  // the computedFlexBasis to 0 instead of measuring and shrinking / flexing the
  // child to exactly match the remaining space
  if (measureModeMainDim == RNYGMeasureModeExactly) {
    for (auto child : children) {
      if (singleFlexChild != nullptr) {
        if (child->isNodeFlexible()) {
          // There is already a flexible child, abort
          singleFlexChild = nullptr;
          break;
        }
      } else if (
          child->resolveFlexGrow() > 0.0f &&
          child->resolveFlexShrink() > 0.0f) {
        singleFlexChild = child;
      }
    }
  }

  for (auto child : children) {
    child->resolveDimension();
    if (child->getStyle().display == RNYGDisplayNone) {
        RNYGZeroOutLayoutRecursivly(child);
      child->setHasNewLayout(true);
      child->setDirty(false);
      continue;
    }
    if (performLayout) {
      // Set the initial position (relative to the parent).
      const RNYGDirection childDirection = child->resolveDirection(direction);
      const float mainDim = RNYGFlexDirectionIsRow(mainAxis)
          ? availableInnerWidth
          : availableInnerHeight;
      const float crossDim = RNYGFlexDirectionIsRow(mainAxis)
          ? availableInnerHeight
          : availableInnerWidth;
      child->setPosition(
          childDirection, mainDim, crossDim, availableInnerWidth);
    }

    if (child->getStyle().positionType == RNYGPositionTypeAbsolute) {
      continue;
    }
    if (child == singleFlexChild) {
      child->setLayoutComputedFlexBasisGeneration(gCurrentGenerationCount);
      child->setLayoutComputedFlexBasis(0);
    } else {
        RNYGNodeComputeFlexBasisForChild(
          node,
          child,
          availableInnerWidth,
          widthMeasureMode,
          availableInnerHeight,
          availableInnerWidth,
          availableInnerHeight,
          heightMeasureMode,
          direction,
          config);
    }

    totalOuterFlexBasis += child->getLayout().computedFlexBasis +
        child->getMarginForAxis(mainAxis, availableInnerWidth);
  }
}

// This function assumes that all the children of node have their
// computedFlexBasis properly computed(To do this use
// YGNodeComputeFlexBasisForChildren function).
// This function calculates YGCollectFlexItemsRowMeasurement
static RNYGCollectFlexItemsRowValues RNYGCalculateCollectFlexItemsRowValues(
    const RNYGNodeRef& node,
    const RNYGDirection parentDirection,
    const float mainAxisParentSize,
    const float availableInnerWidth,
    const float availableInnerMainDim,
    const uint32_t startOfLineIndex,
    const uint32_t lineCount) {
    RNYGCollectFlexItemsRowValues flexAlgoRowMeasurement = {};
  flexAlgoRowMeasurement.relativeChildren.reserve(node->getChildren().size());

  float sizeConsumedOnCurrentLineIncludingMinConstraint = 0;
  const RNYGFlexDirection mainAxis = RNYGResolveFlexDirection(
      node->getStyle().flexDirection, node->resolveDirection(parentDirection));
  const bool isNodeFlexWrap = node->getStyle().flexWrap != RNYGWrapNoWrap;

  // Add items to the current line until it's full or we run out of items.
  uint32_t endOfLineIndex = startOfLineIndex;
  for (; endOfLineIndex < node->getChildrenCount(); endOfLineIndex++) {
    const RNYGNodeRef child = node->getChild(endOfLineIndex);
    if (child->getStyle().display == RNYGDisplayNone ||
        child->getStyle().positionType == RNYGPositionTypeAbsolute) {
      continue;
    }
    child->setLineIndex(lineCount);
    const float childMarginMainAxis =
        child->getMarginForAxis(mainAxis, availableInnerWidth);
    const float flexBasisWithMinAndMaxConstraints =
      RNYGNodeBoundAxisWithinMinAndMax(
            child,
            mainAxis,
            child->getLayout().computedFlexBasis,
            mainAxisParentSize);

    // If this is a multi-line flow and this item pushes us over the
    // available size, we've
    // hit the end of the current line. Break out of the loop and lay out
    // the current line.
    if (sizeConsumedOnCurrentLineIncludingMinConstraint +
                flexBasisWithMinAndMaxConstraints + childMarginMainAxis >
            availableInnerMainDim &&
        isNodeFlexWrap && flexAlgoRowMeasurement.itemsOnLine > 0) {
      break;
    }

    sizeConsumedOnCurrentLineIncludingMinConstraint +=
        flexBasisWithMinAndMaxConstraints + childMarginMainAxis;
    flexAlgoRowMeasurement.sizeConsumedOnCurrentLine +=
        flexBasisWithMinAndMaxConstraints + childMarginMainAxis;
    flexAlgoRowMeasurement.itemsOnLine++;

    if (child->isNodeFlexible()) {
      flexAlgoRowMeasurement.totalFlexGrowFactors += child->resolveFlexGrow();

      // Unlike the grow factor, the shrink factor is scaled relative to the
      // child dimension.
      flexAlgoRowMeasurement.totalFlexShrinkScaledFactors +=
          -child->resolveFlexShrink() * child->getLayout().computedFlexBasis;
    }

    flexAlgoRowMeasurement.relativeChildren.push_back(child);
  }

  // The total flex factor needs to be floored to 1.
  if (flexAlgoRowMeasurement.totalFlexGrowFactors > 0 &&
      flexAlgoRowMeasurement.totalFlexGrowFactors < 1) {
    flexAlgoRowMeasurement.totalFlexGrowFactors = 1;
  }

  // The total flex shrink factor needs to be floored to 1.
  if (flexAlgoRowMeasurement.totalFlexShrinkScaledFactors > 0 &&
      flexAlgoRowMeasurement.totalFlexShrinkScaledFactors < 1) {
    flexAlgoRowMeasurement.totalFlexShrinkScaledFactors = 1;
  }
  flexAlgoRowMeasurement.endOfLineIndex = endOfLineIndex;
  return flexAlgoRowMeasurement;
}

// It distributes the free space to the flexible items and ensures that the size
// of the flex items abide the min and max constraints. At the end of this
// function the child nodes would have proper size. Prior using this function
// please ensure that YGDistributeFreeSpaceFirstPass is called.
static float RNYGDistributeFreeSpaceSecondPass(
    RNYGCollectFlexItemsRowValues& collectedFlexItemsValues,
    const RNYGNodeRef node,
    const RNYGFlexDirection mainAxis,
    const RNYGFlexDirection crossAxis,
    const float mainAxisParentSize,
    const float availableInnerMainDim,
    const float availableInnerCrossDim,
    const float availableInnerWidth,
    const float availableInnerHeight,
    const bool flexBasisOverflows,
    const RNYGMeasureMode measureModeCrossDim,
    const bool performLayout,
    const RNYGConfigRef config) {
  float childFlexBasis = 0;
  float flexShrinkScaledFactor = 0;
  float flexGrowFactor = 0;
  float deltaFreeSpace = 0;
  const bool isMainAxisRow = RNYGFlexDirectionIsRow(mainAxis);
  const bool isNodeFlexWrap = node->getStyle().flexWrap != RNYGWrapNoWrap;

  for (auto currentRelativeChild : collectedFlexItemsValues.relativeChildren) {
    childFlexBasis = RNYGNodeBoundAxisWithinMinAndMax(
        currentRelativeChild,
        mainAxis,
        currentRelativeChild->getLayout().computedFlexBasis,
        mainAxisParentSize);
    float updatedMainSize = childFlexBasis;

    if (collectedFlexItemsValues.remainingFreeSpace < 0) {
      flexShrinkScaledFactor =
          -currentRelativeChild->resolveFlexShrink() * childFlexBasis;
      // Is this child able to shrink?
      if (flexShrinkScaledFactor != 0) {
        float childSize;

        if (collectedFlexItemsValues.totalFlexShrinkScaledFactors == 0) {
          childSize = childFlexBasis + flexShrinkScaledFactor;
        } else {
          childSize = childFlexBasis +
              (collectedFlexItemsValues.remainingFreeSpace /
               collectedFlexItemsValues.totalFlexShrinkScaledFactors) *
                  flexShrinkScaledFactor;
        }

        updatedMainSize = RNYGNodeBoundAxis(
            currentRelativeChild,
            mainAxis,
            childSize,
            availableInnerMainDim,
            availableInnerWidth);
      }
    } else if (collectedFlexItemsValues.remainingFreeSpace > 0) {
      flexGrowFactor = currentRelativeChild->resolveFlexGrow();

      // Is this child able to grow?
      if (flexGrowFactor != 0) {
        updatedMainSize = RNYGNodeBoundAxis(
            currentRelativeChild,
            mainAxis,
            childFlexBasis +
                collectedFlexItemsValues.remainingFreeSpace /
                    collectedFlexItemsValues.totalFlexGrowFactors *
                    flexGrowFactor,
            availableInnerMainDim,
            availableInnerWidth);
      }
    }

    deltaFreeSpace += updatedMainSize - childFlexBasis;

    const float marginMain =
        currentRelativeChild->getMarginForAxis(mainAxis, availableInnerWidth);
    const float marginCross =
        currentRelativeChild->getMarginForAxis(crossAxis, availableInnerWidth);

    float childCrossSize;
    float childMainSize = updatedMainSize + marginMain;
      RNYGMeasureMode childCrossMeasureMode;
      RNYGMeasureMode childMainMeasureMode = RNYGMeasureModeExactly;

    if (!RNYGFloatIsUndefined(currentRelativeChild->getStyle().aspectRatio)) {
      childCrossSize = isMainAxisRow ? (childMainSize - marginMain) /
              currentRelativeChild->getStyle().aspectRatio
                                     : (childMainSize - marginMain) *
              currentRelativeChild->getStyle().aspectRatio;
      childCrossMeasureMode = RNYGMeasureModeExactly;

      childCrossSize += marginCross;
    } else if (
        !RNYGFloatIsUndefined(availableInnerCrossDim) &&
        !RNYGNodeIsStyleDimDefined(
            currentRelativeChild, crossAxis, availableInnerCrossDim) &&
        measureModeCrossDim == RNYGMeasureModeExactly &&
        !(isNodeFlexWrap && flexBasisOverflows) &&
               RNYGNodeAlignItem(node, currentRelativeChild) == RNYGAlignStretch &&
        currentRelativeChild->marginLeadingValue(crossAxis).unit !=
               RNYGUnitAuto &&
        currentRelativeChild->marginTrailingValue(crossAxis).unit !=
               RNYGUnitAuto) {
      childCrossSize = availableInnerCrossDim;
      childCrossMeasureMode = RNYGMeasureModeExactly;
    } else if (!RNYGNodeIsStyleDimDefined(
                   currentRelativeChild, crossAxis, availableInnerCrossDim)) {
      childCrossSize = availableInnerCrossDim;
      childCrossMeasureMode = RNYGFloatIsUndefined(childCrossSize)
          ? RNYGMeasureModeUndefined
          : RNYGMeasureModeAtMost;
    } else {
      childCrossSize =
        RNYGResolveValue(
              currentRelativeChild->getResolvedDimension(dim[crossAxis]),
              availableInnerCrossDim) +
          marginCross;
      const bool isLoosePercentageMeasurement =
          currentRelativeChild->getResolvedDimension(dim[crossAxis]).unit ==
        RNYGUnitPercent &&
          measureModeCrossDim != RNYGMeasureModeExactly;
      childCrossMeasureMode =
        RNYGFloatIsUndefined(childCrossSize) || isLoosePercentageMeasurement
          ? RNYGMeasureModeUndefined
          : RNYGMeasureModeExactly;
    }

      RNYGConstrainMaxSizeForMode(
        currentRelativeChild,
        mainAxis,
        availableInnerMainDim,
        availableInnerWidth,
        &childMainMeasureMode,
        &childMainSize);
      RNYGConstrainMaxSizeForMode(
        currentRelativeChild,
        crossAxis,
        availableInnerCrossDim,
        availableInnerWidth,
        &childCrossMeasureMode,
        &childCrossSize);

    const bool requiresStretchLayout =
        !RNYGNodeIsStyleDimDefined(
            currentRelativeChild, crossAxis, availableInnerCrossDim) &&
      RNYGNodeAlignItem(node, currentRelativeChild) == RNYGAlignStretch &&
        currentRelativeChild->marginLeadingValue(crossAxis).unit !=
            RNYGUnitAuto &&
        currentRelativeChild->marginTrailingValue(crossAxis).unit != RNYGUnitAuto;

    const float childWidth = isMainAxisRow ? childMainSize : childCrossSize;
    const float childHeight = !isMainAxisRow ? childMainSize : childCrossSize;

    const RNYGMeasureMode childWidthMeasureMode =
        isMainAxisRow ? childMainMeasureMode : childCrossMeasureMode;
    const RNYGMeasureMode childHeightMeasureMode =
        !isMainAxisRow ? childMainMeasureMode : childCrossMeasureMode;

    // Recursively call the layout algorithm for this child with the updated
    // main size.
      RNYGLayoutNodeInternal(
        currentRelativeChild,
        childWidth,
        childHeight,
        node->getLayout().direction,
        childWidthMeasureMode,
        childHeightMeasureMode,
        availableInnerWidth,
        availableInnerHeight,
        performLayout && !requiresStretchLayout,
        "flex",
        config);
    node->setLayoutHadOverflow(
        node->getLayout().hadOverflow ||
        currentRelativeChild->getLayout().hadOverflow);
  }
  return deltaFreeSpace;
}

// It distributes the free space to the flexible items.For those flexible items
// whose min and max constraints are triggered, those flex item's clamped size
// is removed from the remaingfreespace.
static void RNYGDistributeFreeSpaceFirstPass(
   RNYGCollectFlexItemsRowValues& collectedFlexItemsValues,
    const RNYGFlexDirection mainAxis,
    const float mainAxisParentSize,
    const float availableInnerMainDim,
    const float availableInnerWidth) {
  float flexShrinkScaledFactor = 0;
  float flexGrowFactor = 0;
  float baseMainSize = 0;
  float boundMainSize = 0;
  float deltaFreeSpace = 0;

  for (auto currentRelativeChild : collectedFlexItemsValues.relativeChildren) {
    float childFlexBasis = RNYGNodeBoundAxisWithinMinAndMax(
        currentRelativeChild,
        mainAxis,
        currentRelativeChild->getLayout().computedFlexBasis,
        mainAxisParentSize);

    if (collectedFlexItemsValues.remainingFreeSpace < 0) {
      flexShrinkScaledFactor =
          -currentRelativeChild->resolveFlexShrink() * childFlexBasis;

      // Is this child able to shrink?
      if (flexShrinkScaledFactor != 0) {
        baseMainSize = childFlexBasis +
            collectedFlexItemsValues.remainingFreeSpace /
                collectedFlexItemsValues.totalFlexShrinkScaledFactors *
                flexShrinkScaledFactor;
        boundMainSize = RNYGNodeBoundAxis(
            currentRelativeChild,
            mainAxis,
            baseMainSize,
            availableInnerMainDim,
            availableInnerWidth);
        if (baseMainSize != boundMainSize) {
          // By excluding this item's size and flex factor from remaining,
          // this item's
          // min/max constraints should also trigger in the second pass
          // resulting in the
          // item's size calculation being identical in the first and second
          // passes.
          deltaFreeSpace += boundMainSize - childFlexBasis;
          collectedFlexItemsValues.totalFlexShrinkScaledFactors -=
              flexShrinkScaledFactor;
        }
      }
    } else if (collectedFlexItemsValues.remainingFreeSpace > 0) {
      flexGrowFactor = currentRelativeChild->resolveFlexGrow();

      // Is this child able to grow?
      if (flexGrowFactor != 0) {
        baseMainSize = childFlexBasis +
            collectedFlexItemsValues.remainingFreeSpace /
                collectedFlexItemsValues.totalFlexGrowFactors * flexGrowFactor;
        boundMainSize = RNYGNodeBoundAxis(
            currentRelativeChild,
            mainAxis,
            baseMainSize,
            availableInnerMainDim,
            availableInnerWidth);

        if (baseMainSize != boundMainSize) {
          // By excluding this item's size and flex factor from remaining,
          // this item's
          // min/max constraints should also trigger in the second pass
          // resulting in the
          // item's size calculation being identical in the first and second
          // passes.
          deltaFreeSpace += boundMainSize - childFlexBasis;
          collectedFlexItemsValues.totalFlexGrowFactors -= flexGrowFactor;
        }
      }
    }
  }
  collectedFlexItemsValues.remainingFreeSpace -= deltaFreeSpace;
}

// Do two passes over the flex items to figure out how to distribute the
// remaining space.
// The first pass finds the items whose min/max constraints trigger,
// freezes them at those
// sizes, and excludes those sizes from the remaining space. The second
// pass sets the size
// of each flexible item. It distributes the remaining space amongst the
// items whose min/max
// constraints didn't trigger in pass 1. For the other items, it sets
// their sizes by forcing
// their min/max constraints to trigger again.
//
// This two pass approach for resolving min/max constraints deviates from
// the spec. The
// spec (https://www.w3.org/TR/YG-flexbox-1/#resolve-flexible-lengths)
// describes a process
// that needs to be repeated a variable number of times. The algorithm
// implemented here
// won't handle all cases but it was simpler to implement and it mitigates
// performance
// concerns because we know exactly how many passes it'll do.
//
// At the end of this function the child nodes would have the proper size
// assigned to them.
//
static void RNYGResolveFlexibleLength(
    const RNYGNodeRef node,
    RNYGCollectFlexItemsRowValues& collectedFlexItemsValues,
    const RNYGFlexDirection mainAxis,
    const RNYGFlexDirection crossAxis,
    const float mainAxisParentSize,
    const float availableInnerMainDim,
    const float availableInnerCrossDim,
    const float availableInnerWidth,
    const float availableInnerHeight,
    const bool flexBasisOverflows,
    const RNYGMeasureMode measureModeCrossDim,
    const bool performLayout,
    const RNYGConfigRef config) {
  const float originalFreeSpace = collectedFlexItemsValues.remainingFreeSpace;
  // First pass: detect the flex items whose min/max constraints trigger
  RNYGDistributeFreeSpaceFirstPass(
      collectedFlexItemsValues,
      mainAxis,
      mainAxisParentSize,
      availableInnerMainDim,
      availableInnerWidth);

  // Second pass: resolve the sizes of the flexible items
  const float distributedFreeSpace = RNYGDistributeFreeSpaceSecondPass(
      collectedFlexItemsValues,
      node,
      mainAxis,
      crossAxis,
      mainAxisParentSize,
      availableInnerMainDim,
      availableInnerCrossDim,
      availableInnerWidth,
      availableInnerHeight,
      flexBasisOverflows,
      measureModeCrossDim,
      performLayout,
      config);

  collectedFlexItemsValues.remainingFreeSpace =
      originalFreeSpace - distributedFreeSpace;
}

static void RNYGJustifyMainAxis(
    const RNYGNodeRef node,
    RNYGCollectFlexItemsRowValues& collectedFlexItemsValues,
    const uint32_t& startOfLineIndex,
    const RNYGFlexDirection& mainAxis,
    const RNYGFlexDirection& crossAxis,
    const RNYGMeasureMode& measureModeMainDim,
    const RNYGMeasureMode& measureModeCrossDim,
    const float& mainAxisParentSize,
    const float& parentWidth,
    const float& availableInnerMainDim,
    const float& availableInnerCrossDim,
    const float& availableInnerWidth,
    const bool& performLayout) {
    const RNYGStyle style = node->getStyle();

  // If we are using "at most" rules in the main axis. Calculate the remaining
  // space when constraint by the min size defined for the main axis.
  if (measureModeMainDim == RNYGMeasureModeAtMost &&
      collectedFlexItemsValues.remainingFreeSpace > 0) {
    if (style.minDimensions[dim[mainAxis]].unit != RNYGUnitUndefined &&
        RNYGResolveValue(
            style.minDimensions[dim[mainAxis]], mainAxisParentSize) >= 0) {
      collectedFlexItemsValues.remainingFreeSpace = fmaxf(
          0,
           RNYGResolveValue(
              style.minDimensions[dim[mainAxis]], mainAxisParentSize) -
              (availableInnerMainDim -
               collectedFlexItemsValues.remainingFreeSpace));
    } else {
      collectedFlexItemsValues.remainingFreeSpace = 0;
    }
  }

  int numberOfAutoMarginsOnCurrentLine = 0;
  for (uint32_t i = startOfLineIndex;
       i < collectedFlexItemsValues.endOfLineIndex;
       i++) {
    const RNYGNodeRef child = node->getChild(i);
    if (child->getStyle().positionType == RNYGPositionTypeRelative) {
      if (child->marginLeadingValue(mainAxis).unit == RNYGUnitAuto) {
        numberOfAutoMarginsOnCurrentLine++;
      }
      if (child->marginTrailingValue(mainAxis).unit == RNYGUnitAuto) {
        numberOfAutoMarginsOnCurrentLine++;
      }
    }
  }

  // In order to position the elements in the main axis, we have two
  // controls. The space between the beginning and the first element
  // and the space between each two elements.
  float leadingMainDim = 0;
  float betweenMainDim = 0;
  const RNYGJustify justifyContent = node->getStyle().justifyContent;

  if (numberOfAutoMarginsOnCurrentLine == 0) {
    switch (justifyContent) {
      case RNYGJustifyCenter:
        leadingMainDim = collectedFlexItemsValues.remainingFreeSpace / 2;
        break;
      case RNYGJustifyFlexEnd:
        leadingMainDim = collectedFlexItemsValues.remainingFreeSpace;
        break;
      case RNYGJustifySpaceBetween:
        if (collectedFlexItemsValues.itemsOnLine > 1) {
          betweenMainDim =
              fmaxf(collectedFlexItemsValues.remainingFreeSpace, 0) /
              (collectedFlexItemsValues.itemsOnLine - 1);
        } else {
          betweenMainDim = 0;
        }
        break;
      case RNYGJustifySpaceEvenly:
        // Space is distributed evenly across all elements
        betweenMainDim = collectedFlexItemsValues.remainingFreeSpace /
            (collectedFlexItemsValues.itemsOnLine + 1);
        leadingMainDim = betweenMainDim;
        break;
      case RNYGJustifySpaceAround:
        // Space on the edges is half of the space between elements
        betweenMainDim = collectedFlexItemsValues.remainingFreeSpace /
            collectedFlexItemsValues.itemsOnLine;
        leadingMainDim = betweenMainDim / 2;
        break;
      case RNYGJustifyFlexStart:
        break;
    }
  }

  const float leadingPaddingAndBorderMain =
      node->getLeadingPaddingAndBorder(mainAxis, parentWidth);
  collectedFlexItemsValues.mainDim =
      leadingPaddingAndBorderMain + leadingMainDim;
  collectedFlexItemsValues.crossDim = 0;

  for (uint32_t i = startOfLineIndex;
       i < collectedFlexItemsValues.endOfLineIndex;
       i++) {
    const RNYGNodeRef child = node->getChild(i);
    const RNYGStyle childStyle = child->getStyle();
    const RNYGLayout childLayout = child->getLayout();
    if (childStyle.display == RNYGDisplayNone) {
      continue;
    }
    if (childStyle.positionType == RNYGPositionTypeAbsolute &&
        child->isLeadingPositionDefined(mainAxis)) {
      if (performLayout) {
        // In case the child is position absolute and has left/top being
        // defined, we override the position to whatever the user said
        // (and margin/border).
        child->setLayoutPosition(
            child->getLeadingPosition(mainAxis, availableInnerMainDim) +
                node->getLeadingBorder(mainAxis) +
                child->getLeadingMargin(mainAxis, availableInnerWidth),
            pos[mainAxis]);
      }
    } else {
      // Now that we placed the element, we need to update the variables.
      // We need to do that only for relative elements. Absolute elements
      // do not take part in that phase.
      if (childStyle.positionType == RNYGPositionTypeRelative) {
        if (child->marginLeadingValue(mainAxis).unit == RNYGUnitAuto) {
          collectedFlexItemsValues.mainDim +=
              collectedFlexItemsValues.remainingFreeSpace /
              numberOfAutoMarginsOnCurrentLine;
        }

        if (performLayout) {
          child->setLayoutPosition(
              childLayout.position[pos[mainAxis]] +
                  collectedFlexItemsValues.mainDim,
              pos[mainAxis]);
        }

        if (child->marginTrailingValue(mainAxis).unit == RNYGUnitAuto) {
          collectedFlexItemsValues.mainDim +=
              collectedFlexItemsValues.remainingFreeSpace /
              numberOfAutoMarginsOnCurrentLine;
        }
        bool canSkipFlex =
            !performLayout && measureModeCrossDim == RNYGMeasureModeExactly;
        if (canSkipFlex) {
          // If we skipped the flex step, then we can't rely on the
          // measuredDims because
          // they weren't computed. This means we can't call
          // YGNodeDimWithMargin.
          collectedFlexItemsValues.mainDim += betweenMainDim +
              child->getMarginForAxis(mainAxis, availableInnerWidth) +
              childLayout.computedFlexBasis;
          collectedFlexItemsValues.crossDim = availableInnerCrossDim;
        } else {
          // The main dimension is the sum of all the elements dimension plus
          // the spacing.
          collectedFlexItemsValues.mainDim += betweenMainDim +
            RNYGNodeDimWithMargin(child, mainAxis, availableInnerWidth);

          // The cross dimension is the max of the elements dimension since
          // there can only be one element in that cross dimension.
          collectedFlexItemsValues.crossDim = fmaxf(
              collectedFlexItemsValues.crossDim,
              RNYGNodeDimWithMargin(child, crossAxis, availableInnerWidth));
        }
      } else if (performLayout) {
        child->setLayoutPosition(
            childLayout.position[pos[mainAxis]] +
                node->getLeadingBorder(mainAxis) + leadingMainDim,
            pos[mainAxis]);
      }
    }
  }
  collectedFlexItemsValues.mainDim +=
      node->getTrailingPaddingAndBorder(mainAxis, parentWidth);
}

//
// This is the main routine that implements a subset of the flexbox layout
// algorithm
// described in the W3C YG documentation: https://www.w3.org/TR/YG3-flexbox/.
//
// Limitations of this algorithm, compared to the full standard:
//  * Display property is always assumed to be 'flex' except for Text nodes,
//  which
//    are assumed to be 'inline-flex'.
//  * The 'zIndex' property (or any form of z ordering) is not supported. Nodes
//  are
//    stacked in document order.
//  * The 'order' property is not supported. The order of flex items is always
//  defined
//    by document order.
//  * The 'visibility' property is always assumed to be 'visible'. Values of
//  'collapse'
//    and 'hidden' are not supported.
//  * There is no support for forced breaks.
//  * It does not support vertical inline directions (top-to-bottom or
//  bottom-to-top text).
//
// Deviations from standard:
//  * Section 4.5 of the spec indicates that all flex items have a default
//  minimum
//    main size. For text blocks, for example, this is the width of the widest
//    word.
//    Calculating the minimum width is expensive, so we forego it and assume a
//    default
//    minimum main size of 0.
//  * Min/Max sizes in the main axis are not honored when resolving flexible
//  lengths.
//  * The spec indicates that the default value for 'flexDirection' is 'row',
//  but
//    the algorithm below assumes a default of 'column'.
//
// Input parameters:
//    - node: current node to be sized and layed out
//    - availableWidth & availableHeight: available size to be used for sizing
//    the node
//      or YGUndefined if the size is not available; interpretation depends on
//      layout
//      flags
//    - parentDirection: the inline (text) direction within the parent
//    (left-to-right or
//      right-to-left)
//    - widthMeasureMode: indicates the sizing rules for the width (see below
//    for explanation)
//    - heightMeasureMode: indicates the sizing rules for the height (see below
//    for explanation)
//    - performLayout: specifies whether the caller is interested in just the
//    dimensions
//      of the node or it requires the entire node and its subtree to be layed
//      out
//      (with final positions)
//
// Details:
//    This routine is called recursively to lay out subtrees of flexbox
//    elements. It uses the
//    information in node.style, which is treated as a read-only input. It is
//    responsible for
//    setting the layout.direction and layout.measuredDimensions fields for the
//    input node as well
//    as the layout.position and layout.lineIndex fields for its child nodes.
//    The
//    layout.measuredDimensions field includes any border or padding for the
//    node but does
//    not include margins.
//
//    The spec describes four different layout modes: "fill available", "max
//    content", "min
//    content",
//    and "fit content". Of these, we don't use "min content" because we don't
//    support default
//    minimum main sizes (see above for details). Each of our measure modes maps
//    to a layout mode
//    from the spec (https://www.w3.org/TR/YG3-sizing/#terms):
//      - YGMeasureModeUndefined: max content
//      - YGMeasureModeExactly: fill available
//      - YGMeasureModeAtMost: fit content
//
//    When calling YGNodelayoutImpl and YGLayoutNodeInternal, if the caller passes
//    an available size of
//    undefined then it must also pass a measure mode of YGMeasureModeUndefined
//    in that dimension.
//
static void RNYGNodelayoutImpl(const RNYGNodeRef node,
                             const float availableWidth,
                             const float availableHeight,
                             const RNYGDirection parentDirection,
                             const RNYGMeasureMode widthMeasureMode,
                             const RNYGMeasureMode heightMeasureMode,
                             const float parentWidth,
                             const float parentHeight,
                             const bool performLayout,
                             const RNYGConfigRef config) {
    RNYGAssertWithNode(node,
                   RNYGFloatIsUndefined(availableWidth) ? widthMeasureMode == RNYGMeasureModeUndefined
                                                      : true,
                   "availableWidth is indefinite so widthMeasureMode must be "
                   "RNYGMeasureModeUndefined");
    RNYGAssertWithNode(node,
                       RNYGFloatIsUndefined(availableHeight) ? heightMeasureMode == RNYGMeasureModeUndefined
                                                       : true,
                   "availableHeight is indefinite so heightMeasureMode must be "
                   "RNYGMeasureModeUndefined");

  // Set the resolved resolution in the node's layout.
  const RNYGDirection direction = node->resolveDirection(parentDirection);
  node->setLayoutDirection(direction);

  const RNYGFlexDirection flexRowDirection = RNYGResolveFlexDirection(RNYGFlexDirectionRow, direction);
  const RNYGFlexDirection flexColumnDirection =
    RNYGResolveFlexDirection(RNYGFlexDirectionColumn, direction);

  node->setLayoutMargin(
      node->getLeadingMargin(flexRowDirection, parentWidth), RNYGEdgeStart);
  node->setLayoutMargin(
      node->getTrailingMargin(flexRowDirection, parentWidth), RNYGEdgeEnd);
  node->setLayoutMargin(
      node->getLeadingMargin(flexColumnDirection, parentWidth), RNYGEdgeTop);
  node->setLayoutMargin(
      node->getTrailingMargin(flexColumnDirection, parentWidth), RNYGEdgeBottom);

  node->setLayoutBorder(node->getLeadingBorder(flexRowDirection), RNYGEdgeStart);
  node->setLayoutBorder(node->getTrailingBorder(flexRowDirection), RNYGEdgeEnd);
  node->setLayoutBorder(node->getLeadingBorder(flexColumnDirection), RNYGEdgeTop);
  node->setLayoutBorder(
      node->getTrailingBorder(flexColumnDirection), RNYGEdgeBottom);

  node->setLayoutPadding(
      node->getLeadingPadding(flexRowDirection, parentWidth), RNYGEdgeStart);
  node->setLayoutPadding(
      node->getTrailingPadding(flexRowDirection, parentWidth), RNYGEdgeEnd);
  node->setLayoutPadding(
      node->getLeadingPadding(flexColumnDirection, parentWidth), RNYGEdgeTop);
  node->setLayoutPadding(
      node->getTrailingPadding(flexColumnDirection, parentWidth), RNYGEdgeBottom);

  if (node->getMeasure() != nullptr) {
      RNYGNodeWithMeasureFuncSetMeasuredDimensions(node,
                                               availableWidth,
                                               availableHeight,
                                               widthMeasureMode,
                                               heightMeasureMode,
                                               parentWidth,
                                               parentHeight);
    return;
  }

  const uint32_t childCount = RNYGNodeGetChildCount(node);
  if (childCount == 0) {
      RNYGNodeEmptyContainerSetMeasuredDimensions(node,
                                              availableWidth,
                                              availableHeight,
                                              widthMeasureMode,
                                              heightMeasureMode,
                                              parentWidth,
                                              parentHeight);
    return;
  }

  // If we're not being asked to perform a full layout we can skip the algorithm if we already know
  // the size
  if (!performLayout && RNYGNodeFixedSizeSetMeasuredDimensions(node,
                                                             availableWidth,
                                                             availableHeight,
                                                             widthMeasureMode,
                                                             heightMeasureMode,
                                                             parentWidth,
                                                             parentHeight)) {
    return;
  }

  // At this point we know we're going to perform work. Ensure that each child has a mutable copy.
  node->cloneChildrenIfNeeded();
  // Reset layout flags, as they could have changed.
  node->setLayoutHadOverflow(false);

  // STEP 1: CALCULATE VALUES FOR REMAINDER OF ALGORITHM
  const RNYGFlexDirection mainAxis =
    RNYGResolveFlexDirection(node->getStyle().flexDirection, direction);
  const RNYGFlexDirection crossAxis = RNYGFlexDirectionCross(mainAxis, direction);
  const bool isMainAxisRow = RNYGFlexDirectionIsRow(mainAxis);
  const bool isNodeFlexWrap = node->getStyle().flexWrap != RNYGWrapNoWrap;

  const float mainAxisParentSize = isMainAxisRow ? parentWidth : parentHeight;
  const float crossAxisParentSize = isMainAxisRow ? parentHeight : parentWidth;

  const float leadingPaddingAndBorderCross =
      node->getLeadingPaddingAndBorder(crossAxis, parentWidth);
  const float paddingAndBorderAxisMain = RNYGNodePaddingAndBorderForAxis(node, mainAxis, parentWidth);
  const float paddingAndBorderAxisCross =
    RNYGNodePaddingAndBorderForAxis(node, crossAxis, parentWidth);

    RNYGMeasureMode measureModeMainDim = isMainAxisRow ? widthMeasureMode : heightMeasureMode;
    RNYGMeasureMode measureModeCrossDim = isMainAxisRow ? heightMeasureMode : widthMeasureMode;

  const float paddingAndBorderAxisRow =
      isMainAxisRow ? paddingAndBorderAxisMain : paddingAndBorderAxisCross;
  const float paddingAndBorderAxisColumn =
      isMainAxisRow ? paddingAndBorderAxisCross : paddingAndBorderAxisMain;

  const float marginAxisRow =
      node->getMarginForAxis(RNYGFlexDirectionRow, parentWidth);
  const float marginAxisColumn =
      node->getMarginForAxis(RNYGFlexDirectionColumn, parentWidth);

  const float minInnerWidth =
    RNYGResolveValue(
          node->getStyle().minDimensions[RNYGDimensionWidth], parentWidth) -
      paddingAndBorderAxisRow;
  const float maxInnerWidth =
    RNYGResolveValue(
          node->getStyle().maxDimensions[RNYGDimensionWidth], parentWidth) -
      paddingAndBorderAxisRow;
  const float minInnerHeight =
    RNYGResolveValue(
          node->getStyle().minDimensions[RNYGDimensionHeight], parentHeight) -
      paddingAndBorderAxisColumn;
  const float maxInnerHeight =
    RNYGResolveValue(
          node->getStyle().maxDimensions[RNYGDimensionHeight], parentHeight) -
      paddingAndBorderAxisColumn;

  const float minInnerMainDim = isMainAxisRow ? minInnerWidth : minInnerHeight;
  const float maxInnerMainDim = isMainAxisRow ? maxInnerWidth : maxInnerHeight;

  // STEP 2: DETERMINE AVAILABLE SIZE IN MAIN AND CROSS DIRECTIONS

  float availableInnerWidth = RNYGNodeCalculateAvailableInnerDim(
      node, RNYGFlexDirectionRow, availableWidth, parentWidth);
  float availableInnerHeight = RNYGNodeCalculateAvailableInnerDim(
      node, RNYGFlexDirectionColumn, availableHeight, parentHeight);

  float availableInnerMainDim =
      isMainAxisRow ? availableInnerWidth : availableInnerHeight;
  const float availableInnerCrossDim =
      isMainAxisRow ? availableInnerHeight : availableInnerWidth;

  float totalOuterFlexBasis = 0;

  // STEP 3: DETERMINE FLEX BASIS FOR EACH ITEM

    RNYGNodeComputeFlexBasisForChildren(
      node,
      availableInnerWidth,
      availableInnerHeight,
      widthMeasureMode,
      heightMeasureMode,
      direction,
      mainAxis,
      config,
      performLayout,
      totalOuterFlexBasis);

  const bool flexBasisOverflows = measureModeMainDim == RNYGMeasureModeUndefined
      ? false
      : totalOuterFlexBasis > availableInnerMainDim;
  if (isNodeFlexWrap && flexBasisOverflows &&
      measureModeMainDim == RNYGMeasureModeAtMost) {
    measureModeMainDim = RNYGMeasureModeExactly;
  }
  // STEP 4: COLLECT FLEX ITEMS INTO FLEX LINES

  // Indexes of children that represent the first and last items in the line.
  uint32_t startOfLineIndex = 0;
  uint32_t endOfLineIndex = 0;

  // Number of lines.
  uint32_t lineCount = 0;

  // Accumulated cross dimensions of all lines so far.
  float totalLineCrossDim = 0;

  // Max main dimension of all the lines.
  float maxLineMainDim = 0;
    RNYGCollectFlexItemsRowValues collectedFlexItemsValues;
  for (; endOfLineIndex < childCount;
       lineCount++, startOfLineIndex = endOfLineIndex) {
    collectedFlexItemsValues = RNYGCalculateCollectFlexItemsRowValues(
        node,
        parentDirection,
        mainAxisParentSize,
        availableInnerWidth,
        availableInnerMainDim,
        startOfLineIndex,
        lineCount);
    endOfLineIndex = collectedFlexItemsValues.endOfLineIndex;

    // If we don't need to measure the cross axis, we can skip the entire flex
    // step.
    const bool canSkipFlex =
        !performLayout && measureModeCrossDim == RNYGMeasureModeExactly;

    // STEP 5: RESOLVING FLEXIBLE LENGTHS ON MAIN AXIS
    // Calculate the remaining available space that needs to be allocated.
    // If the main dimension size isn't known, it is computed based on
    // the line length, so there's no more space left to distribute.

    bool sizeBasedOnContent = false;
    // If we don't measure with exact main dimension we want to ensure we don't violate min and max
    if (measureModeMainDim != RNYGMeasureModeExactly) {
      if (!RNYGFloatIsUndefined(minInnerMainDim) &&
          collectedFlexItemsValues.sizeConsumedOnCurrentLine <
              minInnerMainDim) {
        availableInnerMainDim = minInnerMainDim;
      } else if (
          !RNYGFloatIsUndefined(maxInnerMainDim) &&
          collectedFlexItemsValues.sizeConsumedOnCurrentLine >
              maxInnerMainDim) {
        availableInnerMainDim = maxInnerMainDim;
      } else {
        if (!node->getConfig()->useLegacyStretchBehaviour &&
            (collectedFlexItemsValues.totalFlexGrowFactors == 0 ||
             node->resolveFlexGrow() == 0)) {
          // If we don't have any children to flex or we can't flex the node
          // itself, space we've used is all space we need. Root node also
          // should be shrunk to minimum
          availableInnerMainDim =
              collectedFlexItemsValues.sizeConsumedOnCurrentLine;
        }

        if (node->getConfig()->useLegacyStretchBehaviour) {
          node->setLayoutDidUseLegacyFlag(true);
        }
        sizeBasedOnContent = !node->getConfig()->useLegacyStretchBehaviour;
      }
    }

    if (!sizeBasedOnContent && !RNYGFloatIsUndefined(availableInnerMainDim)) {
      collectedFlexItemsValues.remainingFreeSpace = availableInnerMainDim -
          collectedFlexItemsValues.sizeConsumedOnCurrentLine;
    } else if (collectedFlexItemsValues.sizeConsumedOnCurrentLine < 0) {
      // availableInnerMainDim is indefinite which means the node is being sized based on its
      // content.
      // sizeConsumedOnCurrentLine is negative which means the node will allocate 0 points for
      // its content. Consequently, remainingFreeSpace is 0 - sizeConsumedOnCurrentLine.
      collectedFlexItemsValues.remainingFreeSpace =
          -collectedFlexItemsValues.sizeConsumedOnCurrentLine;
    }

    if (!canSkipFlex) {
        RNYGResolveFlexibleLength(
          node,
          collectedFlexItemsValues,
          mainAxis,
          crossAxis,
          mainAxisParentSize,
          availableInnerMainDim,
          availableInnerCrossDim,
          availableInnerWidth,
          availableInnerHeight,
          flexBasisOverflows,
          measureModeCrossDim,
          performLayout,
          config);
    }

    node->setLayoutHadOverflow(
        node->getLayout().hadOverflow |
        (collectedFlexItemsValues.remainingFreeSpace < 0));

    // STEP 6: MAIN-AXIS JUSTIFICATION & CROSS-AXIS SIZE DETERMINATION

    // At this point, all the children have their dimensions set in the main
    // axis.
    // Their dimensions are also set in the cross axis with the exception of
    // items
    // that are aligned "stretch". We need to compute these stretch values and
    // set the final positions.

      RNYGJustifyMainAxis(
        node,
        collectedFlexItemsValues,
        startOfLineIndex,
        mainAxis,
        crossAxis,
        measureModeMainDim,
        measureModeCrossDim,
        mainAxisParentSize,
        parentWidth,
        availableInnerMainDim,
        availableInnerCrossDim,
        availableInnerWidth,
        performLayout);

    float containerCrossAxis = availableInnerCrossDim;
    if (measureModeCrossDim == RNYGMeasureModeUndefined ||
        measureModeCrossDim == RNYGMeasureModeAtMost) {
      // Compute the cross axis from the max cross dimension of the children.
      containerCrossAxis =
        RNYGNodeBoundAxis(
              node,
              crossAxis,
              collectedFlexItemsValues.crossDim + paddingAndBorderAxisCross,
              crossAxisParentSize,
              parentWidth) -
          paddingAndBorderAxisCross;
    }

    // If there's no flex wrap, the cross dimension is defined by the container.
    if (!isNodeFlexWrap && measureModeCrossDim == RNYGMeasureModeExactly) {
      collectedFlexItemsValues.crossDim = availableInnerCrossDim;
    }

    // Clamp to the min/max size specified on the container.
    collectedFlexItemsValues.crossDim =
      RNYGNodeBoundAxis(
            node,
            crossAxis,
            collectedFlexItemsValues.crossDim + paddingAndBorderAxisCross,
            crossAxisParentSize,
            parentWidth) -
        paddingAndBorderAxisCross;

    // STEP 7: CROSS-AXIS ALIGNMENT
    // We can skip child alignment if we're just measuring the container.
    if (performLayout) {
      for (uint32_t i = startOfLineIndex; i < endOfLineIndex; i++) {
        const RNYGNodeRef child = node->getChild(i);
        if (child->getStyle().display == RNYGDisplayNone) {
          continue;
        }
        if (child->getStyle().positionType == RNYGPositionTypeAbsolute) {
          // If the child is absolutely positioned and has a
          // top/left/bottom/right set, override
          // all the previously computed positions to set it correctly.
          const bool isChildLeadingPosDefined =
              child->isLeadingPositionDefined(crossAxis);
          if (isChildLeadingPosDefined) {
            child->setLayoutPosition(
                child->getLeadingPosition(crossAxis, availableInnerCrossDim) +
                    node->getLeadingBorder(crossAxis) +
                    child->getLeadingMargin(crossAxis, availableInnerWidth),
                pos[crossAxis]);
          }
          // If leading position is not defined or calculations result in Nan, default to border + margin
          if (!isChildLeadingPosDefined ||
              RNYGFloatIsUndefined(child->getLayout().position[pos[crossAxis]])) {
            child->setLayoutPosition(
                node->getLeadingBorder(crossAxis) +
                    child->getLeadingMargin(crossAxis, availableInnerWidth),
                pos[crossAxis]);
          }
        } else {
          float leadingCrossDim = leadingPaddingAndBorderCross;

          // For a relative children, we're either using alignItems (parent) or
          // alignSelf (child) in order to determine the position in the cross
          // axis
          const RNYGAlign alignItem = RNYGNodeAlignItem(node, child);

          // If the child uses align stretch, we need to lay it out one more
          // time, this time
          // forcing the cross-axis size to be the computed cross size for the
          // current line.
          if (alignItem == RNYGAlignStretch &&
              child->marginLeadingValue(crossAxis).unit != RNYGUnitAuto &&
              child->marginTrailingValue(crossAxis).unit != RNYGUnitAuto) {
            // If the child defines a definite size for its cross axis, there's
            // no need to stretch.
            if (!RNYGNodeIsStyleDimDefined(child, crossAxis, availableInnerCrossDim)) {
              float childMainSize =
                  child->getLayout().measuredDimensions[dim[mainAxis]];
              float childCrossSize =
                  !RNYGFloatIsUndefined(child->getStyle().aspectRatio)
                  ? ((child->getMarginForAxis(crossAxis, availableInnerWidth) +
                      (isMainAxisRow
                           ? childMainSize / child->getStyle().aspectRatio
                           : childMainSize * child->getStyle().aspectRatio)))
                  : collectedFlexItemsValues.crossDim;

              childMainSize +=
                  child->getMarginForAxis(mainAxis, availableInnerWidth);

                RNYGMeasureMode childMainMeasureMode = RNYGMeasureModeExactly;
                RNYGMeasureMode childCrossMeasureMode = RNYGMeasureModeExactly;
                RNYGConstrainMaxSizeForMode(child,
                                        mainAxis,
                                        availableInnerMainDim,
                                        availableInnerWidth,
                                        &childMainMeasureMode,
                                        &childMainSize);
                RNYGConstrainMaxSizeForMode(child,
                                        crossAxis,
                                        availableInnerCrossDim,
                                        availableInnerWidth,
                                        &childCrossMeasureMode,
                                        &childCrossSize);

              const float childWidth = isMainAxisRow ? childMainSize : childCrossSize;
              const float childHeight = !isMainAxisRow ? childMainSize : childCrossSize;

              const RNYGMeasureMode childWidthMeasureMode =
                RNYGFloatIsUndefined(childWidth) ? RNYGMeasureModeUndefined
                                                 : RNYGMeasureModeExactly;
              const RNYGMeasureMode childHeightMeasureMode =
                RNYGFloatIsUndefined(childHeight) ? RNYGMeasureModeUndefined
                                                  : RNYGMeasureModeExactly;

                RNYGLayoutNodeInternal(
                  child,
                  childWidth,
                  childHeight,
                  direction,
                  childWidthMeasureMode,
                  childHeightMeasureMode,
                  availableInnerWidth,
                  availableInnerHeight,
                  true,
                  "stretch",
                  config);
            }
          } else {
            const float remainingCrossDim = containerCrossAxis -
              RNYGNodeDimWithMargin(child, crossAxis, availableInnerWidth);

            if (child->marginLeadingValue(crossAxis).unit == RNYGUnitAuto &&
                child->marginTrailingValue(crossAxis).unit == RNYGUnitAuto) {
              leadingCrossDim += fmaxf(0.0f, remainingCrossDim / 2);
            } else if (
                child->marginTrailingValue(crossAxis).unit == RNYGUnitAuto) {
              // No-Op
            } else if (
                child->marginLeadingValue(crossAxis).unit == RNYGUnitAuto) {
              leadingCrossDim += fmaxf(0.0f, remainingCrossDim);
            } else if (alignItem == RNYGAlignFlexStart) {
              // No-Op
            } else if (alignItem == RNYGAlignCenter) {
              leadingCrossDim += remainingCrossDim / 2;
            } else {
              leadingCrossDim += remainingCrossDim;
            }
          }
          // And we apply the position
          child->setLayoutPosition(
              child->getLayout().position[pos[crossAxis]] + totalLineCrossDim +
                  leadingCrossDim,
              pos[crossAxis]);
        }
      }
    }

    totalLineCrossDim += collectedFlexItemsValues.crossDim;
    maxLineMainDim = fmaxf(maxLineMainDim, collectedFlexItemsValues.mainDim);
  }

  // STEP 8: MULTI-LINE CONTENT ALIGNMENT
  if (performLayout && (lineCount > 1 || RNYGIsBaselineLayout(node)) &&
      !RNYGFloatIsUndefined(availableInnerCrossDim)) {
    const float remainingAlignContentDim = availableInnerCrossDim - totalLineCrossDim;

    float crossDimLead = 0;
    float currentLead = leadingPaddingAndBorderCross;

    switch (node->getStyle().alignContent) {
      case RNYGAlignFlexEnd:
        currentLead += remainingAlignContentDim;
        break;
      case RNYGAlignCenter:
        currentLead += remainingAlignContentDim / 2;
        break;
      case RNYGAlignStretch:
        if (availableInnerCrossDim > totalLineCrossDim) {
          crossDimLead = remainingAlignContentDim / lineCount;
        }
        break;
      case RNYGAlignSpaceAround:
        if (availableInnerCrossDim > totalLineCrossDim) {
          currentLead += remainingAlignContentDim / (2 * lineCount);
          if (lineCount > 1) {
            crossDimLead = remainingAlignContentDim / lineCount;
          }
        } else {
          currentLead += remainingAlignContentDim / 2;
        }
        break;
      case RNYGAlignSpaceBetween:
        if (availableInnerCrossDim > totalLineCrossDim && lineCount > 1) {
          crossDimLead = remainingAlignContentDim / (lineCount - 1);
        }
        break;
      case RNYGAlignAuto:
      case RNYGAlignFlexStart:
      case RNYGAlignBaseline:
        break;
    }

    uint32_t endIndex = 0;
    for (uint32_t i = 0; i < lineCount; i++) {
      const uint32_t startIndex = endIndex;
      uint32_t ii;

      // compute the line's height and find the endIndex
      float lineHeight = 0;
      float maxAscentForCurrentLine = 0;
      float maxDescentForCurrentLine = 0;
      for (ii = startIndex; ii < childCount; ii++) {
        const RNYGNodeRef child = node->getChild(ii);
        if (child->getStyle().display == RNYGDisplayNone) {
          continue;
        }
        if (child->getStyle().positionType == RNYGPositionTypeRelative) {
          if (child->getLineIndex() != i) {
            break;
          }
          if (RNYGNodeIsLayoutDimDefined(child, crossAxis)) {
            lineHeight = fmaxf(
                lineHeight,
                child->getLayout().measuredDimensions[dim[crossAxis]] +
                    child->getMarginForAxis(crossAxis, availableInnerWidth));
          }
          if (RNYGNodeAlignItem(node, child) == RNYGAlignBaseline) {
            const float ascent = RNYGBaseline(child) +
                child->getLeadingMargin(
                    RNYGFlexDirectionColumn, availableInnerWidth);
            const float descent =
                child->getLayout().measuredDimensions[RNYGDimensionHeight] +
                child->getMarginForAxis(
                    RNYGFlexDirectionColumn, availableInnerWidth) -
                ascent;
            maxAscentForCurrentLine = fmaxf(maxAscentForCurrentLine, ascent);
            maxDescentForCurrentLine = fmaxf(maxDescentForCurrentLine, descent);
            lineHeight = fmaxf(lineHeight, maxAscentForCurrentLine + maxDescentForCurrentLine);
          }
        }
      }
      endIndex = ii;
      lineHeight += crossDimLead;

      if (performLayout) {
        for (ii = startIndex; ii < endIndex; ii++) {
          const RNYGNodeRef child = node->getChild(ii);
          if (child->getStyle().display == RNYGDisplayNone) {
            continue;
          }
          if (child->getStyle().positionType == RNYGPositionTypeRelative) {
            switch (RNYGNodeAlignItem(node, child)) {
              case RNYGAlignFlexStart: {
                child->setLayoutPosition(
                    currentLead +
                        child->getLeadingMargin(crossAxis, availableInnerWidth),
                    pos[crossAxis]);
                break;
              }
              case RNYGAlignFlexEnd: {
                child->setLayoutPosition(
                    currentLead + lineHeight -
                        child->getTrailingMargin(
                            crossAxis, availableInnerWidth) -
                        child->getLayout().measuredDimensions[dim[crossAxis]],
                    pos[crossAxis]);
                break;
              }
              case RNYGAlignCenter: {
                float childHeight =
                    child->getLayout().measuredDimensions[dim[crossAxis]];

                child->setLayoutPosition(
                    currentLead + (lineHeight - childHeight) / 2,
                    pos[crossAxis]);
                break;
              }
              case RNYGAlignStretch: {
                child->setLayoutPosition(
                    currentLead +
                        child->getLeadingMargin(crossAxis, availableInnerWidth),
                    pos[crossAxis]);

                // Remeasure child with the line height as it as been only measured with the
                // parents height yet.
                if (!RNYGNodeIsStyleDimDefined(child, crossAxis, availableInnerCrossDim)) {
                  const float childWidth = isMainAxisRow
                      ? (child->getLayout()
                             .measuredDimensions[RNYGDimensionWidth] +
                         child->getMarginForAxis(mainAxis, availableInnerWidth))
                      : lineHeight;

                  const float childHeight = !isMainAxisRow
                      ? (child->getLayout()
                             .measuredDimensions[RNYGDimensionHeight] +
                         child->getMarginForAxis(
                             crossAxis, availableInnerWidth))
                      : lineHeight;

                  if (!(RNYGFloatsEqual(
                            childWidth,
                            child->getLayout()
                                .measuredDimensions[RNYGDimensionWidth]) &&
                        RNYGFloatsEqual(
                            childHeight,
                            child->getLayout()
                                .measuredDimensions[RNYGDimensionHeight]))) {
                                    RNYGLayoutNodeInternal(child,
                                         childWidth,
                                         childHeight,
                                         direction,
                                         RNYGMeasureModeExactly,
                                         RNYGMeasureModeExactly,
                                         availableInnerWidth,
                                         availableInnerHeight,
                                         true,
                                         "multiline-stretch",
                                         config);
                  }
                }
                break;
              }
              case RNYGAlignBaseline: {
                child->setLayoutPosition(
                    currentLead + maxAscentForCurrentLine - RNYGBaseline(child) +
                        child->getLeadingPosition(
                           RNYGFlexDirectionColumn, availableInnerCrossDim),
                        RNYGEdgeTop);

                break;
              }
              case RNYGAlignAuto:
              case RNYGAlignSpaceBetween:
              case RNYGAlignSpaceAround:
                break;
            }
          }
        }
      }

      currentLead += lineHeight;
    }
  }

  // STEP 9: COMPUTING FINAL DIMENSIONS

  node->setLayoutMeasuredDimension(
       RNYGNodeBoundAxis(
          node,
          RNYGFlexDirectionRow,
          availableWidth - marginAxisRow,
          parentWidth,
          parentWidth),
        RNYGDimensionWidth);

  node->setLayoutMeasuredDimension(
       RNYGNodeBoundAxis(
          node,
          RNYGFlexDirectionColumn,
          availableHeight - marginAxisColumn,
          parentHeight,
          parentWidth),
       RNYGDimensionHeight);

  // If the user didn't specify a width or height for the node, set the
  // dimensions based on the children.
  if (measureModeMainDim == RNYGMeasureModeUndefined ||
      (node->getStyle().overflow != RNYGOverflowScroll &&
       measureModeMainDim == RNYGMeasureModeAtMost)) {
    // Clamp the size to the min/max size, if specified, and make sure it
    // doesn't go below the padding and border amount.
    node->setLayoutMeasuredDimension(
        RNYGNodeBoundAxis(
            node, mainAxis, maxLineMainDim, mainAxisParentSize, parentWidth),
        dim[mainAxis]);

  } else if (
      measureModeMainDim == RNYGMeasureModeAtMost &&
      node->getStyle().overflow == RNYGOverflowScroll) {
    node->setLayoutMeasuredDimension(
        fmaxf(
            fminf(
                availableInnerMainDim + paddingAndBorderAxisMain,
                RNYGNodeBoundAxisWithinMinAndMax(
                    node, mainAxis, maxLineMainDim, mainAxisParentSize)),
            paddingAndBorderAxisMain),
        dim[mainAxis]);
  }

  if (measureModeCrossDim == RNYGMeasureModeUndefined ||
      (node->getStyle().overflow != RNYGOverflowScroll &&
       measureModeCrossDim == RNYGMeasureModeAtMost)) {
    // Clamp the size to the min/max size, if specified, and make sure it
    // doesn't go below the padding and border amount.

    node->setLayoutMeasuredDimension(
        RNYGNodeBoundAxis(
            node,
            crossAxis,
            totalLineCrossDim + paddingAndBorderAxisCross,
            crossAxisParentSize,
            parentWidth),
        dim[crossAxis]);

  } else if (
      measureModeCrossDim == RNYGMeasureModeAtMost &&
      node->getStyle().overflow == RNYGOverflowScroll) {
    node->setLayoutMeasuredDimension(
        fmaxf(
            fminf(
                availableInnerCrossDim + paddingAndBorderAxisCross,
                  RNYGNodeBoundAxisWithinMinAndMax(
                    node,
                    crossAxis,
                    totalLineCrossDim + paddingAndBorderAxisCross,
                    crossAxisParentSize)),
            paddingAndBorderAxisCross),
        dim[crossAxis]);
  }

  // As we only wrapped in normal direction yet, we need to reverse the positions on wrap-reverse.
  if (performLayout && node->getStyle().flexWrap == RNYGWrapWrapReverse) {
    for (uint32_t i = 0; i < childCount; i++) {
      const RNYGNodeRef child = RNYGNodeGetChild(node, i);
      if (child->getStyle().positionType == RNYGPositionTypeRelative) {
        child->setLayoutPosition(
            node->getLayout().measuredDimensions[dim[crossAxis]] -
                child->getLayout().position[pos[crossAxis]] -
                child->getLayout().measuredDimensions[dim[crossAxis]],
            pos[crossAxis]);
      }
    }
  }

  if (performLayout) {
    // STEP 10: SIZING AND POSITIONING ABSOLUTE CHILDREN
    for (auto child : node->getChildren()) {
      if (child->getStyle().positionType != RNYGPositionTypeAbsolute) {
        continue;
      }
        RNYGNodeAbsoluteLayoutChild(
          node,
          child,
          availableInnerWidth,
          isMainAxisRow ? measureModeMainDim : measureModeCrossDim,
          availableInnerHeight,
          direction,
          config);
    }

    // STEP 11: SETTING TRAILING POSITIONS FOR CHILDREN
    const bool needsMainTrailingPos =
        mainAxis == RNYGFlexDirectionRowReverse || mainAxis == RNYGFlexDirectionColumnReverse;
    const bool needsCrossTrailingPos =
        crossAxis == RNYGFlexDirectionRowReverse || crossAxis == RNYGFlexDirectionColumnReverse;

    // Set trailing position if necessary.
    if (needsMainTrailingPos || needsCrossTrailingPos) {
      for (uint32_t i = 0; i < childCount; i++) {
        const RNYGNodeRef child = node->getChild(i);
        if (child->getStyle().display == RNYGDisplayNone) {
          continue;
        }
        if (needsMainTrailingPos) {
            RNYGNodeSetChildTrailingPosition(node, child, mainAxis);
        }

        if (needsCrossTrailingPos) {
            RNYGNodeSetChildTrailingPosition(node, child, crossAxis);
        }
      }
    }
  }
}

uint32_t gDepth = 0;
bool gPrintTree = false;
bool gPrintChanges = false;
bool gPrintSkips = false;

static const char *spacer = "                                                            ";

static const char *RNYGSpacer(const unsigned long level) {
  const size_t spacerLen = strlen(spacer);
  if (level > spacerLen) {
    return &spacer[0];
  } else {
    return &spacer[spacerLen - level];
  }
}

static const char *RNYGMeasureModeName(const RNYGMeasureMode mode, const bool performLayout) {
  const char *kMeasureModeNames[RNYGMeasureModeCount] = {"UNDEFINED", "EXACTLY", "AT_MOST"};
  const char *kLayoutModeNames[RNYGMeasureModeCount] = {"LAY_UNDEFINED",
                                                      "LAY_EXACTLY",
                                                      "LAY_AT_"
                                                      "MOST"};

  if (mode >= RNYGMeasureModeCount) {
    return "";
  }

  return performLayout ? kLayoutModeNames[mode] : kMeasureModeNames[mode];
}

static inline bool RNYGMeasureModeSizeIsExactAndMatchesOldMeasuredSize(RNYGMeasureMode sizeMode,
                                                                     float size,
                                                                     float lastComputedSize) {
  return sizeMode == RNYGMeasureModeExactly && RNYGFloatsEqual(size, lastComputedSize);
}

static inline bool RNYGMeasureModeOldSizeIsUnspecifiedAndStillFits(RNYGMeasureMode sizeMode,
                                                                 float size,
                                                                   RNYGMeasureMode lastSizeMode,
                                                                 float lastComputedSize) {
  return sizeMode == RNYGMeasureModeAtMost && lastSizeMode == RNYGMeasureModeUndefined &&
         (size >= lastComputedSize || RNYGFloatsEqual(size, lastComputedSize));
}

static inline bool RNYGMeasureModeNewMeasureSizeIsStricterAndStillValid(RNYGMeasureMode sizeMode,
                                                                      float size,
                                                                      RNYGMeasureMode lastSizeMode,
                                                                      float lastSize,
                                                                      float lastComputedSize) {
  return lastSizeMode == RNYGMeasureModeAtMost && sizeMode == RNYGMeasureModeAtMost &&
         lastSize > size && (lastComputedSize <= size || RNYGFloatsEqual(size, lastComputedSize));
}

float RNYGRoundValueToPixelGrid(const float value,
                              const float pointScaleFactor,
                              const bool forceCeil,
                              const bool forceFloor) {
  const float roundingError = fmax(0.0001, 0.01 * pointScaleFactor);
  float scaledValue = value * pointScaleFactor;
  float fractial = fmodf(scaledValue, 1.0);
  if (RNYGFloatsEqualWithPrecision(fractial, 0.0, roundingError)) {
    // First we check if the value is already rounded
    scaledValue = scaledValue - fractial;
  } else if (RNYGFloatsEqualWithPrecision(fractial, 1.0, roundingError)) {
    scaledValue = scaledValue - fractial + 1.0;
  } else if (forceCeil) {
    // Next we check if we need to use forced rounding
    scaledValue = scaledValue - fractial + 1.0f;
  } else if (forceFloor) {
    scaledValue = scaledValue - fractial;
  } else {
    // Finally we just round the value
    scaledValue = scaledValue - fractial +
        (fractial > 0.5f || RNYGFloatsEqualWithPrecision(fractial, 0.5f, roundingError) ? 1.0f : 0.0f);
  }
  return scaledValue / pointScaleFactor;
}

bool RNYGNodeCanUseCachedMeasurement(const RNYGMeasureMode widthMode,
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
                                   const RNYGConfigRef config) {
  if (lastComputedHeight < 0 || lastComputedWidth < 0) {
    return false;
  }
  bool useRoundedComparison =
      config != nullptr && config->pointScaleFactor != 0;
  const float effectiveWidth =
      useRoundedComparison ? RNYGRoundValueToPixelGrid(width, config->pointScaleFactor, false, false)
                           : width;
  const float effectiveHeight =
      useRoundedComparison ? RNYGRoundValueToPixelGrid(height, config->pointScaleFactor, false, false)
                           : height;
  const float effectiveLastWidth =
      useRoundedComparison
          ? RNYGRoundValueToPixelGrid(lastWidth, config->pointScaleFactor, false, false)
          : lastWidth;
  const float effectiveLastHeight =
      useRoundedComparison
          ? RNYGRoundValueToPixelGrid(lastHeight, config->pointScaleFactor, false, false)
          : lastHeight;

  const bool hasSameWidthSpec =
      lastWidthMode == widthMode && RNYGFloatsEqual(effectiveLastWidth, effectiveWidth);
  const bool hasSameHeightSpec =
      lastHeightMode == heightMode && RNYGFloatsEqual(effectiveLastHeight, effectiveHeight);

  const bool widthIsCompatible =
      hasSameWidthSpec || RNYGMeasureModeSizeIsExactAndMatchesOldMeasuredSize(widthMode,
                                                                            width - marginRow,
                                                                            lastComputedWidth) ||
    RNYGMeasureModeOldSizeIsUnspecifiedAndStillFits(widthMode,
                                                    width - marginRow,
                                                    lastWidthMode,
                                                    lastComputedWidth) ||
    RNYGMeasureModeNewMeasureSizeIsStricterAndStillValid(
          widthMode, width - marginRow, lastWidthMode, lastWidth, lastComputedWidth);

  const bool heightIsCompatible =
      hasSameHeightSpec || RNYGMeasureModeSizeIsExactAndMatchesOldMeasuredSize(heightMode,
                                                                             height - marginColumn,
                                                                             lastComputedHeight) ||
    RNYGMeasureModeOldSizeIsUnspecifiedAndStillFits(heightMode,
                                                    height - marginColumn,
                                                    lastHeightMode,
                                                    lastComputedHeight) ||
    RNYGMeasureModeNewMeasureSizeIsStricterAndStillValid(
          heightMode, height - marginColumn, lastHeightMode, lastHeight, lastComputedHeight);

  return widthIsCompatible && heightIsCompatible;
}

//
// This is a wrapper around the YGNodelayoutImpl function. It determines
// whether the layout request is redundant and can be skipped.
//
// Parameters:
//  Input parameters are the same as YGNodelayoutImpl (see above)
//  Return parameter is true if layout was performed, false if skipped
//
bool RNYGLayoutNodeInternal(const RNYGNodeRef node,
                          const float availableWidth,
                          const float availableHeight,
                          const RNYGDirection parentDirection,
                          const RNYGMeasureMode widthMeasureMode,
                          const RNYGMeasureMode heightMeasureMode,
                          const float parentWidth,
                          const float parentHeight,
                          const bool performLayout,
                          const char *reason,
                          const RNYGConfigRef config) {
    RNYGLayout* layout = &node->getLayout();

  gDepth++;

  const bool needToVisitNode =
      (node->isDirty() && layout->generationCount != gCurrentGenerationCount) ||
      layout->lastParentDirection != parentDirection;

  if (needToVisitNode) {
    // Invalidate the cached results.
    layout->nextCachedMeasurementsIndex = 0;
    layout->cachedLayout.widthMeasureMode = (RNYGMeasureMode) -1;
    layout->cachedLayout.heightMeasureMode = (RNYGMeasureMode) -1;
    layout->cachedLayout.computedWidth = -1;
    layout->cachedLayout.computedHeight = -1;
  }

    RNYGCachedMeasurement* cachedResults = nullptr;

  // Determine whether the results are already cached. We maintain a separate
  // cache for layouts and measurements. A layout operation modifies the
  // positions
  // and dimensions for nodes in the subtree. The algorithm assumes that each
  // node
  // gets layed out a maximum of one time per tree layout, but multiple
  // measurements
  // may be required to resolve all of the flex dimensions.
  // We handle nodes with measure functions specially here because they are the
  // most
  // expensive to measure, so it's worth avoiding redundant measurements if at
  // all possible.
  if (node->getMeasure() != nullptr) {
    const float marginAxisRow =
        node->getMarginForAxis(RNYGFlexDirectionRow, parentWidth);
    const float marginAxisColumn =
        node->getMarginForAxis(RNYGFlexDirectionColumn, parentWidth);

    // First, try to use the layout cache.
    if (RNYGNodeCanUseCachedMeasurement(widthMeasureMode,
                                      availableWidth,
                                      heightMeasureMode,
                                      availableHeight,
                                      layout->cachedLayout.widthMeasureMode,
                                      layout->cachedLayout.availableWidth,
                                      layout->cachedLayout.heightMeasureMode,
                                      layout->cachedLayout.availableHeight,
                                      layout->cachedLayout.computedWidth,
                                      layout->cachedLayout.computedHeight,
                                      marginAxisRow,
                                      marginAxisColumn,
                                      config)) {
      cachedResults = &layout->cachedLayout;
    } else {
      // Try to use the measurement cache.
      for (uint32_t i = 0; i < layout->nextCachedMeasurementsIndex; i++) {
        if (RNYGNodeCanUseCachedMeasurement(widthMeasureMode,
                                          availableWidth,
                                          heightMeasureMode,
                                          availableHeight,
                                          layout->cachedMeasurements[i].widthMeasureMode,
                                          layout->cachedMeasurements[i].availableWidth,
                                          layout->cachedMeasurements[i].heightMeasureMode,
                                          layout->cachedMeasurements[i].availableHeight,
                                          layout->cachedMeasurements[i].computedWidth,
                                          layout->cachedMeasurements[i].computedHeight,
                                          marginAxisRow,
                                          marginAxisColumn,
                                          config)) {
          cachedResults = &layout->cachedMeasurements[i];
          break;
        }
      }
    }
  } else if (performLayout) {
    if (RNYGFloatsEqual(layout->cachedLayout.availableWidth, availableWidth) &&
        RNYGFloatsEqual(layout->cachedLayout.availableHeight, availableHeight) &&
        layout->cachedLayout.widthMeasureMode == widthMeasureMode &&
        layout->cachedLayout.heightMeasureMode == heightMeasureMode) {
      cachedResults = &layout->cachedLayout;
    }
  } else {
    for (uint32_t i = 0; i < layout->nextCachedMeasurementsIndex; i++) {
      if (RNYGFloatsEqual(layout->cachedMeasurements[i].availableWidth, availableWidth) &&
          RNYGFloatsEqual(layout->cachedMeasurements[i].availableHeight, availableHeight) &&
          layout->cachedMeasurements[i].widthMeasureMode == widthMeasureMode &&
          layout->cachedMeasurements[i].heightMeasureMode == heightMeasureMode) {
        cachedResults = &layout->cachedMeasurements[i];
        break;
      }
    }
  }

  if (!needToVisitNode && cachedResults != nullptr) {
    layout->measuredDimensions[RNYGDimensionWidth] = cachedResults->computedWidth;
    layout->measuredDimensions[RNYGDimensionHeight] = cachedResults->computedHeight;

    if (gPrintChanges && gPrintSkips) {
        RNYGLog(node, RNYGLogLevelVerbose, "%s%d.{[skipped] ", RNYGSpacer(gDepth), gDepth);
      if (node->getPrintFunc() != nullptr) {
        node->getPrintFunc()(node);
      }
        RNYGLog(
          node,
          RNYGLogLevelVerbose,
          "wm: %s, hm: %s, aw: %f ah: %f => d: (%f, %f) %s\n",
          RNYGMeasureModeName(widthMeasureMode, performLayout),
          RNYGMeasureModeName(heightMeasureMode, performLayout),
          availableWidth,
          availableHeight,
          cachedResults->computedWidth,
          cachedResults->computedHeight,
          reason);
    }
  } else {
    if (gPrintChanges) {
        RNYGLog(
          node,
          RNYGLogLevelVerbose,
          "%s%d.{%s",
          RNYGSpacer(gDepth),
          gDepth,
          needToVisitNode ? "*" : "");
      if (node->getPrintFunc() != nullptr) {
        node->getPrintFunc()(node);
      }
        RNYGLog(
          node,
          RNYGLogLevelVerbose,
          "wm: %s, hm: %s, aw: %f ah: %f %s\n",
          RNYGMeasureModeName(widthMeasureMode, performLayout),
          RNYGMeasureModeName(heightMeasureMode, performLayout),
          availableWidth,
          availableHeight,
          reason);
    }

      RNYGNodelayoutImpl(node,
                     availableWidth,
                     availableHeight,
                     parentDirection,
                     widthMeasureMode,
                     heightMeasureMode,
                     parentWidth,
                     parentHeight,
                     performLayout,
                     config);

    if (gPrintChanges) {
        RNYGLog(
          node,
          RNYGLogLevelVerbose,
          "%s%d.}%s",
          RNYGSpacer(gDepth),
          gDepth,
          needToVisitNode ? "*" : "");
      if (node->getPrintFunc() != nullptr) {
        node->getPrintFunc()(node);
      }
        RNYGLog(
          node,
          RNYGLogLevelVerbose,
          "wm: %s, hm: %s, d: (%f, %f) %s\n",
          RNYGMeasureModeName(widthMeasureMode, performLayout),
          RNYGMeasureModeName(heightMeasureMode, performLayout),
          layout->measuredDimensions[RNYGDimensionWidth],
          layout->measuredDimensions[RNYGDimensionHeight],
          reason);
    }

    layout->lastParentDirection = parentDirection;

    if (cachedResults == nullptr) {
      if (layout->nextCachedMeasurementsIndex == RNYG_MAX_CACHED_RESULT_COUNT) {
        if (gPrintChanges) {
            RNYGLog(node, RNYGLogLevelVerbose, "Out of cache entries!\n");
        }
        layout->nextCachedMeasurementsIndex = 0;
      }

        RNYGCachedMeasurement *newCacheEntry;
      if (performLayout) {
        // Use the single layout cache entry.
        newCacheEntry = &layout->cachedLayout;
      } else {
        // Allocate a new measurement cache entry.
        newCacheEntry = &layout->cachedMeasurements[layout->nextCachedMeasurementsIndex];
        layout->nextCachedMeasurementsIndex++;
      }

      newCacheEntry->availableWidth = availableWidth;
      newCacheEntry->availableHeight = availableHeight;
      newCacheEntry->widthMeasureMode = widthMeasureMode;
      newCacheEntry->heightMeasureMode = heightMeasureMode;
      newCacheEntry->computedWidth = layout->measuredDimensions[RNYGDimensionWidth];
      newCacheEntry->computedHeight = layout->measuredDimensions[RNYGDimensionHeight];
    }
  }

  if (performLayout) {
    node->setLayoutDimension(
        node->getLayout().measuredDimensions[RNYGDimensionWidth],
        RNYGDimensionWidth);
    node->setLayoutDimension(
        node->getLayout().measuredDimensions[RNYGDimensionHeight],
        RNYGDimensionHeight);

    node->setHasNewLayout(true);
    node->setDirty(false);
  }

  gDepth--;
  layout->generationCount = gCurrentGenerationCount;
  return (needToVisitNode || cachedResults == nullptr);
}

void RNYGConfigSetPointScaleFactor(const RNYGConfigRef config, const float pixelsInPoint) {
  RNYGAssertWithConfig(config, pixelsInPoint >= 0.0f, "Scale factor should not be less than zero");

  // We store points for Pixel as we will use it for rounding
  if (pixelsInPoint == 0.0f) {
    // Zero is used to skip rounding
    config->pointScaleFactor = 0.0f;
  } else {
    config->pointScaleFactor = pixelsInPoint;
  }
}

static void RNYGRoundToPixelGrid(const RNYGNodeRef node,
                               const float pointScaleFactor,
                               const float absoluteLeft,
                               const float absoluteTop) {
  if (pointScaleFactor == 0.0f) {
    return;
  }

  const float nodeLeft = node->getLayout().position[RNYGEdgeLeft];
  const float nodeTop = node->getLayout().position[RNYGEdgeTop];

  const float nodeWidth = node->getLayout().dimensions[RNYGDimensionWidth];
  const float nodeHeight = node->getLayout().dimensions[RNYGDimensionHeight];

  const float absoluteNodeLeft = absoluteLeft + nodeLeft;
  const float absoluteNodeTop = absoluteTop + nodeTop;

  const float absoluteNodeRight = absoluteNodeLeft + nodeWidth;
  const float absoluteNodeBottom = absoluteNodeTop + nodeHeight;

  // If a node has a custom measure function we never want to round down its size as this could
  // lead to unwanted text truncation.
  const bool textRounding = node->getNodeType() == RNYGNodeTypeText;

  node->setLayoutPosition(
        RNYGRoundValueToPixelGrid(nodeLeft, pointScaleFactor, false, textRounding),
        RNYGEdgeLeft);

  node->setLayoutPosition(
        RNYGRoundValueToPixelGrid(nodeTop, pointScaleFactor, false, textRounding),
        RNYGEdgeTop);

  // We multiply dimension by scale factor and if the result is close to the whole number, we don't
  // have any fraction
  // To verify if the result is close to whole number we want to check both floor and ceil numbers
  const bool hasFractionalWidth = !RNYGFloatsEqual(fmodf(nodeWidth * pointScaleFactor, 1.0), 0) &&
                                  !RNYGFloatsEqual(fmodf(nodeWidth * pointScaleFactor, 1.0), 1.0);
  const bool hasFractionalHeight = !RNYGFloatsEqual(fmodf(nodeHeight * pointScaleFactor, 1.0), 0) &&
                                   !RNYGFloatsEqual(fmodf(nodeHeight * pointScaleFactor, 1.0), 1.0);

  node->setLayoutDimension(
          RNYGRoundValueToPixelGrid(
          absoluteNodeRight,
          pointScaleFactor,
          (textRounding && hasFractionalWidth),
          (textRounding && !hasFractionalWidth)) -
          RNYGRoundValueToPixelGrid(
              absoluteNodeLeft, pointScaleFactor, false, textRounding),
          RNYGDimensionWidth);

  node->setLayoutDimension(
       RNYGRoundValueToPixelGrid(
          absoluteNodeBottom,
          pointScaleFactor,
          (textRounding && hasFractionalHeight),
          (textRounding && !hasFractionalHeight)) -
          RNYGRoundValueToPixelGrid(
              absoluteNodeTop, pointScaleFactor, false, textRounding),
       RNYGDimensionHeight);

  const uint32_t childCount = RNYGNodeGetChildCount(node);
  for (uint32_t i = 0; i < childCount; i++) {
      RNYGRoundToPixelGrid(RNYGNodeGetChild(node, i), pointScaleFactor, absoluteNodeLeft, absoluteNodeTop);
  }
}

void RNYGNodeCalculateLayout(const RNYGNodeRef node,
                           const float parentWidth,
                           const float parentHeight,
                           const RNYGDirection parentDirection) {
  // Increment the generation count. This will force the recursive routine to
  // visit
  // all dirty nodes at least once. Subsequent visits will be skipped if the
  // input
  // parameters don't change.
  gCurrentGenerationCount++;
  node->resolveDimension();
  float width = RNYGUndefined;
    RNYGMeasureMode widthMeasureMode = RNYGMeasureModeUndefined;
  if (RNYGNodeIsStyleDimDefined(node, RNYGFlexDirectionRow, parentWidth)) {
    width =
      RNYGResolveValue(
            node->getResolvedDimension(dim[RNYGFlexDirectionRow]), parentWidth) +
        node->getMarginForAxis(RNYGFlexDirectionRow, parentWidth);
    widthMeasureMode = RNYGMeasureModeExactly;
  } else if (
      RNYGResolveValue(
          node->getStyle().maxDimensions[RNYGDimensionWidth], parentWidth) >=
      0.0f) {
    width = RNYGResolveValue(
        node->getStyle().maxDimensions[RNYGDimensionWidth], parentWidth);
    widthMeasureMode = RNYGMeasureModeAtMost;
  } else {
    width = parentWidth;
    widthMeasureMode = RNYGFloatIsUndefined(width) ? RNYGMeasureModeUndefined : RNYGMeasureModeExactly;
  }

  float height = RNYGUndefined;
    RNYGMeasureMode heightMeasureMode = RNYGMeasureModeUndefined;
  if (RNYGNodeIsStyleDimDefined(node, RNYGFlexDirectionColumn, parentHeight)) {
    height = RNYGResolveValue(
                 node->getResolvedDimension(dim[RNYGFlexDirectionColumn]),
                 parentHeight) +
        node->getMarginForAxis(RNYGFlexDirectionColumn, parentWidth);
    heightMeasureMode = RNYGMeasureModeExactly;
  } else if (
      RNYGResolveValue(
          node->getStyle().maxDimensions[RNYGDimensionHeight], parentHeight) >=
      0.0f) {
    height = RNYGResolveValue(
        node->getStyle().maxDimensions[RNYGDimensionHeight], parentHeight);
    heightMeasureMode = RNYGMeasureModeAtMost;
  } else {
    height = parentHeight;
    heightMeasureMode = RNYGFloatIsUndefined(height) ? RNYGMeasureModeUndefined : RNYGMeasureModeExactly;
  }

  if (RNYGLayoutNodeInternal(
          node,
          width,
          height,
          parentDirection,
          widthMeasureMode,
          heightMeasureMode,
          parentWidth,
          parentHeight,
          true,
          "initial",
          node->getConfig())) {
    node->setPosition(
        node->getLayout().direction, parentWidth, parentHeight, parentWidth);
    RNYGRoundToPixelGrid(node, node->getConfig()->pointScaleFactor, 0.0f, 0.0f);

    if (gPrintTree) {
        RNYGNodePrint(
          node,
          (RNYGPrintOptions)(
            RNYGPrintOptionsLayout | RNYGPrintOptionsChildren |
            RNYGPrintOptionsStyle));
    }
  }

  // We want to get rid off `useLegacyStretchBehaviour` from YGConfig. But we
  // aren't sure whether client's of yoga have gotten rid off this flag or not.
  // So logging this in YGLayout would help to find out the call sites depending
  // on this flag. This check would be removed once we are sure no one is
  // dependent on this flag anymore. The flag
  // `shouldDiffLayoutWithoutLegacyStretchBehaviour` in YGConfig will help to
  // run experiments.
  if (node->getConfig()->shouldDiffLayoutWithoutLegacyStretchBehaviour &&
      node->didUseLegacyFlag()) {
    const RNYGNodeRef originalNode = RNYGNodeDeepClone(node);
    originalNode->resolveDimension();
    // Recursively mark nodes as dirty
    originalNode->markDirtyAndPropogateDownwards();
    gCurrentGenerationCount++;
    // Rerun the layout, and calculate the diff
    originalNode->setAndPropogateUseLegacyFlag(false);
    if (RNYGLayoutNodeInternal(
            originalNode,
            width,
            height,
            parentDirection,
            widthMeasureMode,
            heightMeasureMode,
            parentWidth,
            parentHeight,
            true,
            "initial",
            originalNode->getConfig())) {
      originalNode->setPosition(
          originalNode->getLayout().direction,
          parentWidth,
          parentHeight,
          parentWidth);
      RNYGRoundToPixelGrid(
          originalNode,
          originalNode->getConfig()->pointScaleFactor,
          0.0f,
          0.0f);

      // Set whether the two layouts are different or not.
      node->setLayoutDoesLegacyFlagAffectsLayout(
          !originalNode->isLayoutTreeEqualToNode(*node));

      if (gPrintTree) {
          RNYGNodePrint(
            originalNode,
            (RNYGPrintOptions)(
              RNYGPrintOptionsLayout | RNYGPrintOptionsChildren |
              RNYGPrintOptionsStyle));
      }
    }
      RNYGConfigFreeRecursive(originalNode);
      RNYGNodeFreeRecursive(originalNode);
  }
}

void RNYGConfigSetLogger(const RNYGConfigRef config, RNYGLogger logger) {
  if (logger != nullptr) {
    config->logger = logger;
  } else {
#ifdef ANDROID
    config->logger = &RNYGAndroidLog;
#else
    config->logger = &RNYGDefaultLog;
#endif
  }
}

static void RNYGVLog(const RNYGConfigRef config,
                   const RNYGNodeRef node,
                     RNYGLogLevel level,
                   const char *format,
                   va_list args) {
  const RNYGConfigRef logConfig = config != nullptr ? config : &gYGConfigDefaults;
  logConfig->logger(logConfig, node, level, format, args);

  if (level == RNYGLogLevelFatal) {
    abort();
  }
}

void RNYGLogWithConfig(const RNYGConfigRef config, RNYGLogLevel level, const char *format, ...) {
  va_list args;
  va_start(args, format);
    RNYGVLog(config, nullptr, level, format, args);
  va_end(args);
}

void RNYGLog(const RNYGNodeRef node, RNYGLogLevel level, const char *format, ...) {
  va_list args;
  va_start(args, format);
    RNYGVLog(
      node == nullptr ? nullptr : node->getConfig(), node, level, format, args);
  va_end(args);
}

void RNYGAssert(const bool condition, const char *message) {
  if (!condition) {
      RNYGLog(nullptr, RNYGLogLevelFatal, "%s\n", message);
  }
}

void RNYGAssertWithNode(const RNYGNodeRef node, const bool condition, const char *message) {
  if (!condition) {
      RNYGLog(node, RNYGLogLevelFatal, "%s\n", message);
  }
}

void RNYGAssertWithConfig(const RNYGConfigRef config, const bool condition, const char *message) {
  if (!condition) {
      RNYGLogWithConfig(config, RNYGLogLevelFatal, "%s\n", message);
  }
}

void RNYGConfigSetExperimentalFeatureEnabled(const RNYGConfigRef config,
                                           const RNYGExperimentalFeature feature,
                                           const bool enabled) {
  config->experimentalFeatures[feature] = enabled;
}

inline bool RNYGConfigIsExperimentalFeatureEnabled(const RNYGConfigRef config,
                                                 const RNYGExperimentalFeature feature) {
  return config->experimentalFeatures[feature];
}

void RNYGConfigSetUseWebDefaults(const RNYGConfigRef config, const bool enabled) {
  config->useWebDefaults = enabled;
}

void RNYGConfigSetUseLegacyStretchBehaviour(const RNYGConfigRef config,
                                          const bool useLegacyStretchBehaviour) {
  config->useLegacyStretchBehaviour = useLegacyStretchBehaviour;
}

bool RNYGConfigGetUseWebDefaults(const RNYGConfigRef config) {
  return config->useWebDefaults;
}

void RNYGConfigSetContext(const RNYGConfigRef config, void *context) {
  config->context = context;
}

void *RNYGConfigGetContext(const RNYGConfigRef config) {
  return config->context;
}

void RNYGConfigSetNodeClonedFunc(const RNYGConfigRef config, const RNYGNodeClonedFunc callback) {
  config->cloneNodeCallback = callback;
}
