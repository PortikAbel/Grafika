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
            Material            *material; // use pointers to pre-defined materials
            //QOpenGLTexture      *texture;  // use pointers to pre-defined textures
            //ShaderProgram       *shaders;   // use pointers to pre-defined shader programs

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
        GLuint _iso_line_count;


    public:
        // special/default ctor
        BicubicCompositeSurface3(GLuint patchCount = 0);

        // operations
        BicubicBezierPatch* InitializePatch();
        GLboolean UpdateVBOs(PatchAttributes &attribute);
        GLboolean InsertNewPatch();
        GLboolean DeleteExistingPatch(GLuint index);

        GLint     IndexOfAttribute(const PatchAttributes &attribute) const;

        GLboolean ContinueExistingPatch(const GLuint &index, Direction direction);
        GLboolean JoinExistingPatches(const GLuint &firstPatchIndex, Direction firstDirection, const GLuint &secondPatchIndex, Direction secondDirection);
        GLboolean MergeExistingPatches(const GLuint &firstPatchIndex, Direction firstDirection, const GLuint &secondPatchIndex, Direction secondDirection);

        GLboolean UpdatePatch(const GLuint patchIndex, const GLuint row, const GLuint column, const DCoordinate3 position);
        GLboolean UpdatePatch(PatchAttributes &attribute, const GLuint row, const GLuint column, const DCoordinate3 position);
        GLboolean MovePatch(const GLuint patchIndex, const DCoordinate3 difference);
        GLboolean RenderAllPatches() const;
        GLboolean RenderAllPatchesIsoU() const;
        GLboolean RenderAllPatchesIsoV() const;
        GLboolean RenderAllPatchesIsoUd1() const;
        GLboolean RenderAllPatchesIsoVd1() const;
        GLboolean RenderAllPatchesData() const;
        GLboolean RenderSelectedPatch(GLuint index, GLuint order, GLenum render_mode) const;

        GLboolean GetDataPointValues(const GLuint patchIndex, const GLuint row, const GLuint column, DCoordinate3 &position);
        GLboolean GetDataPointValues(const GLuint patchIndex, const GLuint row, const GLuint column, GLdouble &x, GLdouble &y, GLdouble &z);
        //GLboolean ChangeDataPointValue(const GLuint &arcInd, const GLuint &dataPointInd, const GLuint &pointComponentInd, const GLdouble &val);

        GLboolean SaveToFile(const std::string filename) const;
        GLboolean LoadFromFile(const std::string filename);
    };
}

#endif // BICUBICCOMPOSITESURFACE3_H
