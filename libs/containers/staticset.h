#ifndef _staticset_h_
#define _staticset_h_

template<typename T, int buffer_size, int set_count> //buffer_size legyen kettohatvany!!!!!!!!!!!!!1
struct static_set
{
	T buffer[buffer_size];
	int set_bottom[set_count];
	int set_top[set_count];

	int elem_count;

	static_set()
	{
		int elemcount_per_set=buffer_size/set_count;

		int i=0;
		for (int n=0; n<set_count; ++n)
		{
			set_bottom[n]=i;
			set_top[n]=i-1;
			i+=elemcount_per_set;
		}

		set_top[0]=buffer_size-1;

		for (int n=0; n<buffer_size; ++n)
			buffer[n]=-999;

		elem_count=0;
	}

	void push(const T& elem, int set_index)
	{
		assertion(elem_count<buffer_size);
		T tmp;
		tmp=elem;
		int prevn=set_index;
		set_top[set_index]=(set_top[set_index]+1) & (buffer_size-1);
		for (int n=(set_index+1) % set_count; n!=set_index; n=(n+1) % set_count)
		{
			if (set_top[prevn]!=set_bottom[n])
				break;

			if (!empty(n))
				swap(tmp,buffer[set_bottom[n]]);
//			else
//				tmp=buffer[set_bottom[n]];
			set_bottom[n]=(set_bottom[n]+1) & (buffer_size-1);
			set_top[n]=(set_top[n]+1) & (buffer_size-1);
			prevn=n;
		}

//		if (!empty(prevn))
		buffer[set_top[prevn]]=tmp;
	}

	T pop(int set_index)
	{
		assertion(empty(set_index));
		--elem_count;
		T elem=buffer[set_top[set_index]];
		buffer[set_top[set_index]]=-999;
		set_top[set_index]+=buffer_size-1;
		set_top[set_index]&=(buffer_size-1);
		return elem;
	}

	bool empty(int set_index) const
	{
		return set_bottom[set_index]==((set_top[set_index]+1) & (buffer_size-1));
	}
};
#endif//_staticset_h_