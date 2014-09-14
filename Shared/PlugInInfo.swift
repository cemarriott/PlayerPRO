//
//  PPPlugInInfo.swift
//  PPMacho
//
//  Created by C.W. Betts on 7/24/14.
//
//

import Cocoa

func ==(lhs: PlugInInfo, rhs: PlugInInfo) -> Bool {
	if (lhs === rhs) {
		return true;
	}
	
	if (lhs.plugName != rhs.plugName) {
		return false;
	} else if (lhs.authorName != rhs.authorName) {
		return false;
	} else if (lhs.plugType != rhs.plugType) {
		return false;
	} else {
		return true;
	}
}

class PlugInInfo: NSObject, Hashable, DebugPrintable, Printable {
	let plugName: String
	let authorName: String
	let plugType: String
	let plugCopyright: String
	let plugURL: NSURL
	
	init(plugName pn: String, author aut: String = "Unknown Author", plugType pt: String = "unknown", plugURL pu: NSURL) {
		let tmpBundle = NSBundle(URL: pu).infoDictionary
		plugName = pn
		authorName = aut
		plugType = pt
		plugURL = pu
		let tmpCopy: AnyObject? = tmpBundle["NSHumanReadableCopyright"]
		if let acopy: AnyObject = tmpCopy {
			plugCopyright = acopy as String
		} else {
			plugCopyright = "No copyright info available"
		}
		super.init()
	}
	
	override var hash: Int { get {
		return self.hashValue
	}}
	
	override var hashValue: Int { get {
		return plugName.hashValue ^ plugType.hashValue ^ authorName.hashValue ^ plugCopyright.hashValue
	}}
	
	override var debugDescription: String { get {
		return "\(plugName): \(authorName) (\(plugCopyright)), \(plugType)"

	}}
	
	override var description: String { get {
		return self.debugDescription
	}}
	
	override func isEqual(object: AnyObject!) -> Bool {
		if (object == nil) {
			return false
		}
		
		if (object === self) {
			return true;
		}
		
		if let unwrapped = object as? PlugInInfo {
			return self == unwrapped
		} else {
			return false;
		}
	}
}
