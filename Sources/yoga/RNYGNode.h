/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once
#include <stdio.h>
#include "RNYGLayout.h"
#include "RNYGStyle.h"
#include "Yoga-internal.h"

struct RNYGNode {
 private:
  void* context_;
    RNYGPrintFunc print_;
  bool hasNewLayout_;
    RNYGNodeType nodeType_;
    RNYGMeasureFunc measure_;
    RNYGBaselineFunc baseline_;
    RNYGDirtiedFunc dirtied_;
    RNYGStyle style_;
    RNYGLayout layout_;
  uint32_t lineIndex_;
    RNYGNodeRef parent_;
    RNYGVector children_;
    RNYGNodeRef nextChild_;
    RNYGConfigRef config_;
  bool isDirty_;
  std::array<RNYGValue, 2> resolvedDimensions_;

  float relativePosition(const RNYGFlexDirection axis, const float axisSize);

 public:
    RNYGNode();
  ~RNYGNode();
  explicit RNYGNode(const RNYGConfigRef newConfig);
    RNYGNode(const RNYGNode& node);
    RNYGNode& operator=(const RNYGNode& node);
    RNYGNode(
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
      std::array<RNYGValue, 2> resolvedDimensions);

  // Getters
  void* getContext() const;
    RNYGPrintFunc getPrintFunc() const;
  bool getHasNewLayout() const;
    RNYGNodeType getNodeType() const;
    RNYGMeasureFunc getMeasure() const;
    RNYGBaselineFunc getBaseline() const;
    RNYGDirtiedFunc getDirtied() const;
  // For Performance reasons passing as reference.
    RNYGStyle& getStyle();
  // For Performance reasons passing as reference.
    RNYGLayout& getLayout();
  uint32_t getLineIndex() const;
    RNYGNodeRef getParent() const;
    RNYGVector getChildren() const;
  uint32_t getChildrenCount() const;
    RNYGNodeRef getChild(uint32_t index) const;
    RNYGNodeRef getNextChild() const;
    RNYGConfigRef getConfig() const;
  bool isDirty() const;
  std::array<RNYGValue, 2> getResolvedDimensions() const;
    RNYGValue getResolvedDimension(int index);

  // Methods related to positions, margin, padding and border
  float getLeadingPosition(const RNYGFlexDirection axis, const float axisSize);
  bool isLeadingPositionDefined(const RNYGFlexDirection axis);
  bool isTrailingPosDefined(const RNYGFlexDirection axis);
  float getTrailingPosition(const RNYGFlexDirection axis, const float axisSize);
  float getLeadingMargin(const RNYGFlexDirection axis, const float widthSize);
  float getTrailingMargin(const RNYGFlexDirection axis, const float widthSize);
  float getLeadingBorder(const RNYGFlexDirection flexDirection);
  float getTrailingBorder(const RNYGFlexDirection flexDirection);
  float getLeadingPadding(const RNYGFlexDirection axis, const float widthSize);
  float getTrailingPadding(const RNYGFlexDirection axis, const float widthSize);
  float getLeadingPaddingAndBorder(
      const RNYGFlexDirection axis,
      const float widthSize);
  float getTrailingPaddingAndBorder(
      const RNYGFlexDirection axis,
      const float widthSize);
  float getMarginForAxis(const RNYGFlexDirection axis, const float widthSize);
  // Setters

  void setContext(void* context);
  void setPrintFunc(RNYGPrintFunc printFunc);
  void setHasNewLayout(bool hasNewLayout);
  void setNodeType(RNYGNodeType nodeTye);
  void setMeasureFunc(RNYGMeasureFunc measureFunc);
  void setBaseLineFunc(RNYGBaselineFunc baseLineFunc);
  void setDirtiedFunc(RNYGDirtiedFunc dirtiedFunc);
  void setStyle(RNYGStyle style);
  void setStyleFlexDirection(RNYGFlexDirection direction);
  void setStyleAlignContent(RNYGAlign alignContent);
  void setLayout(RNYGLayout layout);
  void setLineIndex(uint32_t lineIndex);
  void setParent(RNYGNodeRef parent);
  void setChildren(RNYGVector children);
  void setNextChild(RNYGNodeRef nextChild);
  void setConfig(RNYGConfigRef config);
  void setDirty(bool isDirty);
  void setLayoutLastParentDirection(RNYGDirection direction);
  void setLayoutComputedFlexBasis(float computedFlexBasis);
  void setLayoutComputedFlexBasisGeneration(
      uint32_t computedFlexBasisGeneration);
  void setLayoutMeasuredDimension(float measuredDimension, int index);
  void setLayoutHadOverflow(bool hadOverflow);
  void setLayoutDimension(float dimension, int index);
  void setLayoutDirection(RNYGDirection direction);
  void setLayoutMargin(float margin, int index);
  void setLayoutBorder(float border, int index);
  void setLayoutPadding(float padding, int index);
  void setLayoutPosition(float position, int index);
  void setPosition(
      const RNYGDirection direction,
      const float mainSize,
      const float crossSize,
      const float parentWidth);
  void setAndPropogateUseLegacyFlag(bool useLegacyFlag);
  void setLayoutDoesLegacyFlagAffectsLayout(bool doesLegacyFlagAffectsLayout);
  void setLayoutDidUseLegacyFlag(bool didUseLegacyFlag);
  void markDirtyAndPropogateDownwards();

  // Other methods
    RNYGValue marginLeadingValue(const RNYGFlexDirection axis) const;
    RNYGValue marginTrailingValue(const RNYGFlexDirection axis) const;
    RNYGValue resolveFlexBasisPtr() const;
  void resolveDimension();
    RNYGDirection resolveDirection(const RNYGDirection parentDirection);
  void clearChildren();
  /// Replaces the occurrences of oldChild with newChild
  void replaceChild(RNYGNodeRef oldChild, RNYGNodeRef newChild);
  void replaceChild(RNYGNodeRef child, uint32_t index);
  void insertChild(RNYGNodeRef child, uint32_t index);
  /// Removes the first occurrence of child
  bool removeChild(RNYGNodeRef child);
  void removeChild(uint32_t index);

  void cloneChildrenIfNeeded();
  void markDirtyAndPropogate();
  float resolveFlexGrow();
  float resolveFlexShrink();
  bool isNodeFlexible();
  bool didUseLegacyFlag();
  bool isLayoutTreeEqualToNode(const RNYGNode& node) const;
};
