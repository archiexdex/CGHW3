uniform sampler2D NormalMap;
uniform sampler2D DiffuseMap;
uniform sampler2D SpecularMap;

varying vec3 lightDir, eyeDir;
varying vec3 varN, varT, varB;

void main ()
{
    vec4 final_color = 
	(gl_FrontLightModelProduct.sceneColor * gl_FrontMaterial.ambient) + 
	(gl_LightSource[0].ambient * gl_FrontMaterial.ambient);  //最原始的final color只有ambient term

    vec4 NMap = texture2D(NormalMap, gl_TexCoord[0].xy).rgba * 2.0 - 1.0; 
    vec4 DMap = texture2D(DiffuseMap, gl_TexCoord[1].xy).rgba;
    vec4 SMap = texture2D(SpecularMap, gl_TexCoord[2].xy).rgba;
    vec3 N = normalize(varN);
    vec3 T = normalize(varT);
    vec3 B = normalize(cross(N, T));
	//vec3 B = normalize(varB);
    
    vec3 New_N = normalize(vec3(NMap.r*T + NMap.g*B + NMap.b*N));
    vec3 L = normalize(lightDir);

    float lambertTerm = dot(New_N,L);

    if(lambertTerm > 0.0)
	{
        final_color += gl_LightSource[0].diffuse * DMap.rgb * lambertTerm;	// final color 再加上diffuse term
        vec3 E = normalize(eyeDir);
		vec3 R = reflect(-L, New_N);
		float specular = pow( max(dot(R, E), 0.0),   //cos當指數
		                 gl_FrontMaterial.shininess );
        final_color += gl_LightSource[0].specular * SMap.rgb * specular;	// final color 再加上specular term
	}

	gl_FragColor = final_color;	
    //gl_FragColor.a = 1.0;
}