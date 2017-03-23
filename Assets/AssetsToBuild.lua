--[[
	This file lists every asset that must be built by the AssetBuildSystem
]]

return
{
	-- The top-level table is a dictionary whose keys
	-- correspond to the asset types (inherited from cbAssetTypeInfo)
	-- that you have defined in AssetBuildSystem.lua

	meshes =
	{
		-- The actual assets can be defined simply as relative path strings (the common case)
		"SquareFrame.Lua",
		"CylinderFrame.Lua",
		"Cylinder.Lua",
		"RedSquare.Lua",
		"celing.Lua",
	},
	--[[shaders =
	{
		-- But it is also possible to define them as tables with more information
		-- (shaders must specify an additional command line argument for ShaderBuilder.exe
		-- to specify whether an asset is a vertex shader or a fragment shader)
		{ path = "vertex.shader", arguments = { "vertex" } },
		{ path = "fragment.shader", arguments = { "fragment" } },
	},]]
	--[[effects =
	{
		"shader.effect",
		"shader2.effect",
	},]]
	textures =
	{
		"default.tga",		
	},
	materials =
	{
		"simpleMaterial.material",
		"flashingMaterial.material",
		"spriteMaterial.material",
		"score0.material",
		"score1.material",
		"score2.material",
		"score3.material",
		"score4.material",
		"score5.material",
		"score6.material",
		"score7.material",
		"score8.material",
		"score9.material",
		"life.material",
		"victory.material",
		"start.material",
	},
}
