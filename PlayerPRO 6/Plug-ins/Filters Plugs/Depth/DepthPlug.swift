//
//  DepthPlug.swift
//  PPMacho
//
//  Created by C.W. Betts on 9/11/14.
//
//

import Foundation
import AppKit.NSWindow
import PlayerPROKit

final public class Depth: NSObject, PPFilterPlugin {
	public var hasUIConfiguration: Bool {
		return true
	}
	
	public convenience init(forPlugIn: ()) {
		self.init()
	}
	
	override public init() {
		super.init()
	}
	
	public func run(withData theData: PPSampleObject, selectionRange selRange: NSRange, onlyCurrentChannel StereoMode: Bool, driver: PPDriver) throws {
		throw PPMADError(.orderNotImplemented)
	}
	
	public func beginRun(withData theData: PPSampleObject, selectionRange selRange: NSRange, onlyCurrentChannel StereoMode: Bool, driver: PPDriver, parentWindow document: NSWindow, handler handle: @escaping PPPlugErrorBlock) {
		let controller = DepthController(windowNibName: NSNib.Name(rawValue: "DepthController"))
		controller.depthAmmount = 8
		controller.theData = theData
		controller.selectionRange = selRange
		controller.stereoMode = StereoMode
		controller.parentWindow = document

		document.beginSheet(controller.window!, completionHandler: { (returnCode) -> Void in
			
		})
	}
}
