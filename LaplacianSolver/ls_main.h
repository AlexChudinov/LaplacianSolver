// stdafx.h: ���������� ���� ��� ����������� ��������� ���������� ������
// ��� ���������� ������ ��� ����������� �������, ������� ����� ������������, ��
// �� ����� ����������
//

#pragma once

#define WIN32_LEAN_AND_MEAN             // ��������� ����� ������������ ���������� �� ���������� Windows
// ����� ���������� Windows:
#include <windows.h>

#define LAPLACIAN_SOLVER_EXPORT __declspec(dllexport)

EXTERN_C LAPLACIAN_SOLVER_EXPORT void testMsg(LPCWCH msg);

// TODO: ���������� ����� ������ �� �������������� ���������, ����������� ��� ���������
