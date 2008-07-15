// mmod-dump.cpp : Defines the entry point for the console application.
//

#include "ChunkStream.h"
#include "Tga.h"
#include <exception>

#include <boost/format.hpp>
#include <map>

#include <windows.h>

#include "render/renderobject3d.h"

using namespace std;

void test();

ostream* output_stream = 0;

ostream& output()
{
	return *output_stream;
}

int all_promitivecount=0;
int all_vertexcount=0;


////////////////////////////////////////////////////////////////////////////////
typedef vector<unsigned short> IndexArray;
typedef vector<string> StringArray;

struct Vect
{
	Vect() {}
	Vect(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

	void Set(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	Vect& operator+=(const Vect& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;

		return *this;
	}

	Vect& operator-=(const Vect& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;

		return *this;
	}

	Vect operator+(const Vect& rhs) const
	{
		return Vect(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	Vect operator-(const Vect& rhs) const
	{
		return Vect(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	Vect operator*(float rhs) const
	{
		return Vect(x * rhs, y * rhs, z * rhs);
	}

	Vect operator^(const Vect& rhs)
	{
		return Vect(y * rhs.z - z * rhs.y,
			z * rhs.x - x * rhs.z,
			x * rhs.y - y * rhs.x);
	}

	float Len2() { return x*x + y*y + z*z; }
	float Len() { return sqrtf(Len2()); }

	float GetX() const { return x; }
	float GetY() const { return y; }
	float GetZ() const { return z; }
protected:
	float x, y, z;
};

float Area(const Vect& v0, const Vect& v1, const Vect& v2)
{
	return (v1-v0 ^ v2-v0).Len() / 2.f;
}

float Area3(const Vect v[])
{
	return Area(v[0], v[1], v[2]);
}

struct SubsetInfo
{
	int vertexStart;
	int vertexNum;
	int indexStart;
	int indexNum;
	StringArray textureNames;
	string shaderName;
	int vertexstreamidx;
};

struct LODInfo
{
	float minLODError;
	float maxLODError;
	int startSubset;
	int endSubset;
};

typedef vector<float> VertexArray;

class Stream
{
public:
	void AddFloat(const VertexArray& i_FloatArr)
	{
		copy(i_FloatArr.begin(), i_FloatArr.end(), back_inserter(m_Vertices));
	}

	const float* GetFloat(int vertexIndex) const
	{
		size_t i = vertexIndex * m_FloatPerVertices;

		assert(i < m_Vertices.size());

		return &m_Vertices[i];
	}

	void SetFloatPerVertices(int i_FloatPerVertices)
	{
		m_FloatPerVertices = i_FloatPerVertices;
	}

	int GetFloatPerVertices() const
	{
		return m_FloatPerVertices;
	}

	void SetMVF(const string& str)
	{
		m_MVF = str;
	}

	const string& GetMVF() const
	{
		return m_MVF;
	}

protected:
	int			m_FloatPerVertices;
	string		m_MVF;
	VertexArray	m_Vertices;
};

class MeshLODInfo
{
public:
	MeshLODInfo() : m_RefCount(0)
	{
	}

	void ClearStreams()
	{
		m_Streams.clear();
		m_Indices.clear();
	}

	IndexArray& GetIndexArray()
	{
		return	m_Indices;
	}

	void AddSubSetInfo(const SubsetInfo& i_Subset)
	{
		m_SubSet.push_back(i_Subset);
	}
	void AddLODInfo(const LODInfo& i_LOD)
	{
		m_LOD.push_back(i_LOD);
	}
	void Clear()
	{
		m_SubSet.clear();
		m_LOD.clear();
		m_RefCount = 0;
	}

	size_t GetSubSetNum() const
	{
		return m_SubSet.size();
	}
	const SubsetInfo& GetSubSet(size_t i) const
	{
		return m_SubSet[i];
	}

	size_t GetLODNum() const
	{
		return m_LOD.size();
	}
	const LODInfo& GetLOD(size_t i) const
	{
		return m_LOD[i];
	}

	size_t GetStreamNum() const
	{
		return m_Streams.size();
	}
	const Stream& GetStream(size_t i) const
	{
		return m_Streams[i];
	}
	void AddStream()
	{
		m_Streams.push_back(Stream());
	}
	Stream& GetLastStream()
	{
		assert(m_Streams.size() > 0);
		return m_Streams[m_Streams.size()-1];
	}

	void IncRefCount() { ++m_RefCount; }
	int  GetRefCount() const { return m_RefCount; }
protected:
	typedef vector<SubsetInfo> SubsetArray;
	typedef vector<LODInfo> LODArray;
	typedef vector<Stream> StreamArray;

	SubsetArray	m_SubSet;
	LODArray	m_LOD;

	StreamArray	m_Streams;
	IndexArray	m_Indices;

	int			m_RefCount;
};

inline float Finf()
{
	return std::numeric_limits<float>::infinity();
}

struct TextureInfo{
	TextureInfo() :	densityMin(Finf()), densityMax(-Finf()), densityAvg1(0.f),	densityAvg2(0.f), width(0), height(0) {}

	int Size() { return width * height; }

	float	densityMin;
	float	densityMax;
	float	densityAvg1;
	float	densityAvg2;
	int		width;
	int		height;
};

typedef map<string, TextureInfo> TextureInfoMap;
TextureInfoMap gTextureInfo;

vector<MeshLODInfo> gMeshLODInfoArray;
float gSpereRadius, gSphereCenterZ;
VertexArray gPrintedFloatArray;

enum {
	STAT_MODE_OFF = 0,
	STAT_MODE_MODEL_PERF = 1<<0,
	STAT_MODE_TEXEL_PERF = 1<<1,
	STAT_MODE_HIERARCHY_OPTIMIZATION_CHECK = 1<<2,
	STAT_MODE_SHIP_CONTOUR_DETECTION = 1<<3,
};

int gLODStatisticsMode;
string gInputFileName;

vector<string> gResourceTypeNames;

const float	gStepZ = 0.1f; // in meter
map<float, float>	gZXContourMap;
float gDeepestY;
float gWaveSternAvgZ;

inline void Extend(float& valMin, float& valMax, float val)
{
	valMin = min(valMin, val);
	valMax = max(valMax, val);
}

inline float Limit(float x, float a, float b)
{
	if (x < a)
		x = a;
	else if (x > b)
		x = b;

	return x;
}

inline bool InInterval(float x, float a, float b)
{
	return a<=x && x<=b;
}

inline bool InIntervalSafe(float x, float a, float b)
{
	if (a <= b)
		return InInterval(x, a, b);
	else
		return InInterval(x, b, a);
}

int Inc3(int i)
{
	if (++i == 3)
		i = 0;
	return i;
}

bool ClipSectionToY(float i_Y, const Vect& i_V0, const Vect& i_V1, Vect& o_VP)
{
	bool ret = false;

	if (InIntervalSafe(i_Y, i_V0.GetY(), i_V1.GetY()))
	{
		Vect v10 = i_V1 - i_V0;
		//		if (abs(v10.GetY()) > 0.001f)
		{
			float t = (i_Y - i_V0.GetY()) / v10.GetY();
			o_VP = i_V0 + v10 * t;

			ret = true;
		}
	}

	return ret;
}

void CheckTriangle(const Vect i_vTri[])
{
	float xMin = Finf();
	float xMax = -Finf();

	float yMin = Finf();
	float yMax = -Finf();

	float zMin = Finf();
	float zMax = -Finf();

	for (int j=0; j<3; ++j)
	{
		Extend(xMin, xMax, i_vTri[j].GetX());
		Extend(yMin, yMax, i_vTri[j].GetY());
		Extend(zMin, zMax, i_vTri[j].GetZ());
	}

	// for deepest y coord
	gDeepestY = min(gDeepestY, yMin);

	if (xMin >= 0.f) // for symmetry in x
	{
		const float yClip = 0.f;//-0.05f;
		if (InInterval(yClip, yMin, yMax)) // y=0 plane crossing triangle
		{
			float z1 = floor(zMin / gStepZ);
			float z2 = floor(zMax / gStepZ);
			if (z1 != z2) // zStep crossing in granularity
			{
				Vect clipPoints[3];
				int clipnum = 0;
				for (int j=0; j<3; ++j)
				{
					if (ClipSectionToY(yClip, i_vTri[j], i_vTri[Inc3(j)], clipPoints[clipnum]))
						++clipnum;
				}
				assert(clipnum==2);

				if (clipPoints[0].GetZ() > clipPoints[1].GetZ())
					swap(clipPoints[0], clipPoints[1]);

				Vect v10 = clipPoints[1] - clipPoints[0];
				float dX = v10.GetX() / v10.GetZ();

				float zF = floor(clipPoints[0].GetZ() / gStepZ);
				float zL = floor(clipPoints[1].GetZ() / gStepZ);
				if (zF * gStepZ != clipPoints[0].GetZ())
				{
					zF += 1.f;
				}

				float x = clipPoints[0].GetX() + dX * (zF*gStepZ - clipPoints[0].GetZ());
				dX *= gStepZ;
				for (; zF <= zL; zF += 1.f, x += dX)
				{
					assert(InInterval(x, xMin, xMax));

					// Finding max x for contour at z
					if (gZXContourMap.find(zF) == gZXContourMap.end() || gZXContourMap[zF] < x)
					{
						gZXContourMap[zF] = x;
					}
				}
			}
		}
	}
}

void CalcShipContour()
{
	MeshLODInfo& curMesh = gMeshLODInfoArray.back();

	size_t lod0Start;
	size_t lod0End;
	if (curMesh.GetLODNum() > 0)
	{
		lod0Start = curMesh.GetLOD(0).startSubset;
		lod0End = curMesh.GetLOD(0).endSubset;
	}
	else
	{
		lod0Start = 0;
		lod0End = curMesh.GetSubSetNum()-1;
	}


	// Only lod0 phase takes effect in high detail contour
	const IndexArray& indices = curMesh.GetIndexArray();
	for (size_t si = lod0Start; si<=lod0End; ++si)
	{
		const SubsetInfo& subset = curMesh.GetSubSet(si);
		if (subset.shaderName == "ship.mshd") //only ship vertex contour
		{
			const Stream& stream = curMesh.GetStream(subset.vertexstreamidx);
			IndexArray::const_iterator iIt = indices.begin() + subset.indexStart;
			for (int n=0; n<subset.indexNum; ++n)
			{
				// Check triangle for ship contour
				Vect vTri[3];
				for (int j=0; j<3; ++j)
				{
					const float *pF = stream.GetFloat(*iIt++);
					vTri[j].Set(pF[0], pF[1], pF[2]);
				}

				CheckTriangle(vTri);
			}
		}
	}
}

bool GetFullPathName(const string& directory, const string& fName, string& fFullName)
{
	WIN32_FIND_DATA FindData;
	HANDLE Handle = FindFirstFile((directory + '*').c_str() , &FindData );

	if (Handle != INVALID_HANDLE_VALUE)
	{
		do
		{
			if ( FindData.cFileName[0] == '.' )
				continue;

			if ( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				// Directory
				if (GetFullPathName(directory+FindData.cFileName+'/', fName, fFullName))
					return true;
			}
			else
			{
				// File
				if (fName == FindData.cFileName)
				{
					fFullName = directory + FindData.cFileName;
					return true;
				}
			}
		}
		while (FindNextFile( Handle, &FindData ) );
	}

	return false;
}

void AddTextureInfo(const string& str)
{
	if (!(gLODStatisticsMode & STAT_MODE_TEXEL_PERF))
		return;

	if (gTextureInfo.find(str) == gTextureInfo.end())
	{
		TextureInfo ti;
		{
			for (string::iterator it = gInputFileName.begin(); it != gInputFileName.end(); ++it)
			{
				if (*it == '\\')
					*it = '/';
			}

			string resPath("Y:/MidwayResource/");
			{
				string dataPath("Y:/MidwayData/");

				size_t s1 = dataPath.size();
				size_t s2 = gInputFileName.find_first_of("/", s1);
				if (s2 == string::npos)
					/*resPath += "PC/"*/;
				else
					resPath += gInputFileName.substr(s1, s2-s1+1);
			}

			string fFullName;
			if (GetFullPathName(resPath, str, fFullName) || GetFullPathName(resPath="Y:/MidwayResource/PC/", str, fFullName))
			{
				unsigned char cBuf[sizeof(TGAHeader)];
				FILE *fp = fopen(fFullName.c_str(), "rb");
				fread(cBuf, sizeof(TGAHeader), 1, fp);
				fclose(fp);

				TGAHeader tgaHeader;
				GetTGA(cBuf, tgaHeader);

				ti.width = tgaHeader.imagespec.width;
				ti.height = tgaHeader.imagespec.height;
			}
			else
			{
				cout << str << " not found in " << resPath << endl;
			}
		}

		gTextureInfo[str] = ti;
	}
}

TextureInfo& GetTextureInfo(const string& str)
{
	assert(gTextureInfo.find(str) != gTextureInfo.end());
	return gTextureInfo[str];
}


////////////////////////////////////////////////////////////////////////////////

class Indenter
{
public:
	static int mIndentLevel;

	friend ostream& operator<< (ostream& os, const Indenter& idr)
	{
		for (int i = 0; i < idr.mIndentLevel; ++i)
			output() << ' ';
		return os;
	}

	static void Indent() { mIndentLevel += 4; }
	static void Unindent() { mIndentLevel -= 4; }

};

int Indenter::mIndentLevel = 0;
Indenter indent;

class IndentLevel
{
public:
	IndentLevel() { output() << indent << "{\n"; Indenter::Indent(); }
	~IndentLevel() { Indenter::Unindent(); output() << indent << "}\n"; }
};

int Version;

////////////////////////////////////////////////////////////////////////////////

void PrintChunkName(MChunk& chunk)
{
	output() << indent << "Chunk \"" << chunk.GetName() << "\"\n";
}

void PrintFloats(MChunk& chunk, int count)
{
	gPrintedFloatArray.resize(0);

	output() << indent;
	for (int i = 0; i < count; ++i)
	{
		float fl = chunk.ReadFloat();
		char num[64];
		sprintf(num, " %8.3f", fl);
		output() << num;

		gPrintedFloatArray.push_back(fl);
	}
	output() << '\n';
}

template <class T>
void PrintArray(MChunk& chunk)
{
	IndentLevel il;
	int count = chunk.ReadInt();
	for (int i = 0; i < count; ++i)
	{
		T data;
		chunk >> data;
		output() << indent << data << '\n';
	}
}

void PrintRaw(MChunk& chunk, int bytes)
{
	string line;
	char num[128];
	for (int i = 0; i < bytes; ++i)
	{
		unsigned byte = (chunk.ReadChar()) & 0xff;
		sprintf(num, " %02x", byte);
		line += num;
	}
	output() << indent << line << '\n';
}

void PrintMatrix(MChunk& chunk)
{
	for (int row = 0; row < 4; ++row)
	{
		PrintFloats(chunk, 4);
	}
}

////////////////////////////////////////////////////////////////////////////////

void PrintVertexStreamChunk(MChunk& chunk)
{
	IndentLevel il;
	int vertexcount = chunk.ReadInt();
	output() << indent << "VertexCount " << vertexcount << "\n";
	all_vertexcount+=vertexcount;
	string strVF = chunk.ReadString();
	output() << indent << "VertexFormatName \"" << strVF << "\"\n";
	unsigned sizeleft = chunk.GetSizeLeft();
	output() << indent << "RawData size " << sizeleft << "\n";
	output() << indent << "Vertex size " << (float)sizeleft/(float)vertexcount << "\n";

	unsigned vertexsize = sizeleft/vertexcount;
	assert(vertexsize * vertexcount == sizeleft);

	gMeshLODInfoArray.back().AddStream();	
	gMeshLODInfoArray.back().GetLastStream().SetFloatPerVertices(vertexsize / 4);
	gMeshLODInfoArray.back().GetLastStream().SetMVF(strVF);

	IndentLevel il2;
	for (int i = 0; i < vertexcount; ++i)
	{
		PrintFloats(chunk, vertexsize / 4);
		if (gLODStatisticsMode & STAT_MODE_MODEL_PERF)
		{
			gMeshLODInfoArray.back().GetLastStream().AddFloat(gPrintedFloatArray);
		}
	}
}

//#define PRINT_INDICES

void PrintIndicesChunk(MChunk& chunk)
{
	IndentLevel il;
	bool printInd = (gLODStatisticsMode & STAT_MODE_MODEL_PERF) ? true : false;
#ifdef PRINT_INDICES
	printInd = true;
#endif
	if (!printInd)
	{
		output() << indent << "IndexCount " << chunk.ReadInt() << "\n";
		chunk.Skip();
	}
	else
	{
		int indexCnt = chunk.ReadInt();
		output() << indent << "IndexCount " << indexCnt << "\n";
		output() << indent << "Format " << chunk.ReadInt() << "\n";

		IndexArray::iterator iti;
		if (gLODStatisticsMode & STAT_MODE_MODEL_PERF)
		{
			gMeshLODInfoArray.back().GetIndexArray().resize(indexCnt);
			iti = gMeshLODInfoArray.back().GetIndexArray().begin();
		}

		for (int i=0; i<indexCnt; ++i)
		{
			unsigned short sh = (unsigned short)chunk.ReadShort();
			output() << indent << sh << "\n";

			if (gLODStatisticsMode & STAT_MODE_MODEL_PERF)
				*iti++ = sh;
		}
	}
}

void PrintTextureChunk(MChunk& chunk, SubsetInfo& subset)
{
	IndentLevel il;

	string str = chunk.ReadString();
	subset.textureNames.push_back(str);
	AddTextureInfo(str);

	output() << indent << "TextureName \"" << str << "\"\n";
	output() << indent << "TextureSlotIndex " << chunk.ReadInt() << "\n";

	while (chunk)
	{
		MChunk subchunk = chunk.GetChunk();
		PrintChunkName(subchunk);
		subchunk.Skip();
	}
}

void PrintBoundingBoxChunk(MChunk& chunk)
{
	IndentLevel il;

	float ix, iy, iz;
	float ax, ay, az;
	chunk >> ix >> iy >> iz;
	chunk >> ax >> ay >> az;

	output() << indent << "MinCorner " << ix << " " << iy << " " << iz << "\n";
	output() << indent << "MaxCorner " << ax << " " << ay << " " << az << "\n";
}

void PrintBoundingSphereChunk(MChunk& chunk)
{
	IndentLevel il;

	float cx, cy, cz;
	float radius;
	chunk >> cx >> cy >> cz >> radius;

	if (gSpereRadius == 0.f)
	{
		gSpereRadius = radius;
		gSphereCenterZ = cz;
	}

	output() << indent << "Center " << cx << " " << cy << " " << cz << "\n";
	output() << indent << "Radius " << radius << "\n";
}

void PrintSubsetChunk(MChunk& chunk)
{
	IndentLevel il;
	output() << indent << "PrimitiveType " << chunk.ReadInt() << "\n";

	SubsetInfo subset;
	subset.vertexStart = chunk.ReadInt();
	subset.vertexNum = chunk.ReadInt();
	subset.indexStart = chunk.ReadInt();
	subset.indexNum = chunk.ReadInt();
	subset.shaderName = chunk.ReadString();
	subset.vertexstreamidx = 0;

	output() << indent << "MinIndex " << subset.vertexStart << "\n";
	output() << indent << "NumVertices " << subset.vertexNum << "\n";
	output() << indent << "StartIndex " << subset.indexStart << "\n";
	output() << indent << "PrimitiveCount " << subset.indexNum << "\n";
	output() << indent << "ShaderName \"" << subset.shaderName << "\"\n";
	all_promitivecount +=subset.indexNum;


	while (chunk)
	{
		MChunk subchunk = chunk.GetChunk();
		PrintChunkName(subchunk);
		if (subchunk.GetName() == "VertexStreamIndex")
		{
			IndentLevel il;
			subset.vertexstreamidx = subchunk.ReadInt();
			output() << indent << "VertexStreamIndex \"" << subset.vertexstreamidx << "\"\n";
			subchunk.Skip();
		}
		else if (subchunk.GetName() == "Texture")
		{
			PrintTextureChunk(subchunk, subset);
		}
		else if (subchunk.GetName() == "BoundingSphere")
		{
			PrintBoundingSphereChunk(subchunk);
		}
		else
		{
			subchunk.Skip();
		}
	}

	gMeshLODInfoArray.back().AddSubSetInfo(subset);
}

void PrintLODPhasesChunk(MChunk &subchunk)
{
	IndentLevel il;
	int LODPhaseCount= subchunk.ReadInt();
	output() << indent << "NumLodPhases:" << LODPhaseCount << "\n";

	for (int i=0; i<LODPhaseCount; ++i)
	{
		output() << indent << i << ".\n";
		{
			LODInfo lod;
			lod.minLODError = subchunk.ReadFloat();
			lod.maxLODError = subchunk.ReadFloat();
			lod.startSubset = subchunk.ReadInt();
			lod.endSubset   = subchunk.ReadInt();
			gMeshLODInfoArray.back().AddLODInfo(lod);

			IndentLevel il;
			output() << indent << "MinLODError:" << lod.minLODError << "\n";
			output() << indent << "MaxLODError:" << lod.maxLODError << "\n";
			output() << indent << "StartSubset:" << lod.startSubset << "\n";
			output() << indent << "  EndSubset:" << lod.endSubset << "\n";
		}
	}

}

void PrintIndicesStripPS2Chunk(MChunk &subchunk)
{
	IndentLevel il;
#ifndef PRINT_INDICES
	output() << indent << "IndexCount " << subchunk.ReadInt() << "\n";
	subchunk.Skip();
#else
	int indexCnt = subchunk.ReadInt();
	output() << indent << "IndexCount " << indexCnt << "\n";
	for (int i=0; i<indexCnt; ++i)
	{
		short sh = subchunk.ReadShort();
		if (sh >= 0){
			output() << indent << sh << "\n";
		}
		else{
			sh &= ~0x8000;
			output() << indent << sh << " nokick\n";
		}
	}
#endif
}

void PrintSubsetPS2Chunk(MChunk& chunk)
{
	IndentLevel il;
	output() << indent << "FirstIndex " << chunk.ReadInt() << "\n";
	output() << indent << "NumIndex " << chunk.ReadInt() << "\n";
}


void CalcTexelPerformanceStatistics()
{
	MeshLODInfo& meshLod = gMeshLODInfoArray.back();
	IndexArray::const_iterator iti = meshLod.GetIndexArray().begin();

	for (size_t j=0; j<meshLod.GetSubSetNum(); ++j)
	{
		const SubsetInfo& subset = meshLod.GetSubSet(j);

		int uv0 = 2*3;
		int uv1 = -1;

		if (subset.shaderName.find("ship") == 0)
		{
			if (meshLod.GetStream(0).GetMVF() == "defaultps2.mvfm")
			{
			}
			else if (meshLod.GetStream(0).GetMVF() == "defaultcolorps2.mvfm")
			{
				uv0 = 3*3;
			}
			else
			{
				uv0 = 4*3;
				uv1 = uv0 + 2;
			}
		}
		else if (subset.shaderName.find("airplane") == 0)
		{
			if (meshLod.GetStream(0).GetMVF() == "defaultps2.mvfm")
			{
			}
			else if (meshLod.GetStream(0).GetMVF() == "defaultcolorps2.mvfm")
			{
				uv0 = 3*3;
			}
			else
			{
				uv0 = 4*3;
			}
		}

		for (int k=0; k<subset.indexNum; ++k, iti += 3)
		{
			Vect vP[3];
			Vect vUV0[3];
			Vect vUV1[3];
			for (int t=0; t<3; ++t)
			{
				size_t idx = iti[t];
				const float* pF = meshLod.GetStream(0).GetFloat(idx);

				vP[t].Set(pF[0], pF[1], pF[2]);

				vUV0[t].Set(pF[uv0], pF[uv0+1], 1.f);

				if (uv1 >= 0)
					vUV1[t].Set(pF[uv1], pF[uv1+1], 1.f);
			}

			float area = Area3(vP);
			float areaUV0 = Area3(vUV0);
			float areaUV1 = uv1 >= 0 ? Area3(vUV1) : 0.f;

			for (size_t k=0; k<subset.textureNames.size(); ++k)
			{
				float areaUV = (uv1 >= 0 && (k==1 || k==2)) ? areaUV1 : areaUV0;
				if (areaUV <= 0.f)
					continue;

				TextureInfo& ti = GetTextureInfo(subset.textureNames[k]);
				areaUV *= ti.Size();

				ti.densityAvg1 += area;
				ti.densityAvg2 += areaUV;

				float density = area / areaUV;
				ti.densityMin = min(ti.densityMin, density);
				ti.densityMax = max(ti.densityMax, density);
			}
		}
	}
}

inline float Meter2Centimeter(float x)
{
	return 100.f * x;
}

void PrintTexelPerfInfo()
{
	TextureInfoMap::const_iterator it = gTextureInfo.begin();
	for (; it != gTextureInfo.end(); ++it)
	{
		cout << it->first << " [" << it->second.width << "x" << it->second.height << "]" << endl;

		float dMin = Meter2Centimeter(sqrtf(it->second.densityMin));
		if (dMin == Finf())
		{
			cout << "None";
		}
		else
		{
			float dMax = Meter2Centimeter(sqrtf(it->second.densityMax));
			float dAvg = Meter2Centimeter(sqrtf(it->second.densityAvg1 / it->second.densityAvg2));

			char cBuf[128];
			sprintf(cBuf, "%.3f\t[%.3f - %.0f]", dAvg, dMin, dMax);
			cout << string(cBuf);
		}

		cout <<	endl;
	}
}

void PrintCompressedVertexFormatDataChunk(MChunk& chunk)
{
	IndentLevel ilevel;

	int Size = chunk.GetSizeLeft();

	for (int i=0;i<Size/32;i++)
	{
		float bias[4];
		float scale[4];

		scale[0]= chunk.ReadFloat();
		scale[1]= chunk.ReadFloat();
		scale[2]= chunk.ReadFloat();
		scale[3]= chunk.ReadFloat();

		bias[0]= chunk.ReadFloat();
		bias[1]= chunk.ReadFloat();
		bias[2]= chunk.ReadFloat();
		bias[3]= chunk.ReadFloat();

		output() << indent << "Scale : " << scale[0] << "," << scale[1] << "," << scale[2] << "," << scale[3] << "\n";
		output() << indent << "Bias  : " << bias[0] << "," << bias[1] << "," << bias[2] << "," << bias[3] << "\n";
	}
}

void PrintMeshChunk(MChunk& chunk)
{
	IndentLevel il;
	output() << indent << "Index \"" << chunk.ReadInt() << "\"\n";

	gMeshLODInfoArray.push_back(MeshLODInfo());
	while (chunk)
	{
		MChunk subchunk = chunk.GetChunk();
		PrintChunkName(subchunk);
		if (subchunk.GetName() == "VertexStream")
		{
			PrintVertexStreamChunk(subchunk);
		}
		else if (subchunk.GetName() == "CompressedVertexFormatData")
		{
			PrintCompressedVertexFormatDataChunk(subchunk);
		}
		else if (subchunk.GetName() == "Indices")
		{
			PrintIndicesChunk(subchunk);
		}
		else if (subchunk.GetName() == "Subset")
		{
			PrintSubsetChunk(subchunk);
		}
		else if (subchunk.GetName() == "BoundingSphere")
		{
			PrintBoundingSphereChunk(subchunk);
		}
		else if (subchunk.GetName() == "BoundingBox")
		{
			PrintBoundingBoxChunk(subchunk);
		}
		else if (subchunk.GetName() == "LODPhases")
		{
			PrintLODPhasesChunk(subchunk);
		}
		else if (subchunk.GetName() == "IndicesStripPS2")
		{
			PrintIndicesStripPS2Chunk(subchunk);
		}
		else if (subchunk.GetName() == "SubSetPS2")
		{
			PrintSubsetPS2Chunk(subchunk);
		}
		else
		{
			subchunk.Skip();
		}
	}

	if (gLODStatisticsMode & STAT_MODE_TEXEL_PERF)
		CalcTexelPerformanceStatistics();
	if (gLODStatisticsMode & STAT_MODE_SHIP_CONTOUR_DETECTION)
		CalcShipContour();
	gMeshLODInfoArray.back().ClearStreams();
}

void PrintNodeChunk(MChunk& chunk)
{
	IndentLevel il;
	output() << indent << "Typename \"" << chunk.ReadString() << "\"\n";
	output() << indent << "Name \"" << chunk.ReadString() << "\"\n";

	if (Version >= 2)
	{
		output() << indent << "Index " << chunk.ReadInt() << "\n";
	}

	while (chunk)
	{
		MChunk subchunk = chunk.GetChunk();
		PrintChunkName(subchunk);
		if (subchunk.GetName() == "Mesh")
		{
			PrintMeshChunk(subchunk);
		}
		else
		{
			subchunk.Skip();
		}
	}
}

void PrintSceneChunk(MChunk& top_chunk)
{
	IndentLevel il;
	while (top_chunk)
	{
		MChunk chunk = top_chunk.GetChunk();
		PrintChunkName(chunk);
		if (chunk.GetName() == "Node" && Version >= 3)
		{
			IndentLevel il;
			while (chunk) 
			{
				MChunk subchunk = chunk.GetChunk();
				PrintChunkName(subchunk);
				if (subchunk.GetName() == "Hierarchy")
				{
					IndentLevel il;
					output() << indent << "Index " << subchunk.ReadInt() << "\n";
					output() << indent << "Parent " << subchunk.ReadInt() << "\n";
					PrintMatrix(subchunk);
				}
				else if (subchunk.GetName() == "Notes")
				{
					PrintArray<string>(subchunk);
				}
				else if (subchunk.GetName() == "Children")
				{
					PrintArray<int>(subchunk);
					/*
					int nchildren = chunk.ReadInt();
					output() << indent << "Count " << nchildren << "\n";
					if (nchildren)
					{
					output() << indent << "Children";
					for (int i = 0; i < nchildren; ++i)
					{
					output() << chunk.ReadInt();
					}
					output() << '\n';
					}
					*/
				}
				else
				{
					subchunk.Skip();
				}
			}
		}
	}
}

void PrintAuxChunk(MChunk& chunk)
{
	IndentLevel il;
	output() << indent << "Name \"" << chunk.ReadString() << "\"\n";

	while (chunk)
	{
		MChunk subchunk = chunk.GetChunk();
		PrintChunkName(subchunk);
		subchunk.Skip();
	}
}

void PrintAux2Chunk(MChunk& chunk)
{
	bool is_wave_stern = false;

	IndentLevel il;
	while (chunk)
	{
		MChunk subchunk = chunk.GetChunk();
		PrintChunkName(subchunk);
		if (subchunk.GetName() == "Identifier")
		{
			IndentLevel il;

			string name;
			int index;
			subchunk >> name >> index;

			output() << indent << "Name \"" << name << "\"\n";
			output() << indent << "Index " << index << "\n";

			is_wave_stern = (name == "wave_stern");
		}
		else if (subchunk.GetName() == "Category")
		{
			IndentLevel il;

			string cat;
			subchunk >> cat;

			output() << indent << "Category \"" << cat << "\"\n";
		}
		else if (subchunk.GetName() == "Points")
		{
			IndentLevel il;

			float zSum = 0.f;
			int num = 0;

			while (subchunk)
			{
				PrintFloats(subchunk, 3);

				++num;
				zSum += gPrintedFloatArray[2]; //summing z coords
			}

			if (is_wave_stern)
			{
				gWaveSternAvgZ = zSum / num;
			}
		}
		else
		{
			subchunk.Skip();
		}
	}
}

void PrintZoneChunk(MChunk& chunk)
{
	IndentLevel il;
	while (chunk)
	{
		MChunk subchunk = chunk.GetChunk();
		PrintChunkName(subchunk);
		if (subchunk.GetName() == "Name")
		{
			IndentLevel il;

			string name;
			subchunk >> name;

			output() << indent << "Name \"" << name << "\"\n";
		}
		else if (subchunk.GetName() == "Index")
		{
			IndentLevel il;

			int index;
			subchunk >> index;

			output() << indent << "Index " << index << "\n";
		}
		else if (subchunk.GetName() == "Armor")
		{
			IndentLevel il;

			int armor;
			subchunk >> armor;

			output() << indent << "Armor " << armor << "\n";
		}
		else if (subchunk.GetName() == "Sphere")
		{
			IndentLevel il;

			float cx, cy, cz;
			float radius;
			subchunk >> cx >> cy >> cz >> radius;

			output() << indent << "Center " << cx << " " << cy << " " << cz << "\n";
			output() << indent << "Radius " << radius << "\n";
		}
		else
		{
			subchunk.Skip();
		}
	}
}

void PrintNoteChunk(MChunk& chunk)
{
	IndentLevel il;

	string note;
	chunk >> note;

	output() << indent << "Note \"" << note << "\"\n";

	gResourceTypeNames.back() = "Note(\"" + note + "\")";
}

typedef void (*GeomMeshDataFunc)(MChunk& chunk);

void EmptyGeomMeshDataChunk(MChunk& chunk)
{
}

void PrintGeomMeshDataChunk(MChunk& chunk)
{
	int id_index, armor;
	chunk >> id_index >> armor;
	output()	<< indent << "Index    " << id_index << endl
		<< indent << "Armor    " << armor << endl;
}

void PrintGeomChunk(MChunk& chunk, GeomMeshDataFunc datafunc)
{
	unsigned npoints = chunk.ReadUnsigned();
	output() << indent << "NumVertices " << npoints << endl;
	{
		IndentLevel il;
		for (unsigned i = 0; i < npoints; ++i)
		{
			PrintFloats(chunk, 3);
		}
	}

	unsigned nedges = chunk.ReadUnsigned();
	output() << indent << "NumEdges " << nedges << endl;
	{
		IndentLevel il;
		for (unsigned i = 0; i < nedges; ++i)
		{
			int a, b;
			chunk >> a >> b;
			output() << indent << a << " " << b << endl;
		}
	}

	unsigned ntris = chunk.ReadUnsigned();
	output() << indent << "NumTris " << ntris << endl;
	{
		IndentLevel il;
		for (unsigned i = 0; i < ntris; ++i)
		{
			IndentLevel il;

			datafunc(chunk);

			int A, B, C;
			chunk >> A >> B >> C;
			output() << indent << "Vertices " << A << " " << B << " " << C << endl;

			int a, b, c;
			chunk >> a >> b >> c;
			output() << indent << "Edges    " << a << " " << b << " " << c << endl;
		}
	}
}

void PrintGeomMeshChunk(MChunk& chunk)
{
	IndentLevel il;

	unsigned nids = chunk.ReadUnsigned();
	output() << indent << "NumIds " << nids << endl;
	{
		IndentLevel il;
		for (unsigned i = 0; i < nids; ++i)
		{
			string cat;
			int index;
			chunk >> cat >> index;

			output() << indent << (boost::format("%-20s %3d     #%d") % cat % index % i) << endl;
		}
	}

	PrintGeomChunk(chunk, PrintGeomMeshDataChunk);
}

void PrintNewGeomChunk(MChunk& chunk)
{
	IndentLevel il;

	while (chunk)
	{
		MChunk subchunk = chunk.GetChunk();
		PrintChunkName(subchunk);

		if (subchunk.GetName() == "Category")
		{
			IndentLevel il;

			string category;
			subchunk >> category;

			output() << indent << "Category \"" << category << "\"" << endl;
		}
		else if (subchunk.GetName() == "Index")
		{
			IndentLevel il;

			int index;
			subchunk >> index;

			output() << indent << "Index " << index << endl;
		}
		else if (subchunk.GetName() == "Armor")
		{
			IndentLevel il;

			int armor;
			subchunk >> armor;

			output() << indent << "Armor " << armor << endl;
		}
		else if (subchunk.GetName() == "Mesh")
		{
			IndentLevel il;

			PrintGeomChunk(subchunk, EmptyGeomMeshDataChunk);
		}
		else
		{
			subchunk.Skip();
		}
	}
}

void PrintAnimationChannelsChunk(MChunk& chunk)
{
	IndentLevel il;

	while (chunk)
	{
		MChunk subchunk = chunk.GetChunk();
		PrintChunkName(subchunk);

		if (subchunk.GetName() == "AnimationGroupName")
		{
			IndentLevel il;

			string animationgroupname;
			subchunk >> animationgroupname;
			output() << indent << "\"" << animationgroupname << "\"" << endl;
		}
		else if (subchunk.GetName() == "ChannelAnimation")
		{
			IndentLevel il;

			string chanName;
			subchunk >> chanName;
			output() << indent << "\"" << chanName << "\"" << endl;

			subchunk.Skip();
		}
		else
		{
			subchunk.Skip();
		}
	}
}

void PrintResourceChunk(MChunk& chunk)
{
	IndentLevel il;
	int index = 0;
	while (chunk)
	{
		MChunk subchunk = chunk.GetChunk();
		output() << indent << "Chunk \"" << subchunk.GetName() << "\" (index " << index++ << ")\n";

		gResourceTypeNames.push_back(subchunk.GetName());

		if (gLODStatisticsMode != STAT_MODE_OFF && subchunk.GetName() != "Mesh" && subchunk.GetName() != "SkinedMesh" && subchunk.GetName() != "Note" && subchunk.GetName() != "Aux")
		{
			// in LOD statistic mode we load only Mesh resource
			subchunk.Skip();
			continue;
		}

		if (subchunk.GetName() == "Node")
		{
			PrintNodeChunk(subchunk);
		}
		else if (subchunk.GetName() == "Mesh" ||  subchunk.GetName() == "SkinedMesh" || subchunk.GetName() == "MatrixIndexedMesh")
		{
			PrintMeshChunk(subchunk);
		}
		else if (subchunk.GetName() == "Zone")
		{
			PrintZoneChunk(subchunk);
		}
		else if (subchunk.GetName() == "Aux")
		{
			PrintAux2Chunk(subchunk);
		}
		else if (subchunk.GetName() == "Note")
		{
			PrintNoteChunk(subchunk);
		}
		else if (subchunk.GetName() == "GeomMesh")
		{
			PrintGeomMeshChunk(subchunk);
		}
		else if (subchunk.GetName() == "Geom")
		{
			PrintNewGeomChunk(subchunk);
		}
		else if (subchunk.GetName() == "AnimationChannels")
		{
			PrintAnimationChannelsChunk(subchunk);
		}
		else
		{
			subchunk.Skip();
		}
	}

	output() << indent << "AllVertexCount: "<<all_vertexcount<<"\n";
	output() << indent << "AllPrimitiveCount: "<<all_promitivecount<<"\n";
}

void PrintHierarchyItemChunk(MChunk& chunk)
{
	bool isRootItem = true;
	bool isOK = false;
	string name;
	vector<string> itemResTypeNames;

	IndentLevel il;
	while (chunk)
	{
		MChunk subchunk = chunk.GetChunk();
		PrintChunkName(subchunk);
		if (subchunk.GetName() == "Parent")
		{
			IndentLevel il;

			int index;
			subchunk >> index;

			output() << indent << "Index " << index << "\n";

			isRootItem = false;
		}
		else if (subchunk.GetName() == "Name")
		{
			IndentLevel il;

			subchunk >> name;

			output() << indent << "Name \"" << name << "\"\n";
		}
		else if (subchunk.GetName() == "Matrix")
		{
			IndentLevel il;

			PrintMatrix(subchunk);
		}
		//else if (subchunk.GetName() == "Name" || subchunk.GetName() == "Note")
		else if (subchunk.GetName() == "Name")
		{
			IndentLevel il;
			string str = subchunk.ReadString();
			output() << str << "\n";
		}
		else if (	subchunk.GetName() == "Resource" || 
			subchunk.GetName() == "RenderMesh" || 
			subchunk.GetName() == "GeomMesh" || 
			subchunk.GetName() == "ConvexObject" || 
			subchunk.GetName() == "Aux" ||
			subchunk.GetName() == "Note" )
		{
			IndentLevel il;

			int index;
			subchunk >> index;

			output() << indent << "Index " << index << "\n";

			const string& resTypeName = gResourceTypeNames[index];
			itemResTypeNames.push_back(resTypeName);

			if (resTypeName == "Mesh" || 
				resTypeName == "SkinedMesh" || 
				resTypeName == "Bone" ||
				resTypeName == "AnimationChannels")
			{
				isOK = true;
			}

			if (subchunk.GetName() == "Resource" && static_cast<size_t>(index) < gMeshLODInfoArray.size())
			{
				gMeshLODInfoArray[index].IncRefCount();
			}
		}
		else if (subchunk.GetName() == "BoundingSphere")
		{
			PrintBoundingSphereChunk(subchunk);
		}
		else
		{
			subchunk.Skip();
		}
	}

	if (gLODStatisticsMode & STAT_MODE_HIERARCHY_OPTIMIZATION_CHECK)
	{
		static int rootItemNum = 0;
		rootItemNum += isRootItem;

		if (rootItemNum == 2)
		{
			cout << "  #PERF: No rootitem in MMOD !!!" << endl;
		}

		if (!isRootItem && !isOK)
		{
			cout << "  #WARNPERF: Hierarchy is NOT optimal at Node " << name;
			cout << " Resources[";
			for(int i=0; i<itemResTypeNames.size(); ++i)
			{
				cout << itemResTypeNames[i];
				if (i+1 < itemResTypeNames.size())
					cout << ", ";
			}
			cout << "]" << endl;
		}
	}
}

void PrintHierarchyChunk(MChunk& chunk)
{
	gSpereRadius = 0.f;

	IndentLevel il;
	while (chunk)
	{
		MChunk subchunk = chunk.GetChunk();
		PrintChunkName(subchunk);
		if (subchunk.GetName() == "Item")
		{
			PrintHierarchyItemChunk(subchunk);
		}
		subchunk.Skip();
	}
}

void here()
{
	output() << "\nThis is terrible.\n";
	terminate();
}

float MakeLODInfo(float i_LODError, int& o_VertexNum, int& o_FaceNum)
{
	o_VertexNum = 0;
	o_FaceNum = 0;

	float maxMaxLODerror = -1.f;

	for (size_t i=0; i<gMeshLODInfoArray.size(); ++i)
	{
		bool found = false;
		size_t startSubset, endSubset;

		for (size_t j=0; j<gMeshLODInfoArray[i].GetLODNum(); ++j)
		{
			const LODInfo& rLOD = gMeshLODInfoArray[i].GetLOD(j);
			if (rLOD.minLODError <= i_LODError && i_LODError <= rLOD.maxLODError)
			{
				startSubset = rLOD.startSubset;
				endSubset = rLOD.endSubset;
				found = true;
				break;
			}
		}

		if (!found)
		{
			if (gMeshLODInfoArray[i].GetLODNum() == 0)
			{
				startSubset = 0;
				endSubset = gMeshLODInfoArray[i].GetSubSetNum()-1;
				found = true;
			}
		}

		if (found)
		{
			for (size_t s = startSubset; s <= endSubset; ++s)
			{
				const SubsetInfo& rSubset = gMeshLODInfoArray[i].GetSubSet(s);
				int refc = gMeshLODInfoArray[i].GetRefCount();
				o_VertexNum += refc * rSubset.vertexNum;
				o_FaceNum += refc * rSubset.indexNum;
			}
		}

		float iErr = 1.f;
		if (gMeshLODInfoArray[i].GetLODNum() > 0)
		{
			iErr = gMeshLODInfoArray[i].GetLOD(gMeshLODInfoArray[i].GetLODNum()-1).maxLODError;
		}
		maxMaxLODerror = max(maxMaxLODerror, iErr);
	}

	return maxMaxLODerror >= 0.f ? maxMaxLODerror : 1.f;
}

template <class T>
static inline void Limit(T& x, const T a, const T b)
{
	if (x > b)
		x = b;
	else if (x < a)
		x = a;
}

static float DistToLODErr(float dist, float mulRad, float centerZ)
{
	float lodErr = mulRad / (centerZ + dist);
	lodErr = 1.f - lodErr;
	Limit(lodErr, 0.f, 1.f);
	return lodErr;
}

static float LODErrToDist(float lodErr, float mulRad, float centerZ)
{
	float e = 1.f - lodErr;
	return (mulRad / e) - centerZ;
}

void PrintLODInfo(float i_Fov, const vector<float>& i_DitanceBuf)
{
	if (gSpereRadius == 0.f)
	{
		gSpereRadius = 1.f;
		gSphereCenterZ = 0.f;
	}

	float mulRadius;
	{
		const float pi = 3.14159265358979323846f;
		float fovRadian = (i_Fov / 180.f) * pi; // degree -> radian
		float h = 1.f / tanf(fovRadian / 2.f); //Cotan(fovradian/2)
		mulRadius = gSpereRadius * h;
	}

	float maxLODerr = 1.f;
	for (size_t e=0; e<i_DitanceBuf.size(); ++e)
	{
		float lodErr = DistToLODErr(i_DitanceBuf[e], mulRadius, gSphereCenterZ);

		int vertexNum, faceNum;
		maxLODerr = MakeLODInfo(lodErr, vertexNum, faceNum);
		cout << faceNum << "\t" << vertexNum << "\t\t";
	}

	if (maxLODerr != 1.f)
	{
		cout << LODErrToDist(maxLODerr, mulRadius, gSphereCenterZ) << " m\t";
	}
	else
	{
		int vertexNum, faceNum;
		MakeLODInfo(1.f, vertexNum, faceNum);
		cout << faceNum << "\t" << vertexNum << "\t\t";
	}

	cout << endl;
}

void WriteContours(string i_Fname)
{
	ofstream contourFile(i_Fname.c_str(), ios::out | ios::binary);

	contourFile.write(reinterpret_cast<char*>(&gWaveSternAvgZ), sizeof(float));
	contourFile.write(reinterpret_cast<char*>(&gDeepestY), sizeof(float));

	cout << gWaveSternAvgZ << ", " << gDeepestY << endl;

	for (map<float, float>::const_iterator it = gZXContourMap.begin(); it != gZXContourMap.end(); ++it)
	{
		float z = it->first * gStepZ;
		float x = it->second;

		contourFile.write(reinterpret_cast<char*>(&z), sizeof(float));
		contourFile.write(reinterpret_cast<char*>(&x), sizeof(float));

		cout << z << ", " << x << endl;
	}
	contourFile.close();

	cout << i_Fname << ": " << gZXContourMap.size() << " (z,x) coords writen." << endl;
}

render::object3d* load_mmod(const char* i_filename)
{

	ofstream outfile("faszomfasza.txt");
	output_stream = &outfile;

	MChunkStream stream(i_filename);

	set_unexpected(here);

	try
	{
		MChunk chunk = stream.GetTopChunk();
		PrintChunkName(chunk);

		IndentLevel il;
		Version = chunk.ReadUnsigned();
		output() << indent << "Version " << Version << "\n";

		while (chunk)
		{
			MChunk subchunk = chunk.GetChunk();
			PrintChunkName(subchunk);

			if (Version >= 5)
			{
				if (subchunk.GetName() == "BoundingSphere")
				{
					PrintBoundingSphereChunk(subchunk);
				}
				else if (subchunk.GetName() == "BoundingBox")
				{
					PrintBoundingBoxChunk(subchunk);
				}
				else if (subchunk.GetName() == "Resource")
				{
					PrintResourceChunk(subchunk);
				}
				else if (subchunk.GetName() == "Hierarchy")
				{
					PrintHierarchyChunk(subchunk);
				}
				else
				{
					subchunk.Skip();
				}
			}
			else
			{
				if (subchunk.GetName() == "Node")
				{
					PrintNodeChunk(subchunk);
				}
				else if (subchunk.GetName() == "Scene")
				{
					PrintSceneChunk(subchunk);
				}
				else if (subchunk.GetName() == "Aux")
				{
					PrintAuxChunk(subchunk);
				}
				else
				{
					subchunk.Skip();
				}
			}
		}


		if (gLODStatisticsMode & STAT_MODE_SHIP_CONTOUR_DETECTION)
		{
			size_t lastPer = max(in_filename.find_last_of('\\'), in_filename.find_last_of('/')) + 1;
			string fname = 
				"Y:\\" + in_filename.substr(lastPer, in_filename.find_last_of('.') - lastPer) + ".cont";

			WriteContours(fname);
		}
	}
	catch (chunk_exception& ce)
	{
		output() << "\n\nREAD ERROR: " << ce.what() << '\n';
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// ez nem ide valo, csak teszteles
//


class formatter
{
	string buffer;

public:
	formatter()
	{
	}

	formatter(const formatter& fmt)
	{
	}

	~formatter()
	{
	}

	formatter& operator<< (const string& str)
	{
		buffer += str;
		return *this;
	}

	formatter& operator<< (const char* str)
	{
		buffer += str;
		return *this;
	}

	const string& str() const { return buffer; }
};

formatter fmt()
{
	return formatter();
}

void test2(const formatter& fmt)
{
	cout << fmt.str();
}



template <unsigned N> 
class HighBit
{
public:
	enum { value = HighBit< (N>>1) >::value + 1 };
};

template <>
class HighBit<1>
{
public:
	enum { value = 0 };
};

template <bool is_pow_2, unsigned N>
class NearestPow2_Helper { };

template <unsigned N>
class NearestPow2_Helper<false, N>
{
public:
	enum { value = 1 << (HighBit<N>::value + 1) };
};

template <unsigned N>
class NearestPow2_Helper<true, N>
{
public:
	enum { value = N };
};

template <unsigned N>
class NearestPow2 
{
public:
	typedef typename NearestPow2_Helper< (N & (N-1)) == 0, N > Helper;
	enum { value = Helper::value };
};


/*
template <unsigned N> 
unsigned Factorial()
{
return Factorial< N-1 >() * N;
}

template <>
inline unsigned Factorial<1>()
{
return 1;
}

template <>
inline unsigned Factorial<0>()
{
return 0;
}
*/

template <unsigned Size>
class BlkSize
{
};

template <unsigned Size>
class MyAllocator
{
	enum { size = NearestPow2<Size>::value };
	typedef BlkSize< size > block_size;
};

void test()
{
	test2(formatter() << "Hello\n");
	test2(fmt() << "Hello\n");

	unsigned hb1 = NearestPow2<1>::value; 
	unsigned hb2 = NearestPow2<129>::value;
	unsigned hb3 = NearestPow2<65536>::value;

	MyAllocator<73> xal;

	cout << hb1 << " " << hb2 << " " << hb3 << endl;

	/*
	unsigned f1 = Factorial<1>(); 
	unsigned f2 = Factorial<5>();
	unsigned f3 = Factorial<9>();

	cout << f1 << " " << f2 << " " << f3 << endl;
	*/
}



