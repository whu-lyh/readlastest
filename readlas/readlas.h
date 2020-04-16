#pragma once
#ifndef READLAS_EXPORTS
#define READLAS_EXPORTS
#endif

#if _DEBUG
#pragma comment(lib,"liblas.lib")
#pragma comment(lib,"libboost_thread-vc141-mt-gd-1_64.lib")
#else
#pragma comment(lib,"liblas.lib")
#pragma comment(lib,"libboost_thread-vc141-mt-1_64.lib")
#endif


#ifdef READLAS_EXPORTS
#define LIBLAS_READLAS_API __declspec(dllexport)
#else
#define LIBLAS_READLAS_API __declspec(dllimport)
#endif

LIBLAS_READLAS_API int readlas ();