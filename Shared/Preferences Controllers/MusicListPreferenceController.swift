//
//  MusicListPreferenceController.swift
//  PPMacho
//
//  Created by C.W. Betts on 3/4/15.
//
//

import Cocoa
import MASPreferences.MASPreferencesViewController

enum PlaylistMode: Int {
	case StopPlaying = 0
	case LoopMusic
	case LoadNext
	case LoadRandom
}

final class MusicListPreferenceController: NSViewController, MASPreferencesViewController {
	@IBOutlet weak var askToSave: NSButtonCell!
	@IBOutlet weak var autoPlay: NSButtonCell!
	@IBOutlet weak var goToStartupPos: NSButtonCell!
	@IBOutlet weak var loadFirstMusic: NSButtonCell!
	@IBOutlet weak var musicPlayingOutlet: NSMatrix!
	@IBOutlet weak var rememberMusicList: NSButtonCell!
	@IBOutlet weak var loopMusic: NSButtonCell!
	
	override var identifier: NSUserInterfaceItemIdentifier? {
		get {
			return NSUserInterfaceItemIdentifier(rawValue: PPMusicListPrefID)
		}
		set {
			// do nothing
		}
	}
	
	var toolbarItemLabel: String? {
		return NSLocalizedString("Music List", tableName: "PreferenceNames", comment: "Music List")
	}
	
	var toolbarItemImage: NSImage? {
		return NSImage(named: NSImage.Name(rawValue: "List"))
	}
	
	static func newPreferenceView() -> MusicListPreferenceController? {
		let aCont = MusicListPreferenceController(nibName:NSNib.Name(rawValue: "MusicListPrefs"), bundle:nil)
		aCont.title = NSLocalizedString("Music List", tableName: "PreferenceNames", comment: "Music List")
		return aCont
	}
	
	override func awakeFromNib() {
		super.awakeFromNib()
		
		let defaults = UserDefaults.standard
		
		rememberMusicList.state = defaults.bool(forKey: PPRememberMusicList) ? NSControl.StateValue.onState : NSControl.StateValue.offState
		loadFirstMusic.state = defaults.bool(forKey: PPLoadMusicAtListLoad) ? NSControl.StateValue.onState : .offState
		goToStartupPos.state = defaults.bool(forKey: PPGotoStartupAfterPlaying) ? NSControl.StateValue.onState : .offState
		askToSave.state = defaults.bool(forKey: PPSaveModList) ? NSControl.StateValue.onState : .offState
		autoPlay.state = defaults.bool(forKey: PPLoadMusicAtMusicLoad) ? NSControl.StateValue.onState : .offState
		loopMusic.state = defaults.bool(forKey: PPLoopMusicWhenDone) ? NSControl.StateValue.onState : .offState
		playedMusic = PlaylistMode(rawValue: defaults.integer(forKey: PPAfterPlayingMusic)) ?? .StopPlaying
	}
	
	@IBAction func changePlayedMusic(_ sender: AnyObject?) {
		let defaults = UserDefaults.standard
		
		defaults.set(playedMusic.rawValue, forKey: PPAfterPlayingMusic)
		
		NotificationCenter.default.post(name: .PPListPreferencesDidChange, object: self)
	}
	
	@IBAction func toggleButton(_ sender: AnyObject?) {
		let defaults = UserDefaults.standard
		
		defaults.set(rememberMusicList.state == NSControl.StateValue.onState, forKey: PPRememberMusicList)
		defaults.set(loadFirstMusic.state == NSControl.StateValue.onState, forKey: PPLoadMusicAtListLoad)
		defaults.set(autoPlay.state == NSControl.StateValue.onState, forKey: PPLoadMusicAtMusicLoad)
		defaults.set(goToStartupPos.state == NSControl.StateValue.onState, forKey: PPGotoStartupAfterPlaying)
		defaults.set(loopMusic.state == NSControl.StateValue.onState, forKey: PPLoopMusicWhenDone)
		defaults.set(goToStartupPos.state == NSControl.StateValue.onState, forKey: PPGotoStartupAfterPlaying)
		
		NotificationCenter.default.post(name: .PPListPreferencesDidChange, object: self)
	}
	
	var playedMusic: PlaylistMode {
		get {
			let curSelected = musicPlayingOutlet.selectedCell()
			
			if musicPlayingOutlet.cell(atRow: 0, column: 0) === curSelected {
				return .StopPlaying
			} else if musicPlayingOutlet.cell(atRow: 0, column: 1) === curSelected {
				return .LoadNext
			} else if musicPlayingOutlet.cell(atRow: 1, column: 0) === curSelected {
				return .LoopMusic
			} else if musicPlayingOutlet.cell(atRow: 0, column: 1) === curSelected {
				return .LoadRandom
			} else {
				return .StopPlaying
			}
		}
		set {
			switch newValue {
			case .LoadNext:
				musicPlayingOutlet.selectCell(atRow: 0, column: 1)
				
			case .LoopMusic:
				musicPlayingOutlet.selectCell(atRow: 1, column: 0)
				
			case .LoadRandom:
				musicPlayingOutlet.selectCell(atRow: 1, column: 1)
				
			default:
				musicPlayingOutlet.selectCell(atRow: 0, column: 0)
			}
		}
	}
	
	var hasResizableWidth: Bool {
		return false
	}
	
	var hasResizableHeight: Bool {
		return false
	}
}
