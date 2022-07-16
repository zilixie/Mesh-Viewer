#version 150 core
#define M_PI 3.141592653589793238

out vec4 outColor;
in vec3 fs_vn;
in vec2 fs_text_coord;
in vec3 fs_fn;
in vec3 fs_tangt;
in vec4 fs_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 camera;
uniform mat4 proj;
uniform vec3 diffuse;
uniform vec3 specular;
uniform vec3 ambient;
uniform vec3 light_pos;
uniform float p;

uniform sampler2D tex_normal0;
uniform sampler2D tex_normal1;
uniform sampler2D tex_normal2;
uniform sampler2D tex_normal3;

float ni = 1.0;
float nt = 2.0;
float alpha = 1.0;

vec3 blinn_phong(vec3 ka, vec3 kd, vec3 ks, float p, vec3 n, vec3 v, vec3 l) {
	vec3 I = vec3(1,1,1);
	float factor = max(n.x * l.x + n.y * l.y + n.z * l.z, -(n.x * l.x + n.y * l.y + n.z * l.z));

	vec3 diffuse = factor * I * kd;

	vec3 h = normalize(v + l);
	factor = pow(max((n.x * h.x + n.y * h.y + n.z * h.z), -(n.x * h.x + n.y * h.y + n.z * h.z)), p);
	vec3 specular = factor * I * ks;

	vec3 ambient = ka;
	return ambient + diffuse + specular;
}

// diffuse, specular, gloss, normal 
vec3 ShadePBRSpecular(vec3 C_diff, vec3 F0, float a, vec3 N, vec3 L, vec3 V) {
	return C_diff;
}


void main() {
	vec4 texture_color = texture(tex_normal3, fs_text_coord);
	vec3 texture_normal = texture(tex_normal1, fs_text_coord).xyz;

	//vec3 kd = diffuse/255.0;
	vec3 kd = texture_color.xyz;
	vec3 ka = ambient/(10 * 255.0);
	vec3 ks = specular/255.0;

	vec3 bi_norm = cross(fs_vn, fs_tangt);
	mat3 toTangentSpc = mat3(
						fs_tangt.x, bi_norm.x, fs_vn.x,
						fs_tangt.y, bi_norm.y, fs_vn.y,
						fs_tangt.z, bi_norm.z, fs_vn.z);
	texture_normal = normalize(texture_normal * 2.0 - 1.0);


	vec4 vertex_normal_4d = model * vec4(fs_vn[0], fs_vn[1], fs_vn[2], 0.0);
	vec3 vertex_normal_3d = vec3(vertex_normal_4d[0], vertex_normal_4d[1], vertex_normal_4d[2]);

	vec3 light_d = normalize(light_pos);
	float r = length(light_pos - fs_pos.xyz);
	vec3 L = normalize(light_pos);
	vec3 V = normalize(light_pos);

	if (vertex_normal_3d.x * light_d.x + vertex_normal_3d.y * light_d.y + vertex_normal_3d.z * light_d.z < 0) {
		vertex_normal_3d = -vertex_normal_3d;
	}

	//vec3 tngt_light = toTangentSpc * light_d;
	//if (vertex_normal_3d.x * light_d.x + vertex_normal_3d.y * light_d.y + vertex_normal_3d.z * light_d.z < 0) {
	//	vertex_normal_3d = -vertex_normal_3d;
	//}
	//if (texture_normal.x * tngt_light.x + texture_normal.y * tngt_light.y + texture_normal.z * tngt_light.z < 0) {
	//	texture_normal = -texture_normal;
	//}
	//outColor = vec4(blinn_phong(ka, kd, ks, p, normalize(vertex_normal_3d), light_d, light_d), 1.0);


	outColor = vec4(blinn_phong(ka, kd, ks, p, normalize(texture_normal), toTangentSpc * light_d, toTangentSpc * light_d), 1.0);
}
