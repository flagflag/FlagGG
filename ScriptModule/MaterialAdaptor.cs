
namespace UnityEngine
{
    public partial class Material : ScriptableObject
    {
        public string[] shaderKeywords;

        public Texture mainTexture;

        public Material(Shader shader)
        {

        }

        public Material(Material otherMaterial)
        {

        }

        public float GetFloat(int propertyId)
        {
            return 0.0f;
        }

        public void SetFloat(int propertyId, float value)
        {

        }

        public int GetInt(int propertyId)
        {
            return 0;
        }

        public void SetInt(int propertyId, int value)
        {

        }

        public int GetInt(string propertyName)
        {
            return 0;
        }

        public void SetInt(string propertyName, int value)
        {

        }

        public Vector4 GetVector(int properyId)
        {
            return Vector4.zero;
        }

        public void SetVector(int propertyId, Vector4 vec4)
        {

        }

        public bool HasProperty(int propertyId)
        {
            return false;
        }

        public void SetTexture(int propertyId, Texture texture)
        {

        }

        public Texture GetTexture(int propertyId)
        {
            return null;
        }

        public void SetPass(int passIndex)
        {

        }

        private string GetTagImpl(string tag, bool currentSubShaderOnly, string defaultValue)
        {
            return null;
        }

        public string GetTag(string tag, bool searchFallbacks, string defaultValue) { return GetTagImpl(tag, !searchFallbacks, defaultValue); }
        public string GetTag(string tag, bool searchFallbacks) { return GetTagImpl(tag, !searchFallbacks, ""); }
    }
}
