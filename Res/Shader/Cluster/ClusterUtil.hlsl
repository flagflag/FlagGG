// from screen coordinates (gl_FragCoord) to eye space
// 注意这里之前是屏幕空间，现在改成了viewport空间
float4 Screen2Eye(float4 coord)
{
#if SHADER_LANGUAGE_GLSL || SHADER_LANGUAGE_GLSL_HLSLCC
    // https://www.khronos.org/opengl/wiki/Compute_eye_space_from_window_space
//    float3 ndc = float3(
//        2.0 * (coord.x - viewRect.x) / viewRect.z - 1.0,
//        2.0 * (coord.y - viewRect.y) / viewRect.w - 1.0,
//        2.0 * coord.z - 1.0 // -> [-1, 1]
//    );

    float3 ndc = float3(
        2.0 * (coord.x - 0.0) / viewRect.z - 1.0,
        2.0 * (coord.y - 0.0) / viewRect.w - 1.0,
        2.0 * coord.z - 1.0 // -> [-1, 1]
    );
#else
//    float3 ndc = float3(
//        2.0 * (coord.x - viewRect.x) / viewRect.z - 1.0,
//        2.0 * (viewRect.w - coord.y - 1 - viewRect.y) / viewRect.w - 1.0, // y is flipped
//        coord.z // -> [0, 1]
//    );
    float3 ndc = float3(
        2.0 * (coord.x - 0.0) / viewRect.z - 1.0,
        2.0 * (viewRect.w - coord.y - 1 - 0.0) / viewRect.w - 1.0, // y is flipped
        coord.z // -> [0, 1]
    );
#endif

    // https://stackoverflow.com/a/16597492/862300
    // 注意行列矩阵，sample和我们相反
    float4 eye = mul(float4(ndc, 1.0), invProjMatrix);
    eye = eye / eye.w;

    return eye;
}

// depth from screen coordinates (gl_FragCoord.z) to eye space
// same as Screen2Eye(float4(0, 0, depth, 1)).z but slightly faster
// (!) this assumes a perspective projection as created by bx::mtxProj
// for a left-handed coordinate system
float Screen2EyeDepth(float depth, float near, float far)
{
    // https://stackoverflow.com/a/45710371/862300

#if SHADER_LANGUAGE_GLSL || SHADER_LANGUAGE_GLSL_HLSLCC
    float ndc = 2.0 * depth - 1.0;
    // ndc = (eye * (far + near) / (far - near) - 2 * (far * near) / (far - near)) / eye
    float eye = 2.0 * far * near / (far + near + ndc * (near - far));
#else
    float ndc = depth;
    // ndc = (eye * far / (far - near) - (far * near) / (far - near)) / eye
    float eye = far * near / (far + ndc * (near - far));
#endif

    return eye;
}
