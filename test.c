#include <stdio.h>

/*int main()
{
    int a = 10;
    int b = 20;
    int const *pci = &a;    //他是个指向常量的指针，通过const修饰，所以无法通过指针修改变量的值
    //*pci = 20             /////////常量指针，无法通过引用一个常量指针去改变一个值。////////////////////
    //pci = &b;             //可以修改指针的指向，不能修改指向的值。
    //a = 20;               //可以修改变量啊a的值。
    printf("%d\n", a);
    printf("%d\n", *pci);
    return 0;
}*/

int main()
{
    int a = 10;
    int b= 20;
    int * const pci = &a;   //它是个int*的指针，然后是个const修饰的，所以是个常指针，指向不能改变。
    //pci = &b;               /////////////常指针，无法指向其他的变量//////////////////////////////////////
    printf("%d\n", a);
    printf("%d\n", *pci);
    return 0;
}
