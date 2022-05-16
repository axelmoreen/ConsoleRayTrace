#include "VecMath.h"
#include <vector>

#ifndef OBJECTS3D_H
#define OBJECTS3D_H

struct Face{
	int verts[3];
	int uvs[3];
	int norms[3];
};

class Obj3D {
public:

	Obj3D() :
		vnum(0), vtnum(0), vnnum(0), fnum(0)
	{};

	Obj3D(int vnum_, int vtnum_, int vnnum_, int fnum_) : 
		vnum(vnum_), vtnum(vtnum_), vnnum(vnnum_), fnum(fnum_) {
		verts = std::make_unique<Vec3[]>(vnum);
		uvs = std::make_unique<Vec2[]>(vtnum);
		norms = std::make_unique<Vec3[]>(vnnum);
		faces = std::make_unique<Face[]>(fnum);
	};
	
	Obj3D(const Obj3D &other) :
		vnum(other.vnum), vtnum(other.vtnum), vnnum(other.vnnum), fnum(other.fnum)
		//verts(copy_unique(other.verts)), uvs(copy_unique(other.uvs)), norms(copy_unique(other.norms)), faces(copy_unique(other.faces))
	{
		verts = std::make_unique<Vec3[]>(vnum);
		uvs = std::make_unique<Vec2[]>(vtnum);
		norms = std::make_unique<Vec3[]>(vnnum);
		faces = std::make_unique<Face[]>(fnum);

		for (int i = 0; i < vnum; i++) {
			verts.get()[i] = other.verts.get()[i];
		}

		for (int i = 0; i < vtnum; i++) {
			uvs.get()[i] = other.uvs.get()[i];
		}
		
		for (int i = 0; i < vnnum; i++) {
			norms.get()[i] = other.norms.get()[i];
		}

		for (int i = 0; i < fnum; i++) {
			faces.get()[i] = other.faces.get()[i];
		}
	}

	int vnum;
	int vtnum;
	int vnnum;
	int fnum;

	std::unique_ptr<Vec3[]> verts;
	std::unique_ptr<Vec2[]> uvs;
	std::unique_ptr<Vec3[]> norms;
	std::unique_ptr<Face[]> faces;
};

#endif

