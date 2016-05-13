#pragma once

#ifdef HELPER_EXPORTS
#define HELPER_API __declspec(dllexport)
#else
#define HELPER_API __declspec(dllimport)
#endif
