// stdafx.h: включаемый файл дл€ стандартных системных включаемых файлов
// или включаемых файлов дл€ конкретного проекта, которые часто используютс€, но
// не часто измен€ютс€
//

#pragma once

#define WIN32_LEAN_AND_MEAN             // »сключите редко используемые компоненты из заголовков Windows
// ‘айлы заголовков Windows:
#include <windows.h>

#define LAPLACIAN_SOLVER_EXPORT __declspec(dllexport)

EXTERN_C LAPLACIAN_SOLVER_EXPORT void testMsg(LPCWCH msg);

// TODO: ”становите здесь ссылки на дополнительные заголовки, требующиес€ дл€ программы
