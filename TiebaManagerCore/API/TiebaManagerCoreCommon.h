#pragma once

#ifdef TIEBA_MANAGER_CORE_EXPORTS
#define TIEBA_MANAGER_CORE_API __declspec(dllexport)
#else
#define TIEBA_MANAGER_CORE_API __declspec(dllimport)
#endif
