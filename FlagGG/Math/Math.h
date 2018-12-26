#ifndef __MATH__
#define __MATH__

#include "Vector3.h"
#include "Matrix4.h"

namespace FlagGG
{
	namespace Math
	{
		bool IsNaN(float number);

		float Sin(float angle);

		float Cos(float angle);

		float Tan(float angle);

		float Asin(float angle);

		float Acos(float angle);

		float Atan(float angle);


		float Abs(float number);

		float Sqrt(float number);


		int Compare(float _1, float _2);

		float Equals(float _1, float _2);

		float Clamp(float target, float min, float max);


		// 位移矩阵
		Matrix4 MatrixTranslation(float dx, float dy, float dz);

		// X轴顺时针旋转矩阵
		// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
		Matrix4 MatrixRotationX(float angle);

		// Y轴顺时针旋转矩阵
		// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
		Matrix4 MatrixRotationY(float angle);

		// Z轴顺时针旋转矩阵
		// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
		Matrix4 MatrixRotationZ(float angle);

		// 任意轴顺时针旋转矩阵
		// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
		Matrix4 MatrixRotationAxis(const Vector3& axis, float angle);

		Vector3 Vector3TransformNormal(const Vector3& target, const Matrix4& T);

		Vector3 Vector3TransformCoord(const Vector3& target, const Matrix4& T);

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
	}
}

#endif