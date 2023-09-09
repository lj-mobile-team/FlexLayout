/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#if FLEXLAYOUT_SWIFT_PACKAGE
import CoreGraphics
import RNFlexLayoutYoga
#endif

postfix operator %

extension Int {
    public static postfix func % (value: Int) -> RNYGValue {
        return RNYGValue(value: Float(value), unit: .percent)
    }
}

extension Float {
    public static postfix func % (value: Float) -> RNYGValue {
        return RNYGValue(value: value, unit: .percent)
    }
}

extension CGFloat {
    public static postfix func % (value: CGFloat) -> RNYGValue {
        return RNYGValue(value: Float(value), unit: .percent)
    }
}

extension RNYGValue: ExpressibleByIntegerLiteral, ExpressibleByFloatLiteral {
    public init(integerLiteral value: Int) {
        self = RNYGValue(value: Float(value), unit: .point)
    }
    
    public init(floatLiteral value: Float) {
        self = RNYGValue(value: value, unit: .point)
    }
  
    public init(_ value: Float) {
        self = RNYGValue(value: value, unit: .point)
    }
  
    public init(_ value: CGFloat) {
        self = RNYGValue(value: Float(value), unit: .point)
    }
}
