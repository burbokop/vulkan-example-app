#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 2, set = 2) uniform sampler2D texSampler;

layout(binding = 0, set = 0) uniform UniformBufferObject {
        vec2 offset;
        float iTime;
        vec2 iMouse;
} ubo;

const vec2 iResolution = vec2(1., 1.);

//void main() {
//        outColor = (texture(texSampler, fragTexCoord) * 0.8 + vec4(fragTexCoord, 0.0, 1.0) * 0.2);
//        //outColor = vec4(fragColor, 1.0);
//	//outColor = texture(texSampler, fragTexCoord);
//        //outColor.w = 0.3;
//        //outColor = vec4(fragTexCoord, 0.0, 1.0);
//}




vec3 palette(float g) {
    const int size = 16;
    vec3 values[size] = vec3[size](
        vec3(0.659737, 0.845218, 1.30191),
        vec3(0.750215, 0.825435, 1.26649),
        vec3(0.830502, 0.803991, 1.23102),
        vec3(0.903685, 0.780708, 1.19548),
        vec3(0.971598, 0.75537, 1.15987),
        vec3(1.03543, 0.727703, 1.12415),
        vec3(1.096, 0.69736, 1.0883),
        vec3(1.1539, 0.663879, 1.05231),
        vec3(1.20958, 0.626635, 1.01614),
        vec3(1.26337, 0.58474, 0.979752),
        vec3(1.31555, 0.536861, 0.943113),
        vec3(1.36633, 0.480829, 0.906171),
        vec3(1.41589, 0.412662, 0.86887),
        vec3(1.46437, 0.32334, 0.831142),
        vec3(1.51189, 0.179122, 0.792903),
        vec3(1.55856, -0.428653, 0.75405)
    );
    return values[int(g * float(size)) % size];
}

mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

struct Shape {
    float dst;
    vec3 color;
};

Shape shapeUnion(Shape s0, Shape s1) {
    if(s0.dst < s1.dst) {
        return s0;
    } else {
        return s1;
    }
}

float sphereSD(in vec3 point, in vec3 center, in float radius) {
    return length(point - center) - radius;
}

float triPrismSD(in vec3 p, in vec2 h)
{
  vec3 q = abs(p);
  return max(q.z-h.y,max(q.x*0.866025+p.y*0.5,-p.y)-h.x*0.5);
}

Shape sceneFloor(in vec3 point, in float rd) {
    float dst = point.y + 0.5;
    return Shape(dst, palette(rd + dst));
}

float line(in vec3 point) {
    point -= vec3(0,0,-3.);
    point = (vec4(point, 0.) * rotationMatrix(vec3(0.,1.,0.), ubo.iTime)).xyz;

    return triPrismSD(point, vec2(1., 2.));
}


float sinDisplacement(in vec3 p, in float coef) {
    return sin(coef * p.x) * sin(coef * p.y) * sin(coef * p.z);
}

Shape prismSegment(in vec3 point, vec3 node, vec3 vec, in float rd){
    float dst = length(point - node)-0.5;
    return Shape(dst, palette(rd + dst));
}

Shape prism(in vec3 point, vec3 node, vec3 vec, int i, in float rd) {
    if(i > 5) return Shape(100000., vec3(0.,0.,0.));

    vec3 v0 = vec * sinDisplacement(vec, 20.);
    vec3 v1 = vec * sinDisplacement(vec, 20.);
    vec3 n0 = node + v0;
    vec3 n1 = node + v1;

    float dst = line(point);

    return Shape(dst, palette(rd + dst));

    //return shapeUnion(
    //    prismSegment(point, node, vec, rd),
     //   shapeUnion(
     //       prism(point, v0, n0, i+1, rd),
     //       prism(point, v1, n1, i+1, rd)));
}

Shape scene(in vec3 point, in float rd) {
    return shapeUnion(
        sceneFloor(point, rd),
        prism(point,
            vec3(0,0,-3.),
            vec3(0,-1.,0),
            0,
            rd));
}

void main()
{
    float ratio = iResolution.y / iResolution.x;

    vec3 ro = vec3(fragTexCoord.x / iResolution.x * 2. - 1., (fragTexCoord.y / iResolution.y * 2. - 1.) * ratio, 0);

    vec3 focalPoint = vec3(0, 0, 1.);
    vec3 rv = normalize(ro - focalPoint);

    vec3 background = vec3(1., 0.9, 0.3);
    vec3 color = background;

    float rd = 0.;
    int i = 0;
    int iterationCount = 80;
    for(; i < iterationCount; ++i) {
        vec3 point = ro + rv * rd;

        Shape s = scene(point, rd);

        rd += s.dst;
        if(abs(s.dst) < 0.001) {
            color = s.color;
            break;
        }
    }

    float maxVisibleDistance = 20.;

    if(rd > maxVisibleDistance) {
        rd = maxVisibleDistance;
        color = background;
    }

    float d = mix(rd / maxVisibleDistance, float(i) / float(iterationCount), 0.89);

    outColor = vec4(mix(vec3(0.1,0.1,0.1), color, d), 1);
}

