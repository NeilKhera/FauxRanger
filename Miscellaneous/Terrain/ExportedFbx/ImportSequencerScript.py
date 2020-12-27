#This script was generated with the addons Blender for UnrealEngine : https://github.com/xavier150/Blender-For-UnrealEngine-Addons
#It will import into Unreal Engine all the assets of type StaticMesh, SkeletalMesh, Animation and Pose
#The script must be used in Unreal Engine Editor with Python plugins : https://docs.unrealengine.com/en-US/Engine/Editor/ScriptingAndAutomation/Python
#Use this command : py "C:\Users\Neil\Desktop\FauxRanger\Miscellaneous\Terrain\ExportedFbx\ImportSequencerScript.py"


def CheckTasks():
	import unreal
	if hasattr(unreal, 'EditorAssetLibrary') == False:
		print('--------------------------------------------------\n /!\ Warning: Editor Scripting Utilities should be activated.\n Plugin > Scripting > Editor Scripting Utilities.')
		return False
	if hasattr(unreal.MovieSceneSequence, 'set_display_rate') == False:
		print('--------------------------------------------------\n /!\ Warning: Editor Scripting Utilities should be activated.\n Plugin > Scripting > Sequencer Scripting.')
		return False
	return True
	
def CreateSequencer():
		import os.path
		import time
		import ConfigParser
		import unreal
	
	
		seqPath = r"/Game/ImportedFbx/Sequencer"
		seqName = r'MySequence'
		seqTempName = r'MySequence'+str(time.time())
		startFrame = 1
		endFrame = 251
		frameRateDenominator = 1.0
		frameRateNumerator = 24
		secureCrop = 0.0001 #add end crop for avoid section overlay
	
	
		def AddSequencerSectionTransformKeysByIniFile(SequencerSection, SectionFileName, FileLoc):
			Config = ConfigParser.ConfigParser()
			Config.read(FileLoc)
			for option in Config.options(SectionFileName):
				frame = float(option)/float(frameRateNumerator) #FrameRate
				list = Config.get(SectionFileName, option)
				for x in range(0, 9): #(x,y,z x,y,z x,y,z)
					value = float(list.split(',')[x])
					SequencerSection.get_channels()[x].add_key(unreal.FrameNumber(frame*float(frameRateNumerator)),value)
	
	
		def AddSequencerSectionFloatKeysByIniFile(SequencerSection, SectionFileName, FileLoc):
			Config = ConfigParser.ConfigParser()
			Config.read(FileLoc)
			for option in Config.options(SectionFileName):
				frame = float(option)/float(frameRateNumerator) #FrameRate
				value = float(Config.get(SectionFileName, option))
				SequencerSection.get_channels()[0].add_key(unreal.FrameNumber(frame*float(frameRateNumerator)),value)
	
	
		def AddSequencerSectionBoolKeysByIniFile(SequencerSection, SectionFileName, FileLoc):
			Config = ConfigParser.ConfigParser()
			Config.read(FileLoc)
			for option in Config.options(SectionFileName):
				frame = float(option)/float(frameRateNumerator) #FrameRate
				value = Config.getboolean(SectionFileName, option)
				SequencerSection.get_channels()[0].add_key(unreal.FrameNumber(frame*float(frameRateNumerator)),value)
	
	
		print("Warning this file already exists")
		factory = unreal.LevelSequenceFactoryNew()
		asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
		seq = asset_tools.create_asset_with_dialog(seqName.replace('.',''), seqPath, None, factory)
		if seq is None:
			return 'Error /!\ level sequencer factory_create fail' 
	
		print("Sequencer reference created")
		print(seq)
		ImportedCamera = [] #(CameraName, CameraGuid)
		print("========================= Import started ! =========================")
		
		#Set frame rate
		myFFrameRate = unreal.FrameRate()
		myFFrameRate.denominator = frameRateDenominator
		myFFrameRate.numerator = frameRateNumerator
		seq.set_display_rate(myFFrameRate)
		#Set playback range
		seq.set_playback_end_seconds((endFrame-secureCrop)/float(frameRateNumerator))
		seq.set_playback_start_seconds(startFrame/float(frameRateNumerator))
		camera_cut_track = seq.add_master_track(unreal.MovieSceneCameraCutTrack)
	
	
		#Import camera cut section
		camera_cut_section = camera_cut_track.add_section()
		#Not camera found for this section
		camera_cut_section.set_end_frame_seconds((251-secureCrop)/float(frameRateNumerator))
		camera_cut_section.set_start_frame_seconds(1/float(frameRateNumerator))
		print('========================= Imports completed ! =========================')
		
		for cam in ImportedCamera:
			print(cam[0])
		
		print('=========================')
	#Select and open seq in content browser
		unreal.AssetToolsHelpers.get_asset_tools().open_editor_for_assets([unreal.load_asset(seqPath+'/'+seqName.replace('.',''))])
		unreal.EditorAssetLibrary.sync_browser_to_objects([seqPath+'/'+seqName.replace('.','')])
		return 'Sequencer created with success !' 
	
if CheckTasks() == True:
	print(CreateSequencer())
