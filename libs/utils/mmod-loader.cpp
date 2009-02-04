// mmod-dump.cpp : Defines the entry point for the console application.
//

#include "ChunkStream.h"
#include "Tga.h"

#include <windows.h>

#include "render/renderobject3d.h"
#include "render/mesh.h"
#include "render/shadermanager.h"
#include "render/texturemanager.h"
#include "math/mtx4x4.h"


int all_promitivecount=0;
int all_vertexcount=0;


////////////////////////////////////////////////////////////////////////////////
typedef vector<unsigned short> IndexArray;
typedef vector<string> StringArray;
typedef vector<float> FloatArray;


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

struct Stream_t
{
	void AddFloat(const FloatArray& i_FloatArr)
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

	void SetMVF(const string& str)
	{
		m_MVF = str;
		m_MVF.to_lower();


		if (m_MVF == "simple.mvfm")
			m_MVF = "pf43nf43uf42.mvfm";
		else if (m_MVF == "simpleindexed.mvfm")
			m_MVF = "pf43nf43uf42if41.mvfm";
		else if (m_MVF == "color.mvfm")
			m_MVF = "pf43cc.mvfm";
		else if (m_MVF == "position.mvfm")
			m_MVF = "pf43.mvfm";
		else if (m_MVF == "oceanheightmapgen.mvfm")
			m_MVF = "pf44uf42.mvfm";
		else if (m_MVF == "oceannormalmapgen.mvfm")
			m_MVF = "pf44uf42uf42uf42uf42uf42.mvfm";
		else if (m_MVF == "oceanheightmap.mvfm")
			//m_MVF = "nf24.mvfm";
			m_MVF = "nc.mvfm";
		else if (m_MVF == "simplecolor.mvfm")
			m_MVF = "pf43ccuf42.mvfm";
		else if (m_MVF == "simplecolor2.mvfm")
			m_MVF = "pf44ccccuf42uf42.mvfm";
		else if (m_MVF == "ship.mvfm")
			m_MVF = "pf43nf43uf42uf42tf43bf43.mvfm";
		else if (m_MVF == "shipindexed.mvfm")
			m_MVF = "pf43nf43uf42uf42tf43bf43if41.mvfm";
		else if (m_MVF == "shipvc.mvfm")
			m_MVF = "pf43nf43uf42uf42tf43bf43cc.mvfm";
		else if (m_MVF == "shipvcindexed.mvfm")
			m_MVF = "pf43nf43uf42uf42tf43bf43ccif41.mvfm";
		else if (m_MVF == "gun.mvfm")
			m_MVF = "pf43nf43uf42.mvfm";
		else if (m_MVF == "gunvc.mvfm")
			m_MVF = "pf43nf43uf42cc.mvfm";
		else if (m_MVF == "gunvcindexed.mvfm")
			m_MVF = "pf43nf43uf42ccif41.mvfm";
		else if (m_MVF == "ocean.mvfm")
			m_MVF = "pf43nf43uf43.mvfm";
		else if (m_MVF == "projocean.mvfm")
			m_MVF = "pf42.mvfm";
		else if (m_MVF == "projoceanfull.mvfm")
			m_MVF = "pf42nc.mvfm";
		else if (m_MVF == "terraindx9ati.mvfm")
			m_MVF = "pcpc.mvfm";
		else if (m_MVF == "shadowmap.mvfm")
			m_MVF = "pf43nf43uf42.mvfm";
		else if (m_MVF == "airplane.mvfm")
			m_MVF = "pf43nf43uf42tf43bf43.mvfm";
		else if (m_MVF == "airplaneindexed.mvfm")
			m_MVF = "pf43nf43uf42tf43bf43if41.mvfm";
		else if (m_MVF == "transformpps.mvfm")
			m_MVF = "pf43.mvfm";
		else if (m_MVF == "particlesprite.mvfm")
			m_MVF = "pf43ccuf41uf41uf42uf42.mvfm";
		else if (m_MVF == "particleaxial.mvfm")
			m_MVF = "pf43ccuf43uf42uf42.mvfm";
		else if (m_MVF == "particlefloating.mvfm")
			m_MVF = "pf44ccuf24uf42.mvfm";
		else if (m_MVF == "particleaxialsprite.mvfm")
			m_MVF = "pf44ccuf44uf24uf42uf22.mvfm";
		else if (m_MVF == "coast.mvfm")
			m_MVF = "pf43.mvfm";
		else if (m_MVF == "beam.mvfm")
			m_MVF = "pf43cc.mvfm";
		else if (m_MVF == "waterparticle.mvfm")
			m_MVF = "cc.mvfm";
		else if (m_MVF == "watertracer.mvfm")
			m_MVF = "pf43ccuf42.mvfm";
		else if (m_MVF == "choppy.mvfm")
			m_MVF = "pf43nf43cc.mvfm";
		else if (m_MVF == "cloud.mvfm")
			m_MVF = "pf43ccuf42uf42.mvfm";
		else if (m_MVF == "impostor.mvfm")
			m_MVF = "pf43uf42.mvfm";
		else if (m_MVF == "skined.mvfm")
			m_MVF = "pf43nf43uf42wf44if44.mvfm";
		else if (m_MVF == "skinedUW.mvfm")
			m_MVF = "pf43nf43tf43bf43uf42wf44if44.mvfm";
		else if (m_MVF == "FoliageNoInst.mvfm")
			m_MVF = "pf43nf43uf42uf41cf43.mvfm";
		else if (m_MVF == "rope.mvfm")
			m_MVF = "pf43nf43uf42uf41.mvfm";
		else if (m_MVF == "FullscreenQuad.mvfm")
			m_MVF = "pf44uf42uf42uf42uf42.mvfm";
		else if (m_MVF == "lightning.mvfm")
			m_MVF = "pf43uf42uf41.mvfm";
		else if (m_MVF == "foliagesprite.mvfm")
			m_MVF = "pf43uf42ccuf43.mvfm";
		else if (m_MVF == "traceline.mvfm")
			m_MVF = "pf43uf43ccuf42uf41.mvfm";
		else
		if (m_MVF == "airfield.mvfm")
			m_MVF = "pf43nf43uf42uf42.mvfm";
		else
		if (m_MVF == "watertracerskined.mvfm")
			m_MVF = "pf44if43wf43uf44uf42.mvfm";
		else
		if (m_MVF == "watertracerskinedstatic.mvfm")
			m_MVF = "pf44uf42uf42.mvfm";
		else if (m_MVF == "waterspray.mvfm")
			m_MVF = "pf43uf42uf44uf43.mvfm";
		else if (m_MVF == "bterrain2.mvfm")
			m_MVF = "pf43nf43uf42uf42.mvfm";
		else
		if (m_MVF == "bterrain3.mvfm")
			m_MVF = "pf43nf43uf42uf42uf42.mvfm";
		if (m_MVF == "bterrain4.mvfm")
			m_MVF = "pf43nf43uf42uf42uf42uf42.mvfm";
		if (m_MVF == "uterrain3.mvfm")
			m_MVF = "pf43nf43uf42uf42uf42cc.mvfm";
		if (m_MVF == "uterrain4.mvfm")
			m_MVF = "pf43nf43uf42uf42uf42uf42cc.mvfm";
		else if (m_MVF == "shore.mvfm")
			m_MVF = "pf43uf42cc.mvfm";
		else
		if (m_MVF == "river.mvfm")
			m_MVF = "pf43nf43tf43bf43uf42cc.mvfm";
		else if (m_MVF == "foliagevc.mvfm")
			m_MVF = "pf43nf43uf42cc.mvfm";
		else if (m_MVF == "decal.mvfm")
			m_MVF = "pf43nf43uf44uf42.mvfm";
		else if (m_MVF=="ship.mvfm")
			m_MVF="pf43nf43uf42uf42.mvfm";
		else if (m_MVF=="shipvc.mvfm")
			m_MVF="pf43nf43uf42uf42cc.mvfm";
		else if (m_MVF=="rope.mvfm")
			m_MVF="pf43nf43uf42uf41.mvfm";
	}

	const string& GetMVF() const
	{
		return m_MVF;
	}

	unsigned GetVertexBufferSize() const
	{
		return m_Vertices.size()*sizeof(float);
	}

	unsigned GetVertexCount() const
	{
		return m_VertexCount;
	}

	void SetVertexCount(unsigned i_vc)
	{
		m_VertexCount=i_vc;
	}

	float* GetVertices()
	{
		return m_Vertices;
	}

	int			m_FloatPerVertices;
	string		m_MVF;
	FloatArray	m_Vertices;
	unsigned m_VertexCount;
};

struct HItem
{
	HItem()
	{
		parent=-1;
		mesh=-1;
	}
	string name;
	int parent;
	int mesh;
	mtx4x4 mtx;

	object3d* generate_object()
	{
		object3d* res=new object3d(name.c_str());
		res->set_localposition(mtx);

		return res;
	}
};

struct MeshLODInfo
{
	MeshLODInfo() : m_RefCount(0)
	{
		m_sphere_center.clear();
		m_sphere_radius=0;
		m_box_min.clear();
		m_box_max.clear();

		ismesh=false;
	}

	int ismesh;

	void PrintVertexStreamChunk(MChunk& chunk);
	void PrintIndicesChunk(MChunk& chunk);
	void PrintBoundingSphereChunk(MChunk& chunk);
	void PrintBoundingBoxChunk(MChunk& chunk);
	void PrintSubsetChunk(MChunk& chunk);
	void PrintLODPhasesChunk(MChunk &subchunk);

	fixedvector<mesh_t*,8> generate_mesh();
	vector<vertexelem> createdecl(string i_filename);

/*
	void ClearStreams()
	{
		m_Streams.clear();
		m_Indices.clear();
	}
*/
	IndexArray& GetIndexArray()
	{
		return	m_Indices;
	}

	vector<unsigned>& Get32bitIndexArray(){return m_32bitIndices;}

	int& Is32bit(){return m_32bit;}

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


	typedef vector<SubsetInfo> SubsetArray;
	typedef vector<LODInfo> LODArray;
	typedef vector<Stream_t> StreamArray;

	SubsetArray	m_SubSet;
	LODArray	m_LOD;

	StreamArray	m_Streams;
	IndexArray	m_Indices;
	vector<unsigned> m_32bitIndices;
	int m_32bit;

	int			m_RefCount;

	vec3 m_sphere_center;
	float m_sphere_radius;
	vec3 m_box_min;
	vec3 m_box_max;
};



vector<MeshLODInfo> gMeshLODInfoArray;
vector<HItem> gHItemArray;

	struct sMappingV
	{
		const char*	Name;
		vertexelemtype	Value;
	};

	struct sMapping
	{
		const char* Name;
		unsigned 	Value;
	};

vector<vertexelem> MeshLODInfo::createdecl(string i_filename)
{
	vector<vertexelem> ret;

	static sMappingV usages[] =
	{
		"p", vertexelement_position,
		"w", vertexelement_unused,
		"i", vertexelement_unused,
		"n", vertexelement_normal,
		"u", vertexelement_uv,
		"t", vertexelement_tangent,
		"b", vertexelement_binormal,
		"c", vertexelement_color,
	};
	static int usageCount = sizeof(usages) / sizeof(usages[0]);
	static sMapping types[] =
	{
		"f41", 1,
		"f42", 2,
		"f43", 3,
		"f44", 4,
		"c",   1,
		"ub4", 1,
		"ss2", 1,
		"ss4", 2,
		"ubn4",1,
		"ssn2",1,
		"ssn4",2,
		"usn2",1,
		"usn4",2,
		"ud3", 1,
		"sdn3",1,
		"f22", 1,
		"f24", 2,
	};
	static int typeCount = sizeof(types) / sizeof(types[0]);

	unsigned i = 0;
	int error = false;

	while (i < i_filename.size() && i_filename[i] != '.')
	{
		vertexelemtype usage;
		int found = false;
		for (int j = 0 ; j < usageCount ; ++j)
		{
			if (i_filename.match(i, usages[j].Name))
//				if (i_filename.substring(i, usages[j].Name.size()) == usages[j].Name)
			{
				usage = usages[j].Value;
				i += strlen(usages[j].Name);
				found = true;
				break;
			}
		}
		if (!found)
		{
			assertion(0,"hibas vertexformatnev");
			error = true;
			break;
		}

		int type;
		found = false;
		for (int j = 0 ; j < typeCount ; ++j)
		{
			if (i_filename.match(i,types[j].Name))
//				if (i_filename.substring(i, types[j].Name.size()) == types[j].Name)
			{
				type = types[j].Value;
				i += strlen(types[j].Name);
				found = true;
				break;
			}
		}
		if (!found)
		{
			assertion(0,"hibas vertexformatnev");
			error = true;
			break;
		}

		ret.push_back(vertexelem(usage,type));
//		Format->AddElement((eDeclType)type, (eDeclUsage)usage);
	}


	return ret;
}

fixedvector<mesh_t*,8> MeshLODInfo::generate_mesh()
{
	assertion(m_Streams.size()>0,"0 a vertexbufferek szama!");
	fixedvector<mesh_t*,8> res;
	for (unsigned streamindex=0; streamindex<m_Streams.size(); ++streamindex)
	{
		mesh_t* m=new mesh_t("mesh");
		res.push_back(m);

		if (m_32bit)
		{
			m->m_ib=new indexbuffer(m_32bitIndices.size(),true);
			int* ib=(int*)m->m_ib->lock();

			memcpy(ib,m_32bitIndices,m_32bitIndices.size()*sizeof(int));
		}
		else
		{
			m->m_ib=new indexbuffer(m_Indices.size(),false);
			unsigned short* ib=(unsigned short*)m->m_ib->lock();
			memcpy(ib,m_Indices,m_Indices.size()*sizeof(short));
			m->m_ib->unlock();
		}



		{
			vector<vertexelem> vdecl=createdecl(m_Streams[streamindex].GetMVF());
			m->m_vb=new vertexbuffer(m_Streams[streamindex].GetVertexCount(),vdecl,m_Streams[streamindex].GetFloatPerVertices()*sizeof(float));
			void* vb=m->m_vb->lock();
			memcpy(vb,m_Streams[streamindex].GetVertices(),m_Streams[streamindex].GetVertexCount()*m_Streams[streamindex].GetFloatPerVertices()*sizeof(float));
			m->m_vb->unlock();
		}

		int startsubset,endsubset;
		if (m_LOD.size())
		{
			startsubset=m_LOD[0].startSubset;
			endsubset=m_LOD[0].endSubset;
		}
		else
		{
			startsubset=0;
			endsubset=m_SubSet.size()-1;
		}

		for (int n=startsubset; n<=endsubset; ++n)
		{
			if (m_SubSet[n].vertexstreamidx!=(int)streamindex)
				continue;

			m->m_submeshbuf.push_back(submesh());
			submesh& sm=m->m_submeshbuf.back();
			sm.set_shader(shadermanager::ptr->get_shader("posnormuv.fx"));
			sm.m_firstindex=m_SubSet[n].indexStart;
			sm.m_numindices=m_SubSet[n].indexNum*3;
			sm.m_firstvertex=m_SubSet[n].vertexStart;
			sm.m_numvertices=m_SubSet[n].vertexNum;
			for (unsigned m=0; m<m_SubSet[n].textureNames.size(); ++m)
			{
				sm.m_texturebuf.push_back(texturemanager::ptr->get_texture(m_SubSet[n].textureNames[m].c_str()));
			}
		}
	}

	return res;
}

enum {
	STAT_MODE_OFF = 0,
	STAT_MODE_MODEL_PERF = 1<<0,
	STAT_MODE_TEXEL_PERF = 1<<1,
	STAT_MODE_HIERARCHY_OPTIMIZATION_CHECK = 1<<2,
};

//int gLODStatisticsMode;

vector<string> gResourceTypeNames;

const float	gStepZ = 0.1f; // in meter
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

inline int InInterval(float x, float a, float b)
{
	return a<=x && x<=b;
}

inline int InIntervalSafe(float x, float a, float b)
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




int GetFullPathName(const string& directory, const string& fName, string& fFullName)
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
void PrintFloats(FloatArray& o_floats, MChunk& chunk, int count)
{
	o_floats.resize(0);

	for (int i = 0; i < count; ++i)
	{
		float fl = chunk.ReadFloat();
		o_floats.push_back(fl);
	}
}

/*
void PrintMatrix(MChunk& chunk)
{
	for (int row = 0; row < 4; ++row)
	{
		PrintFloats(chunk, 4);
	}
}
*/

////////////////////////////////////////////////////////////////////////////////

void MeshLODInfo::PrintVertexStreamChunk(MChunk& chunk)
{
	int vertexcount = chunk.ReadInt();
	all_vertexcount+=vertexcount;
	string strVF = chunk.ReadString();
	unsigned sizeleft = chunk.GetSizeLeft();

	unsigned vertexsize = sizeleft/vertexcount;
	assert(vertexsize * vertexcount == sizeleft);

	AddStream();
	GetLastStream().SetFloatPerVertices(vertexsize / 4);
	GetLastStream().SetMVF(strVF);
	GetLastStream().SetVertexCount((unsigned)vertexcount);

	for (int i = 0; i < vertexcount; ++i)
	{
		FloatArray arr;
		PrintFloats(arr,chunk, vertexsize / 4);
		GetLastStream().AddFloat(arr);
	}
}

//#define PRINT_INDICES

void MeshLODInfo::PrintIndicesChunk(MChunk& chunk)
{
	{
		int indexCnt = chunk.ReadInt();
		unsigned indexformat=chunk.ReadUnsigned();

		m_32bit=(indexformat!=101);

		if (!m_32bit)
		{
			m_Indices.resize(indexCnt);
			IndexArray::iterator iti = m_Indices.begin();
			for (int i=0; i<indexCnt; ++i)
			{
				unsigned short sh = (unsigned short)chunk.ReadShort();
				*iti++ = sh;
			}
		}
		else
		{
			m_32bitIndices.resize(indexCnt);
			vector<unsigned>::iterator iti = m_32bitIndices.begin();
			for (int i=0; i<indexCnt; ++i)
			{
				unsigned sh = (unsigned)chunk.ReadUnsigned();
				*iti++ = sh;
			}
		}
	}
}


void MeshLODInfo::PrintBoundingBoxChunk(MChunk& chunk)
{
	chunk >> m_box_min.x >> m_box_min.y >> m_box_min.z;
	chunk >> m_box_max.x >> m_box_max.y >> m_box_max.z;
}

void MeshLODInfo::PrintBoundingSphereChunk(MChunk& chunk)
{
	chunk >> m_sphere_center.x >> m_sphere_center.y >> m_sphere_center.z >> m_sphere_radius;
}

void MeshLODInfo::PrintSubsetChunk(MChunk& chunk)
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
			string str = subchunk.ReadString();
			subset.textureNames.push_back(str);
			subchunk.Skip();
		}
/*
		else if (subchunk.GetName() == "BoundingSphere")
		{
			PrintBoundingSphereChunk(subchunk);
		}
*/
		else
		{
			subchunk.Skip();
		}
	}

	AddSubSetInfo(subset);
}

void MeshLODInfo::PrintLODPhasesChunk(MChunk &subchunk)
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
			AddLODInfo(lod);
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

//	gMeshLODInfoArray.push_back(MeshLODInfo());
	MeshLODInfo& mesh=gMeshLODInfoArray.back();
	mesh.ismesh=true;

	while (chunk)
	{
		MChunk subchunk = chunk.GetChunk();
		if (subchunk.GetName() == "VertexStream")
		{
			mesh.PrintVertexStreamChunk(subchunk);
		}
/*
		else if (subchunk.GetName() == "CompressedVertexFormatData")
		{
			PrintCompressedVertexFormatDataChunk(subchunk);
		}
*/
		else if (subchunk.GetName() == "Indices")
		{
			mesh.PrintIndicesChunk(subchunk);
		}
		else if (subchunk.GetName() == "Subset")
		{
			mesh.PrintSubsetChunk(subchunk);
		}
		else if (subchunk.GetName() == "BoundingSphere")
		{
			mesh.PrintBoundingSphereChunk(subchunk);
		}
		else if (subchunk.GetName() == "BoundingBox")
		{
			mesh.PrintBoundingBoxChunk(subchunk);
		}
		else if (subchunk.GetName() == "LODPhases")
		{
			mesh.PrintLODPhasesChunk(subchunk);
		}
		else
		{
			subchunk.Skip();
		}
	}

//	gMeshLODInfoArray.back().ClearStreams();
}

void PrintNodeChunk(MChunk& chunk)
{
	string nodetypename=chunk.ReadString();
	string nodename=chunk.ReadString();
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

//					PrintMatrix(subchunk);
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


void PrintAnimationChannelsChunk(MChunk& chunk)
{
	while (chunk)
	{
		MChunk subchunk = chunk.GetChunk();

		if (subchunk.GetName() == "AnimationGroupName")
		{
			string animationgroupname;
			subchunk >> animationgroupname;
		}
		else if (subchunk.GetName() == "ChannelAnimation")
		{
			string chanName;
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
		const string& subchunkname=subchunk.GetName();

		gResourceTypeNames.push_back(subchunk.GetName());

		gMeshLODInfoArray.push_back(MeshLODInfo());

		if (subchunk.GetName() != "Mesh" && subchunk.GetName() != "SkinedMesh" && subchunk.GetName() != "Note" && subchunk.GetName() != "Aux")
		{
			// in LOD statistic mode we load only Mesh resource
			subchunk.Skip();
			continue;
		}

		if (subchunkname == "Node")
		{
			PrintNodeChunk(subchunk);
		}
		else if (subchunkname == "Mesh" ||  subchunkname == "SkinedMesh" || subchunkname == "MatrixIndexedMesh")
		{
			PrintMeshChunk(subchunk);
		}
		else if (subchunkname == "AnimationChannels")
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
	int isRootItem = true;
	int isOK = false;
	string name;
	vector<string> itemResTypeNames;


	gHItemArray.push_back(HItem());
	HItem& item=gHItemArray.back();

	while (chunk)
	{
		MChunk subchunk = chunk.GetChunk();
		const string& subchunkname=subchunk.GetName();

		if (subchunkname == "Parent")
		{
			subchunk >> item.parent;

			isRootItem = false;
		}
		else if (subchunkname == "Name")
		{
			subchunk >> item.name;
		}
		else if (subchunkname == "Matrix")
		{
			float* ptr=(float*)&item.mtx;

			for (int n=0; n<16; ++n)
			{
				subchunk >> ptr[n];
			}
		}
		else if (subchunkname == "Resource")
		{
			int index;
			subchunk >> index;

			if ((int)gMeshLODInfoArray.size()>index && gMeshLODInfoArray[index].ismesh)
				item.mesh=index;
		}
		else if (	subchunkname == "Resource" ||
			subchunkname == "mesh" ||
			subchunkname == "GeomMesh" ||
			subchunkname  == "ConvexObject" ||
			subchunkname == "Aux" ||
			subchunkname == "Note" )
		{
			int index;
			subchunk >> index;

			const string& resTypeName = gResourceTypeNames[index];
			itemResTypeNames.push_back(resTypeName);

			if (resTypeName == "Mesh" ||
				resTypeName == "SkinedMesh" ||
				resTypeName == "Bone" ||
				resTypeName == "AnimationChannels")
			{
				isOK = true;
			}

			if (subchunkname == "Resource" && static_cast<size_t>(index) < gMeshLODInfoArray.size())
			{
				gMeshLODInfoArray[index].IncRefCount();
			}
		}
/*
		else if (subchunk.GetName() == "BoundingSphere")
		{
			PrintBoundingSphereChunk(subchunk);
		}
*/
		else
		{
			subchunk.Skip();
		}
	}
}

void PrintHierarchyChunk(MChunk& chunk)
{
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


template <typename T>
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



object3d* load_mmod(file& i_file)
{
	gMeshLODInfoArray.clear();
	gHItemArray.clear();

	MChunkStream stream(i_file);

	MChunk chunk = stream.GetTopChunk();
	Version = chunk.ReadUnsigned();

	while (chunk)
	{
		MChunk subchunk = chunk.GetChunk();
		const string& subchunkname=subchunk.GetName();

/*
		if (subchunk.GetName() == "BoundingSphere")
		{
			PrintBoundingSphereChunk(subchunk);
		}
		else if (subchunk.GetName() == "BoundingBox")
		{
			PrintBoundingBoxChunk(subchunk);
		}

		else*/
		if (subchunkname == "Resource")
		{
			PrintResourceChunk(subchunk);
		}
		else if (subchunkname == "Hierarchy")
		{
			PrintHierarchyChunk(subchunk);
		}
		else
		{
			subchunk.Skip();
		}
	}

	vector<fixedvector<mesh_t*,8> > meshbuf;

	for (unsigned n=0; n<gMeshLODInfoArray.size();++n)
	{
		fixedvector<mesh_t*,8> mesh;
		if (gMeshLODInfoArray[n].ismesh)
			mesh=gMeshLODInfoArray[n].generate_mesh();

		meshbuf.push_back(mesh);
	}

	vector<object3d*> objbuf;
	object3d* root=NULL;

	unsigned parentnum=0;

	for (unsigned n=0; n<gHItemArray.size(); ++n)
	{
		objbuf.push_back(gHItemArray[n].generate_object());
		if (gHItemArray[n].parent==-1)
		{
			root=objbuf.back();
			++parentnum;
		}

		if (gHItemArray[n].mesh!=-1)
		{
			for (unsigned k=0; k<meshbuf[gHItemArray[n].mesh].size(); ++k)
				objbuf.back()->add_mesh(meshbuf[gHItemArray[n].mesh][k]);
		}
	}

	assertion(parentnum>0,"nincs parentitem?");

	if (parentnum>1)
	{
		root=new object3d("rootitem");
	}

	for (unsigned n=0; n<gHItemArray.size(); ++n)
	{
		if (gHItemArray[n].parent!=-1)
			objbuf[gHItemArray[n].parent]->add_child(objbuf[n]);
		else if (parentnum>1)
				root->add_child(objbuf[n]);
	}
	gMeshLODInfoArray.clear();
	gHItemArray.clear();

	return root;
}

object3d* load_mmod(const char* i_filename)
{
	file f(i_filename,"rb");

	return load_mmod(f);
}








