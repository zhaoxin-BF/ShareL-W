#include<stdio.h>
#include<stdlib.h>
#include<math.h>

int main()
{
    int n,i;
    scanf("%d",&n);
	float a,b,c;
    for(i=0;i<n;i++)
    {
            scanf("%f%f%f\n",&a,&b,&c);
            if((a>(b+c))||(b>(a+c))||c>(a+b))
            {
                printf("0.00\n");
            }
            else
            {
                float s,p;
                p=(a+b+c)/2;
                s=sqrt(p*(p-a)*(p-b)*(p-c));
                printf("%.2f\n",s);
                getchar();
            }
    }
    return 0;
}
