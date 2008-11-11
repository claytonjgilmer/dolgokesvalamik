#ifndef _sorting_h_
#define _sorting_h_

#include "math.h"

template <typename T,int (*comparefn)(T,T)>
void q_sort(T numbers[], int left, int right)
{
	T pivot;
	int l_hold, r_hold;

	int leftstack[1000];
	int rightstack[1000];
	int stacksize=0;

	leftstack[0]=left;
	rightstack[0]=right;
	++stacksize;


	while (stacksize)
	{
		--stacksize;
		left=leftstack[stacksize];
		right=rightstack[stacksize];

		l_hold = left;
		r_hold = right;
		int pivot_index=(left+right)>>1;
		pivot = numbers[pivot_index];
		while (left < right)
		{
			while (comparefn(pivot,numbers[right]) && (left < right))
				right--;

			while (comparefn(numbers[left],pivot) && (left < right))
				left++;


			swap(numbers[left],numbers[right]);
			left++;
			right--;
		}


		if (l_hold < right)
		{
			leftstack[stacksize]=l_hold;
			rightstack[stacksize]=right;
			++stacksize;
		}

		if (r_hold > left)
		{
			leftstack[stacksize]=left;
			rightstack[stacksize]=r_hold;
			++stacksize;
		}
	}
}


#include <algorithm>

void qf(float a[], int l, int r)
{
	int i = l-1, j = r, p = l-1, q = r;
	float v = a[r];
//	float v = a[(l+r)/2];

	if (r <= l) return;
	for (;;)
	{
		while (a[++i] < v) ;
		while (v < a[--j]) if (j == l) break;
		if (i >= j) break;
		std::swap(a[i], a[j]);
		if (a[i] == v) { p++; std::swap(a[p], a[i]); }
		if (v == a[j]) { q--; std::swap(a[j], a[q]); }
	}
	std::swap(a[i], a[r]); j = i-1; i = i+1;
	for (int k = l; k < p; k++, j--) std::swap(a[k], a[j]);
	for (int k = r-1; k > q; k--, i++) std::swap(a[i], a[k]);
	qf(a, l, j);
	qf(a, i, r);
}







template <typename T,int (*comparefn)(T,T)>
void q_sort_rec(T numbers[], int left, int right)
{
	int pivot_index;
	int l_hold, r_hold;

	l_hold = left;
	r_hold = right;
	pivot_index = (left+right)>> 1;
	T pivot_elem=numbers[pivot_index];

	while (left < right)
	{
		while (comparefn(pivot_elem,numbers[right]) && (left < right))
			right--;

		while (comparefn(numbers[left],pivot_elem) && (left < right))
			left++;

		std::swap(numbers[left],numbers[right]);
		left++;
		right--;
	}

	if (l_hold < right)
		q_sort_rec<T,comparefn>(numbers, l_hold, right);

	if (r_hold > left)
		q_sort_rec<T,comparefn>(numbers, left, r_hold);
}

void quicksortfloat(float a[], int lo, int hi)
{
	int i=lo, j=hi;
	float x=med3(a[lo],a[hi],a[(lo+hi)/2]);

	do
	{
		while (a[i]<x) i++;
		while (a[j]>x) j--;
		if (i<=j)
		{
			float h;
			h=a[i]; a[i]=a[j]; a[j]=h;
			i++; j--;
		}
	} while (i<=j);

	//  recursion
	if (lo<j) quicksortfloat(a, lo, j);
	if (i<hi) quicksortfloat(a, i, hi);
}

void quick4(float a[], int lo, int hi)
{
	int i=lo, j=hi;
//	float x=med3(a[lo],a[hi],a[(lo+hi)/2]);
	float x=a[(lo+hi)/2];

	while (i<j)
	{
		int volt=0;
		if (a[i]>=x)
		{
			i++;
			volt=1;
		}

		if (a[j]<=x  && i<j)
		{
			j--;
			volt=1;
		}

		if (!volt)
		{
			float h;
			h=a[i]; a[i]=a[j]; a[j]=h;
			i++; j--;
		}
	}

	//  recursion
	if (lo<j) quick4(a, lo, j);
	if (i<hi) quick4(a, i, hi);
}


template <typename T,int (*comparefn)(T,T)>
void insertion_sort(T numbers[] , int size)
{
	int i, j;
	T act_elem;
	for(i=1; i < size;i++)
	{
		act_elem = numbers[i];
		j = i;

		while((j > 0) && (comparefn(act_elem,numbers[j-1])))
		{
			numbers[j] = numbers[j-1];
			--j;
		}

		numbers[j] = act_elem;
	}
}

template <typename T,int (*comparefn)(T,T)>
void merge(T numbers[], T tmpbuf[],int lo, int hi)
{
	if (lo == hi)
	{
//		tmpbuf[lo]=numbers[lo];
		return;
	}

	int mid = (lo+hi) / 2;
	merge<T,comparefn>(numbers,tmpbuf, lo, mid);
	merge<T,comparefn>(numbers,tmpbuf, mid+1, hi);

	int k, t_lo = lo, t_hi = mid+1;
	for (k = lo; k <= hi; k++)
	{
		if ((t_lo <= mid) && ((t_hi > hi) || (comparefn(numbers[t_lo],numbers[t_hi]))))
			tmpbuf[k] = numbers[t_lo++];
		else
			tmpbuf[k] = numbers[t_hi++];
	}

	for (k=lo; k<=hi; ++k)
		numbers[k]=tmpbuf[k];
}

template <typename T,int (*comparefn)(T,T)>
void merge_sort(T numbers[], int size)
{
	T* tmpbuf=new T[size];

	merge<T,comparefn>(numbers,tmpbuf,0,size-1);

	delete [] tmpbuf;
}


template<typename T,unsigned (*i_KeyFn)(T)>
inline void radixsort(T* i_Data, int i_NumObjects)
{
	T* i_TempData=new T[i_NumObjects];
	unsigned counter[256][4];

	memset(counter,0,sizeof(counter));

	for (int n=0; n<i_NumObjects; ++n)
	{
		unsigned actKey=i_KeyFn(i_Data[n]);
		++counter[(actKey & 0x000000ff)       ][0];
		++counter[(actKey & 0x0000ff00) >> 8  ][1];
		++counter[(actKey & 0x00ff0000) >> 16 ][2];
		++counter[(actKey & 0xff000000) >> 24 ][3];
	}

	unsigned sum0=0,sum1=0,sum2=0,sum3=0;
	for (int n=0; n<256; ++n)
	{
		int act;
		act=counter[n][0];
		counter[n][0]=sum0;
		sum0+=act;
		act=counter[n][1];
		counter[n][1]=sum1;
		sum1+=act;
		act=counter[n][2];
		counter[n][2]=sum2;
		sum2+=act;
		act=counter[n][3];
		counter[n][3]=sum3;
		sum3+=act;
	}

	for (int m=0; m<i_NumObjects; ++m)
	{
		unsigned actKey=i_KeyFn(i_Data[m]) & 0xff;
		int actIndex=counter[actKey][0]++;
		i_TempData[actIndex]=i_Data[m];

	}
	for (int m=0; m<i_NumObjects; ++m)
	{
		unsigned actKey=(i_KeyFn(i_TempData[m]) & 0xff00) >> 8;
		int actIndex=counter[actKey][1]++;
		i_Data[actIndex]=i_TempData[m];

	}
	for (int m=0; m<i_NumObjects; ++m)
	{
		unsigned actKey=(i_KeyFn(i_Data[m]) & 0xff0000) >> 16;
		int actIndex=counter[actKey][2]++;
		i_TempData[actIndex]=i_Data[m];

	}
	for (int m=0; m<i_NumObjects; ++m)
	{
		unsigned actKey=(i_KeyFn(i_TempData[m]) & 0xff000000) >> 24;
		int actIndex=counter[actKey][3]++;
		i_Data[actIndex]=i_TempData[m];
	}

	delete [] i_TempData;
}

union FloatToUnsigned
{
	float f;
	unsigned i;

	FloatToUnsigned(float i_Value)
	{
		f=i_Value;
		i^=((~(i >> 31))+1) | 0x80000000;
	}

	void operator =(float i_Value)
	{
		f=i_Value;
		i^=((~(i >> 31))+1) | 0x80000000;
	}

	FloatToUnsigned()
	{
	}

	FloatToUnsigned(unsigned i_Value)
	{
		i=i_Value;
	}

	void operator=(unsigned i_Value)
	{
		i=i_Value;
	}

	operator const unsigned&() const
	{
		return i;
	}
};



#endif//_sorting_h_
