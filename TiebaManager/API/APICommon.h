#pragma once

#ifdef TIEBA_MANAGER_EXPORTS
#define TIEBA_MANAGER_API __declspec(dllexport)
#else
#define TIEBA_MANAGER_API __declspec(dllimport)
#endif
