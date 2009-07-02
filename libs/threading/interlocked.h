#ifndef _interlocked_h_
#define _interlocked_h_

MLINLINE void tc_spinloop()
{
	__asm { pause };
}


/*
MLINLINE void tc_interlockedExchange(void *dest, const __int64 exchange)
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

MLINLINE int tc_interlockedCompareExchange(void *dest,int exchange,int compare)
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

MLINLINE int tc_interlockedCompareExchange(void *dest, const int exchange1, const int exchange2, const int compare1, const int compare2)
{
	char _ret;
	//
	__asm
	{
		mov     ebx, [exchange1]
		mov     ecx, [exchange2]
		mov     edi, [dest]
		mov     eax, [compare1]
		mov     edx, [compare2]
		lock cmpxchg8b [edi]
		setz    al
			mov     byte ptr [_ret], al
	}
	return _ret;
}
*/

#endif//_interlocked_h_