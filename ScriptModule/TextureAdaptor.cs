using UnityEngine.Bindings;

namespace UnityEngine
{
    public class Texture : ScriptableObject
    {
        public int width;
        public int height;

        public TextureFormat format;
        public FilterMode filterMode;
        public TextureWrapMode wrapMode;

        public ColorSpace activeTextureColorSpace;

        public bool isReadable;

        public float pixelsPerPoint;

        public bool Resize(int width, int height)
        {
            return true;
        }

        public bool Resize(int width, int height, TextureFormat format, bool hasMipMap)
        {
            return true;
        }

        public void Apply(bool updateMipmaps, bool makeNoLongerReadable)
        {

        }

        public void Apply() { Apply(true, false); }
    }

    public sealed partial class Texture2D : Texture
    {
        public Texture2D(int width, int height, TextureFormat format, bool mipChain)
        {

        }

        public Texture2D(int width, int height, TextureFormat textureFormat, int mipCount, bool linear)
        {
            
        }

        public void SetPixel(int x, int y, Color color)
        {

        }

        public void SetPixels32(Color32[] colors)
        {
            
        }
    }

    public partial class RenderTexture : Texture
    {
        public RenderTexture(int width, int height, int depth, RenderTextureFormat format)
        {

        }

        [FreeFunction("RenderTexture::GetActive")] extern private static RenderTexture GetActive();
        [FreeFunction("RenderTextureScripting::SetActive")] extern private static void SetActive(RenderTexture rt);
        public static RenderTexture active { get { return GetActive(); } set { SetActive(value); } }

        extern public bool Create();
        extern public void Release();
        extern public bool IsCreated();
        extern public void GenerateMips();

        extern internal void SetSRGBReadWrite(bool srgb);
    }
}
