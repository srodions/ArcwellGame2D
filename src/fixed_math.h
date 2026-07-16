#ifndef FIXED_MATH_H_
#define FIXED_MATH_H_

#include <stdint.h>

typedef int32_t fixed_t;

#define FIXED_SHIFT 16					// Bits to shift
#define FIXED_ONE   (1 << FIXED_SHIFT) 	// 65536

// Convert macro
#define INT_TO_FIXED(x)   ((fixed_t)(x) << FIXED_SHIFT)
#define FIXED_TO_INT(x)   ((x) >> FIXED_SHIFT)
#define DOUBLE_TO_FIXED(x) ((fixed_t)((x) * FIXED_ONE))
#define FIXED_TO_DOUBLE(x) ((double)(x) / FIXED_ONE)

// Arithmetic functions
#define FIX_MUL(x, y) ((fixed_t)(((int64_t)(x) * (int64_t)(y)) >> FIXED_SHIFT))
#define FIX_DIV(x, y) ((fixed_t)(((int64_t)(x) << FIXED_SHIFT) / (y)))

#endif /* FIXED_MATH_H_ */
