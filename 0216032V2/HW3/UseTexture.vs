attribute vec3 tangent;
//attribute vec3 bitangent;
varying vec3  lightDir, eyeDir;
varying vec3 varN, varT, varB;

void main()
{	
	varN = gl_NormalMatrix * gl_Normal;  // 法相量

	vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);  //light and eye 需要做model view 轉換

	varT = gl_NormalMatrix * tangent;
	//varB = gl_NormalMatrix * bitangent;

	lightDir = vec3(gl_LightSource[0].position.xyz - vVertex);
	eyeDir = -vVertex;
	
	gl_TexCoord[0].xy = gl_MultiTexCoord0.xy;
	gl_TexCoord[1].xy = gl_MultiTexCoord1.xy;
	gl_TexCoord[2].xy = gl_MultiTexCoord2.xy;

	gl_Position = ftransform();	
	//gl_Position = gl_ProjectionMatrix*gl_ModelViewMatrix*gl_Vertex;
}