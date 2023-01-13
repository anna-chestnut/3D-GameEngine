--[[
	This file lists every asset that must be built by the AssetBuildSystem
]]

return
{
	shaders =
	{
		{ path = "Shaders/Vertex/standard.shader", arguments = { "vertex" } },
		{ path = "Shaders/Fragment/standard.shader", arguments = { "fragment" } },
		{ path = "Shaders/Fragment/animatedColor.shader", arguments = { "fragment" } },
		{ path = "Shaders/Fragment/myColor.shader", arguments = { "fragment" } },
		{ path = "Shaders/Fragment/redcolor.shader", arguments = { "fragment" } },
		{ path = "Shaders/Fragment/brownColor.shader", arguments = { "fragment" } },
		{ path = "Shaders/Fragment/animatedColorCustomer.shader", arguments = { "fragment" } },

		{ path = "Shaders/Vertex/vertexInputLayout_mesh.shader", arguments = { "vertex" } },
	},

	meshes = 
	{
		"Meshes/mesh.lua",
		"Meshes/mesh2.lua",
		"Meshes/plane.lua",
		"Meshes/cube.lua",
		"Meshes/sphere.lua",
		"Meshes/table.lua",
		"Meshes/astro.lua",
		"Meshes/customer.lua",
		"Meshes/cup.lua",
	},
}
