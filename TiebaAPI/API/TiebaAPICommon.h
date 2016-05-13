#pragma once

#ifdef TIEBA_API_EXPORTS
#define TIEBA_API_API __declspec(dllexport)
#else
#define TIEBA_API_API __declspec(dllimport)
#endif
