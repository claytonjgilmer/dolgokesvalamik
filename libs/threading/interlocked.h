#ifndef _interlocked_h_
#define _interlocked_h_

MLINLINE void spin_loop()
{
	__asm { pause };
}

MLINLINE void interlocked_exchange_32(void* dest, const long exchange)
{
	_InterlockedExchange((long*)dest,exchange);
}

MLINLINE void interlocked_exchange_64(void *dest, const __int64 exchange)
{
	__asm
	{
		mov      ebx, dword ptr [exchange]
		mov      ecx, dword ptr [exchange + 4]
		mov      edi, dest
			mov      eax, dword ptr [edi]
		mov      edx, dword ptr [edi + 4]
		jmp      start
retry:
		pause
start:
		lock cmpxchg8b [edi]
		jnz      retry
	};
}

//return: 1, ha sikerult, 0, ha nem
MLINLINE int interlocked_compare_exchange_32(void *dest,int exchange,int compare)
{
	char _ret;
	//
	__asm
	{
		mov      edx, [dest]
		mov      eax, [compare]
		mov      ecx, [exchange]
		lock cmpxchg [edx], ecx
		setz    al
		mov     byte ptr [_ret], al
	}
	//
	return _ret;
}

//return: 1 ha sikerult, 0, ha nem
MLINLINE int interlocked_compare_exchange_64(void* dest, __int64 exchange, __int64 compare)
//MLINLINE int tc_interlockedCompareExchange(void *dest, const int exchange1, const int exchange2, const int compare1, const int compare2)
{
	char _ret;
	//
	__asm
	{
		mov     ebx, dword ptr [exchange]
		mov     ecx, dword ptr [exchange+4]
		mov     edi, [dest]
		mov     eax, dword ptr [compare]
		mov     edx, dword ptr [compare+4]
		lock cmpxchg8b [edi]
		setz    al
		mov     byte ptr [_ret], al
	}
	return _ret;
}


#endif//_interlocked_h_