/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#import <UIKit/UIKit.h>

#if FLEXLAYOUT_SWIFT_PACKAGE
#import <yoga/YGEnums.h>
#import <yoga/Yoga.h>
#import <yoga/YGMacros.h>
#else
#import "RNYGEnums.h"
#import "RNYoga.h"
#import "RNYGMacros.h"
#endif

RNYG_EXTERN_C_BEGIN

extern RNYGValue RNYGPointValue(CGFloat value)
    NS_SWIFT_UNAVAILABLE("Use the swift Int and FloatingPoint extensions instead");
extern RNYGValue RNYGPercentValue(CGFloat value)
    NS_SWIFT_UNAVAILABLE("Use the swift Int and FloatingPoint extensions instead");

RNYG_EXTERN_C_END

typedef NS_OPTIONS(NSInteger, RNYGDimensionFlexibility) {
    RNYGDimensionFlexibilityFlexibleWidth = 1 << 0,
    RNYGDimensionFlexibilityFlexibleHeight = 1 << 1,
};

@interface RNYGLayout : NSObject

/**
 Make default init unavailable, as it will not initialise YGNode which is
 required for the setters and getters of YGLayout's properties to work properly.
*/
- (instancetype)init
    __attribute__((unavailable("you are not meant to initialise RNYGLayout")));

/**
  The property that decides if we should include this view when calculating
  layout. Defaults to YES.
 */
@property (nonatomic, readwrite, assign, setter=setIncludedInLayout:) BOOL isIncludedInLayout;

/**
 The property that decides during layout/sizing whether or not styling properties should be applied.
 Defaults to NO.
 */
@property (nonatomic, readwrite, assign, setter=setEnabled:) BOOL isEnabled;

@property (nonatomic, readwrite, assign) RNYGDirection direction;
@property (nonatomic, readwrite, assign) RNYGFlexDirection flexDirection;
@property (nonatomic, readwrite, assign) RNYGJustify justifyContent;
@property (nonatomic, readwrite, assign) RNYGAlign alignContent;
@property (nonatomic, readwrite, assign) RNYGAlign alignItems;
@property (nonatomic, readwrite, assign) RNYGAlign alignSelf;
@property (nonatomic, readwrite, assign) RNYGPositionType position;
@property (nonatomic, readwrite, assign) RNYGWrap flexWrap;
@property (nonatomic, readwrite, assign) RNYGOverflow overflow;
@property (nonatomic, readwrite, assign) RNYGDisplay display;

@property (nonatomic, readwrite, assign) CGFloat flexGrow;
@property (nonatomic, readwrite, assign) CGFloat flexShrink;
@property (nonatomic, readwrite, assign) RNYGValue flexBasis;

@property (nonatomic, readwrite, assign) RNYGValue left;
@property (nonatomic, readwrite, assign) RNYGValue top;
@property (nonatomic, readwrite, assign) RNYGValue right;
@property (nonatomic, readwrite, assign) RNYGValue bottom;
@property (nonatomic, readwrite, assign) RNYGValue start;
@property (nonatomic, readwrite, assign) RNYGValue end;

@property (nonatomic, readwrite, assign) RNYGValue marginLeft;
@property (nonatomic, readwrite, assign) RNYGValue marginTop;
@property (nonatomic, readwrite, assign) RNYGValue marginRight;
@property (nonatomic, readwrite, assign) RNYGValue marginBottom;
@property (nonatomic, readwrite, assign) RNYGValue marginStart;
@property (nonatomic, readwrite, assign) RNYGValue marginEnd;
@property (nonatomic, readwrite, assign) RNYGValue marginHorizontal;
@property (nonatomic, readwrite, assign) RNYGValue marginVertical;
@property (nonatomic, readwrite, assign) RNYGValue margin;

@property (nonatomic, readwrite, assign) RNYGValue paddingLeft;
@property (nonatomic, readwrite, assign) RNYGValue paddingTop;
@property (nonatomic, readwrite, assign) RNYGValue paddingRight;
@property (nonatomic, readwrite, assign) RNYGValue paddingBottom;
@property (nonatomic, readwrite, assign) RNYGValue paddingStart;
@property (nonatomic, readwrite, assign) RNYGValue paddingEnd;
@property (nonatomic, readwrite, assign) RNYGValue paddingHorizontal;
@property (nonatomic, readwrite, assign) RNYGValue paddingVertical;
@property (nonatomic, readwrite, assign) RNYGValue padding;

@property (nonatomic, readwrite, assign) CGFloat borderLeftWidth;
@property (nonatomic, readwrite, assign) CGFloat borderTopWidth;
@property (nonatomic, readwrite, assign) CGFloat borderRightWidth;
@property (nonatomic, readwrite, assign) CGFloat borderBottomWidth;
@property (nonatomic, readwrite, assign) CGFloat borderStartWidth;
@property (nonatomic, readwrite, assign) CGFloat borderEndWidth;
@property (nonatomic, readwrite, assign) CGFloat borderWidth;

@property (nonatomic, readwrite, assign) RNYGValue width;
@property (nonatomic, readwrite, assign) RNYGValue height;
@property (nonatomic, readwrite, assign) RNYGValue minWidth;
@property (nonatomic, readwrite, assign) RNYGValue minHeight;
@property (nonatomic, readwrite, assign) RNYGValue maxWidth;
@property (nonatomic, readwrite, assign) RNYGValue maxHeight;

// Yoga specific properties, not compatible with flexbox specification
@property (nonatomic, readwrite, assign) CGFloat aspectRatio;

/**
 Get the resolved direction of this node. This won't be YGDirectionInherit
 */
@property (nonatomic, readonly, assign) RNYGDirection resolvedDirection;

/**
 Perform a layout calculation and update the frames of the views in the hierarchy with the results.
 If the origin is not preserved, the root view's layout results will applied from {0,0}.
 */
- (void)applyLayoutPreservingOrigin:(BOOL)preserveOrigin
    NS_SWIFT_NAME(applyLayout(preservingOrigin:));

/**
 Perform a layout calculation and update the frames of the views in the hierarchy with the results.
 If the origin is not preserved, the root view's layout results will applied from {0,0}.
 */
- (void)applyLayoutPreservingOrigin:(BOOL)preserveOrigin
               dimensionFlexibility:(RNYGDimensionFlexibility)dimensionFlexibility
    NS_SWIFT_NAME(applyLayout(preservingOrigin:dimensionFlexibility:));

/**
 Returns the size of the view if no constraints were given. This could equivalent to calling [self
 sizeThatFits:CGSizeMake(CGFLOAT_MAX, CGFLOAT_MAX)];
 */
@property (nonatomic, readonly, assign) CGSize intrinsicSize;

/**
  Returns the size of the view based on provided constraints. Pass NaN for an unconstrained dimension.
 */
- (CGSize)calculateLayoutWithSize:(CGSize)size
    NS_SWIFT_NAME(calculateLayout(with:));

/**
 Returns the number of children that are using Flexbox.
 */
@property (nonatomic, readonly, assign) NSUInteger numberOfChildren;

/**
 Return a BOOL indiciating whether or not we this node contains any subviews that are included in
 Yoga's layout.
 */
@property (nonatomic, readonly, assign) BOOL isLeaf;

/**
 Return's a BOOL indicating if a view is dirty. When a node is dirty
 it usually indicates that it will be remeasured on the next layout pass.
 */
@property (nonatomic, readonly, assign) BOOL isDirty;

/**
 Mark that a view's layout needs to be recalculated. Only works for leaf views.
 */
- (void)markDirty;

@end
