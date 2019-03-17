#include<stdio.h>
#include<stdlib.h>
#include<math.h>
int main()
{
	int n, i;
	float a, b, c;
	float s, p;
	scanf_s("%d", &n);
	float arr[100] = { 0 };

	for (i = 0; i<n * 3; i = i + 3)
	{
		scanf_s("%f%f%f", &a, &b, &c);
		arr[i] = a;
		arr[i + 1] = b;
		arr[i + 2] = c;
	}

	for (i = 0; i<n * 3; i = i + 3)
	{
		a = arr[i];
		b = arr[i + 1];
		c = arr[i + 2];

		if ((a>(b + c)) || (b>(a + c)) || c>(a + b))
		{
			printf("0.00\n");

		}
		else
		{
			p = (a + b + c) / 2;
			s = sqrt(p*(p - a)*(p - b)*(p - c));
			printf("%.2f\n", s);
		}
	}
	system("pause");
	return 0;
}