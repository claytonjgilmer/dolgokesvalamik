// mmod-dump.cpp : Defines the entry point for the console application.
//

#include "ChunkStream.h"
#include "Tga.h"

#include <windows.h>

#include "render/renderobject3d.h"


int all_promitivecount=0;
int all_vertexcount=0;


////////////////////////////////////////////////////////////////////////////////
typedef ctr::vector<unsigned short> IndexArray;
typedef ctr::vector<ctr::string> StringArray;

struct SubsetInfo
{
	int vertexStart;
	int vertexNum;
	int indexStart;
	int indexNum;
	StringArray textureNames;
	ctr::string shaderName;
	int vertexstreamidx;
};

struct LODInfo
{
	float minLODError;
	float maxLODError;
	int startSubset;
	int endSubset;
};

typedef ctr::vector<float> VertexArray;

class Stream_t
{
public:
	void AddFloat(const VertexArray& i_FloatArr)
	{
		for (unsigned int n=0; n<i_FloatArr.size(); ++n)
		{
			m_Vertices.push_back(i_FloatArr[n]);
		}
//		std::copy(i_FloatArr.begin(), i_FloatArr.end(), std::back_inserter(m_Vertices));
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

	void SetMVF(const ctr::string& str)
	{
		m_MVF = str;
	}

	const ctr::string& GetMVF() const
	{
		return m_MVF;
	}

protected:
	int			m_FloatPerVertices;
	ctr::string		m_MVF;
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

	ctr::vector<unsigned>& Get32bitIndexArray(){return m_32bitIndices;}

	bool& Is32bit(){return m_32bit;}

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
	const Stream_t& GetStream(size_t i) const
	{
		return m_Streams[i];
	}
	void AddStream()
	{
		m_Streams.push_back(Stream_t());
	}
	Stream_t& GetLastStream()
	{
		assert(m_Streams.size() > 0);
		return m_Streams[m_Streams.size()-1];
	}

	void IncRefCount() { ++m_RefCount; }
	int  GetRefCount() const { return m_RefCount; }
protected:
	typedef ctr::vector<SubsetInfo> SubsetArray;
	typedef ctr::vector<LODInfo> LODArray;
	typedef ctr::vector<Stream_t> StreamArray;

	SubsetArray	m_SubSet;
	LODArray	m_LOD;

	StreamArray	m_Streams;
	IndexArray	m_Indices;
	ctr::vector<unsigned> m_32bitIndices;
	bool m_32bit;

	int			m_RefCount;
};

inline float Finf()
{
	return FLT_MAX;// std::numeric_limits<float>::infinity();
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


ctr::vector<MeshLODInfo> gMeshLODInfoArray;

float g_sphere_radius;
math::vec3 g_sphere_center;

math::vec3 g_box_min,g_box_max;

VertexArray gPrintedFloatArray;

enum {
	STAT_MODE_OFF = 0,
	STAT_MODE_MODEL_PERF = 1<<0,
	STAT_MODE_TEXEL_PERF = 1<<1,
	STAT_MODE_HIERARCHY_OPTIMIZATION_CHECK = 1<<2,
};

int gLODStatisticsMode;
ctr::string gInputFileName;

ctr::vector<ctr::string> gResourceTypeNames;

const float	gStepZ = 0.1f; // in meter
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




bool GetFullPathName(const ctr::string& directory, const ctr::string& fName, ctr::string& fFullName)
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


int Version;

////////////////////////////////////////////////////////////////////////////////
void PrintFloats(MChunk& chunk, int count)
{
	gPrintedFloatArray.resize(0);

	for (int i = 0; i < count; ++i)
	{
		float fl = chunk.ReadFloat();
		gPrintedFloatArray.push_back(fl);
	}
}
/*

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
*/
/*
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
*/
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
	int vertexcount = chunk.ReadInt();
	all_vertexcount+=vertexcount;
	ctr::string strVF = chunk.ReadString();
	unsigned sizeleft = chunk.GetSizeLeft();

	unsigned vertexsize = sizeleft/vertexcount;
	assert(vertexsize * vertexcount == sizeleft);

	gMeshLODInfoArray.back().AddStream();	
	gMeshLODInfoArray.back().GetLastStream().SetFloatPerVertices(vertexsize / 4);
	gMeshLODInfoArray.back().GetLastStream().SetMVF(strVF);

	for (int i = 0; i < vertexcount; ++i)
	{
		PrintFloats(chunk, vertexsize / 4);
		gMeshLODInfoArray.back().GetLastStream().AddFloat(gPrintedFloatArray);
	}
}

//#define PRINT_INDICES

void PrintIndicesChunk(MChunk& chunk)
{
	bool printInd = (gLODStatisticsMode & STAT_MODE_MODEL_PERF) ? true : false;

	{
		int indexCnt = chunk.ReadInt();
		unsigned indexformat=chunk.ReadUnsigned();

		bool shortindex=(indexformat==101);
		gMeshLODInfoArray.back().Is32bit()=shortindex;

		if (shortindex)
		{
			gMeshLODInfoArray.back().GetIndexArray().resize(indexCnt);
			IndexArray::iterator iti = gMeshLODInfoArray.back().GetIndexArray().begin();
			for (int i=0; i<indexCnt; ++i)
			{
				unsigned short sh = (unsigned short)chunk.ReadShort();
				*iti++ = sh;
			}
		}
		else
		{
			gMeshLODInfoArray.back().Get32bitIndexArray().resize(indexCnt);
			ctr::vector<unsigned>::iterator iti = gMeshLODInfoArray.back().Get32bitIndexArray().begin();
			for (int i=0; i<indexCnt; ++i)
			{
				unsigned sh = (unsigned)chunk.ReadUnsigned();
				*iti++ = sh;
			}
		}
	}
}

void PrintTextureChunk(MChunk& chunk, SubsetInfo& subset)
{
	ctr::string str = chunk.ReadString();
	subset.textureNames.push_back(str);
	chunk.Skip();
	return;
}

void PrintBoundingBoxChunk(MChunk& chunk)
{
	chunk >> g_box_min.x >> g_box_min.y >> g_box_min.z;
	chunk >> g_box_max.x >> g_box_max.y >> g_box_max.z;
}

void PrintBoundingSphereChunk(MChunk& chunk)
{
	chunk >> g_sphere_center.x >> g_sphere_center.y >> g_sphere_center.z >> g_sphere_radius;
}

void PrintSubsetChunk(MChunk& chunk)
{
	int primitive_type=chunk.ReadInt();

	SubsetInfo subset;
	subset.vertexStart = chunk.ReadInt();
	subset.vertexNum = chunk.ReadInt();
	subset.indexStart = chunk.ReadInt();
	subset.indexNum = chunk.ReadInt();
	subset.shaderName = chunk.ReadString();
	subset.vertexstreamidx = 0;

	all_promitivecount +=subset.indexNum;


	while (chunk)
	{
		MChunk subchunk = chunk.GetChunk();
		if (subchunk.GetName() == "VertexStreamIndex")
		{
			subset.vertexstreamidx = subchunk.ReadInt();
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
	int LODPhaseCount= subchunk.ReadInt();

	for (int i=0; i<LODPhaseCount; ++i)
	{
		{
			LODInfo lod;
			lod.minLODError = subchunk.ReadFloat();
			lod.maxLODError = subchunk.ReadFloat();
			lod.startSubset = subchunk.ReadInt();
			lod.endSubset   = subchunk.ReadInt();
			gMeshLODInfoArray.back().AddLODInfo(lod);
		}
	}

}





/*
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
*/
void PrintMeshChunk(MChunk& chunk)
{
	int index=chunk.ReadInt();

	gMeshLODInfoArray.push_back(MeshLODInfo());
	while (chunk)
	{
		MChunk subchunk = chunk.GetChunk();
		if (subchunk.GetName() == "VertexStream")
		{
			PrintVertexStreamChunk(subchunk);
		}
/*
		else if (subchunk.GetName() == "CompressedVertexFormatData")
		{
			PrintCompressedVertexFormatDataChunk(subchunk);
		}
*/
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
		else
		{
			subchunk.Skip();
		}
	}

	gMeshLODInfoArray.back().ClearStreams();
}

void PrintNodeChunk(MChunk& chunk)
{
	ctr::string nodetypename=chunk.ReadString();
	ctr::string nodename=chunk.ReadString();
	int nodeindex=chunk.ReadInt();

	while (chunk)
	{
		MChunk subchunk = chunk.GetChunk();
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
	while (top_chunk)
	{
		MChunk chunk = top_chunk.GetChunk();
		if (chunk.GetName() == "Node" && Version >= 3)
		{
			while (chunk) 
			{
				MChunk subchunk = chunk.GetChunk();
				if (subchunk.GetName() == "Hierarchy")
				{
					int nodeindex=subchunk.ReadInt();
					int parentindex=subchunk.ReadInt();
					PrintMatrix(subchunk);
				}
/*
				else if (subchunk.GetName() == "Notes")
				{
					PrintArray<string>(subchunk);
				}
				else if (subchunk.GetName() == "Children")
				{
					PrintArray<int>(subchunk);
				}
*/
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
	ctr::string name=chunk.ReadString();

	while (chunk)
	{
		MChunk subchunk = chunk.GetChunk();
//		PrintChunkName(subchunk);
		subchunk.Skip();
	}
}

void PrintAux2Chunk(MChunk& chunk)
{
	bool is_wave_stern = false;

	while (chunk)
	{
		MChunk subchunk = chunk.GetChunk();
		if (subchunk.GetName() == "Identifier")
		{
			ctr::string name;
			int index;
			subchunk >> name >> index;

			is_wave_stern = (name == "wave_stern");
		}
		else if (subchunk.GetName() == "Category")
		{
			ctr::string cat;
			subchunk >> cat;
		}
		else if (subchunk.GetName() == "Points")
		{
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


void PrintNoteChunk(MChunk& chunk)
{
	ctr::string note;
	chunk >> note;

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
}

void PrintGeomChunk(MChunk& chunk, GeomMeshDataFunc datafunc)
{
	unsigned npoints = chunk.ReadUnsigned();
	{
		for (unsigned i = 0; i < npoints; ++i)
		{
			PrintFloats(chunk, 3);
		}
	}

	unsigned nedges = chunk.ReadUnsigned();
	{
		for (unsigned i = 0; i < nedges; ++i)
		{
			int a, b;
			chunk >> a >> b;
		}
	}

	unsigned ntris = chunk.ReadUnsigned();
	{
		for (unsigned i = 0; i < ntris; ++i)
		{

			datafunc(chunk);

			int A, B, C;
			chunk >> A >> B >> C;

			int a, b, c;
			chunk >> a >> b >> c;
		}
	}
}

void PrintGeomMeshChunk(MChunk& chunk)
{
	unsigned nids = chunk.ReadUnsigned();
	{
		for (unsigned i = 0; i < nids; ++i)
		{
			ctr::string cat;
			int index;
			chunk >> cat >> index;
		}
	}

	PrintGeomChunk(chunk, PrintGeomMeshDataChunk);
}

void PrintNewGeomChunk(MChunk& chunk)
{
	while (chunk)
	{
		MChunk subchunk = chunk.GetChunk();

		if (subchunk.GetName() == "Category")
		{
			ctr::string category;
			subchunk >> category;
		}
		else if (subchunk.GetName() == "Index")
		{
			int index;
			subchunk >> index;
		}
		else if (subchunk.GetName() == "Armor")
		{
			int armor;
			subchunk >> armor;
		}
		else if (subchunk.GetName() == "Mesh")
		{
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
	while (chunk)
	{
		MChunk subchunk = chunk.GetChunk();

		if (subchunk.GetName() == "AnimationGroupName")
		{
			ctr::string animationgroupname;
			subchunk >> animationgroupname;
		}
		else if (subchunk.GetName() == "ChannelAnimation")
		{
			ctr::string chanName;
			subchunk >> chanName;

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
	int index = 0;
	while (chunk)
	{
		MChunk subchunk = chunk.GetChunk();

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
}

void PrintHierarchyItemChunk(MChunk& chunk)
{
	bool isRootItem = true;
	bool isOK = false;
	ctr::string name;
	ctr::vector<ctr::string> itemResTypeNames;

	while (chunk)
	{
		MChunk subchunk = chunk.GetChunk();
		if (subchunk.GetName() == "Parent")
		{
			int index;
			subchunk >> index;

			isRootItem = false;
		}
		else if (subchunk.GetName() == "Name")
		{
			subchunk >> name;
		}
		else if (subchunk.GetName() == "Matrix")
		{
			PrintMatrix(subchunk);
		}
		else if (subchunk.GetName() == "Name")
		{
			ctr::string str = subchunk.ReadString();
		}
		else if (	subchunk.GetName() == "Resource" || 
			subchunk.GetName() == "mesh" || 
			subchunk.GetName() == "GeomMesh" || 
			subchunk.GetName() == "ConvexObject" || 
			subchunk.GetName() == "Aux" ||
			subchunk.GetName() == "Note" )
		{
			int index;
			subchunk >> index;

			const ctr::string& resTypeName = gResourceTypeNames[index];
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
}

void PrintHierarchyChunk(MChunk& chunk)
{
	g_sphere_radius= 0.f;

	while (chunk)
	{
		MChunk subchunk = chunk.GetChunk();
		if (subchunk.GetName() == "Item")
		{
			PrintHierarchyItemChunk(subchunk);
		}
		subchunk.Skip();
	}
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

void PrintLODInfo(float i_Fov, const ctr::vector<float>& i_DitanceBuf)
{
	if (g_sphere_radius== 0.f)
	{
		g_sphere_radius= 1.f;
		g_sphere_center.set(0,0,0);
	}

	float mulRadius;
	{
		const float pi = 3.14159265358979323846f;
		float fovRadian = (i_Fov / 180.f) * pi; // degree -> radian
		float h = 1.f / tanf(fovRadian / 2.f); //Cotan(fovradian/2)
		mulRadius = g_sphere_radius * h;
	}

	float maxLODerr = 1.f;
	for (size_t e=0; e<i_DitanceBuf.size(); ++e)
	{
		float lodErr = DistToLODErr(i_DitanceBuf[e], mulRadius, g_sphere_center.z);

		int vertexNum, faceNum;
		maxLODerr = MakeLODInfo(lodErr, vertexNum, faceNum);
	}

	if (maxLODerr != 1.f)
	{
	}
	else
	{
		int vertexNum, faceNum;
		MakeLODInfo(1.f, vertexNum, faceNum);
	}
}


render::object3d* load_mmod(const char* i_filename)
{

	MChunkStream stream(i_filename);

	MChunk chunk = stream.GetTopChunk();
	Version = chunk.ReadUnsigned();

	while (chunk)
	{
		MChunk subchunk = chunk.GetChunk();

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

	return 0;
}









