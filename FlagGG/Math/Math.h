#ifndef __MATH__
#define __MATH__

#include "Export.h"

#include "Vector3.h"
#include "Matrix4.h"

namespace FlagGG
{
	namespace Math
	{
		enum Intersection
		{
			OUTSIDE,
			INTERSECTS,
			INSIDE
		};

		extern const float PI;
		extern const float EPS;
		extern const float F_INFINITY;

		template < class T >
		inline T Max(T value1, T value2)
		{
			return value1 > value2 ? value1 : value2;
		}

		template < class T >
		inline T Min(T value1, T value2)
		{
			return value1 < value2 ? value1 : value2;
		}

		bool FlagGG_API IsNaN(float number);

		float FlagGG_API Sin(float angle);

		float FlagGG_API Cos(float angle);

		float FlagGG_API Tan(float angle);

		float FlagGG_API Asin(float angle);

		float FlagGG_API Acos(float angle);

		float FlagGG_API Atan(float angle);


		float FlagGG_API Abs(float number);

		float FlagGG_API Sqrt(float number);

		float FlagGG_API Fract(float number);


		int FlagGG_API Compare(float _1, float _2);

		float FlagGG_API Equals(float _1, float _2);

		float FlagGG_API Clamp(float target, float min, float max);


		// λ�ƾ���
		Matrix4 FlagGG_API MatrixTranslation(float dx, float dy, float dz);

		// X��˳ʱ����ת����
		// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
		Matrix4 FlagGG_API MatrixRotationX(float angle);

		// Y��˳ʱ����ת����
		// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
		Matrix4 FlagGG_API MatrixRotationY(float angle);

		// Z��˳ʱ����ת����
		// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
		Matrix4 FlagGG_API MatrixRotationZ(float angle);

		// ������˳ʱ����ת����
		// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
		Matrix4 FlagGG_API MatrixRotationAxis(const Vector3& axis, float angle);

		Vector3 FlagGG_API Vector3TransformNormal(const Vector3& target, const Matrix4& T);

		Vector3 FlagGG_API Vector3TransformCoord(const Vector3& target, const Matrix4& T);

		/**
		 * @brief
		 * @param eye - �������λ��
		 * @param at  - �۲���λ��
		 * @param up  - ����������Ϸ���
		 */
		Matrix4 FlagGG_API MatrixLookAtLH(const Matrix4& eye, const Matrix4& at, const Matrix4& up);

		/**
		 * @brief ͶӰ����任
		 * @param fovy   - ����ָ���Ի���Ϊ��λ�������������y���ϵĳ���Ƕȣ�������Ƕȣ�View of View��������Ƕ�Խ��ӳ�䵽ͶӰ�����е�ͼ�ξ�ԽС����֮��ͶӰͼ���Խ��
		 * @param aspect - ����������Ļ��ʾ���ĺ��ݱȣ�����ֵ��Ϊ��Ļ�Ŀ��/�߶ȡ���Ӧ��ͬ��������ʾ��Ļ������16/9��4/3�ȵȣ�������ʾ��ͶӰͼ����ܻ�ʹͼ������
		 * @param zn     - ��ʾ�ӽ����н��ü���������������λ�ã������۵���������ش���֮��ľ���
		 * @param zf     - ��ʾ�ӽ�����Զ�ü���������������λ�ã������۵��������ɫǽ�ڡ�֮��ľ���
		 * ע��������Բο����https://blog.csdn.net/poem_qianmo/article/details/8408723
		 */
		Matrix4 FlagGG_API MatrixPerspectiveFovLH(float fovy, float aspect, float zn, float zf);
	}
}

#endif