#ifndef _stringhash_h_
#define _stringhash_h_

#include "base/misc.h"
#include "containers/string.h"

namespace utils
{
	MLINLINE unsigned int RSHash(const ctr::string& str)
	{
		unsigned int b    = 378551;
		unsigned int a    = 63689;
		unsigned int hash = 0;

		for(unsigned i = 0; i < str.size(); i++)
		{
			hash = hash * a + str[i];
			a    = a * b;
		}

		return hash;
	}
	/* End Of RS Hash  */


	MLINLINE unsigned int JSHash(const ctr::string& str)
	{
		unsigned int hash = 1315423911;

		for(unsigned i = 0; i < str.size(); i++)
		{
			hash ^= ((hash << 5) + str[i] + (hash >> 2));
		}

		return hash;
	}
	/* End Of JS Hash  */


	MLINLINE unsigned int PJWHash(const ctr::string& str)
	{
		const unsigned int BitsInUnsignedInt = (unsigned int)(sizeof(unsigned int) * 8);
		const unsigned int ThreeQuarters     = (unsigned int)((BitsInUnsignedInt  * 3) / 4);
		const unsigned int OneEighth         = (unsigned int)(BitsInUnsignedInt / 8);
		const unsigned int HighBits          = (unsigned int)(0xFFFFFFFF) << (BitsInUnsignedInt - OneEighth);

		unsigned int hash              = 0;
		unsigned int test              = 0;

		for(unsigned i = 0; i < str.size(); i++)
		{
			hash = (hash << OneEighth) + str[i];

			if((test = hash & HighBits)  != 0)
			{
				hash = (( hash ^ (test >> ThreeQuarters)) & (~HighBits));
			}
		}

		return hash;
	}
	/* End Of  P. J. Weinberger Hash  */


	MLINLINE unsigned int ELFHash(const ctr::string& str)
	{
		unsigned int hash = 0;
		unsigned int x    = 0;

		for(unsigned i = 0; i < str.size(); i++)
		{
			hash = (hash << 4) + str[i];
			if((x = hash & 0xF0000000L) != 0)
			{
				hash ^= (x >> 24);
			}
			hash &= ~x;
		}

		return hash;
	}
	/* End Of ELF Hash  */


	MLINLINE unsigned int BKDRHash(const ctr::string& str)
	{
		unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
		unsigned int hash = 0;

		for(unsigned i = 0; i < str.size(); i++)
		{
			hash = (hash * seed) + str[i];
		}

		return hash;
	}
	/* End Of BKDR Hash  */


	MLINLINE unsigned int SDBMHash(const ctr::string& str)
	{
		unsigned int hash = 0;

		for(unsigned i = 0; i < str.size(); i++)
		{
			hash = str[i] + (hash << 6) + (hash << 16) - hash;
		}

		return hash;
	}
	/* End Of SDBM Hash  */


	MLINLINE unsigned int DJBHash(const ctr::string& str)
	{
		unsigned int hash = 5381;

		for(unsigned i = 0; i < str.size(); i++)
		{
			hash = ((hash << 5) + hash) + str[i];
		}

		return hash;
	}
	/* End Of DJB Hash  */


	MLINLINE unsigned int DEKHash(const ctr::string& str)
	{
		unsigned int hash = static_cast<unsigned int>(str.size());

		for(unsigned i = 0; i < str.size(); i++)
		{
			hash = ((hash << 5) ^ (hash >> 27)) ^ str[i];
		}

		return hash;
	}
	/* End Of DEK Hash  */


	MLINLINE unsigned int BPHash(const ctr::string& str)
	{
		unsigned int hash = 0;
		for(unsigned i = 0; i < str.size(); i++)
		{
			hash = hash << 7 ^ str[i];
		}

		return hash;
	}
	/* End Of BP Hash  */


	MLINLINE unsigned int FNVHash(const ctr::string& str)
	{
		const unsigned int fnv_prime = 0x811C9DC5;
		unsigned int hash = 0;
		for(unsigned i = 0; i < str.size(); i++)
		{
			hash *= fnv_prime;
			hash ^= str[i];
		}

		return hash;
	}
	/* End Of FNV Hash  */


	MLINLINE unsigned int APHash(const ctr::string& str)
	{
		unsigned int hash = 0xAAAAAAAA;

		for(unsigned i = 0; i < str.size(); i++)
		{
			hash ^= ((i & 1) == 0) ? (  (hash <<  7) ^ str[i] ^ (hash >> 3)) :
				(~((hash << 11) ^ str[i] ^ (hash >> 5)));
		}

		return hash;
	}
	/* End Of AP Hash  */


	MLINLINE unsigned Hash (ctr::string const& s)
	{
		unsigned int const shift = 6;
		unsigned const mask = ~0U << (32 - shift);
		unsigned result = 0;

		for (unsigned int i = 0; s [i] != 0; ++i)
		{
			result = (result & mask) ^ (result << shift) ^ s [i];
		}

		return result;
	}
}
#endif//_stringhash_h_