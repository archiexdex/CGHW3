varying vec3 normal, lightDir, eyeDir;
uniform sampler2D colorTexture_N,colorTexture_D,colorTexture_S;

void main ()
{
    vec3 cN,T,B,N;
    vec4 cD,cS;

    cN = texture2D(colorTexture_N,gl_TexCoord[0].xy).rgb;
    cD = texture2D(colorTexture_D,gl_TexCoord[0].xy).rgba;
    cS = texture2D(colorTexture_S,gl_TexCoord[0].xy).rgba;

    cN = normalize((cN-0.5)*2);
	T = normalize(gl_TexCoord[1].rgb);
	B = normalize(gl_TexCoord[2].rgb);
    N = normalize(normal);

    vec3 new_normal = cN.r*T.rgb + cN.g*B.rgb + cN.b*N;
    new_normal = normalize(new_normal);

    vec4 final_color = (gl_FrontLightModelProduct.sceneColor * gl_FrontMaterial.ambient) + 
    (gl_LightSource[0].ambient * gl_FrontMaterial.ambient);

    vec3 L = normalize(lightDir);
 
    float lambertTerm = dot(new_normal,L);

    if(lambertTerm > 0.0)
    {
        final_color += gl_LightSource[0].diffuse * cD * lambertTerm ;
        vec3 E = normalize(eyeDir);
        vec3 R = reflect(-L, new_normal);
        float specular = pow( max(dot(R, E), 0.0), gl_FrontMaterial.shininess );
        final_color += gl_LightSource[0].specular * cS * specular;
    }

    gl_FragColor = final_color;
}