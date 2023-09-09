//
//  FlexLayout+Private.swift
//  FlexLayout
//
//  Created by DION, Luc (MTL) on 2017-11-23.
//  Copyright © 2017 Mirego. All rights reserved.
//

import UIKit
#if FLEXLAYOUT_SWIFT_PACKAGE
import RNFlexLayoutYoga
#endif

extension Flex {
    func valueOrUndefined(_ value: CGFloat?) -> RNYGValue {
        if let value = value {
            return RNYGValue(value)
        } else {
            return RNYGValueUndefined
        }
    }
    
    func valueOrAuto(_ value: CGFloat?) -> RNYGValue {
        if let value = value {
            return RNYGValue(value)
        } else {
            return RNYGValueAuto
        }
    }
}
