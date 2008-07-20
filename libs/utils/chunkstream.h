#ifndef _M_CHUNKREADER_H_
#define _M_CHUNKREADER_H_

#include "containers/vector.h"
#include "containers/string.h"
//#include <fstream>
#include "file/file.h"
//#include <exception>

//#include <boost/format.hpp>

//#include <boost/shared_ptr.hpp>

class MChunk;
class MChunkHandle;
class MChunkStream;


////////////////////////////////////////////////////////////////////////////////

class MChunkStream
{
public:
	friend class MChunkHandle;

//	MChunkStream(const ctr::string& filename);
	MChunkStream(file::file& i_file);
	~MChunkStream();

	/// read the top chunk from the file
	/// you should use this chunk to interface with this file afterwards
	MChunk GetTopChunk();

protected:
	void PushChunk(MChunkHandle*);
	void PopChunk(MChunkHandle*);

	/// these are intended to be used from MChunk only
	float ReadFloat(unsigned& ucs);
	int ReadInt(unsigned& ucs);
	char ReadChar(unsigned& ucs);
	unsigned ReadUnsigned(unsigned& ucs);
	short ReadShort(unsigned& ucs);
	ctr::string ReadString(unsigned& ucs);
	void Skip(unsigned size);

	ctr::vector<ctr::string> mStack;

//	std::ifstream m_Istream;
	file::file& m_file;
};

////////////////////////////////////////////////////////////////////////////////

/// helper class to read chunks from a ChunkStream. Don't use this
/// class, use MChunk instead which is a proxy class to use handles
class MChunkHandle
{
	friend class MChunkStream;
	friend class MChunk;

	MChunkStream* mStream;
	MChunkHandle* mParent;
	ctr::string mName;
	int mDepth;
	unsigned mTotalSize;
	unsigned mSizeLeft;

public:

	MChunkHandle(MChunkHandle* p);

	MChunkHandle(MChunkStream* s);
	MChunkHandle(MChunkStream* s, const ctr::string& name);
	MChunkHandle(MChunkStream* s, const ctr::string& name, unsigned size);

	MChunkHandle(const MChunkHandle& c); // undefined
	MChunkHandle& operator= (const MChunkHandle& c); // undefined

	MChunkHandle();
	~MChunkHandle();

	// read functions
	//--------------------------------------------------------------------------------

	/// read a float from the current chunk
	float ReadFloat();

	/// read an int from the current chunk
	int ReadInt();

	/// read a char from the current chunk
	char ReadChar();

	/// read an unsigned int from the current chunk
	unsigned ReadUnsigned();

	// read a short from the current chunk
	short ReadShort();

	/// read a string from the current chunk
	ctr::string ReadString();

	/// start reading a sub-chunk inside the current chunk
	MChunk GetChunk();

	// stream-like read functions
	//--------------------------------------------------------------------------------

	/// end of chunk
	bool eof() const { return mSizeLeft == 0; }

	/// close the chunk so it can be reused
	void Close();

	/// skip the data left in the current chunk and close it.
	/// This function can be used as a "safe close" because it
	/// handles unprocessed data at the end of the chunk. 
	/// Close gives an error in this case.
	void Skip();

	/// close the chunk so it can be reused
	const ctr::string& GetName() const { return mName; }
};

////////////////////////////////////////////////////////////////////////////////

/// helper class to read chunks from a ChunkStream
class MChunk
{
//	boost::shared_ptr<MChunkHandle> ptr;
	MChunkHandle* ptr;

public:
	MChunk();

	MChunk(MChunkHandle* c);

	MChunk(const MChunk& other);
	MChunk& operator= (const MChunk& other);

	~MChunk();

	// read functions
	//--------------------------------------------------------------------------------

	/// read a float from the current chunk
	float ReadFloat();

	/// read an int from the current chunk
	int ReadInt();

	/// read a char from the current chunk
	char ReadChar();

	/// read an unsigned int from the current chunk
	unsigned ReadUnsigned();

	/// read a short from the current chunk
	short ReadShort();

	/// read a string from the current chunk
	ctr::string ReadString();

	/// start reading a sub-chunk inside the current chunk
	MChunk GetChunk();

	// stream-like read functions
	//--------------------------------------------------------------------------------

	/// read a float from the current chunk
	MChunk& operator>> (float& f);

	/// read an int from the current chunk
	MChunk& operator>> (int& i);

	/// read an unsigned int from the current chunk
	MChunk& operator>> (unsigned& u);

	/// read a string from the current chunk
	MChunk& operator>> (ctr::string& s);

	/// returns true at the end of the chunk
	bool operator! () const { return !ptr || ptr->eof(); }

	/// returns the status of the chunk
	operator bool () const { return ptr && !ptr->eof(); }

	/// end of chunk
	bool eof() const { return ptr->eof(); }

	/// close the chunk so it can be reused
	void Close();

	/// skip the data left in the current chunk and close it.
	/// This function can be used as a "safe close" because it
	/// handles unprocessed data at the end of the chunk. 
	/// Close gives an error in this case.
	void Skip();

	/// close the chunk so it can be reused
	const ctr::string& GetName() const { return ptr->GetName(); }

	int GetSizeLeft() const { return ptr->mSizeLeft; }

};

#endif // _M_CHUNKREADER_H_
