//
//  File.swift
//  PPMacho
//
//  Created by C.W. Betts on 9/6/14.
//
//

import Foundation
import PlayerPROKit

public final class Revert: NSObject, PPDigitalPlugin {
	public var hasUIConfiguration: Bool {
		return false
	}
	
	public convenience init(forPlugIn: ()) {
		self.init()
	}
	
	override public init() {
		super.init()
	}

	public func run(with myPcmd: UnsafeMutablePointer<Pcmd>, driver: PPDriver) throws {
		let srcCmd = PPKPcmd(myPcmd)
		let maxLenth = srcCmd.length - 1
		
		for track in 0 ..< srcCmd.tracks {
			for row in 0 ... maxLenth {
				let myCmdsrc = getCommand(atRow: maxLenth - row, track: track, aIntPcmd: srcCmd)
				replaceCommand(row: row, track: track, command: myCmdsrc, aPcmd: myPcmd)
			}
		}
	}
}
