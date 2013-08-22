#ifndef _KINDLEAPP_FRAMEWORK_PAINTFLAG_H_
#define _KINDLEAPP_FRAMEWORK_PAINTFLAG_H_
#include <stdint.h>

typedef uint32_t paintflag_t;
const paintflag_t PAINT_FLAG_NONE = 0x0;
const paintflag_t PAINT_FLAG_CACHABLE = 0x1;
const paintflag_t PAINT_FLAG_FULL = 0x2;
const paintflag_t PAINT_FLAG_REPAINT = 0x4;
const paintflag_t PAINT_FLAG_A2 = 0x8;
const paintflag_t PAINT_FLAG_DU = 0x10;


#endif
