#include <future>

#include "loader.h"
#include "vertex.h"

Loader::Loader(QObject* parent, const QString& filename, bool is_reload)
    : QThread(parent), filename(filename), is_reload(is_reload)
{
    // Nothing to do here
}

void Loader::run()
{
    Mesh* mesh = load_sml();
    if (mesh)
    {
        if (mesh->empty())
        {
            emit error_empty_mesh();
            delete mesh;
        }
        else
        {
            emit got_mesh(mesh, is_reload);
            emit loaded_file(filename);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void parallel_sort(Vertex* begin, Vertex* end, int threads)
{
    if (threads < 2 || end - begin < 2)
    {
        std::sort(begin, end);
    }
    else
    {
        const auto mid = begin + (end - begin) / 2;
        if (threads == 2)
        {
            auto future = std::async(parallel_sort, begin, mid, threads / 2);
            std::sort(mid, end);
            future.wait();
        }
        else
        {
            auto a = std::async(std::launch::async, parallel_sort, begin, mid, threads / 2);
            auto b = std::async(std::launch::async, parallel_sort, mid, end, threads / 2);
            a.wait();
            b.wait();
        }
        std::inplace_merge(begin, mid, end);
    }
}

Mesh* mesh_from_verts(uint32_t tri_count, QVector<Vertex>& verts)
{
    // Save indicies as the second element in the array
    // (so that we can reconstruct triangle order after sorting)
    for (size_t i=0; i < tri_count*3; ++i)
    {
        verts[i].i = i;
    }

    // Check how many threads the hardware can safely support. This may return
    // 0 if the property can't be read so we shoud check for that too.
    auto threads = std::thread::hardware_concurrency();
    if (threads == 0)
    {
        threads = 8;
    }

    // Sort the set of vertices (to deduplicate)
    parallel_sort(verts.begin(), verts.end(), threads);

    // This vector will store triangles as sets of 3 indices
    std::vector<GLuint> indices(tri_count*3);

    // Go through the sorted vertex list, deduplicating and creating
    // an indexed geometry representation for the triangles.
    // Unique vertices are moved so that they occupy the first vertex_count
    // positions in the verts array.
    size_t vertex_count = 0;
    for (auto v : verts)
    {
        if (!vertex_count || v != verts[vertex_count-1])
        {
            verts[vertex_count++] = v;
        }
        indices[v.i] = vertex_count - 1;
    }
    verts.resize(vertex_count);

    std::vector<GLfloat> flat_verts;
    flat_verts.reserve(vertex_count*3);
    for (auto v : verts)
    {
        flat_verts.push_back(v.x);
        flat_verts.push_back(v.y);
        flat_verts.push_back(v.z);
    }

    return new Mesh(std::move(flat_verts), std::move(indices));
}

////////////////////////////////////////////////////////////////////////////////

Mesh* Loader::load_sml()
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        emit error_missing_file();
        return NULL;
    }

	if (file.read(4) == "SML1") {
		return read_sml1(file);
	}

	emit error_bad_stl();
	return NULL;
}

Mesh* Loader::read_sml1(QFile& file)
{
    QDataStream data(&file);
    data.setByteOrder(QDataStream::LittleEndian);
    data.setFloatingPointPrecision(QDataStream::SinglePrecision);

    file.seek(4);
	uint32_t crc;
	data >> crc;
	// TODO: Check this for validity and warn the user if it's not.
	
	QVector<Vertex> tris;
	QVector<Vertex> vertlist;
	
	while (!data.atEnd()) {
		uint8_t segtype;
		data >> segtype;
		
		uint32_t seglen;
		data >> seglen;
		
		switch (segtype) {
			case 0: { // Comment
				data.skipRawData(seglen);
			} break;
			case 1: { // Float vertex list
				uint32_t count = seglen / 12;
				vertlist.clear();
				vertlist.reserve(count);
				float coords[3];
				for (uint32_t i = 0; i < count; i++) {
					data.readRawData((char*)coords, 12);
					vertlist.push_back(Vertex(coords[0], coords[1], coords[2]));
				}
			} break;
			case 2: { // Double vertex list
				uint32_t count = seglen / 24;
				vertlist.clear();
				vertlist.reserve(count);
				double coords[3];
				for (uint32_t i = 0; i < count; i++) {
					data.readRawData((char*)coords, 24);
					vertlist.push_back(Vertex(coords[0], coords[1], coords[2]));
				}
			} break;
			case 3: { // Triangle list
				uint32_t count = seglen / 12;
				uint32_t verts[3];
				for (uint32_t i = 0; i < count; i++) {
					data.readRawData((char*)verts, 12);
					
					tris.push_back(vertlist[verts[0]]);
					tris.push_back(vertlist[verts[1]]);
					tris.push_back(vertlist[verts[2]]);
				}
			} break;
			case 4: { // Quad list
				uint32_t count = seglen / 16;
				uint32_t verts[4];
				for (uint32_t i = 0; i < count; i++) {
					data.readRawData((char*)verts, 16);
					
					tris.push_back(vertlist[verts[0]]);
					tris.push_back(vertlist[verts[1]]);
					tris.push_back(vertlist[verts[2]]);
					
					tris.push_back(vertlist[verts[0]]);
					tris.push_back(vertlist[verts[2]]);
					tris.push_back(vertlist[verts[3]]);
				}
			} break;
			case 5: { // Triangle strip
				int points = seglen / 4;
				uint32_t verts[3];
				
				data.readRawData((char*)verts, 12);
				tris.push_back(vertlist[verts[0]]);
				tris.push_back(vertlist[verts[1]]);
				tris.push_back(vertlist[verts[2]]);
				
				for (int i = 3; i < points; i++) {
					/* i=0  0 1 2
					   i=3  0 2 3	2->1
					   i=4  3 2 4	2->0 */
					verts[i & 1] = verts[2];
					data >> verts[2];
					
					tris.push_back(vertlist[verts[0]]);
					tris.push_back(vertlist[verts[1]]);
					tris.push_back(vertlist[verts[2]]);
				}
			} break;
			default:
				emit error_bad_stl();
				return NULL;
		}
	}

    return mesh_from_verts(tris.size()/3, tris);
}


