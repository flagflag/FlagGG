#ifndef __MATRIX4__
#define __MATRIX4__

namespace FlagGG
{
	namespace Math
	{
		/// 4x4 matrix for arbitrary linear transforms including projection.
		class Matrix4
		{
		public:
			Matrix4();

			Matrix4(const Matrix4& matrix);

			Matrix4(
				float v00, float v01, float v02, float v03,
				float v10, float v11, float v12, float v13,
				float v20, float v21, float v22, float v23,
				float v30, float v31, float v32, float v33);

			explicit Matrix4(const float* data);

			Matrix4& operator =(const Matrix4& rhs);

			bool operator ==(const Matrix4& rhs) const;

			bool operator !=(const Matrix4& rhs) const;

			// 矩阵分量减rhs
			Matrix4 operator +(const Matrix4& rhs) const;

			// 矩阵减法
			Matrix4 operator -(const Matrix4& rhs) const;

			// 矩阵分量乘rhs
			Matrix4 operator *(float rhs) const;

			// 矩阵乘法
			Matrix4 operator *(const Matrix4& rhs) const;

			// 矩阵转置
			Matrix4 Transpose() const;

			// 逆矩阵
			Matrix4 Inverse() const;

			// 矩阵数据数组
			const float* Data() const;

			// 矩阵元素 <=> [i][j]
			float Element(unsigned i, unsigned j) const;

			float m00_;
			float m01_;
			float m02_;
			float m03_;
			float m10_;
			float m11_;
			float m12_;
			float m13_;
			float m20_;
			float m21_;
			float m22_;
			float m23_;
			float m30_;
			float m31_;
			float m32_;
			float m33_;

			// 批量转置
			static void BulkTranspose(float* dest, const float* src, unsigned count);

			// 位移矩阵
			static Matrix4 MatrixTranslation(float dx, float dy, float dz);

			// X轴顺时针旋转矩阵
			// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
			static Matrix4 MatrixRotationX(float angle);

			// Y轴顺时针旋转矩阵
			// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
			static Matrix4 MatrixRotationY(float angle);

			// Z轴顺时针旋转矩阵
			// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
			static Matrix4 MatrixRotationZ(float angle);

			/**
			 * @brief 
			 * @param eye - 摄像机的位置
			 * @param at  - 观察点的位置
			 * @param up  - 摄像机的向上分量
			 */
			static Matrix4 MatrixLookAtLH(const Matrix4& eye, const Matrix4& at, const Matrix4& up);

			/**
			 * @brief 投影矩阵变换
			 * @param fovy   - 用于指定以弧度为单位的虚拟摄像机在y轴上的成像角度，即视域角度（View of View），成像角度越大，映射到投影窗口中的图形就越小；反之，投影图像就越大
			 * @param aspect - 用于描述屏幕显示区的横纵比，他的值就为屏幕的宽度/高度。对应不同比例的显示屏幕，比如16/9，4/3等等，最终显示的投影图像可能会使图像被拉伸
			 * @param zn     - 表示视截体中近裁剪面距我们摄像机的位置，即人眼到“室内落地窗”之间的距离
			 * @param zf     - 表示视截体中远裁剪面距我们摄像机的位置，即人眼到“室外黑色墙壁”之间的距离
			 * 注：具体可以参考这里：https://blog.csdn.net/poem_qianmo/article/details/8408723
			 */
			static Matrix4 MatrixPerspectiveFovLH(float fovy, float aspect, float zn, float zf);

			/// Zero matrix.
			static const Matrix4 ZERO;
			/// Identity matrix.
			static const Matrix4 IDENTITY;
		};
	}
}

#endif