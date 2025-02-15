
// 0: classic with weighted sample, 1: don't normalize and adjust the formula to be simpler and faster - can look better and is cheaper (Alchemy like?)
#define OPTIMIZATION_O1 1

// 1:lowest quality, 2:medium , 3:high, more doesn't give too much (maybe HZB mip computations should `be adjusted)
//#define SAMPLE_STEPS 3

// 0:off / 1:show samples on the right side of the screen
#define DEBUG_LOOKUPS 0

// 0:off / 1:take into account scene normals in the computations
#define USE_NORMALS 1

// useful to remove high frequency dither pattern, not that needed with more sample
// 0:off (fast but dither pattern with low sample count), 1:non normal aware (half res look), 2:normal aware (slower), 3:normal and depth aware (slowest, doesn't add much)
//#define QUAD_MESSAGE_PASSING_BLUR 2

// ambient occlusion
// AO_SAMPLE_QUALITY = 0 : no AO sampling, only upsampling
// AO_SAMPLE_QUALITY = 1 : no dither/per pixel randomization
// AO_SAMPLE_QUALITY = 2 : efficient high frequency 4x4 pattern without jitter for TemporalAA
// AO_SAMPLE_QUALITY = 3 : efficient high frequency 4x4 pattern with jitter for TemporalAA

// SHADER_QUALITY 0-4
#if SHADER_QUALITY == 0
	// very low
	#define USE_SAMPLESET 1
	#define SAMPLE_STEPS 1
	#define QUAD_MESSAGE_PASSING_BLUR 0
#elif SHADER_QUALITY == 1
	// low
	#define USE_SAMPLESET 1
	#define SAMPLE_STEPS 1
	#define QUAD_MESSAGE_PASSING_BLUR 2
#elif SHADER_QUALITY == 2
	// medium
	#define USE_SAMPLESET 1
	#define SAMPLE_STEPS 2
	#define QUAD_MESSAGE_PASSING_BLUR 2
#elif SHADER_QUALITY == 3
	// high
	#define USE_SAMPLESET 1
	#define SAMPLE_STEPS 3
	#define QUAD_MESSAGE_PASSING_BLUR 0
#else // SHADER_QUALITY == 4
	// very high
	#define USE_SAMPLESET 3
	#define SAMPLE_STEPS 3
	#define QUAD_MESSAGE_PASSING_BLUR 0
#endif

#if QUAD_MESSAGE_PASSING_BLUR == 0
	#define QUAD_MESSAGE_PASSING_NORMAL 0
	#define QUAD_MESSAGE_PASSING_DEPTH 0
#elif QUAD_MESSAGE_PASSING_BLUR == 1
	#define QUAD_MESSAGE_PASSING_NORMAL 0
	#define QUAD_MESSAGE_PASSING_DEPTH 0
#elif QUAD_MESSAGE_PASSING_BLUR == 2
	#define QUAD_MESSAGE_PASSING_NORMAL 1
	#define QUAD_MESSAGE_PASSING_DEPTH 0
#elif QUAD_MESSAGE_PASSING_BLUR == 3
	#define QUAD_MESSAGE_PASSING_NORMAL 1
	#define QUAD_MESSAGE_PASSING_DEPTH 1
#endif

// 0:4 samples, 1:9 samples (only really noticable with dither usage ??)
//#define AO_UPSAMPLE_QUALITY 

#if USE_AO_SETUP_AS_INPUT == 1
	// lower resolution
	#define AO_SAMPLE_QUALITY 3
	#undef USE_SAMPLESET
	#define USE_SAMPLESET 3
	#define AO_UPSAMPLE_QUALITY 1
#else
	// full resolution is expensive, do lower quality
	#define AO_SAMPLE_QUALITY 3
	#define AO_UPSAMPLE_QUALITY 0
#endif

// 0: 1 point (for testing)
// 1: 3 points
// 2: more evenly spread (5 points - slightly faster, stronger effect, better with multiple levels?)
// 3: near the surface very large, softly fading out (6 points)
#if USE_SAMPLESET == 0
	#define SAMPLESET_ARRAY_SIZE 1
	static const float2 occlusionSamplesOffsets[SAMPLESET_ARRAY_SIZE]=
	{
		// one sample, for testing
		float2(0.500, 0.500), 
	};
#elif USE_SAMPLESET == 1
	#define SAMPLESET_ARRAY_SIZE 3
	static const float2 occlusionSamplesOffsets[SAMPLESET_ARRAY_SIZE]=
	{
		// 3 points distributed on the unit disc, spiral order and distance
		float2(0, -1.0f) * 0.43f, 
		float2(0.58f, 0.814f) * 0.7f, 
		float2(-0.58f, 0.814f) 
	};
#elif USE_SAMPLESET == 2
	#define SAMPLESET_ARRAY_SIZE 5
	static const float2 occlusionSamplesOffsets[SAMPLESET_ARRAY_SIZE]=
	{
		// 5 points distributed on a ring
		float2(0.156434, 0.987688),
		float2(0.987688, 0.156434)*0.9,
		float2(0.453990, -0.891007)*0.8,
		float2(-0.707107, -0.707107)*0.7,
		float2(-0.891006, 0.453991)*0.65,
	};
#else // USE_SAMPLESET == 3
	#define SAMPLESET_ARRAY_SIZE 6
	static const float2 occlusionSamplesOffsets[SAMPLESET_ARRAY_SIZE]=
	{
		// 6 points distributed on the unit disc, spiral order and distance
		float2(0.000, 0.200), 
		float2(0.325, 0.101), 
		float2(0.272, -0.396), 
		float2(-0.385, -0.488), 
		float2(-0.711, 0.274), 
		float2(0.060, 0.900) 
	};
#endif // USE_SAMPLESET

// needed to prevent AO seam near 16 bit float maximum, this feactor pushed the problem far out and it seems to not have a visual degradion nearby
const static float Constant_Float16F_Scale =  4096.0f * 32.0f;

cbuffer HiZParam : register(b1)
{
    float ambientOcclusionPower;          // ao强度
    float ambientOcclusionBias;           // ao偏移
    float invAmbientOcclusionDistance;    // ao距离倒数
    float ambientOcclusionIntensity;      // ao强度
    float ambientOcclusionFadeRadius;     // 过度半径
    float ambientOcclusionFadeDistance;   // 过度距离
    float ambientOcclusionMipThreshold;
    float ambientOcclusionMipBlend;
    float AORadiusInShader;               // ao半径
    float ratio;                          //
    float2 viewportUVToRandomUV;
    float scaleFactor;                    // 缩放系数
    float scaleRadiusInWorldSpace;        // 世界空间的缩放半径
    float invTanHalfFov;                  // 1.0 / tan(fov * 0.5)
    float HiZStepMipLevelFactor;          // Hi-Z步进mip的系数
    float2 temporalOffset;
    float2 SSAO_DownsampledAOInverseSize;
    float2 AOViewport_ViewportSize;
	float4 HiZRemapping;
}

// 屏幕法线
Texture2D screenNormalTexture : register(t0);
SamplerState screenNormalSampler : register(s0);

// 屏幕深度
Texture2D screenDepthTexture : register(t1);
SamplerState screenDepthSampler : register(s1);

// 随机法线
Texture2D randomNormalTexture : register(t2);
SamplerState randomNormalSampler : register(s2);

// Hi-Z map
Texture2D HiZTexture : register(t3);
SamplerState HiZSampler : register(s3);

// Setup normal + depth
Texture2D SSAO_SetupTexture : register(t4);
SamplerState SSAO_SetupTexutreSampler : register(s4);

// Downsample normal + depth
Texture2D SSAO_NormalsTexture : register(t5);
SamplerState SSAO_NormalsTextureSampler : register(s5);

// Downsample ao
Texture2D SSAO_DownsampledAO : register(t6);
SamplerState SSAO_DownsampledAOSampler : register(s6);
