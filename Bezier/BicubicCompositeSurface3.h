#ifndef BICUBICCOMPOSITESURFACE3_H
#define BICUBICCOMPOSITESURFACE3_H

#include <Core/Materials.h>
#include "BicubicBezierPatches.h"
#include <Core/ShaderPrograms.h>
#include <QOpenGLTexture>

namespace cagd
{
    class BicubicCompositeSurface3;

    std::ostream& operator << (std::ostream& lhs, const BicubicCompositeSurface3& rhs);
    std::istream& operator >> (std::istream& lhs, BicubicCompositeSurface3& rhs);

    class BicubicCompositeSurface3
    {
        friend std::ostream& operator << (std::ostream& lhs, const BicubicCompositeSurface3& rhs);
        friend std::istream& operator >> (std::istream& lhs, BicubicCompositeSurface3& rhs);

    public:
        enum Direction{N, NW, W, SW, S, SE, E, NE};

        class PatchAttributes
        {
        public:
            BicubicBezierPatch  *patch;
            TriangulatedMesh3   *image;
            GLuint              matInd;
            GLuint              texInd;

            std::vector<PatchAttributes*>   neighbours;

            RowMatrix<GenericCurve3*>* u_lines;
            RowMatrix<GenericCurve3*>* v_lines;

            void _GetSymmetricPointIndexes(const GLuint row, const GLuint column, Direction direction, GLuint &symmetricRow, GLuint &symmetricColumn) const;
            PatchAttributes();
            PatchAttributes(const PatchAttributes &attributes);
            ~PatchAttributes();
        };

    protected:
        std::vector<PatchAttributes> _attributes;
        std::vector<Material>        _materials{ MatFBBrass, MatFBEmerald, MatFBPearl, MatFBRuby, MatFBTurquoise };
        std::vector<QOpenGLTexture*> _textures;
        GLuint _u_iso_line_count;
        GLuint _v_iso_line_count;

    private:
        GLvoid   _loadTextures();

    public:
        // special/default ctor
        BicubicCompositeSurface3(GLuint patchCount = 0);

        // operations
        BicubicBezierPatch* InitializePatch();
        GLboolean InsertNewPatch();
        GLboolean UpdateVBOs(PatchAttributes &attribute);
        GLboolean DeleteExistingPatch(GLuint index);

        GLint     IndexOfAttribute(const PatchAttributes &attribute) const;

        GLboolean ContinueExistingPatch(const GLuint &index, Direction direction);
        GLboolean JoinExistingPatches(const GLuint &firstPatchIndex, Direction firstDirection, const GLuint &secondPatchIndex, Direction secondDirection);
        GLboolean MergeExistingPatches(const GLuint &firstPatchIndex, Direction firstDirection, const GLuint &secondPatchIndex, Direction secondDirection);

        GLboolean UpdatePatch(const GLuint patchIndex, const GLuint row, const GLuint column, const DCoordinate3 position);
        GLboolean UpdatePatch(PatchAttributes &attribute, const GLuint row, const GLuint column, const DCoordinate3 position);
        GLboolean MovePatch(const GLuint patchIndex, const DCoordinate3 difference);

        GLboolean RenderAllPatchesWithMaterials();
        GLboolean RenderAllPatchesWithTextures();
        GLboolean RenderAllPatchesIsoU() const;
        GLboolean RenderAllPatchesIsoV() const;
        GLboolean RenderAllPatchesIsoUd1() const;
        GLboolean RenderAllPatchesIsoVd1() const;
        GLboolean RenderAllPatchesNormal() const;
        GLboolean RenderAllPatchesData(GLuint selectedPatchInd, GLuint selectedRowInd, GLuint selectedColInd) const;
        GLboolean RenderHighlightedPatches(GLuint patchInd1, GLuint patchInd2) const;

        int       GetPatchCount();

        GLboolean GetDataPointValues(const GLuint patchIndex, const GLuint row, const GLuint column, DCoordinate3 &position);
        GLboolean GetDataPointValues(const GLuint patchIndex, const GLuint row, const GLuint column, GLdouble &x, GLdouble &y, GLdouble &z);

        GLboolean ChangeMaterial(GLuint patchInd, GLuint matInd);
        GLboolean ChangeTexture(GLuint patchInd, GLuint texInd);

        GLuint    GetMatInd(GLuint patchInd);
        GLuint    GetTexInd(GLuint patchInd);
        GLuint    GetUIsoLineCount();
        GLuint    GetVIsoLineCount();

        GLvoid UpdateUIsoLines(GLuint iso_line_count);
        GLvoid UpdateVIsoLines(GLuint iso_line_count);
    };
}

#endif // BICUBICCOMPOSITESURFACE3_H
