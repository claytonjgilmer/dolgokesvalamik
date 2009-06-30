#ifndef _M_CHUNKREADER_H_
#define _M_CHUNKREADER_H_

#include "containers/vector.h"
#include "containers/string.h"
//#include <fstream>
#include "file/file.h"
//#include <exception>

//#include <boost/format.hpp>

//#include <boost/shared_ptr.hpp>

struct MChunk;
struct MChunkHandle;
struct MChunkStream;


////////////////////////////////////////////////////////////////////////////////

struct MChunkStream
{
//	MChunkStream(const string& filename);
	MChunkStream(file_t& i_file);
	~MChunkStream();

	/// read the top chunk from the file
	/// you should use this chunk to interface with this file afterwards
	MChunk GetTopChunk();

	void PushChunk(MChunkHandle*);
	void PopChunk(MChunkHandle*);

	/// these are intended to be used from MChunk only
	f32 ReadFloat(unsigned& ucs);
	int ReadInt(unsigned& ucs);
	char ReadChar(unsigned& ucs);
	unsigned ReadUnsigned(unsigned& ucs);
	short ReadShort(unsigned& ucs);
	string ReadString(unsigned& ucs);
	void Skip(unsigned size);

	vector<string> mStack;

	file_t& m_file;
};

////////////////////////////////////////////////////////////////////////////////

/// helper class to read chunks from a ChunkStream. Don't use this
/// class, use MChunk instead which is a proxy class to use handles
struct MChunkHandle
{
	MChunkStream* mStream;
	MChunkHandle* mParent;
	string mName;
	unsigned mDepth;
	unsigned mTotalSize;
	unsigned mSizeLeft;

	MChunkHandle(MChunkHandle* p);

	MChunkHandle(MChunkStream* s);
	MChunkHandle(MChunkStream* s, const string& name);
	MChunkHandle(MChunkStream* s, const string& name, unsigned size);

	MChunkHandle(const MChunkHandle& c); // undefined
	MChunkHandle& operator= (const MChunkHandle& c); // undefined

	MChunkHandle();
	~MChunkHandle();

	// read functions
	//--------------------------------------------------------------------------------

	/// read a f32 from the current chunk
	f32 ReadFloat();

	/// read an int from the current chunk
	int ReadInt();

	/// read a char from the current chunk
	char ReadChar();

	/// read an unsigned int from the current chunk
	unsigned ReadUnsigned();

	// read a short from the current chunk
	short ReadShort();

	/// read a string from the current chunk
	string ReadString();

	/// start reading a sub-chunk inside the current chunk
	MChunk GetChunk();

	// stream-like read functions
	//--------------------------------------------------------------------------------

	/// end of chunk
	int eof() const { return mSizeLeft == 0; }

	/// close the chunk so it can be reused
	void Close();

	/// skip the data left in the current chunk and close it.
	/// This function can be used as a "safe close" because it
	/// handles unprocessed data at the end of the chunk.
	/// Close gives an error in this case.
	void Skip();

	/// close the chunk so it can be reused
	const string& GetName() const { return mName; }
};

////////////////////////////////////////////////////////////////////////////////

/// helper class to read chunks from a ChunkStream
struct MChunk
{
//	boost::shared_ptr<MChunkHandle> ptr;
	MChunkHandle* ptr;

	MChunk();

	MChunk(MChunkHandle* c);

	MChunk(const MChunk& other);
	MChunk& operator= (const MChunk& other);

	~MChunk();

	// read functions
	//--------------------------------------------------------------------------------

	/// read a f32 from the current chunk
	f32 ReadFloat();

	/// read an int from the current chunk
	int ReadInt();

	/// read a char from the current chunk
	char ReadChar();

	/// read an unsigned int from the current chunk
	unsigned ReadUnsigned();

	/// read a short from the current chunk
	short ReadShort();

	/// read a string from the current chunk
	string ReadString();

	/// start reading a sub-chunk inside the current chunk
	MChunk GetChunk();

	// stream-like read functions
	//--------------------------------------------------------------------------------

	/// read a f32 from the current chunk
	MChunk& operator>> (f32& f);

	/// read an int from the current chunk
	MChunk& operator>> (int& i);

	/// read an unsigned int from the current chunk
	MChunk& operator>> (unsigned& u);

	/// read a string from the current chunk
	MChunk& operator>> (string& s);

	/// returns true at the end of the chunk
	int operator! () const { return !ptr || ptr->eof(); }

	/// returns the status of the chunk
	operator int () const { return ptr && !ptr->eof(); }

	/// end of chunk
	int eof() const { return ptr->eof(); }

	/// close the chunk so it can be reused
	void Close();

	/// skip the data left in the current chunk and close it.
	/// This function can be used as a "safe close" because it
	/// handles unprocessed data at the end of the chunk.
	/// Close gives an error in this case.
	void Skip();

	/// close the chunk so it can be reused
	const string& GetName() const { return ptr->GetName(); }

	int GetSizeLeft() const { return ptr->mSizeLeft; }

};

#endif // _M_CHUNKREADER_H_
