

#include "assert.h"
#include "ChunkStream.h"

static void Assert(int condition, const char* message = "Unspecified exception")
{
	if (!condition)
		__asm ("int $3");
}


MChunkHandle::MChunkHandle() :
mStream(0)
{
}

MChunkHandle::MChunkHandle(MChunkHandle* parent_chunk) :
mStream(parent_chunk->mStream),
mParent(parent_chunk),
mDepth(parent_chunk->mDepth+1)
{
	mName = mStream->ReadString(mParent->mSizeLeft);
	mTotalSize = mStream->ReadInt(mParent->mSizeLeft);
	mSizeLeft = mTotalSize;
	//Assert(mTotalSize > 0, "hey!");

	mStream->PushChunk(this);
}

MChunkHandle::MChunkHandle(MChunkStream* s) :
mStream(s),
mParent(0),
mDepth(0)
{
	unsigned tmp = 1000;
	mName = s->ReadString(tmp);
	mTotalSize = s->ReadInt(tmp);
	mSizeLeft = mTotalSize;

	mStream->PushChunk(this);
}

MChunkHandle::MChunkHandle(MChunkStream* s, const string& name) :
mStream(s),
mParent(0),
mDepth(0),
mName(name)
{
	unsigned tmp;
	mTotalSize = s->ReadInt(tmp);
	mSizeLeft = mTotalSize;

	mStream->PushChunk(this);
}

MChunkHandle::MChunkHandle(MChunkStream* s, const string& name, unsigned size) :
mStream(s),
mParent(0),
mDepth(0),
mName(name)
{
	mTotalSize = size;
	mSizeLeft = mTotalSize;

	mStream->PushChunk(this);
}

MChunkHandle::~MChunkHandle()
{
	if (mStream)
	{
		Close();
	}
}

void MChunkHandle::Close()
{
#ifdef _DEBUG
	Assert(mSizeLeft == 0, "Garbage at the end of chunk. Use Skip() if this is OK");

	if (mSizeLeft)
	{
		Skip();
		return;
	}
#endif
	if (mParent)
		mParent->mSizeLeft -= mTotalSize;
	mStream->PopChunk(this);
	mStream = 0;
}

void MChunkHandle::Skip()
{
	if (mParent)
		mParent->mSizeLeft -= mTotalSize;
	if (mSizeLeft)
	{
		unsigned dummy = 0;
		mStream->Skip(mSizeLeft);
		mSizeLeft = 0;
	}
	mStream->PopChunk(this);
	mStream = 0;
}

float MChunkHandle::ReadFloat()
{
	Assert(mStream!=NULL, "Reading from closed chunk");
	float f = mStream->ReadFloat(mSizeLeft);
	Assert(mSizeLeft >= 0, "Reading past end of chunk");
	return f;
}

char MChunkHandle::ReadChar()
{
	Assert(mStream!=NULL, "Reading from closed chunk");
	char c = mStream->ReadChar(mSizeLeft);
	Assert(mSizeLeft >= 0, "Reading past end of chunk");
	return c;
}

int MChunkHandle::ReadInt()
{
	Assert(mStream!=NULL, "Reading from closed chunk");
	int i = mStream->ReadInt(mSizeLeft);
	Assert(mSizeLeft >= 0, "Reading past end of chunk");
	return i;
}

unsigned MChunkHandle::ReadUnsigned()
{
	Assert(mStream!=NULL, "Reading from closed chunk");
	unsigned u = mStream->ReadUnsigned(mSizeLeft);
	Assert(mSizeLeft >= 0, "Reading past end of chunk");
	return u;
}

short MChunkHandle::ReadShort()
{
	Assert(mStream!=NULL, "Reading from closed chunk");
	short sh = mStream->ReadShort(mSizeLeft);
	Assert(mSizeLeft >= 0, "Reading past end of chunk");
	return sh;
}

string MChunkHandle::ReadString()
{
	Assert(mStream!=NULL, "Reading from closed chunk");
	string s = mStream->ReadString(mSizeLeft);
	Assert(mSizeLeft >= 0, "Reading past end of chunk");
	return s;
}

////////////////////////////////////////////////////////////////////////////////

MChunk::MChunk()
{
	ptr=NULL;
}

MChunk::MChunk(MChunkHandle* c) :
ptr(c)
{
}

MChunk::MChunk(const MChunk& other)
{
}

MChunk& MChunk::operator= (const MChunk& other)
{
	return *this;
}

MChunk::~MChunk()
{
	if (ptr) delete ptr;
}

void MChunk::Close()
{
	ptr->Close();
}

void MChunk::Skip()
{
	ptr->Skip();
}

float MChunk::ReadFloat()
{
	float f = ptr->ReadFloat();
	return f;
}

char MChunk::ReadChar()
{
	int i = ptr->ReadChar();
	return i;
}

int MChunk::ReadInt()
{
	int i = ptr->ReadInt();
	return i;
}

unsigned MChunk::ReadUnsigned()
{
	unsigned u = ptr->ReadUnsigned();
	return u;
}

short MChunk::ReadShort()
{
	short sh = ptr->ReadShort();
	return sh;
}

string MChunk::ReadString()
{
	string s = ptr->ReadString();
	return s;
}

MChunk MChunk::GetChunk()
{
	return MChunk(new MChunkHandle(&*ptr));
}

MChunk& MChunk::operator>> (float& f)
{
	f = ReadFloat();
	return *this;
}

MChunk& MChunk::operator>> (int& i)
{
	i = ReadInt();
	return *this;
}

MChunk& MChunk::operator>> (unsigned& u)
{
	u = ReadUnsigned();
	return *this;
}

MChunk& MChunk::operator>> (string& s)
{
	s = ReadString();
	return *this;
}

////////////////////////////////////////////////////////////////////////////////

MChunkStream::MChunkStream(file& i_file):
m_file(i_file)
{
}

MChunkStream::~MChunkStream()
{
}

MChunk MChunkStream::GetTopChunk()
{
	return MChunk(new MChunkHandle(this));
}

float MChunkStream::ReadFloat(unsigned& ucs)
{
//	Assert(!m_Istream.fail());
	float f = 0;
//	m_Istream.read((char*)&f, 4);
	m_file.read_bytes((char*)&f, 4);
	ucs -= 4;
	return f;
}

int MChunkStream::ReadInt(unsigned& ucs)
{
//	Assert(!m_Istream.fail());
	int i = 0;
//	m_Istream.read((char*)&i, 4);
	m_file.read_bytes((char*)&i, 4);
	ucs -= 4;
	return i;
}

char MChunkStream::ReadChar(unsigned& ucs)
{
//	Assert(!m_Istream.fail());
	ucs -= 1;
	char ch;
	m_file.read_bytes((char*)&ch, 1);
//	return m_Istream.get();
	return ch;
}

unsigned MChunkStream::ReadUnsigned(unsigned& ucs)
{
//	Assert(!m_Istream.fail());
	unsigned u = 0;
//	m_Istream.read((char*)&u, 4);
	m_file.read_bytes((char*)&u, 4);
	ucs -= 4;
	return u;
}

short MChunkStream::ReadShort(unsigned& ucs)
{
//	Assert(!m_Istream.fail());
	short sh = 0;
//	m_Istream.read((char*)&sh, sizeof(short));
	m_file.read_bytes((char*)&sh, 2);
	ucs -= sizeof(short);
	return sh;
}

string MChunkStream::ReadString(unsigned& ucs)
{
//	Assert(!m_Istream.fail());
	unsigned len = ReadUnsigned(ucs);
	char* buf = new char[len+1];
//	m_Istream.read(buf, len);
	m_file.read_bytes(buf,len);
	buf[len] = '\0';
	string ret = buf;
	delete buf;
	ucs -= len;
	return ret;
}

void MChunkStream::Skip(unsigned size)
{
//	unsigned pos = m_Istream.tellg();
//	m_Istream.seekg(size, std::ios_base::cur);
	m_file.seek_cur(size);
	//Assert(m_Istream.tellg() == pos + size && !m_Istream.fail());
}

void MChunkStream::PushChunk(MChunkHandle* chk)
{
	Assert(mStack.size() == chk->mDepth, "Hierarchy error during chunk reading. Forgot to close chunk?");
	mStack.push_back(chk->GetName());
}

void MChunkStream::PopChunk(MChunkHandle* chk)
{
	string last = mStack.back();
	Assert(mStack.size() == chk->mDepth + 1 && last == chk->GetName(),
		"Hierarchy error during chunk reading. Forgot to close chunk?");
	mStack.pop_back();
}













