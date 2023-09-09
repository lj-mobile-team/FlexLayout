/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "RNYGNode.h"
#include <iostream>
#include "RNUtils.h"

void* RNYGNode::getContext() const {
  return context_;
}

RNYGPrintFunc RNYGNode::getPrintFunc() const {
  return print_;
}

bool RNYGNode::getHasNewLayout() const {
  return hasNewLayout_;
}

RNYGNodeType RNYGNode::getNodeType() const {
  return nodeType_;
}

RNYGMeasureFunc RNYGNode::getMeasure() const {
  return measure_;
}

RNYGBaselineFunc RNYGNode::getBaseline() const {
  return baseline_;
}

RNYGDirtiedFunc RNYGNode::getDirtied() const {
  return dirtied_;
}

RNYGStyle& RNYGNode::getStyle() {
  return style_;
}

RNYGLayout& RNYGNode::getLayout() {
  return layout_;
}

uint32_t RNYGNode::getLineIndex() const {
  return lineIndex_;
}

RNYGNodeRef RNYGNode::getParent() const {
  return parent_;
}

RNYGVector RNYGNode::getChildren() const {
  return children_;
}

uint32_t RNYGNode::getChildrenCount() const {
  return static_cast<uint32_t>(children_.size());
}

RNYGNodeRef RNYGNode::getChild(uint32_t index) const {
  return children_.at(index);
}

RNYGNodeRef RNYGNode::getNextChild() const {
  return nextChild_;
}

RNYGConfigRef RNYGNode::getConfig() const {
  return config_;
}

bool RNYGNode::isDirty() const {
  return isDirty_;
}

RNYGValue RNYGNode::getResolvedDimension(int index) {
  return resolvedDimensions_[index];
}

std::array<RNYGValue, 2> RNYGNode::getResolvedDimensions() const {
  return resolvedDimensions_;
}

float RNYGNode::getLeadingPosition(
    const RNYGFlexDirection axis,
    const float axisSize) {
  if (RNYGFlexDirectionIsRow(axis)) {
    const RNYGValue* leadingPosition =
      RNYGComputedEdgeValue(style_.position, RNYGEdgeStart, &RNYGValueUndefined);
    if (leadingPosition->unit != RNYGUnitUndefined) {
      return RNYGResolveValue(*leadingPosition, axisSize);
    }
  }

  const RNYGValue* leadingPosition =
    RNYGComputedEdgeValue(style_.position, leading[axis], &RNYGValueUndefined);

  return leadingPosition->unit == RNYGUnitUndefined
      ? 0.0f
      : RNYGResolveValue(*leadingPosition, axisSize);
}

float RNYGNode::getTrailingPosition(
    const RNYGFlexDirection axis,
    const float axisSize) {
  if (RNYGFlexDirectionIsRow(axis)) {
    const RNYGValue* trailingPosition =
      RNYGComputedEdgeValue(style_.position, RNYGEdgeEnd, &RNYGValueUndefined);
    if (trailingPosition->unit != RNYGUnitUndefined) {
      return RNYGResolveValue(*trailingPosition, axisSize);
    }
  }

  const RNYGValue* trailingPosition =
    RNYGComputedEdgeValue(style_.position, trailing[axis], &RNYGValueUndefined);

  return trailingPosition->unit == RNYGUnitUndefined
      ? 0.0f
      : RNYGResolveValue(*trailingPosition, axisSize);
}

bool RNYGNode::isLeadingPositionDefined(const RNYGFlexDirection axis) {
  return (RNYGFlexDirectionIsRow(axis) &&
          RNYGComputedEdgeValue(style_.position, RNYGEdgeStart, &RNYGValueUndefined)
                  ->unit != RNYGUnitUndefined) ||
    RNYGComputedEdgeValue(style_.position, leading[axis], &RNYGValueUndefined)
          ->unit != RNYGUnitUndefined;
}

bool RNYGNode::isTrailingPosDefined(const RNYGFlexDirection axis) {
  return (RNYGFlexDirectionIsRow(axis) &&
          RNYGComputedEdgeValue(style_.position, RNYGEdgeEnd, &RNYGValueUndefined)
                  ->unit != RNYGUnitUndefined) ||
    RNYGComputedEdgeValue(style_.position, trailing[axis], &RNYGValueUndefined)
          ->unit != RNYGUnitUndefined;
}

float RNYGNode::getLeadingMargin(
    const RNYGFlexDirection axis,
    const float widthSize) {
  if (RNYGFlexDirectionIsRow(axis) &&
      style_.margin[RNYGEdgeStart].unit != RNYGUnitUndefined) {
    return RNYGResolveValueMargin(style_.margin[RNYGEdgeStart], widthSize);
  }

  return RNYGResolveValueMargin(
      *RNYGComputedEdgeValue(style_.margin, leading[axis], &RNYGValueZero),
      widthSize);
}

float RNYGNode::getTrailingMargin(
    const RNYGFlexDirection axis,
    const float widthSize) {
  if (RNYGFlexDirectionIsRow(axis) &&
      style_.margin[RNYGEdgeEnd].unit != RNYGUnitUndefined) {
    return RNYGResolveValueMargin(style_.margin[RNYGEdgeEnd], widthSize);
  }

  return RNYGResolveValueMargin(
      *RNYGComputedEdgeValue(style_.margin, trailing[axis], &RNYGValueZero),
      widthSize);
}

float RNYGNode::getMarginForAxis(
    const RNYGFlexDirection axis,
    const float widthSize) {
  return getLeadingMargin(axis, widthSize) + getTrailingMargin(axis, widthSize);
}

// Setters

void RNYGNode::setContext(void* context) {
  context_ = context;
}

void RNYGNode::setPrintFunc(RNYGPrintFunc printFunc) {
  print_ = printFunc;
}

void RNYGNode::setHasNewLayout(bool hasNewLayout) {
  hasNewLayout_ = hasNewLayout;
}

void RNYGNode::setNodeType(RNYGNodeType nodeType) {
  nodeType_ = nodeType;
}

void RNYGNode::setStyleFlexDirection(RNYGFlexDirection direction) {
  style_.flexDirection = direction;
}

void RNYGNode::setStyleAlignContent(RNYGAlign alignContent) {
  style_.alignContent = alignContent;
}

void RNYGNode::setMeasureFunc(RNYGMeasureFunc measureFunc) {
  if (measureFunc == nullptr) {
    measure_ = nullptr;
    // TODO: t18095186 Move nodeType to opt-in function and mark appropriate
    // places in Litho
    nodeType_ = RNYGNodeTypeDefault;
  } else {
      RNYGAssertWithNode(
        this,
        children_.size() == 0,
        "Cannot set measure function: Nodes with measure functions cannot have children.");
    measure_ = measureFunc;
    // TODO: t18095186 Move nodeType to opt-in function and mark appropriate
    // places in Litho
    setNodeType(RNYGNodeTypeText);
  }

  measure_ = measureFunc;
}

void RNYGNode::setBaseLineFunc(RNYGBaselineFunc baseLineFunc) {
  baseline_ = baseLineFunc;
}

void RNYGNode::setDirtiedFunc(RNYGDirtiedFunc dirtiedFunc) {
  dirtied_ = dirtiedFunc;
}

void RNYGNode::setStyle(RNYGStyle style) {
  style_ = style;
}

void RNYGNode::setLayout(RNYGLayout layout) {
  layout_ = layout;
}

void RNYGNode::setLineIndex(uint32_t lineIndex) {
  lineIndex_ = lineIndex;
}

void RNYGNode::setParent(RNYGNodeRef parent) {
  parent_ = parent;
}

void RNYGNode::setChildren(RNYGVector children) {
  children_ = children;
}

void RNYGNode::setNextChild(RNYGNodeRef nextChild) {
  nextChild_ = nextChild;
}

void RNYGNode::replaceChild(RNYGNodeRef child, uint32_t index) {
  children_[index] = child;
}

void RNYGNode::replaceChild(RNYGNodeRef oldChild, RNYGNodeRef newChild) {
  std::replace(children_.begin(), children_.end(), oldChild, newChild);
}

void RNYGNode::insertChild(RNYGNodeRef child, uint32_t index) {
  children_.insert(children_.begin() + index, child);
}

void RNYGNode::setConfig(RNYGConfigRef config) {
  config_ = config;
}

void RNYGNode::setDirty(bool isDirty) {
  if (isDirty == isDirty_) {
    return;
  }
  isDirty_ = isDirty;
  if (isDirty && dirtied_) {
    dirtied_(this);
  }
}

bool RNYGNode::removeChild(RNYGNodeRef child) {
  std::vector<RNYGNodeRef>::iterator p =
      std::find(children_.begin(), children_.end(), child);
  if (p != children_.end()) {
    children_.erase(p);
    return true;
  }
  return false;
}

void RNYGNode::removeChild(uint32_t index) {
  children_.erase(children_.begin() + index);
}

void RNYGNode::setLayoutDirection(RNYGDirection direction) {
  layout_.direction = direction;
}

void RNYGNode::setLayoutMargin(float margin, int index) {
  layout_.margin[index] = margin;
}

void RNYGNode::setLayoutBorder(float border, int index) {
  layout_.border[index] = border;
}

void RNYGNode::setLayoutPadding(float padding, int index) {
  layout_.padding[index] = padding;
}

void RNYGNode::setLayoutLastParentDirection(RNYGDirection direction) {
  layout_.lastParentDirection = direction;
}

void RNYGNode::setLayoutComputedFlexBasis(float computedFlexBasis) {
  layout_.computedFlexBasis = computedFlexBasis;
}

void RNYGNode::setLayoutPosition(float position, int index) {
  layout_.position[index] = position;
}

void RNYGNode::setLayoutComputedFlexBasisGeneration(
    uint32_t computedFlexBasisGeneration) {
  layout_.computedFlexBasisGeneration = computedFlexBasisGeneration;
}

void RNYGNode::setLayoutMeasuredDimension(float measuredDimension, int index) {
  layout_.measuredDimensions[index] = measuredDimension;
}

void RNYGNode::setLayoutHadOverflow(bool hadOverflow) {
  layout_.hadOverflow = hadOverflow;
}

void RNYGNode::setLayoutDimension(float dimension, int index) {
  layout_.dimensions[index] = dimension;
}

// If both left and right are defined, then use left. Otherwise return
// +left or -right depending on which is defined.
float RNYGNode::relativePosition(
    const RNYGFlexDirection axis,
    const float axisSize) {
  return isLeadingPositionDefined(axis) ? getLeadingPosition(axis, axisSize)
                                        : -getTrailingPosition(axis, axisSize);
}

void RNYGNode::setPosition(
    const RNYGDirection direction,
    const float mainSize,
    const float crossSize,
    const float parentWidth) {
  /* Root nodes should be always layouted as LTR, so we don't return negative
   * values. */
  const RNYGDirection directionRespectingRoot =
      parent_ != nullptr ? direction : RNYGDirectionLTR;
  const RNYGFlexDirection mainAxis =
    RNYGResolveFlexDirection(style_.flexDirection, directionRespectingRoot);
  const RNYGFlexDirection crossAxis =
    RNYGFlexDirectionCross(mainAxis, directionRespectingRoot);

  const float relativePositionMain = relativePosition(mainAxis, mainSize);
  const float relativePositionCross = relativePosition(crossAxis, crossSize);

  setLayoutPosition(
      getLeadingMargin(mainAxis, parentWidth) + relativePositionMain,
      leading[mainAxis]);
  setLayoutPosition(
      getTrailingMargin(mainAxis, parentWidth) + relativePositionMain,
      trailing[mainAxis]);
  setLayoutPosition(
      getLeadingMargin(crossAxis, parentWidth) + relativePositionCross,
      leading[crossAxis]);
  setLayoutPosition(
      getTrailingMargin(crossAxis, parentWidth) + relativePositionCross,
      trailing[crossAxis]);
}

RNYGNode::RNYGNode()
    : context_(nullptr),
      print_(nullptr),
      hasNewLayout_(true),
      nodeType_(RNYGNodeTypeDefault),
      measure_(nullptr),
      baseline_(nullptr),
      dirtied_(nullptr),
      style_(RNYGStyle()),
      layout_(RNYGLayout()),
      lineIndex_(0),
      parent_(nullptr),
      children_(RNYGVector()),
      nextChild_(nullptr),
      config_(nullptr),
      isDirty_(false),
      resolvedDimensions_({{RNYGValueUndefined, RNYGValueUndefined}}) {}

RNYGNode::RNYGNode(const RNYGNode& node)
    : context_(node.context_),
      print_(node.print_),
      hasNewLayout_(node.hasNewLayout_),
      nodeType_(node.nodeType_),
      measure_(node.measure_),
      baseline_(node.baseline_),
      dirtied_(node.dirtied_),
      style_(node.style_),
      layout_(node.layout_),
      lineIndex_(node.lineIndex_),
      parent_(node.parent_),
      children_(node.children_),
      nextChild_(node.nextChild_),
      config_(node.config_),
      isDirty_(node.isDirty_),
      resolvedDimensions_(node.resolvedDimensions_) {}

RNYGNode::RNYGNode(const RNYGConfigRef newConfig) : RNYGNode() {
  config_ = newConfig;
}

RNYGNode::RNYGNode(
    void* context,
    RNYGPrintFunc print,
    bool hasNewLayout,
    RNYGNodeType nodeType,
    RNYGMeasureFunc measure,
    RNYGBaselineFunc baseline,
    RNYGDirtiedFunc dirtied,
    RNYGStyle style,
    RNYGLayout layout,
    uint32_t lineIndex,
    RNYGNodeRef parent,
    RNYGVector children,
    RNYGNodeRef nextChild,
    RNYGConfigRef config,
    bool isDirty,
    std::array<RNYGValue, 2> resolvedDimensions)
    : context_(context),
      print_(print),
      hasNewLayout_(hasNewLayout),
      nodeType_(nodeType),
      measure_(measure),
      baseline_(baseline),
      dirtied_(dirtied),
      style_(style),
      layout_(layout),
      lineIndex_(lineIndex),
      parent_(parent),
      children_(children),
      nextChild_(nextChild),
      config_(config),
      isDirty_(isDirty),
      resolvedDimensions_(resolvedDimensions) {}

RNYGNode& RNYGNode::operator=(const RNYGNode& node) {
  if (&node == this) {
    return *this;
  }

  for (auto child : children_) {
    delete child;
  }

  context_ = node.getContext();
  print_ = node.getPrintFunc();
  hasNewLayout_ = node.getHasNewLayout();
  nodeType_ = node.getNodeType();
  measure_ = node.getMeasure();
  baseline_ = node.getBaseline();
  dirtied_ = node.getDirtied();
  style_ = node.style_;
  layout_ = node.layout_;
  lineIndex_ = node.getLineIndex();
  parent_ = node.getParent();
  children_ = node.getChildren();
  nextChild_ = node.getNextChild();
  config_ = node.getConfig();
  isDirty_ = node.isDirty();
  resolvedDimensions_ = node.getResolvedDimensions();

  return *this;
}

RNYGValue RNYGNode::marginLeadingValue(const RNYGFlexDirection axis) const {
  if (RNYGFlexDirectionIsRow(axis) &&
      style_.margin[RNYGEdgeStart].unit != RNYGUnitUndefined) {
    return style_.margin[RNYGEdgeStart];
  } else {
    return style_.margin[leading[axis]];
  }
}

RNYGValue RNYGNode::marginTrailingValue(const RNYGFlexDirection axis) const {
  if (RNYGFlexDirectionIsRow(axis) &&
      style_.margin[RNYGEdgeEnd].unit != RNYGUnitUndefined) {
    return style_.margin[RNYGEdgeEnd];
  } else {
    return style_.margin[trailing[axis]];
  }
}

RNYGValue RNYGNode::resolveFlexBasisPtr() const {
    RNYGValue flexBasis = style_.flexBasis;
  if (flexBasis.unit != RNYGUnitAuto && flexBasis.unit != RNYGUnitUndefined) {
    return flexBasis;
  }
  if (!RNYGFloatIsUndefined(style_.flex) && style_.flex > 0.0f) {
    return config_->useWebDefaults ? RNYGValueAuto : RNYGValueZero;
  }
  return RNYGValueAuto;
}

void RNYGNode::resolveDimension() {
  for (uint32_t dim = RNYGDimensionWidth; dim < RNYGDimensionCount; dim++) {
    if (getStyle().maxDimensions[dim].unit != RNYGUnitUndefined &&
        RNYGValueEqual(
            getStyle().maxDimensions[dim], style_.minDimensions[dim])) {
      resolvedDimensions_[dim] = style_.maxDimensions[dim];
    } else {
      resolvedDimensions_[dim] = style_.dimensions[dim];
    }
  }
}

RNYGDirection RNYGNode::resolveDirection(const RNYGDirection parentDirection) {
  if (style_.direction == RNYGDirectionInherit) {
    return parentDirection > RNYGDirectionInherit ? parentDirection
                                                : RNYGDirectionLTR;
  } else {
    return style_.direction;
  }
}

void RNYGNode::clearChildren() {
  children_.clear();
  children_.shrink_to_fit();
}

RNYGNode::~RNYGNode() {
  // All the member variables are deallocated externally, so no need to
  // deallocate here
}

// Other Methods

void RNYGNode::cloneChildrenIfNeeded() {
  // YGNodeRemoveChild in yoga.cpp has a forked variant of this algorithm
  // optimized for deletions.

  const uint32_t childCount = static_cast<uint32_t>(children_.size());
  if (childCount == 0) {
    // This is an empty set. Nothing to clone.
    return;
  }

  const RNYGNodeRef firstChild = children_.front();
  if (firstChild->getParent() == this) {
    // If the first child has this node as its parent, we assume that it is
    // already unique. We can do this because if we have it has a child, that
    // means that its parent was at some point cloned which made that subtree
    // immutable. We also assume that all its sibling are cloned as well.
    return;
  }

  const RNYGNodeClonedFunc cloneNodeCallback = config_->cloneNodeCallback;
  for (uint32_t i = 0; i < childCount; ++i) {
    const RNYGNodeRef oldChild = children_[i];
    const RNYGNodeRef newChild = RNYGNodeClone(oldChild);
    replaceChild(newChild, i);
    newChild->setParent(this);
    if (cloneNodeCallback) {
      cloneNodeCallback(oldChild, newChild, this, i);
    }
  }
}

void RNYGNode::markDirtyAndPropogate() {
  if (!isDirty_) {
    setDirty(true);
    setLayoutComputedFlexBasis(RNYGUndefined);
    if (parent_) {
      parent_->markDirtyAndPropogate();
    }
  }
}

void RNYGNode::markDirtyAndPropogateDownwards() {
  isDirty_ = true;
  for_each(children_.begin(), children_.end(), [](RNYGNodeRef childNode) {
    childNode->markDirtyAndPropogateDownwards();
  });
}

float RNYGNode::resolveFlexGrow() {
  // Root nodes flexGrow should always be 0
  if (parent_ == nullptr) {
    return 0.0;
  }
  if (!RNYGFloatIsUndefined(style_.flexGrow)) {
    return style_.flexGrow;
  }
  if (!RNYGFloatIsUndefined(style_.flex) && style_.flex > 0.0f) {
    return style_.flex;
  }
  return kDefaultFlexGrow;
}

float RNYGNode::resolveFlexShrink() {
  if (parent_ == nullptr) {
    return 0.0;
  }
  if (!RNYGFloatIsUndefined(style_.flexShrink)) {
    return style_.flexShrink;
  }
  if (!config_->useWebDefaults && !RNYGFloatIsUndefined(style_.flex) &&
      style_.flex < 0.0f) {
    return -style_.flex;
  }
  return config_->useWebDefaults ? kWebDefaultFlexShrink : kDefaultFlexShrink;
}

bool RNYGNode::isNodeFlexible() {
  return (
      (style_.positionType == RNYGPositionTypeRelative) &&
      (resolveFlexGrow() != 0 || resolveFlexShrink() != 0));
}

float RNYGNode::getLeadingBorder(const RNYGFlexDirection axis) {
  if (RNYGFlexDirectionIsRow(axis) &&
      style_.border[RNYGEdgeStart].unit != RNYGUnitUndefined &&
      style_.border[RNYGEdgeStart].value >= 0.0f) {
    return style_.border[RNYGEdgeStart].value;
  }

  return fmaxf(
               RNYGComputedEdgeValue(style_.border, leading[axis], &RNYGValueZero)->value,
      0.0f);
}

float RNYGNode::getTrailingBorder(const RNYGFlexDirection flexDirection) {
  if (RNYGFlexDirectionIsRow(flexDirection) &&
      style_.border[RNYGEdgeEnd].unit != RNYGUnitUndefined &&
      style_.border[RNYGEdgeEnd].value >= 0.0f) {
    return style_.border[RNYGEdgeEnd].value;
  }

  return fmaxf(
               RNYGComputedEdgeValue(style_.border, trailing[flexDirection], &RNYGValueZero)
          ->value,
      0.0f);
}

float RNYGNode::getLeadingPadding(
    const RNYGFlexDirection axis,
    const float widthSize) {
  if (RNYGFlexDirectionIsRow(axis) &&
      style_.padding[RNYGEdgeStart].unit != RNYGUnitUndefined &&
      RNYGResolveValue(style_.padding[RNYGEdgeStart], widthSize) >= 0.0f) {
    return RNYGResolveValue(style_.padding[RNYGEdgeStart], widthSize);
  }
  return fmaxf(
               RNYGResolveValue(
          *RNYGComputedEdgeValue(style_.padding, leading[axis], &RNYGValueZero),
          widthSize),
      0.0f);
}

float RNYGNode::getTrailingPadding(
    const RNYGFlexDirection axis,
    const float widthSize) {
  if (RNYGFlexDirectionIsRow(axis) &&
      style_.padding[RNYGEdgeEnd].unit != RNYGUnitUndefined &&
      RNYGResolveValue(style_.padding[RNYGEdgeEnd], widthSize) >= 0.0f) {
    return RNYGResolveValue(style_.padding[RNYGEdgeEnd], widthSize);
  }
  return fmaxf(
               RNYGResolveValue(
          *RNYGComputedEdgeValue(style_.padding, trailing[axis], &RNYGValueZero),
          widthSize),
      0.0f);
}

float RNYGNode::getLeadingPaddingAndBorder(
    const RNYGFlexDirection axis,
    const float widthSize) {
  return getLeadingPadding(axis, widthSize) + getLeadingBorder(axis);
}

float RNYGNode::getTrailingPaddingAndBorder(
    const RNYGFlexDirection axis,
    const float widthSize) {
  return getTrailingPadding(axis, widthSize) + getTrailingBorder(axis);
}

bool RNYGNode::didUseLegacyFlag() {
  bool didUseLegacyFlag = layout_.didUseLegacyFlag;
  if (didUseLegacyFlag) {
    return true;
  }
  for (const auto& child : children_) {
    if (child->layout_.didUseLegacyFlag) {
      didUseLegacyFlag = true;
      break;
    }
  }
  return didUseLegacyFlag;
}

void RNYGNode::setAndPropogateUseLegacyFlag(bool useLegacyFlag) {
  config_->useLegacyStretchBehaviour = useLegacyFlag;
  for_each(children_.begin(), children_.end(), [=](RNYGNodeRef childNode) {
    childNode->getConfig()->useLegacyStretchBehaviour = useLegacyFlag;
  });
}

void RNYGNode::setLayoutDoesLegacyFlagAffectsLayout(
    bool doesLegacyFlagAffectsLayout) {
  layout_.doesLegacyStretchFlagAffectsLayout = doesLegacyFlagAffectsLayout;
}

void RNYGNode::setLayoutDidUseLegacyFlag(bool didUseLegacyFlag) {
  layout_.didUseLegacyFlag = didUseLegacyFlag;
}

bool RNYGNode::isLayoutTreeEqualToNode(const RNYGNode& node) const {
  if (children_.size() != node.children_.size()) {
    return false;
  }
  if (layout_ != node.layout_) {
    return false;
  }
  if (children_.size() == 0) {
    return true;
  }

  bool isLayoutTreeEqual = true;
    RNYGNodeRef otherNodeChildren = nullptr;
  for (std::vector<RNYGNodeRef>::size_type i = 0; i < children_.size(); ++i) {
    otherNodeChildren = node.children_[i];
    isLayoutTreeEqual =
        children_[i]->isLayoutTreeEqualToNode(*otherNodeChildren);
    if (!isLayoutTreeEqual) {
      return false;
    }
  }
  return isLayoutTreeEqual;
}
