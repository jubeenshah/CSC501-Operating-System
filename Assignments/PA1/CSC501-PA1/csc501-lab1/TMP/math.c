
#include <stdio.h>
#include<math.h>

double pow(double x, int y)
{
//https://en.wikipedia.org/wiki/Exponentiation_by_squaring
//def myPow(x,n): 
//    if n == 0: return 1
//        if n < 0 : return myPow(1/x, -n )
//            if n == 1 : return x
//                if n % 2 == 0:
//                        return myPow(x * x, n//2 )
//                            else:
//                                    return x * myPow(x * x, (n-1)//2)
//
//                                    for i in range(100):
//                                        x,n = random.randint(1,10), random.randint(-10,10)
//                                            res = myPow(x,n)
//                                                print(str(x) + "^" + str(n) + " = " + str(res))
//IMplemented in python first, pasting here for reference

	if (y == 0) {
		return 1;
	}
	if (y < 0) {
		return pow(1/x, (-1*y));
	}
	if (y == 1) {
		return x;
	}
	if (y%2 ==0) {
		return pow((x*x),(int) y/2);
	} else {
		return (x * pow((x*x),(int) (y-1)/2));
	}
}

double log(double x)
{
	double logToReturn = 0;
	int i=1;	
	double valueToAdd = 0;
	do {
		//https://www.dreamincode.net/forums/topic/91613-logarithmic-expansion-using-taylor-series/
		valueToAdd = ( pow ( -1 , i+1 ) * ( ( pow ( x-1 , i ) ) / i ) );
		logToReturn += valueToAdd;
		i = i + 1;
	} while (i<=20);

	return (logToReturn);

}

double expdev(double lambda) {
    double dummy;
    do{
        dummy= (double) rand() / RAND_MAX;   
   } while (dummy == 0.0);
    return -log(dummy) / lambda;
}


