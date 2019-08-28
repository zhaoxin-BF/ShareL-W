#include <iostream>
using namespace std;


void swap(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
    return;
}

int position(int* arr, int left, int right)
{
    int start = left;
    int key = arr[left];
    while(left < right)
    {
        while(left < right && arr[left] <= key)
            left++;
        while(left < right && arr[right] >= key)
            right--;
        if(left < right)
            swap(&arr[left], &arr[right]);
    }
    if(arr[left] < key)
        swap(&arr[start], &arr[left]);
    return left;
}

void fast_sort(int* arr, int left, int right)
{
    while(left < right)
    {
        int mid = position(arr,left,right);
        fast_sort(arr,left,mid-1);
        fast_sort(arr,mid+1,right);
    }
    return;
}

void print(int* arr, int size)
{
    for(int i = 0; i < size; i++)
    {
        cout<<arr[i]<<" ";
    }
    cout<<endl;
}
int main()
{
    int arr[10] = {6,8,2,4,5,3,1,0,7,9};
    print(arr, 10);
    fast_sort(arr, 0, 9);
    print(arr, 10);
    return 0;
}
