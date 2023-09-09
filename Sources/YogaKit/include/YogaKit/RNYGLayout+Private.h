/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#import "RNYGLayout.h"
#if FLEXLAYOUT_SWIFT_PACKAGE
#import <yoga/Yoga.h>
#else
#import "RNYoga.h"
#endif

@interface RNYGLayout ()

@property (nonatomic, assign, readonly) RNYGNodeRef node;

- (instancetype)initWithView:(UIView *)view;

@end
