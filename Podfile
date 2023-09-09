source 'https://cdn.cocoapods.org/'
use_frameworks!
platform :ios, '11.0'

workspace 'FlexLayout.xcworkspace'

target 'FlexLayoutTests' do
  project 'FlexLayout.xcodeproj'
  pod 'RNFlexLayout', :path => './'

  pod 'Quick'
  pod 'Nimble', :inhibit_warnings => true
end

target 'FlexLayoutSample' do
  project 'Example/FlexLayoutSample.xcodeproj'

  pod 'RNFlexLayout', :path => './'
  pod 'PinLayout'

  # Debug only
  pod 'SwiftLint'
end

