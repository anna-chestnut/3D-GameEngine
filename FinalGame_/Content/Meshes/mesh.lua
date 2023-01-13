--[[
	This is an example of tables nested within the main asset table
]]

return
{
	-- (using a dictionary)
	--parameters = 
	--{
	--	vertexCount = 4,
	--	indexCountToRender = 6,
	--}
	
	-- (using an array)
	-- Use left-handed as default
	indexData =
	{
		0, 3, 1,
		0, 2, 3
	},

	vertexData  = 
	{
		1.0, 1.0, 0.0,

		0.0, 1.0, 0.0,

		1.0, 0.0, 0.0,

		0.0, 0.0, 0.0,
	},

}
