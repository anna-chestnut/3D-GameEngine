	#version 420
layout( std140, binding = 0 ) uniform g_constantBuffer_frame
{
	mat4 g_transform_worldToCamera;
	mat4 g_transform_cameraToProjected;
	float g_elapsedSecondCount_systemTime;
	float g_elapsedSecondCount_simulationTime;
	vec2 g_padding;
};
layout( std140, binding = 2 ) uniform g_constantBuffer_drawCall
{
  mat4 g_transform_localToWorld;
};
out vec4 o_color;
void main()
{
	o_color = vec4(
		0.56, 0.38, 0.02,
		1.0 );
		
}
