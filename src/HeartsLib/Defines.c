#ifdef _TINSPIRE
#include <os.h>
#endif
#include "Defines.h"

int T_Min(int a, int b) { return a<b ? a : b; }

int T_NumberOfDigits(int n)//Very quickly thrown together
{
   int nDigits = 0;
   if( n == 0 )
      return 1;
   while(n/10 > 0 || n%10 > 0)
   {
      nDigits++;
      n/=10;
   }
   return nDigits;
}

void T_itoa(int n, char buffer[], int nBufferSize)//Very quickly thrown together
{
   int i=2;
   nBufferSize = T_Min(nBufferSize, T_NumberOfDigits(n)+1);
   for(; i<=nBufferSize; i++)
   {
      int nRemainder = n%10;
      buffer[nBufferSize-i] = '0' + nRemainder;
      n/=10;
   }
   buffer[nBufferSize-1] = '\0';
}

