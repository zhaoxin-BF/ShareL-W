#include<iostream>
#include<string>
using namespace std;

void Swap(int& a, int& b)
{
	a = a - b;
	b = a + b;
	a = b - a;
}

void insert_sort(int array[], size_t size)
{
	int i, j, tmp = 0;
	for (i = 1; i < size; i++)
	{
		for (j = i - 1; j >= 0; j--)
		{
			if (array[j] > array[i])
				;
			else
			{
				break;
			}
		}
		tmp = i;
		while (tmp != j + 1)
		{
			Swap(array[tmp], array[tmp - 1]);
			tmp--;
		}
	}
}

void Print(int array[], size_t size)
{
	int i;
	for (i = 0; i < size; i++)
	{
		cout << array[i] <<endl;
	}
}

int main()
{
	int n, array[1000] = { 0 }, j = 0;
	cin >> n;
	int sum1,sum2,g,s,b,r,i = 0;
	for (i = 111; i <= 999; i++)
	{
		b = i / 100;
		r = i % 100;
		s = r / 10;
		g = r % 10;
		sum1 = (g + s + b);
		sum2 = (s + b);
		if (n % 2 == 0)
		{
			if ((sum2 == (n - g) / 2) && ((n - g) % 2 == 0))
			{
				array[j] = b * 10000 + s * 1000 + g * 100 + s * 10 + b;
				j++;
			}
			if (sum1 == n / 2)
			{
				array[j] = b * 100000 + s * 10000 + g * 1000 + g * 100 + s * 10 + b;
				j++;
			}
		}
		else
		{
			if ((sum2 == (n - g) / 2) &&((n-g)%2==0))
			{
				cout << i << s << b << " ";
				cout << endl;
			}
		}
	}
	if (j)
	{
		insert_sort(array, j);
		Print(array,j);
	}
	system("pause");
	return 0;
}