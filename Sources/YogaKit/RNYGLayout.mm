/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#import "RNYGLayout+Private.h"
#import "RNUIView+Yoga.h"

#define RNYG_PROPERTY(type, lowercased_name, capitalized_name)    \
- (type)lowercased_name                                         \
{                                                               \
  return RNYGNodeStyleGet##capitalized_name(self.node);           \
}                                                               \
                                                                \
- (void)set##capitalized_name:(type)lowercased_name             \
{                                                               \
RNYGNodeStyleSet##capitalized_name(self.node, lowercased_name); \
}

#define RNYG_VALUE_PROPERTY(lowercased_name, capitalized_name)                       \
- (RNYGValue)lowercased_name                                                         \
{                                                                                  \
  return RNYGNodeStyleGet##capitalized_name(self.node);                              \
}                                                                                  \
                                                                                   \
- (void)set##capitalized_name:(RNYGValue)lowercased_name                             \
{                                                                                  \
  switch (lowercased_name.unit) {                                                  \
     case RNYGUnitUndefined:                                                         \
RNYGNodeStyleSet##capitalized_name(self.node, lowercased_name.value);          \
      break;                                                                       \
    case RNYGUnitPoint:                                                              \
RNYGNodeStyleSet##capitalized_name(self.node, lowercased_name.value);          \
      break;                                                                       \
    case RNYGUnitPercent:                                                            \
RNYGNodeStyleSet##capitalized_name##Percent(self.node, lowercased_name.value); \
      break;                                                                       \
    default:                                                                       \
      NSAssert(NO, @"Not implemented");                                            \
  }                                                                                \
}

#define RNYG_AUTO_VALUE_PROPERTY(lowercased_name, capitalized_name)                  \
- (RNYGValue)lowercased_name                                                         \
{                                                                                  \
  return RNYGNodeStyleGet##capitalized_name(self.node);                              \
}                                                                                  \
                                                                                   \
- (void)set##capitalized_name:(RNYGValue)lowercased_name                             \
{                                                                                  \
  switch (lowercased_name.unit) {                                                  \
    case RNYGUnitPoint:                                                              \
RNYGNodeStyleSet##capitalized_name(self.node, lowercased_name.value);          \
      break;                                                                       \
    case RNYGUnitPercent:                                                            \
RNYGNodeStyleSet##capitalized_name##Percent(self.node, lowercased_name.value); \
      break;                                                                       \
    case RNYGUnitAuto:                                                               \
RNYGNodeStyleSet##capitalized_name##Auto(self.node);                           \
      break;                                                                       \
    default:                                                                       \
      NSAssert(NO, @"Not implemented");                                            \
  }                                                                                \
}

#define RNYG_EDGE_PROPERTY_GETTER(type, lowercased_name, capitalized_name, property, edge) \
- (type)lowercased_name                                                                  \
{                                                                                        \
  return RNYGNodeStyleGet##property(self.node, edge);                                      \
}

#define RNYG_EDGE_PROPERTY_SETTER(lowercased_name, capitalized_name, property, edge) \
- (void)set##capitalized_name:(CGFloat)lowercased_name                             \
{                                                                                  \
RNYGNodeStyleSet##property(self.node, edge, lowercased_name);                      \
}

#define RNYG_EDGE_PROPERTY(lowercased_name, capitalized_name, property, edge)         \
RNYG_EDGE_PROPERTY_GETTER(CGFloat, lowercased_name, capitalized_name, property, edge) \
RNYG_EDGE_PROPERTY_SETTER(lowercased_name, capitalized_name, property, edge)

#define RNYG_VALUE_EDGE_PROPERTY_SETTER(objc_lowercased_name, objc_capitalized_name, c_name, edge) \
- (void)set##objc_capitalized_name:(RNYGValue)objc_lowercased_name                                 \
{                                                                                                \
  switch (objc_lowercased_name.unit) {                                                           \
    case RNYGUnitUndefined:                                                                        \
RNYGNodeStyleSet##c_name(self.node, edge, objc_lowercased_name.value);                       \
      break;                                                                                     \
    case RNYGUnitPoint:                                                                            \
RNYGNodeStyleSet##c_name(self.node, edge, objc_lowercased_name.value);                       \
      break;                                                                                     \
    case RNYGUnitPercent:                                                                          \
RNYGNodeStyleSet##c_name##Percent(self.node, edge, objc_lowercased_name.value);              \
      break;                                                                                     \
    default:                                                                                     \
      NSAssert(NO, @"Not implemented");                                                          \
  }                                                                                              \
}

#define RNYG_VALUE_EDGE_PROPERTY(lowercased_name, capitalized_name, property, edge)   \
RNYG_EDGE_PROPERTY_GETTER(RNYGValue, lowercased_name, capitalized_name, property, edge) \
RNYG_VALUE_EDGE_PROPERTY_SETTER(lowercased_name, capitalized_name, property, edge)

#define RNYG_VALUE_EDGES_PROPERTIES(lowercased_name, capitalized_name)                                                  \
RNYG_VALUE_EDGE_PROPERTY(lowercased_name##Left, capitalized_name##Left, capitalized_name, RNYGEdgeLeft)                   \
RNYG_VALUE_EDGE_PROPERTY(lowercased_name##Top, capitalized_name##Top, capitalized_name, RNYGEdgeTop)                      \
RNYG_VALUE_EDGE_PROPERTY(lowercased_name##Right, capitalized_name##Right, capitalized_name, RNYGEdgeRight)                \
RNYG_VALUE_EDGE_PROPERTY(lowercased_name##Bottom, capitalized_name##Bottom, capitalized_name, RNYGEdgeBottom)             \
RNYG_VALUE_EDGE_PROPERTY(lowercased_name##Start, capitalized_name##Start, capitalized_name, RNYGEdgeStart)                \
RNYG_VALUE_EDGE_PROPERTY(lowercased_name##End, capitalized_name##End, capitalized_name, RNYGEdgeEnd)                      \
RNYG_VALUE_EDGE_PROPERTY(lowercased_name##Horizontal, capitalized_name##Horizontal, capitalized_name, RNYGEdgeHorizontal) \
RNYG_VALUE_EDGE_PROPERTY(lowercased_name##Vertical, capitalized_name##Vertical, capitalized_name, RNYGEdgeVertical)       \
RNYG_VALUE_EDGE_PROPERTY(lowercased_name, capitalized_name, capitalized_name, RNYGEdgeAll)

RNYGValue RNYGPointValue(CGFloat value)
{
  return (RNYGValue) { .value = (float) value, .unit = (RNYGUnit) RNYGUnitPoint };
}

RNYGValue RNYGPercentValue(CGFloat value)
{
  return (RNYGValue) { .value = (float) value, .unit = RNYGUnitPercent };
}

static RNYGConfigRef globalConfig;

@interface RNYGLayout ()

@property (nonatomic, weak, readonly) UIView *view;

@end

@implementation RNYGLayout

@synthesize isEnabled=_isEnabled;
@synthesize isIncludedInLayout=_isIncludedInLayout;
@synthesize node=_node;

+ (void)initialize
{
  globalConfig = RNYGConfigNew();
    RNYGConfigSetExperimentalFeatureEnabled(globalConfig, RNYGExperimentalFeatureWebFlexBasis, true);
    RNYGConfigSetPointScaleFactor(globalConfig, [UIScreen mainScreen].scale);
}

- (instancetype)initWithView:(UIView*)view
{
  if (self = [super init]) {
    _view = view;
    _node = RNYGNodeNewWithConfig(globalConfig);
      RNYGNodeSetContext(_node, (__bridge void *) view);
    _isEnabled = NO;
    _isIncludedInLayout = YES;

    if ([view isKindOfClass:[UILabel class]]) {
      if (RNYGNodeGetBaselineFunc(_node) == NULL) {
          RNYGNodeSetBaselineFunc(_node, RNYGMeasureBaselineLabel);
      }
    }

    if ([view isKindOfClass:[UITextView class]]) {
      if (RNYGNodeGetBaselineFunc(_node) == NULL) {
          RNYGNodeSetBaselineFunc(_node, RNYGMeasureBaselineTextView);
      }
    }

    if ([view isKindOfClass:[UITextField class]]) {
      if (RNYGNodeGetBaselineFunc(_node) == NULL) {
          RNYGNodeSetBaselineFunc(_node, RNYGMeasureBaselineTextField);
      }
    }
  }

  return self;
}

- (void)dealloc
{
    RNYGNodeFree(self.node);
}

- (BOOL)isDirty
{
  return RNYGNodeIsDirty(self.node);
}

- (void)markDirty
{
  if (self.isDirty || !self.isLeaf) {
    return;
  }

  // Yoga is not happy if we try to mark a node as "dirty" before we have set
  // the measure function. Since we already know that this is a leaf,
  // this *should* be fine. Forgive me Hack Gods.
  const RNYGNodeRef node = self.node;
  if (RNYGNodeGetMeasureFunc(node) == NULL) {
      RNYGNodeSetMeasureFunc(node, RNYGMeasureView);
  }

    RNYGNodeMarkDirty(node);
}

- (NSUInteger)numberOfChildren
{
  return RNYGNodeGetChildCount(self.node);
}

- (BOOL)isLeaf
{
  NSAssert([NSThread isMainThread], @"This method must be called on the main thread.");
  if (self.isEnabled) {
    for (UIView *subview in self.view.subviews) {
        RNYGLayout *const yoga = subview.yoga;
      if (yoga.isEnabled && yoga.isIncludedInLayout) {
        return NO;
      }
    }
  }

  return YES;
}

#pragma mark - Style

- (RNYGPositionType)position
{
  return RNYGNodeStyleGetPositionType(self.node);
}

- (void)setPosition:(RNYGPositionType)position
{
    RNYGNodeStyleSetPositionType(self.node, position);
}

RNYG_PROPERTY(RNYGDirection, direction, Direction)
RNYG_PROPERTY(RNYGFlexDirection, flexDirection, FlexDirection)
RNYG_PROPERTY(RNYGJustify, justifyContent, JustifyContent)
RNYG_PROPERTY(RNYGAlign, alignContent, AlignContent)
RNYG_PROPERTY(RNYGAlign, alignItems, AlignItems)
RNYG_PROPERTY(RNYGAlign, alignSelf, AlignSelf)
RNYG_PROPERTY(RNYGWrap, flexWrap, FlexWrap)
RNYG_PROPERTY(RNYGOverflow, overflow, Overflow)
RNYG_PROPERTY(RNYGDisplay, display, Display)

RNYG_PROPERTY(CGFloat, flexGrow, FlexGrow)
RNYG_PROPERTY(CGFloat, flexShrink, FlexShrink)
RNYG_AUTO_VALUE_PROPERTY(flexBasis, FlexBasis)

RNYG_VALUE_EDGE_PROPERTY(left, Left, Position, RNYGEdgeLeft)
RNYG_VALUE_EDGE_PROPERTY(top, Top, Position, RNYGEdgeTop)
RNYG_VALUE_EDGE_PROPERTY(right, Right, Position, RNYGEdgeRight)
RNYG_VALUE_EDGE_PROPERTY(bottom, Bottom, Position, RNYGEdgeBottom)
RNYG_VALUE_EDGE_PROPERTY(start, Start, Position, RNYGEdgeStart)
RNYG_VALUE_EDGE_PROPERTY(end, End, Position, RNYGEdgeEnd)
RNYG_VALUE_EDGES_PROPERTIES(margin, Margin)
RNYG_VALUE_EDGES_PROPERTIES(padding, Padding)

RNYG_EDGE_PROPERTY(borderLeftWidth, BorderLeftWidth, Border, RNYGEdgeLeft)
RNYG_EDGE_PROPERTY(borderTopWidth, BorderTopWidth, Border, RNYGEdgeTop)
RNYG_EDGE_PROPERTY(borderRightWidth, BorderRightWidth, Border, RNYGEdgeRight)
RNYG_EDGE_PROPERTY(borderBottomWidth, BorderBottomWidth, Border, RNYGEdgeBottom)
RNYG_EDGE_PROPERTY(borderStartWidth, BorderStartWidth, Border, RNYGEdgeStart)
RNYG_EDGE_PROPERTY(borderEndWidth, BorderEndWidth, Border, RNYGEdgeEnd)
RNYG_EDGE_PROPERTY(borderWidth, BorderWidth, Border, RNYGEdgeAll)

RNYG_AUTO_VALUE_PROPERTY(width, Width)
RNYG_AUTO_VALUE_PROPERTY(height, Height)
RNYG_VALUE_PROPERTY(minWidth, MinWidth)
RNYG_VALUE_PROPERTY(minHeight, MinHeight)
RNYG_VALUE_PROPERTY(maxWidth, MaxWidth)
RNYG_VALUE_PROPERTY(maxHeight, MaxHeight)
RNYG_PROPERTY(CGFloat, aspectRatio, AspectRatio)

#pragma mark - Layout and Sizing

- (RNYGDirection)resolvedDirection
{
  return RNYGNodeLayoutGetDirection(self.node);
}

- (void)applyLayout
{
  [self calculateLayoutWithSize:self.view.bounds.size];
    RNYGApplyLayoutToViewHierarchy(self.view, NO);
}

- (void)applyLayoutPreservingOrigin:(BOOL)preserveOrigin
{
  [self calculateLayoutWithSize:self.view.bounds.size];
    RNYGApplyLayoutToViewHierarchy(self.view, preserveOrigin);
}

- (void)applyLayoutPreservingOrigin:(BOOL)preserveOrigin dimensionFlexibility:(RNYGDimensionFlexibility)dimensionFlexibility
{
  CGSize size = self.view.bounds.size;
  if (dimensionFlexibility & RNYGDimensionFlexibilityFlexibleWidth) {
    size.width = RNYGUndefined;
  }
  if (dimensionFlexibility & RNYGDimensionFlexibilityFlexibleHeight) {
    size.height = RNYGUndefined;
  }
  [self calculateLayoutWithSize:size];
    RNYGApplyLayoutToViewHierarchy(self.view, preserveOrigin);
}


- (CGSize)intrinsicSize
{
  CGSize constrainedSize = {
    .width = RNYGUndefined,
    .height = RNYGUndefined,
  };
  constrainedSize = [self calculateLayoutWithSize:constrainedSize];
  return (CGSize){
    .width = RNYGRoundPixelValue(constrainedSize.width),
    .height = RNYGRoundPixelValue(constrainedSize.height),
  };
}

- (CGSize)calculateLayoutWithSize:(CGSize)size
{
  NSAssert([NSThread isMainThread], @"Yoga calculation must be done on main.");
  NSAssert(self.isEnabled, @"Yoga is not enabled for this view.");

  RNYGAttachNodesFromViewHierachy(self.view);

  const RNYGNodeRef node = self.node;
  RNYGNodeCalculateLayout(
    node,
    size.width,
    size.height,
    RNYGNodeStyleGetDirection(node));

  return (CGSize) {
    .width = RNYGNodeLayoutGetWidth(node),
    .height = RNYGNodeLayoutGetHeight(node),
  };
}

#pragma mark - Private

static float RNYGMeasureBaselineLabel(
  RNYGNodeRef node,
  const float width,
  const float height) {

  UILabel* view = (__bridge UILabel*) RNYGNodeGetContext(node);
  return view.font.ascender; // height + view.font.ascender for lastBaseline
}

static float RNYGMeasureBaselineTextView(
  RNYGNodeRef node,
  const float width,
  const float height) {

  UITextView* view = (__bridge UITextView*) RNYGNodeGetContext(node);
  return view.font.ascender + view.contentInset.top + view.textContainerInset.top;
}

static float RNYGMeasureBaselineTextField(
  RNYGNodeRef node,
  const float width,
  const float height) {

  UITextField* view = (__bridge UITextField*) RNYGNodeGetContext(node);

  switch (view.borderStyle) {
  case UITextBorderStyleNone:
    return view.font.ascender;
  case UITextBorderStyleLine:
    return view.font.ascender + 4;
  case UITextBorderStyleBezel:
  case UITextBorderStyleRoundedRect:
    return view.font.ascender + 7;
  }
}

static RNYGSize RNYGMeasureView(
  RNYGNodeRef node,
  float width,
  RNYGMeasureMode widthMode,
  float height,
  RNYGMeasureMode heightMode)
{
  const CGFloat constrainedWidth = (widthMode == RNYGMeasureModeUndefined) ? CGFLOAT_MAX : width;
  const CGFloat constrainedHeight = (heightMode == RNYGMeasureModeUndefined) ? CGFLOAT_MAX: height;

  UIView *view = (__bridge UIView*) RNYGNodeGetContext(node);
  const CGSize sizeThatFits = [view sizeThatFits:(CGSize) {
    .width = constrainedWidth,
    .height = constrainedHeight,
  }];

  return (RNYGSize) {
    .width = (float) RNYGSanitizeMeasurement(constrainedWidth, sizeThatFits.width, widthMode),
    .height = (float) RNYGSanitizeMeasurement(constrainedHeight, sizeThatFits.height, heightMode),
  };
}

static CGFloat RNYGSanitizeMeasurement(
  CGFloat constrainedSize,
  CGFloat measuredSize,
  RNYGMeasureMode measureMode)
{
  CGFloat result;
  if (measureMode == RNYGMeasureModeExactly) {
    result = constrainedSize;
  } else if (measureMode == RNYGMeasureModeAtMost) {
    result = MIN(constrainedSize, measuredSize);
  } else {
    result = measuredSize;
  }

  return result;
}

static BOOL RNYGNodeHasExactSameChildren(const RNYGNodeRef node, NSArray<UIView *> *subviews)
{
  if (RNYGNodeGetChildCount(node) != subviews.count) {
    return NO;
  }

  for (int i=0; i<subviews.count; i++) {
    if (RNYGNodeGetChild(node, i) != subviews[i].yoga.node) {
      return NO;
    }
  }

  return YES;
}

static void RNYGAttachNodesFromViewHierachy(UIView *const view)
{
  RNYGLayout *const yoga = view.yoga;
  const RNYGNodeRef node = yoga.node;

  // Only leaf nodes should have a measure function
  if (yoga.isLeaf) {
    RNYGRemoveAllChildren(node);
    RNYGNodeSetMeasureFunc(node, RNYGMeasureView);
  } else {
    RNYGNodeSetMeasureFunc(node, NULL);

    NSMutableArray<UIView *> *subviewsToInclude = [[NSMutableArray alloc] initWithCapacity:view.subviews.count];
    for (UIView *subview in view.subviews) {
      if (subview.yoga.isIncludedInLayout) {
        [subviewsToInclude addObject:subview];
      }
    }

    if (!RNYGNodeHasExactSameChildren(node, subviewsToInclude)) {
      RNYGRemoveAllChildren(node);
      for (int i=0; i<subviewsToInclude.count; i++) {
        RNYGNodeRef child = subviewsToInclude[i].yoga.node;
        RNYGNodeRef parent = RNYGNodeGetParent(child);
        if (parent != NULL) {
          RNYGNodeRemoveChild(parent, child);
        }
        RNYGNodeInsertChild(node, child, i);
      }
    }

    for (UIView *const subview in subviewsToInclude) {
      RNYGAttachNodesFromViewHierachy(subview);
    }
  }
}

static void RNYGRemoveAllChildren(const RNYGNodeRef node)
{
  if (node == NULL) {
    return;
  }

  while (RNYGNodeGetChildCount(node) > 0) {
      RNYGNodeRemoveChild(node, RNYGNodeGetChild(node, RNYGNodeGetChildCount(node) - 1));
  }
}

static CGFloat RNYGRoundPixelValue(CGFloat value)
{
  static CGFloat scale;
  static dispatch_once_t onceToken;
  dispatch_once(&onceToken, ^(){
    scale = [UIScreen mainScreen].scale;
  });

  return roundf(value * scale) / scale;
}

static void RNYGApplyLayoutToViewHierarchy(UIView *view, BOOL preserveOrigin)
{
  NSCAssert([NSThread isMainThread], @"Framesetting should only be done on the main thread.");

  const RNYGLayout *yoga = view.yoga;

  if (!yoga.isIncludedInLayout) {
     return;
  }

  RNYGNodeRef node = yoga.node;
  const CGPoint topLeft = {
      RNYGNodeLayoutGetLeft(node),
      RNYGNodeLayoutGetTop(node),
  };
  CGPoint bottomRight = {
    topLeft.x + RNYGNodeLayoutGetWidth(node),
    topLeft.y + RNYGNodeLayoutGetHeight(node),
  };
	
  if (isnan(bottomRight.x)) {
    bottomRight.x = 0;
  }
  if (isnan(bottomRight.y)) {
    bottomRight.y = 0;
  }

  const CGPoint origin = preserveOrigin ? view.frame.origin : CGPointZero;
  view.frame = (CGRect) {
    .origin = {
      .x = RNYGRoundPixelValue(topLeft.x + origin.x),
      .y = RNYGRoundPixelValue(topLeft.y + origin.y),
    },
    .size = {
      .width = MAX(0, RNYGRoundPixelValue(bottomRight.x - topLeft.x)),
      .height = MAX(0, RNYGRoundPixelValue(bottomRight.y - topLeft.y)),
    },
  };

  if (!yoga.isLeaf) {
    for (NSUInteger i=0; i<view.subviews.count; i++) {
        RNYGApplyLayoutToViewHierarchy(view.subviews[i], NO);
    }
  }
}

@end
