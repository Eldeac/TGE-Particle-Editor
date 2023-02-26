----------------------------------------------------------------------------
-- the dirs table is a listing of absolute paths, since we generate projects
-- and files it makes a lot of sense to make them absolute to avoid problems
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
dirs = {}
dirs["root"] 			= os.realpath("../")
dirs["bin"]				= os.realpath(dirs.root .. "Bin/")
dirs["temp"]			= os.realpath(dirs.root .. "Temp/")
dirs["lib"]				= os.realpath(dirs.root .. "Lib/")
dirs["projectfiles"]	= os.realpath(dirs.root .. "Local/")
dirs["source"] 			= os.realpath(dirs.root .. "Source/")
dirs["dependencies"]	= os.realpath(dirs.root .. "Dependencies/")
dirs["external"]		= os.realpath(dirs.root .. "Source/External/")
dirs["engine"]			= os.realpath(dirs.root .. "Source/Engine")
dirs["settings"]		= os.realpath(dirs.root .. "Bin/settings/")
dirs["shaders"]			= os.realpath(dirs.root .. "Bin/Shaders/")

engine_settings = os.realpath(dirs.settings .. "/EngineSettings.json")


-----------------------------------------------------------------------
-- These should be more or less equivalent with EngineCreateParameters
-- Some of it can't be set like this, things like callbacks.
function default_settings()
	return {
		engine_assets_path = os.realpath("../EngineAssets/"),
		window_settings = {
			window_rect = {x=0, y=0, w=1600, h=900},
			render_rect = {x=0, y=0, w=1600, h=900},
			title = "TGE - Never give up on your dreams!",
	 		clear_color = {r=0.0, g=0.2, b=0.25, a=1.0},
	 		start_in_fullscreen = false
		},
		enable_vsync = true,
		use_letterbox_and_pillarbox = true,
	}
end

if not os.isdir (dirs.bin) then
	os.mkdir (dirs.bin)
end

if not os.isdir(dirs.settings) then 
	os.mkdir (dirs.settings)
end

-------------------------------------------------------------
-- Update engine asset path to absolute path for current user
if os.isfile(engine_settings) then
	print ("Engine asset path set to: " .. path.translate(os.realpath("../EngineAssets/"), '/'))
	local settings = json.decode(io.readfile(engine_settings))
	settings.engine_assets_path = path.translate(os.realpath("../EngineAssets/"), '/')
	io.writefile(
		engine_settings,
		json.encode(settings)
	)
else
	print("Creating Engine settings")
	io.writefile(
		engine_settings,
		json.encode(default_settings())
	)
end

---------------------------------------------------------------------------
-- Utility function to create individual project-settings any game project, 
-- tutorial or similar is responsible for running:
-- Tga::LoadSettings("game_name.json"); at startup
function verify_or_create_settings(game_name)
	local settings_filename = game_name .. ".json"
	defines { 'TGE_PROJECT_SETTINGS_FILE="' .. settings_filename .. '"' }
	local game_settings = dirs["settings"] .. settings_filename
	
	datadir = path.translate(os.realpath("./data/"), "/")
	local settings = nil
	if os.isfile(game_settings) then
		settings = json.decode(io.readfile(game_settings))
		settings.asset_path = datadir
	else
		settings = {
			name = game_name,
			assets_path = datadir
		}
	end

	io.writefile(
		game_settings,
		json.encode(settings)
	)
end