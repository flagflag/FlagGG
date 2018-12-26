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


		// λ�ƾ���
		Matrix4 MatrixTranslation(float dx, float dy, float dz);

		// X��˳ʱ����ת����
		// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
		Matrix4 MatrixRotationX(float angle);

		// Y��˳ʱ����ת����
		// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
		Matrix4 MatrixRotationY(float angle);

		// Z��˳ʱ����ת����
		// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
		Matrix4 MatrixRotationZ(float angle);

		// ������˳ʱ����ת����
		// https://baike.baidu.com/item/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5/3265181?fr=aladdin
		Matrix4 MatrixRotationAxis(const Vector3& axis, float angle);

		Vector3 Vector3TransformNormal(const Vector3& target, const Matrix4& T);

		Vector3 Vector3TransformCoord(const Vector3& target, const Matrix4& T);

		/**
		 * @brief
		 * @param eye - �������λ��
		 * @param at  - �۲���λ��
		 * @param up  - ����������Ϸ���
		 */
		static Matrix4 MatrixLookAtLH(const Matrix4& eye, const Matrix4& at, const Matrix4& up);

		/**
		 * @brief ͶӰ����任
		 * @param fovy   - ����ָ���Ի���Ϊ��λ�������������y���ϵĳ���Ƕȣ�������Ƕȣ�View of View��������Ƕ�Խ��ӳ�䵽ͶӰ�����е�ͼ�ξ�ԽС����֮��ͶӰͼ���Խ��
		 * @param aspect - ����������Ļ��ʾ���ĺ��ݱȣ�����ֵ��Ϊ��Ļ�Ŀ��/�߶ȡ���Ӧ��ͬ��������ʾ��Ļ������16/9��4/3�ȵȣ�������ʾ��ͶӰͼ����ܻ�ʹͼ������
		 * @param zn     - ��ʾ�ӽ����н��ü���������������λ�ã������۵���������ش���֮��ľ���
		 * @param zf     - ��ʾ�ӽ�����Զ�ü���������������λ�ã������۵��������ɫǽ�ڡ�֮��ľ���
		 * ע��������Բο����https://blog.csdn.net/poem_qianmo/article/details/8408723
		 */
		static Matrix4 MatrixPerspectiveFovLH(float fovy, float aspect, float zn, float zf);
	}
}

#endif