#ifndef __SHIFTMUL_H
#define __SHIFTMUL_H
namespace Internal
{
   template <unsigned a, unsigned x, unsigned y>
   struct static_if
   {
      enum { value = x };
   };

   template <unsigned x, unsigned y>
   struct static_if<0, x, y>
   {
      enum { value = y };
   };

   template <unsigned a, unsigned b = 31>
   struct low_shift
   {
      enum { value = static_if<a & (1U << b), b, low_shift<a, b-1>::value>::value };
   };

   template <unsigned a>
   struct low_shift<a, 0>
   {
      enum { value = 0 };
   };

   template <unsigned a>
   struct rest_value
   {
      enum { value = a - (1 << low_shift<a>::value) };
   };

   template <unsigned a>
   struct rest_value_sub
   {
      enum { value = (1 << (low_shift<a>::value + 1)) - a };
   };

   template <unsigned>
   inline unsigned mul(unsigned);

   template <unsigned a>
   inline unsigned mul_add(unsigned b)
   {
      return (b << low_shift<a>::value) + mul<rest_value<a>::value>(b);
   }

   template <unsigned a>
   inline unsigned mul_sub(unsigned b)
   {
      return (b << (low_shift<a>::value + 1)) - mul<rest_value_sub<a>::value>(b);
   }

   template <unsigned a>
   inline unsigned mul(unsigned b)
   {
      // This can be statically determined, so shouldn't compile to any branch...
      return ((unsigned)Internal::rest_value<a>::value <= (unsigned)Internal::rest_value_sub<a>::value) ? mul_add<a>(b) : mul_sub<a>(b);
   }

   template <>
   inline unsigned mul<0>(unsigned) { return 0; }

   template <>
   inline unsigned mul<1>(unsigned b) { return b; }
}

template <unsigned amt, class T>
inline T mul(T in)
{
   return static_cast<T>(Internal::mul<amt>(static_cast<unsigned>(in)));
}

#endif
