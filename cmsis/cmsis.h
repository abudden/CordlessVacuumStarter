#if defined(TARGET_STM32F4)
#include "cmsis_f4.h"
#elif defined(TARGET_STM32F1)
#include "cmsis_f1.h"
#else
#error No recognised target defined
#endif
