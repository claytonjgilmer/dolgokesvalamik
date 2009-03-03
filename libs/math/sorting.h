#ifndef _sorting_h_
#define _sorting_h_

#include "math.h"

template <typename T,int (*comparefn)(T,T)>
inline void q_sort(T numbers[], int left, int right)
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


//#include <algorithm>

inline void qf(f32 a[], int l, int r)
{
	int i = l-1, j = r, p = l-1, q = r;
	f32 v = a[r];
//	f32 v = a[(l+r)/2];

	if (r <= l) return;
	for (;;)
	{
		while (a[++i] < v) ;
		while (v < a[--j]) if (j == l) break;
		if (i >= j) break;
		swap(a[i], a[j]);
		if (a[i] == v) { p++; swap(a[p], a[i]); }
		if (v == a[j]) { q--; swap(a[j], a[q]); }
	}
	swap(a[i], a[r]); j = i-1; i = i+1;
	for (int k = l; k < p; k++, j--) swap(a[k], a[j]);
	for (int k = r-1; k > q; k--, i++) swap(a[i], a[k]);
	qf(a, l, j);
	qf(a, i, r);
}







template <typename T,int (*comparefn)(T,T)>
inline void q_sort_rec(T numbers[], int left, int right)
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

		swap(numbers[left],numbers[right]);
		left++;
		right--;
	}

	if (l_hold < right)
		q_sort_rec<T,comparefn>(numbers, l_hold, right);

	if (r_hold > left)
		q_sort_rec<T,comparefn>(numbers, left, r_hold);
}

inline void quicksortfloat(f32 a[], int lo, int hi)
{
	int i=lo, j=hi;
	f32 x=med3(a[lo],a[hi],a[(lo+hi)/2]);

	do
	{
		while (a[i]<x) i++;
		while (a[j]>x) j--;
		if (i<=j)
		{
			f32 h;
			h=a[i]; a[i]=a[j]; a[j]=h;
			i++; j--;
		}
	} while (i<=j);

	//  recursion
	if (lo<j) quicksortfloat(a, lo, j);
	if (i<hi) quicksortfloat(a, i, hi);
}

inline void quick4(f32 a[], int lo, int hi)
{
	int i=lo, j=hi;
//	f32 x=med3(a[lo],a[hi],a[(lo+hi)/2]);
	f32 x=a[(lo+hi)/2];

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
			f32 h;
			h=a[i]; a[i]=a[j]; a[j]=h;
			i++; j--;
		}
	}

	//  recursion
	if (lo<j) quick4(a, lo, j);
	if (i<hi) quick4(a, i, hi);
}


template <typename T,int (*comparefn)(T,T)>
inline void insertion_sort(T numbers[] , int size)
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
inline void merge(T numbers[], T tmpbuf[],int lo, int hi)
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
inline void merge_sort(T numbers[], int size)
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

	FloatToUnsigned(f32 i_Value)
	{
		f=i_Value;
		i^=((~(i >> 31))+1) | 0x80000000;
	}

	void operator =(f32 i_Value)
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




////////////////////////////////////
//		intro sort
////////////////////////////////////


#define MIN_LENGTH_FOR_QUICKSORT	64
#define MAX_DEPTH_BEFORE_HEAPSORT	256

template<typename T>
struct std_pred
{
	static bool less(const T& i1, const T& i2)
	{
		return i1<i2;
	}

	static bool eq(const T& i1, const T& i2)
	{
		return i1==i2;
	}
};


template <typename T, typename Pred=std_pred<T> >
struct introsort
{
	void operator()(T * array, unsigned int count);

	void Partition(T * left, unsigned int count, unsigned int depth = 1);

	T SelectPivot(T value1, T value2, T value3);

	void Swap(T * valueA, T * valueB);

	// insertion sort methods
	void InsertionSort(T * array, unsigned int count);

	// heap sort methods
	void HeapSort(T * array, int length);

	void HeapSort(T * array, int k, int N);
};

template <typename T,typename Pred>
inline void introsort<T,Pred>::operator()(T * array, unsigned int count)
{
	// Public method used to invoke the sort.


	// Call quick sort partition method if there are enough
	// elements to warrant it or insertion sort otherwise.
	if (count >= MIN_LENGTH_FOR_QUICKSORT)
		Partition(array, count);
	else
		InsertionSort(array, count);
}




template <typename T,typename Pred>
inline void introsort<T,Pred>::Swap(T * valueA, T * valueB)
{
	// do the ol' "switch-a-me-do" on two values.
	T temp = *valueA;
	*valueA = *valueB;
	*valueB = temp;
}





template <typename T,typename Pred>
inline T introsort<T,Pred>::SelectPivot(T value1, T value2, T value3)
{
	// middle of three method.
	if (Pred::less(value1 , value2))
		return (Pred::less(value2 , value3) ? value2 : Pred::less(value1 , value3) ? value3 : value1);
	return (Pred::less(value1 , value3) ? value1 : Pred::less(value2 , value3) ? value3 : value2);
}





template <typename T,typename Pred>
inline void introsort<T,Pred>::Partition(T * left, unsigned int count, unsigned int depth)
{
	if (depth > MAX_DEPTH_BEFORE_HEAPSORT)
	{
		// If enough recursion has happened then we bail to heap sort since it looks
		// as if we are experiencing a 'worst case' for quick sort.  This should not
		// happen very often at all.
		HeapSort(left, count);
		return;
	}

	T * right = left + count - 1;
	T * startingLeft = left;
	T * startingRight = right;
	T * equalLeft = left;
	T * equalRight = right;

	// select the pivot value.
	T pivot = SelectPivot(left[0], right[0], left[((right - left) >> 1)]);

	// do three way partitioning.
	do
	{
		while ((left < right) && !Pred::less(pivot, *left))
			if (Pred::eq(*(left++), pivot))
			{
				// equal to pivot value.  move to far left.
				Swap(equalLeft++, left - 1);
			}

		while ((left < right) && !Pred::less(*right, pivot))
			if (Pred::eq(*(right--), pivot))
			{
				// equal to pivot value.  move to far right.
				Swap(equalRight--, right + 1);
			}

		if (left >= right)
		{
			if (left == right)
			{
				if (!Pred::less(*left, pivot))
					left--;
				if (!Pred::less(pivot, *right))
					right++;
			}
			else
			{
				left--;
				right++;
			}
			break;	// done partitioning
		}

		// left and right are ready for swaping
		Swap(left++, right--);
	} while (true);


	// move values that were equal to pivot from the far left into the middle.
	// these values are now placed in their final sorted position.
	if (equalLeft > startingLeft)
	{
		do
		{
			Swap(--equalLeft, left--);
		} while (equalLeft > startingLeft);
		if (left < startingLeft)
			left = startingLeft;
	}

	// move values that were equal to pivot from the far right into the middle.
	// these values are now placed in their final sorted position.
	if (equalRight < startingRight)
	{
		do
		{
			Swap(++equalRight, right++);
		} while (equalRight < startingRight);
		if (right > startingRight)
			right = startingRight;
	}

	// Calculate new partition sizes ...
	unsigned int leftSize = left - startingLeft + 1;
	unsigned int rightSize = startingRight - right + 1;

	// Partition left (less than pivot) if there are enough values to warrant it
	// otherwise do insertion sort on the values.
	if (leftSize >= MIN_LENGTH_FOR_QUICKSORT)
		Partition(startingLeft, leftSize, depth + 1);
	else
		InsertionSort(startingLeft, leftSize);

	// Partition right (greater than pivot) if there are enough values to warrant it
	// otherwise do insertion sort on the values.
	if (rightSize >= MIN_LENGTH_FOR_QUICKSORT)
		Partition(right, rightSize, depth + 1);
	else
		InsertionSort(right, rightSize);
}







template <typename T,typename Pred>
inline void introsort<T,Pred>::InsertionSort(T * array, unsigned int count)
{
	// A basic insertion sort.
	if (count < 3)
	{
		if (count == 2)
			if (Pred::less(array[1],array[0]))
			{
				T temp = array[0];
				array[0] = array[1];
				array[1] = temp;
			}
		return;
	}

	T * ptr2, * ptr3 = array + 1, * ptr4 = array + count;

	if (Pred::less(array[1],array[0]))
	{
		T temp = array[0];
		array[0] = array[1];
		array[1] = temp;
	}


	while (true)
	{
		while ((++ptr3 < ptr4) && !Pred::less(*ptr3, ptr3[-1])){}
		if (ptr3 >= ptr4)
			break;

		if (!Pred::less(*ptr3, ptr3[-2]))
		{
			if (Pred::less(*ptr3, ptr3[-1]))
			{
				T temp = *ptr3;
				*ptr3 = ptr3[-1];
				ptr3[-1] = temp;
			}
			continue;
		}

		ptr2 = ptr3 - 1;
		T v = *ptr3;
		while ((ptr2 >= array) && Pred::less(v,*ptr2))
		{
			ptr2[1] = ptr2[0];
			ptr2--;
		}
		ptr2[1] = v;
	}
}





template <typename T,typename Pred>
inline void introsort<T,Pred>::HeapSort(T * array, int length)
{
	// A basic heapsort.
	for (int k = length >> 1; k > 0; k--)
	    HeapSort(array, k, length);

	do
	{
		T temp = array[0];
		array[0] = array[--length];
		array[length] = temp;
        HeapSort(array, 1, length);
	} while (length > 1);
}





template <typename T,typename Pred>
inline void introsort<T,Pred>::HeapSort(T * array, int k, int N)
{
	// A basic heapsort.
	T temp = array[k - 1];
	int n = N >> 1;

	while (k <= n)
	{
		int j = (k << 1);
        if ((j < N) && Pred::less(array[j - 1] , array[j]))
	        j++;
	    if (!Pred::less(temp , array[j - 1]))
			break;
	    else
		{
			array[k - 1] = array[j - 1];
			k = j;
        }
	}

    array[k - 1] = temp;
}


template <typename T, typename Pred>
inline void intro_sort(T* array, int count, Pred pr)
{
	introsort<T,Pred> i; i(array, count);
}

template<typename T>
inline void intro_sort(T* array, int count)
{
	introsort<T,std_pred<T> > i; i(array, count);
}



#endif//_sorting_h_
