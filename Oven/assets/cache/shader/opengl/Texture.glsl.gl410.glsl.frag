#version 410

struct _18
{
    vec4 _m0;
    vec2 _m1;
    vec2 _m2;
    float _m3;
};

uniform sampler2D _16[16];

layout(location = 0) out vec4 _9;
layout(location = 0) in _18 _20;
layout(location = 1) out int _45;
layout(location = 4) flat in int _47;

void main()
{
    _9 = texture(_16[int(_20._m3)], _20._m1 * _20._m2) * _20._m0;
    _45 = _47;
}

