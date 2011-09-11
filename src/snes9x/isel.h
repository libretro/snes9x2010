static inline int64_t isel_if(int64_t condition, int64_t a, int64_t b) // condition ? a : b
{
   int64_t mask = (condition | -condition) >> 63;
   return (a & mask) | (b & ~mask);
}

static inline int32_t isel_if32(int64_t condition, int32_t a, int32_t b) // condition ? a : b
{
   int32_t mask = (condition | -condition) >> 31;
   return (a & mask) | (b & ~mask);
}

#define ISEL_IF8(var, cond, a, b) { int8_t condition = cond; int8_t mask = ((condition) | -(condition)) >> 7; var = (a & mask) | (b & ~mask); }
#define ISEL_IF16(var, cond, a, b) { int16_t condition = cond; int16_t mask = ((condition) | -(condition)) >> 15; var = (a & mask) | (b & ~mask); }
#define ISEL_IF32(var, cond, a, b) { int32_t condition = cond; int32_t mask = ((condition) | -(condition)) >> 31; var = (a & mask) | (b & ~mask); }
#define ISEL_IF64(var, cond, a, b) { int64_t condition = cond; int64_t mask = ((condition) | -(condition)) >> 63; var = (a & mask) | (b & ~mask); }
