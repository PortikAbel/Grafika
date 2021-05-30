#include "BicubicCompositeSurface3.h"
#include "../Core/Materials.h"
#include <Core/Exceptions.h>
#include <iostream>
#include <fstream>

using namespace std;

namespace cagd
{
    // --------------------------------------------------------------------------------
    // PatchAttributess
    // --------------------------------------------------------------------------------

    BicubicCompositeSurface3::PatchAttributes::PatchAttributes():
        patch(nullptr), image(nullptr), neighbours(8, nullptr), u_lines(nullptr), v_lines(nullptr)
    {
        material = new Material(MatFBGold);
    }

    BicubicCompositeSurface3::PatchAttributes::PatchAttributes(const PatchAttributes &attribute)
    {
        if (attribute.patch)
        {
            patch = new BicubicBezierPatch(*attribute.patch);
            image = new TriangulatedMesh3(*attribute.image);
        }
        else
        {
            patch = nullptr;
            image = nullptr;
        }

        material = attribute.material;

        neighbours.resize(8, nullptr);
        for (int i = 0; i < 8; ++i)
        {
            neighbours[i] = attribute.neighbours[i];
        }
    }

    BicubicCompositeSurface3::PatchAttributes::~PatchAttributes()
    {
        if (patch)
        {
            delete patch; patch = nullptr;
        }
        if (image)
        {
            delete image; image = nullptr;
        }

        if (u_lines)
        {
            for (GLuint i=0; i<u_lines->GetColumnCount(); ++i)
            {
                if ((*u_lines)[i])
                {
                    delete (*u_lines)[i], (*u_lines)[i] = nullptr;
                }
            }

            delete u_lines, u_lines = nullptr;
        }

        if (v_lines)
        {
            for (GLuint i=0; i<v_lines->GetColumnCount(); ++i)
            {
                if ((*v_lines)[i])
                {
                    delete (*v_lines)[i], (*v_lines)[i] = nullptr;
                }
            }

            delete v_lines, v_lines = nullptr;
        }
    }


    void BicubicCompositeSurface3::PatchAttributes::_GetSymmetricPointIndexes(
            const GLuint row,
            const GLuint column,
            Direction direction,
            GLuint &symmetricRow,
            GLuint &symmetricColumn) const
    {
        if (direction == N)
        {
            if (neighbours[N]->neighbours[N] == this)
            {
                symmetricRow = row;
                symmetricColumn = column;
            }
            else if (neighbours[N]->neighbours[W] == this)
            {
                symmetricRow = column;
                symmetricColumn = row;
            }
            else if (neighbours[N]->neighbours[S] == this)
            {
                symmetricRow = 3 - row;
                symmetricColumn = column;
            }
            else if (neighbours[N]->neighbours[E] == this)
            {
                symmetricRow =  3 - column;
                symmetricColumn = 3 - row;
            }

            return;
        }

        if (direction == W)
        {
            if (neighbours[W]->neighbours[N] == this)
            {
                symmetricRow = column;
                symmetricColumn = row;
            }
            else if (neighbours[W]->neighbours[W] == this)
            {
                symmetricRow = column;
                symmetricColumn = 3 - row;
            }
            else if (neighbours[W]->neighbours[S] == this)
            {
                symmetricRow = 3 - column;
                symmetricColumn = 3 - row;
            }
            else if (neighbours[W]->neighbours[E] == this)
            {
                symmetricRow = row;
                symmetricColumn = 3 - column;
            }

            return;
        }


        if (direction == S)
        {
            if (neighbours[S]->neighbours[N] == this)
            {
                symmetricRow = 3-row;
                symmetricColumn = column;
            }
            else if (neighbours[S]->neighbours[W] == this)
            {
                symmetricRow = 3 - column;
                symmetricColumn = 3 - row;
            }
            else if (neighbours[S]->neighbours[S] == this)
            {
                symmetricRow = row;
                symmetricColumn = 3 - column;
            }
            else if (neighbours[S]->neighbours[E] == this)
            {
                symmetricRow = column;
                symmetricColumn = row;
            }

            return;
        }

        if (direction == E)
        {
            if (neighbours[E]->neighbours[N] == this)
            {
                symmetricRow =  3 - column;
                symmetricColumn = 3 - row;
            }
            else if (neighbours[E]->neighbours[W] == this)
            {
                symmetricRow = row;
                symmetricColumn = 3 - column;
            }
            else if (neighbours[E]->neighbours[S] == this)
            {
                symmetricRow = column;
                symmetricColumn = row;
            }
            else if (neighbours[E]->neighbours[E] == this)
            {
                symmetricRow = 3 - row;
                symmetricColumn = column;
            }
        }
    }

    // --------------------------------------------------------------------------------

    BicubicCompositeSurface3::BicubicCompositeSurface3(GLuint patchCount):
        _iso_line_count(50)
    {
        _attributes.resize(patchCount);
        for (GLuint i = 0; i < patchCount; i++)
        {
            _attributes[i].patch = InitializePatch();
            UpdateVBOs(_attributes[i]);
        }
    }

    BicubicBezierPatch* BicubicCompositeSurface3::InitializePatch()
    {
        BicubicBezierPatch* patch = new BicubicBezierPatch();

        patch->SetData(0, 0, -2.0, -2.0, 0.0);
        patch->SetData(0, 1, -2.0, -1.0, 0.0);
        patch->SetData(0, 2, -2.0,  0.0, 0.0);
        patch->SetData(0, 3, -2.0,  1.0, 0.0);

        patch->SetData(1, 0, -1.0, -2.0, 0.0);
        patch->SetData(1, 1, -1.0, -1.0, 0.0);
        patch->SetData(1, 2, -1.0,  0.0, 0.0);
        patch->SetData(1, 3, -1.0,  1.0, 0.0);

        patch->SetData(2, 0, 0.0, -2.0, 0.0);
        patch->SetData(2, 1, 0.0, -1.0, 0.0);
        patch->SetData(2, 2, 0.0,  0.0, 0.0);
        patch->SetData(2, 3, 0.0,  1.0, 0.0);

        patch->SetData(3, 0, 1.0, -2.0, 0.0);
        patch->SetData(3, 1, 1.0, -1.0, 0.0);
        patch->SetData(3, 2, 1.0,  0.0, 0.0);
        patch->SetData(3, 3, 1.0,  1.0, 0.0);

        if (!patch->UpdateVertexBufferObjectsOfData())
        {
            throw Exception("Could not update the VBO of data of the patch!");
        }

        return patch;
    }

    GLboolean BicubicCompositeSurface3::UpdateVBOs(PatchAttributes &attribute)
    {
        if (!attribute.patch->UpdateVertexBufferObjectsOfData())
        {
            throw Exception("Could not update the VBO of data of the patch!");
        }

        attribute.u_lines = attribute.patch->GenerateUIsoparametricLines(_iso_line_count, 1, 30);
        attribute.v_lines = attribute.patch->GenerateVIsoparametricLines(_iso_line_count, 1, 30);
        for(GLuint i=0; i<attribute.u_lines->GetColumnCount(); ++i)
        {
            if((*attribute.u_lines)[i])
            {
                (*attribute.u_lines)[i]->UpdateVertexBufferObjects();
            }
        }
        for(GLuint i=0; i<attribute.v_lines->GetColumnCount(); ++i)
        {
            if((*attribute.v_lines)[i])
            {
                (*attribute.v_lines)[i]->UpdateVertexBufferObjects();
            }
        }

        attribute.image = attribute.patch->GenerateImage(_iso_line_count, _iso_line_count);
        if (!attribute.image)
        {
            throw Exception("Could not generate the image of patch!");
        }
        if (!attribute.image->UpdateVertexBufferObjects())
        {
            throw Exception("Could not update the VBO of patch image");
        }

        return GL_TRUE;
    }

    GLint BicubicCompositeSurface3::IndexOfAttribute(const PatchAttributes &attribute) const
    {
        for (GLuint i=0; i<_attributes.size(); ++i)
        {
            if (&_attributes[i] == &attribute)
            {
                return i;
            }
        }

        return -1;
    }

    GLboolean BicubicCompositeSurface3::RenderAllPatches() const
    {
        for (auto it = _attributes.begin(); it != _attributes.end(); ++it)
        {
            if (it->image)
            {
                glEnable(GL_LIGHTING);
                glEnable(GL_NORMALIZE);

                    it->material->Apply();
                    it->image->Render();

                glDisable(GL_LIGHTING);
                glDisable(GL_NORMALIZE);
            }
        }

        return GL_TRUE;
    }

    GLboolean BicubicCompositeSurface3::RenderAllPatchesIsoU() const
    {
        glPointSize(6.0);

        for (auto it = _attributes.begin(); it != _attributes.end(); ++it)
        {
            if (it->u_lines)
            {
                glColor3f(1.0f, 0.0f, 0.0f); // red for iso lines

                for(GLuint i=0; i<it->u_lines->GetColumnCount(); ++i)
                {
                    (*(it->u_lines))[i]->RenderDerivatives(0, GL_LINE_STRIP);
                }
            }
        }

        glPointSize(1.0);
        return GL_TRUE;
    }

    GLboolean BicubicCompositeSurface3::RenderAllPatchesIsoV() const
    {
        glPointSize(6.0);

        for (auto it = _attributes.begin(); it != _attributes.end(); ++it)
        {
            if (it->v_lines)
            {
                glColor3f(1.0f, 0.0f, 0.0f); // red for iso lines

                for(GLuint i=0; i<it->v_lines->GetColumnCount(); ++i)
                {
                    (*(it->v_lines))[i]->RenderDerivatives(0, GL_LINE_STRIP);
                }
            }

        }

        glPointSize(1.0);
        return GL_TRUE;
    }

    GLboolean BicubicCompositeSurface3::RenderAllPatchesIsoUd1() const
    {
        glPointSize(6.0);

        for (auto it = _attributes.begin(); it != _attributes.end(); ++it)
        {
            if (it->u_lines)
            {
                glColor3f(0.0f, 0.5f, 0.0f); // green for first derivatives

                for(GLuint i=0; i<it->u_lines->GetColumnCount(); ++i)
                {
                    (*(it->u_lines))[i]->RenderDerivatives(1, GL_POINTS);
                    (*(it->u_lines))[i]->RenderDerivatives(1, GL_LINES);
                }
            }
        }

        glPointSize(1.0);
        return GL_TRUE;
    }

    GLboolean BicubicCompositeSurface3::RenderAllPatchesIsoVd1() const
    {
        glPointSize(6.0);

        for (auto it = _attributes.begin(); it != _attributes.end(); ++it)
        {
            if (it->v_lines)
            {
                glColor3f(0.0f, 0.5f, 0.0f); // green for first derivatives

                for(GLuint i=0; i<it->v_lines->GetColumnCount(); ++i)
                {
                    (*(it->v_lines))[i]->RenderDerivatives(1, GL_POINTS);
                    (*(it->v_lines))[i]->RenderDerivatives(1, GL_LINES);
                }
            }
        }

        glPointSize(1.0);
        return GL_TRUE;
    }

    GLboolean BicubicCompositeSurface3::RenderAllPatchesData() const
    {
        glPointSize(6.0);

        for (auto it = _attributes.begin(); it != _attributes.end(); ++it)
        {
            if (it->patch)
            {
                glColor3f(0.7f, 1.0f, 0.0f); // neongreen for control net
                it->patch->RenderData();
            }
        }

        glPointSize(1.0);
        return GL_TRUE;
    }

    GLboolean BicubicCompositeSurface3::RenderSelectedPatch(GLuint patchInd, GLuint selectionInd) const
    {
        if (!_attributes[patchInd].image)
        {
            return GL_FALSE;
        }

        glEnable(GL_LIGHTING);
        glEnable(GL_NORMALIZE);

        if (selectionInd == 1)
        {
            MatFBGold.Apply();
        }
        else if (selectionInd == 2)
        {
            MatFBSilver.Apply();
        }
        else
        {
            return GL_FALSE;
        }

        _attributes[patchInd].image->Render();

        glDisable(GL_LIGHTING);
        glDisable(GL_NORMALIZE);

        return GL_TRUE;
    }


    GLboolean BicubicCompositeSurface3::GetDataPointValues(const GLuint patchIndex, const GLuint row, const GLuint column, DCoordinate3 &position)
    {
        (*_attributes[patchIndex].patch).GetData(row,column,position);
        return GL_TRUE;
    }

    GLboolean BicubicCompositeSurface3::GetDataPointValues(const GLuint patchIndex, const GLuint row, const GLuint column, GLdouble &x, GLdouble &y, GLdouble &z)
    {
        (*_attributes[patchIndex].patch).GetData(row,column,x,y,z);
        return GL_TRUE;
    }

    GLboolean BicubicCompositeSurface3::InsertNewPatch()
    {
        PatchAttributes attr;
        _attributes.push_back(attr);
        PatchAttributes& attribute = _attributes.back();

        try
        {
            attribute.patch = InitializePatch();
            return UpdateVBOs(attribute);
        }
        catch (Exception ex)
        {
            cout << "Could not insert isolated patch!" << endl;
            return GL_FALSE;
        }
    }

    GLboolean BicubicCompositeSurface3::UpdatePatch(const GLuint patchIndex, const GLuint row, const GLuint column, const DCoordinate3 position)
    {
        if (patchIndex >= _attributes.size() || row > 3 || column > 3)
        {
            cout << "Invalid patch or control point index!" << endl;
            return GL_FALSE;
        }

        return UpdatePatch(_attributes[patchIndex], row, column, position);
    }

    GLboolean BicubicCompositeSurface3::UpdatePatch(PatchAttributes &attribute, const GLuint row, const GLuint column, const DCoordinate3 position)
    {
        GLint r = -1, c = -1;

        DCoordinate3 difference = position - (*attribute.patch)(row, column);

        (*attribute.patch)(row, column) = position;

        if (row == 1 && attribute.neighbours[N])
        {
            PatchAttributes &neighbour = *attribute.neighbours[N];
            GLuint i, j;
            attribute._GetSymmetricPointIndexes(1, column, N, i, j);

            UpdatePatch(neighbour, i, j, 2 * (*attribute.patch)(0, column) - (*attribute.patch)(1, column));
        }
        if (row == 2 && attribute.neighbours[S])
        {
            PatchAttributes &neighbour = *attribute.neighbours[S];
            GLuint i, j;
            attribute._GetSymmetricPointIndexes(2, column, S, i, j);

            UpdatePatch(neighbour, i, j, 2 * (*attribute.patch)(3, column) - (*attribute.patch)(2, column));
        }

        if (column == 1 && attribute.neighbours[W])
        {
            PatchAttributes &neighbour = *attribute.neighbours[W];
            GLuint i, j;
            attribute._GetSymmetricPointIndexes(row, 1, W, i, j);

            UpdatePatch(neighbour, i, j, 2 * (*attribute.patch)(row, 0) - (*attribute.patch)(row, 1));
        }
        if (column == 2 && attribute.neighbours[E])
        {
            PatchAttributes &neighbour = *attribute.neighbours[E];
            GLuint i, j;
            attribute._GetSymmetricPointIndexes(row, 2, E, i, j);

            UpdatePatch(neighbour, i, j, 2 * (*attribute.patch)(row, 3) - (*attribute.patch)(row, 2));
        }


        if (row == 0 && attribute.neighbours[N])
        {
            r = 1;

            PatchAttributes &neighbour = *attribute.neighbours[N];
            GLuint i, j;
            attribute._GetSymmetricPointIndexes(0, column, N, i, j);

            UpdatePatch(neighbour, i, j, position);

            attribute._GetSymmetricPointIndexes(1, column, N, i, j);
//            UpdatePatch(attribute, 1, column, (*attribute.patch)(1, column) + difference);
            (*attribute.patch)(1, column) += difference;
//            (*neighbour.patch)(i, j) += difference;
//            _UpdateVBOs(neighbour);
        }
        if (row == 3 && attribute.neighbours[S])
        {
            r = 2;

            PatchAttributes &neighbour = *attribute.neighbours[S];
            GLuint i, j;
            attribute._GetSymmetricPointIndexes(3, column, S, i, j);

            UpdatePatch(neighbour, i, j, position);

            attribute._GetSymmetricPointIndexes(2, column, S, i, j);
//            UpdatePatch(attribute, 2, column, (*attribute.patch)(2, column) + difference);
            (*attribute.patch)(2, column) += difference;
//            (*neighbour.patch)(i, j) += difference;
//            _UpdateVBOs(neighbour);
        }

        if (column == 0 && attribute.neighbours[W])
        {
            c = 1;

            PatchAttributes &neighbour = *attribute.neighbours[W];
            GLuint i, j;
            attribute._GetSymmetricPointIndexes(row, 0, W, i, j);

            UpdatePatch(neighbour, i, j, position);

            attribute._GetSymmetricPointIndexes(row, 1, W, i, j);
//            UpdatePatch(attribute, row, 1, (*attribute.patch)(row, 1) + difference);
            (*attribute.patch)(row, 1) += difference;
//            (*neighbour.patch)(i, j) += difference;
//            _UpdateVBOs(neighbour);
        }
        if (column == 3 && attribute.neighbours[E])
        {
            c = 2;

            PatchAttributes &neighbour = *attribute.neighbours[E];
            GLuint i, j;
            attribute._GetSymmetricPointIndexes(row, 3, E, i, j);

            UpdatePatch(neighbour, i, j, position);

            attribute._GetSymmetricPointIndexes(row, 2, E, i, j);
//            UpdatePatch(attribute, row, 2, (*attribute.patch)(row, 2) + difference);
            (*attribute.patch)(row, 2) += difference;
//            (*neighbour.patch)(i, j) += difference;
//            _UpdateVBOs(neighbour);
        }

        // corner point
        if (r != -1 && c != -1)
        {
            (*attribute.patch)(r, c) += difference;
            // UpdatePatch(attribute, r, c, (*attribute.patch)(r, c) + difference);
        }

        return UpdateVBOs(attribute);
    }

    GLboolean BicubicCompositeSurface3::MovePatch(const GLuint patchIndex, const DCoordinate3 difference)
    {
        if (patchIndex >= _attributes.size())
        {
            cout << "Invalid patch index!" << endl;
            return GL_FALSE;
        }

        PatchAttributes &attribute = _attributes[patchIndex];

        for (GLuint i=0; i<=3; ++i)
        {
            for (GLuint j=0; j<=3; ++j)
            {
                (*attribute.patch)(i, j) += difference;
            }
        }

        return UpdateVBOs(attribute);
    }

    GLboolean BicubicCompositeSurface3::ContinueExistingPatch(const GLuint &patchIndex, Direction direction)
    {
        if (patchIndex >= _attributes.size())
        {
            cout << "Invalid patch index!" << endl;
            return GL_FALSE;
        }

        PatchAttributes &attribute = _attributes[patchIndex];

         if (attribute.neighbours[direction])
         {
             cout << "The patch arlready has a neighbor in the given direction!" << endl;
             return GL_FALSE;
         }

        PatchAttributes newAttr;
        _attributes.push_back(newAttr);
        PatchAttributes &newAttribute = _attributes.back();

        newAttribute.patch = new BicubicBezierPatch();
        BicubicBezierPatch* patch = newAttribute.patch;

        if (direction == N)
        {
            for(GLuint i=0; i<=3; ++i)
            {
               (*patch)(3, i) = (*attribute.patch)(0, i);
               (*patch)(2, i) = 2 * (*attribute.patch)(0, i) - (*attribute.patch)(1, i);
               (*patch)(1, i) = 2 * (*patch)(2, i) - (*patch)(3, i);
               (*patch)(0, i) = 2 * (*patch)(1, i) - (*patch)(2, i);

            }
            attribute.neighbours[0] = &newAttribute;
            newAttribute.neighbours[4] = &attribute;
        }
        else   if (direction == W)
              {
                for(GLuint i=0; i<=3; ++i)
                {
                  (*patch)(i, 3) = (*attribute.patch)(i, 0);
                  (*patch)(i, 2) = 2 * (*attribute.patch)(i, 0) - (*attribute.patch)(i, 1);
                  (*patch)(i, 1) = 2 * (*patch)(i, 2) - (*patch)(i, 3);
                  (*patch)(i, 0) = 2 * (*patch)(i, 1) - (*patch)(i, 2);

                }
                attribute.neighbours[2] = &newAttribute;
                newAttribute.neighbours[6] = &attribute;
              }
        else   if (direction == S)
              {
                for(GLuint i=0; i<=3; ++i)
                {
                  (*patch)(0, i) = (*attribute.patch)(3, i);
                  (*patch)(1, i) = 2 * (*attribute.patch)(3, i) - (*attribute.patch)(2, i);
                  (*patch)(2, i) = 2 * (*patch)(1, i) - (*patch)(0, i);
                  (*patch)(3, i) = 2 * (*patch)(2, i) - (*patch)(1, i);

                }
                attribute.neighbours[4] = &newAttribute;
                newAttribute.neighbours[0] = &attribute;
              }
        else   if (direction == E)
              {
                for(GLuint i=0; i<=3; ++i)
                {
                  (*patch)(i, 0) = (*attribute.patch)(i, 3);
                  (*patch)(i, 1) = 2 * (*attribute.patch)(i, 3) - (*attribute.patch)(i, 2);
                  (*patch)(i, 2) = 2 * (*patch)(i, 1) - (*patch)(i, 0);
                  (*patch)(i, 3) = 2 * (*patch)(i, 2) - (*patch)(i, 1);

                }
                attribute.neighbours[6] = &newAttribute;
                newAttribute.neighbours[2] = &attribute;
              }


        return UpdateVBOs(newAttribute);
    }

    // TODO: szomszédokat bekötni
    GLboolean BicubicCompositeSurface3::JoinExistingPatches(const GLuint &firstPatchIndex, Direction firstDirection, const GLuint &secondPatchIndex, Direction secondDirection)
    {
        if (firstPatchIndex >= _attributes.size() || secondPatchIndex >= _attributes.size())
        {
            cout << "Invalid patch index(es)!" << endl;
            return GL_FALSE;
        }

        PatchAttributes &firstAttribute = _attributes[firstPatchIndex];
        PatchAttributes &secondAttribute = _attributes[secondPatchIndex];

        if (firstAttribute.neighbours[firstDirection] || secondAttribute.neighbours[secondDirection])
        {
            cout << "One of the patches arlready has a neighbor in the given direction!" << endl;
            return GL_FALSE;
        }

        PatchAttributes newAttr;
        _attributes.push_back(newAttr);
        PatchAttributes &newAttribute = _attributes.back();

        newAttribute.patch = new BicubicBezierPatch();

        //---------------------------------------------------------
        //                  FIRST DIRECTION
        //---------------------------------------------------------
        if (firstDirection == N)
        {
            for (GLuint i=0; i<=3; ++i)
            {
                (*newAttribute.patch)(0, i) = (*firstAttribute.patch)(0, i);
                (*newAttribute.patch)(1, i) = 2 * (*firstAttribute.patch)(0, i) - (*firstAttribute.patch)(1, i);
            }
            firstAttribute.neighbours[0] = &newAttribute;
            newAttribute.neighbours[4] = &firstAttribute;
        }
        else if (firstDirection == W)
        {
            for (GLuint i=0; i<=3; ++i)
            {
                (*newAttribute.patch)(0, i) = (*firstAttribute.patch)(i, 0);
                (*newAttribute.patch)(1, i) = 2 * (*firstAttribute.patch)(i, 0) - (*firstAttribute.patch)(i, 1);
            }
            firstAttribute.neighbours[2] = &newAttribute;
            newAttribute.neighbours[6] = &firstAttribute;
        }
        else if (firstDirection == S)
        {
            for (GLuint i = 0; i < 4; ++i)
            {
                (*newAttribute.patch)(0,i) = (*firstAttribute.patch)(3,i);
                (*newAttribute.patch)(1,i) = 2 * (*firstAttribute.patch)(3,i) - (*firstAttribute.patch)(2,i);
            }
            firstAttribute.neighbours[4] = &newAttribute;
            newAttribute.neighbours[0] = &firstAttribute;
        }
        else if (firstDirection == E)
        {
            for (GLuint i = 0; i < 4; ++i)
            {
                (*newAttribute.patch)(0,i) = (*firstAttribute.patch)(i,3);
                (*newAttribute.patch)(1,i) = 2 * (*firstAttribute.patch)(i,3) - (*firstAttribute.patch)(i,2);
            }
            firstAttribute.neighbours[6] = &newAttribute;
            newAttribute.neighbours[2] = &firstAttribute;
        }

        //---------------------------------------------------------
        //                  SECOND DIRECTION
        //---------------------------------------------------------
        if (secondDirection == N)
        {
            for (GLuint i=0; i<=3; ++i)
            {
                (*newAttribute.patch)(3, i) = (*secondAttribute.patch)(0, i);
                (*newAttribute.patch)(2, i) = 2 * (*secondAttribute.patch)(0, i) - (*secondAttribute.patch)(1, i);
            }
            secondAttribute.neighbours[0] = &newAttribute;
            newAttribute.neighbours[4] = &secondAttribute;
        }
        else if (secondDirection == W)
        {
            for (GLuint i=0; i<=3; ++i)
            {
                (*newAttribute.patch)(3, i) = (*secondAttribute.patch)(i, 0);
                (*newAttribute.patch)(2, i) = 2 * (*secondAttribute.patch)(i, 0) - (*secondAttribute.patch)(i, 1);
            }
            secondAttribute.neighbours[2] = &newAttribute;
            newAttribute.neighbours[6] = &secondAttribute;
        }
        else if (secondDirection == S)
        {
            for (GLuint i = 0; i < 4; ++i)
            {
                (*newAttribute.patch)(3,i) = (*secondAttribute.patch)(3,i);
                (*newAttribute.patch)(2,i) = 2 * (*secondAttribute.patch)(3,i) - (*secondAttribute.patch)(2,i);
            }
            secondAttribute.neighbours[4] = &newAttribute;
            newAttribute.neighbours[0] = &secondAttribute;
        }
        else if (secondDirection == E)
        {
            for (GLuint i = 0; i < 4; ++i)
            {
                (*newAttribute.patch)(3,i) = (*secondAttribute.patch)(i,3);
                (*newAttribute.patch)(2,i) = 2 * (*secondAttribute.patch)(3,i) - (*secondAttribute.patch)(i,2);
            }
            secondAttribute.neighbours[6] = &newAttribute;
            newAttribute.neighbours[2] = &secondAttribute;
        }

        return UpdateVBOs(newAttribute);
    }

    GLboolean BicubicCompositeSurface3::MergeExistingPatches(const GLuint &firstPatchIndex, Direction firstDirection, const GLuint &secondPatchIndex, Direction secondDirection)
    {
        if (firstPatchIndex >= _attributes.size() || secondPatchIndex >= _attributes.size())
        {
            cout << "Invalid patch index(es)!" << endl;
            return GL_FALSE;
        }

        PatchAttributes &firstAttribute = _attributes[firstPatchIndex];
        PatchAttributes &secondAttribute = _attributes[secondPatchIndex];

        if (firstAttribute.neighbours[firstDirection] || secondAttribute.neighbours[secondDirection])
        {
            cout << "One of the patches arlready has a neighbor in the given direction!" << endl;
            return GL_FALSE;
        }

        if (firstDirection == N && secondDirection == N)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute.patch)(0, i) = (*secondAttribute.patch)(0, i) = 0.5 * ((*firstAttribute.patch)(1, i) + (*secondAttribute.patch)(1, i));
            }

            firstAttribute.neighbours[0] = &secondAttribute;
            secondAttribute.neighbours[0] = &firstAttribute;
        }
        else if (firstDirection == N && secondDirection == W)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute.patch)(0, i) = (*secondAttribute.patch)(i, 0) = 0.5 * ((*firstAttribute.patch)(1, i) + (*secondAttribute.patch)(i, 1));
            }

            firstAttribute.neighbours[0] = &secondAttribute;
            secondAttribute.neighbours[2] = &firstAttribute;
        }
        else if (firstDirection == N && secondDirection == S)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute.patch)(0, i) = (*secondAttribute.patch)(3, i) = 0.5 * ((*firstAttribute.patch)(1, i) + (*secondAttribute.patch)(2, i));
            }

            firstAttribute.neighbours[0] = &secondAttribute;
            secondAttribute.neighbours[4] = &firstAttribute;

            //here

            //Down circle

            PatchAttributes* neighbour = firstAttribute.neighbours[2];


            while (neighbour) {
                (*neighbour->patch)(0,3) = (*firstAttribute.patch)(0,0);
                if (!UpdateVBOs(*neighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                neighbour = neighbour->neighbours[0];
                if(!neighbour){
                    break;
                }


                (*neighbour->patch)(3,3) =  (*firstAttribute.patch)(0,0);

                if (!UpdateVBOs(*neighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                neighbour = neighbour->neighbours[6];
                if(!neighbour){
                    break;
                }



                (*neighbour->patch)(3,0) = (*firstAttribute.patch)(0,0);

                if (!UpdateVBOs(*neighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                neighbour = neighbour->neighbours[4];

                if(!neighbour || neighbour == &firstAttribute)
                {
                    break;
                }


                (*neighbour->patch)(0,0) = (*firstAttribute.patch)(0,0);

                if (!UpdateVBOs(*neighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                neighbour = neighbour->neighbours[2];



            }


            //Up circle

            PatchAttributes* upNeighbour = firstAttribute.neighbours[6];


            while (upNeighbour) {
                (*upNeighbour->patch)(0,0) = (*firstAttribute.patch)(0,3);
                if (!UpdateVBOs(*upNeighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                upNeighbour = upNeighbour->neighbours[0];
                if(!upNeighbour){
                    break;
                }



                (*upNeighbour->patch)(3,0) =  (*firstAttribute.patch)(0,3);

                if (!UpdateVBOs(*upNeighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                upNeighbour = upNeighbour->neighbours[2];
                if(!upNeighbour){
                    break;
                }



                (*upNeighbour->patch)(3,3) = (*firstAttribute.patch)(0,3);

                if (!UpdateVBOs(*upNeighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                upNeighbour = upNeighbour->neighbours[4];

                if(!upNeighbour || upNeighbour == &firstAttribute)
                {
                    break;
                }


                (*upNeighbour->patch)(0,3) = (*firstAttribute.patch)(0,3);

                if (!UpdateVBOs(*upNeighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                upNeighbour = upNeighbour->neighbours[6];



            }


            //end here
        }
        else if (firstDirection == N && secondDirection == E)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute.patch)(0, i) = (*secondAttribute.patch)(i, 3) = 0.5 * ((*firstAttribute.patch)(1, i) + (*secondAttribute.patch)(i, 2));
            }

            firstAttribute.neighbours[0] = &secondAttribute;
            secondAttribute.neighbours[6] = &firstAttribute;
        }
        else if (firstDirection == W && secondDirection == N)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute.patch)(i, 0) = (*secondAttribute.patch)(0, i) = 0.5 * ((*firstAttribute.patch)(i, 1) + (*secondAttribute.patch)(1, i));
            }

            firstAttribute.neighbours[2] = &secondAttribute;
            secondAttribute.neighbours[0] = &firstAttribute;
        }
        else if (firstDirection == W && secondDirection == W)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute.patch)(i, 0) = (*secondAttribute.patch)(i, 0) = 0.5 * ((*firstAttribute.patch)(i, 1) + (*secondAttribute.patch)(i, 1));
            }

            firstAttribute.neighbours[2] = &secondAttribute;
            secondAttribute.neighbours[2] = &firstAttribute;
        }
        else if (firstDirection == W && secondDirection == S)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute.patch)(i, 0) = (*secondAttribute.patch)(3, i) = 0.5 * ((*firstAttribute.patch)(i, 1) + (*secondAttribute.patch)(2, i));
            }

            firstAttribute.neighbours[2] = &secondAttribute;
            secondAttribute.neighbours[4] = &firstAttribute;
        }
        else if (firstDirection == W && secondDirection == E)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute.patch)(i, 0) = (*secondAttribute.patch)(i, 3) = 0.5 * ((*firstAttribute.patch)(i, 1) + (*secondAttribute.patch)(i, 2));
            }

            firstAttribute.neighbours[2] = &secondAttribute;
            secondAttribute.neighbours[6] = &firstAttribute;

            // here


            //left circle
            PatchAttributes* neighbour = firstAttribute.neighbours[0];

            while (neighbour) {

                (*neighbour->patch)(3,0) = (*firstAttribute.patch)(0,0);

                if (!UpdateVBOs(*neighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                neighbour = neighbour->neighbours[2];
                if(!neighbour){
                    break;
                }

                (*neighbour->patch)(3,3) = (*firstAttribute.patch)(0,0);

                if (!UpdateVBOs(*neighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                neighbour = neighbour->neighbours[4];
                if(!neighbour){
                    break;
                }

                (*neighbour->patch)(0,3) = (*firstAttribute.patch)(0,0);

                if (!UpdateVBOs(*neighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                neighbour = neighbour->neighbours[6];
                if(!neighbour || neighbour == &firstAttribute  ){
                    break;
                }

                (*neighbour->patch)(0,0) = (*firstAttribute.patch)(0,0);

                if (!UpdateVBOs(*neighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                neighbour = neighbour->neighbours[0];
            }


            //right circle

            PatchAttributes* rightNeighbour = firstAttribute.neighbours[4];

            while (rightNeighbour) {

                (*rightNeighbour->patch)(0,0) = (*firstAttribute.patch)(3,0);

                if (!UpdateVBOs(*rightNeighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                rightNeighbour = rightNeighbour->neighbours[2];
                if(!rightNeighbour){
                    break;
                }

                (*rightNeighbour->patch)(0,3) = (*firstAttribute.patch)(3,0);

                if (!UpdateVBOs(*rightNeighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                rightNeighbour = rightNeighbour->neighbours[0];
                if(!rightNeighbour){
                    break;
                }

                (*rightNeighbour->patch)(3,3) = (*firstAttribute.patch)(3,0);

                if (!UpdateVBOs(*rightNeighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                rightNeighbour = rightNeighbour->neighbours[6];
                if(!rightNeighbour || rightNeighbour == &firstAttribute  ){
                    break;
                }

                (*rightNeighbour->patch)(3,0) = (*firstAttribute.patch)(3,0);

                if (!UpdateVBOs(*rightNeighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                rightNeighbour = rightNeighbour->neighbours[0];
            }


            //end here


        }
        else if (firstDirection == S && secondDirection == N)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute.patch)(3, i) = (*secondAttribute.patch)(0, i) = 0.5 * ((*firstAttribute.patch)(2, i) + (*secondAttribute.patch)(1, i));
            }

            firstAttribute.neighbours[4] = &secondAttribute;
            secondAttribute.neighbours[0] = &firstAttribute;


            //here

            //Down circle

            PatchAttributes* neighbour = firstAttribute.neighbours[2];

            while (neighbour) {
                (*neighbour->patch)(3,3) = (*firstAttribute.patch)(3,0);
                if (!UpdateVBOs(*neighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                neighbour = neighbour->neighbours[4];
                if(!neighbour){
                    break;
                }


                (*neighbour->patch)(0,3) =  (*firstAttribute.patch)(3,0);

                if (!UpdateVBOs(*neighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                neighbour = neighbour->neighbours[6];
                if(!neighbour){
                    break;
                }



                (*neighbour->patch)(0,0) = (*firstAttribute.patch)(3,0);

                if (!UpdateVBOs(*neighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                neighbour = neighbour->neighbours[0];

                if(!neighbour || neighbour == &firstAttribute)
                {
                    break;
                }


                (*neighbour->patch)(3,0) = (*firstAttribute.patch)(3,0);

                if (!UpdateVBOs(*neighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                neighbour = neighbour->neighbours[2];



            }


            //Up circle

            PatchAttributes* upNeighbour = firstAttribute.neighbours[6];




            while (upNeighbour) {
                (*upNeighbour->patch)(3,0) = (*firstAttribute.patch)(3,3);
                if (!UpdateVBOs(*upNeighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                upNeighbour = upNeighbour->neighbours[4];
                if(!upNeighbour){
                    break;
                }


                (*upNeighbour->patch)(0,0) =  (*firstAttribute.patch)(3,3);

                if (!UpdateVBOs(*upNeighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                upNeighbour = upNeighbour->neighbours[2];
                if(!upNeighbour){
                    break;
                }



                (*upNeighbour->patch)(0,3) = (*firstAttribute.patch)(3,3);

                if (!UpdateVBOs(*upNeighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                upNeighbour = upNeighbour->neighbours[0];

                if(!upNeighbour || upNeighbour == &firstAttribute)
                {
                    break;
                }


                (*upNeighbour->patch)(3,3) = (*firstAttribute.patch)(3,3);

                if (!UpdateVBOs(*upNeighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                upNeighbour = upNeighbour->neighbours[6];



            }


            //end here
        }
        else if (firstDirection == S && secondDirection == W)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute.patch)(3, i) = (*secondAttribute.patch)(i, 0) = 0.5 * ((*firstAttribute.patch)(2, i) + (*secondAttribute.patch)(i, 1));
            }

            firstAttribute.neighbours[4] = &secondAttribute;
            secondAttribute.neighbours[2] = &firstAttribute;
        }
        else if (firstDirection == S && secondDirection == S)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute.patch)(3, i) = (*secondAttribute.patch)(3, i) = 0.5 * ((*firstAttribute.patch)(2, i) + (*secondAttribute.patch)(2, i));
            }

            firstAttribute.neighbours[4] = &secondAttribute;
            secondAttribute.neighbours[4] = &firstAttribute;
        }
        else if (firstDirection == S && secondDirection == E)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute.patch)(3, i) = (*secondAttribute.patch)(i, 3) = 0.5 * ((*firstAttribute.patch)(2, i) + (*secondAttribute.patch)(i, 2));
            }

            firstAttribute.neighbours[4] = &secondAttribute;
            secondAttribute.neighbours[6] = &firstAttribute;
        }
        else if (firstDirection == E && secondDirection == N)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute.patch)(i, 3) = (*secondAttribute.patch)(0, i) = 0.5 * ((*firstAttribute.patch)(i, 2) + (*secondAttribute.patch)(1, i));
            }

            firstAttribute.neighbours[6] = &secondAttribute;
            secondAttribute.neighbours[0] = &firstAttribute;
        }
        else if (firstDirection == E && secondDirection == W)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute.patch)(i, 3) = (*secondAttribute.patch)(i, 0) = 0.5 * ((*firstAttribute.patch)(i, 2) + (*secondAttribute.patch)(i, 1));
            }

            firstAttribute.neighbours[6] = &secondAttribute;
            secondAttribute.neighbours[2] = &firstAttribute;


            // here


            //left circle
            PatchAttributes* neighbour = firstAttribute.neighbours[0];

            while (neighbour) {

                (*neighbour->patch)(3,3) = (*firstAttribute.patch)(0,3);

                if (!UpdateVBOs(*neighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                neighbour = neighbour->neighbours[6];
                if(!neighbour){
                    break;
                }

                (*neighbour->patch)(3,0) = (*firstAttribute.patch)(0,3);

                if (!UpdateVBOs(*neighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                neighbour = neighbour->neighbours[4];
                if(!neighbour){
                    break;
                }

                (*neighbour->patch)(0,0) = (*firstAttribute.patch)(0,3);

                if (!UpdateVBOs(*neighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                neighbour = neighbour->neighbours[2];
                if(!neighbour || neighbour == &firstAttribute  ){
                    break;
                }

                (*neighbour->patch)(0,3) = (*firstAttribute.patch)(0,3);

                if (!UpdateVBOs(*neighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                neighbour = neighbour->neighbours[0];
            }


//            //right circle



            PatchAttributes* rightNeighbour = firstAttribute.neighbours[4];

            while (rightNeighbour) {

                (*rightNeighbour->patch)(0,3) = (*firstAttribute.patch)(3,3);

                if (!UpdateVBOs(*rightNeighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                rightNeighbour = rightNeighbour->neighbours[6];
                if(!rightNeighbour){
                    break;
                }

                (*rightNeighbour->patch)(0,0) = (*firstAttribute.patch)(3,3);

                if (!UpdateVBOs(*rightNeighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                rightNeighbour = rightNeighbour->neighbours[0];
                if(!rightNeighbour){
                    break;
                }

                (*rightNeighbour->patch)(3,0) = (*firstAttribute.patch)(3,3);

                if (!UpdateVBOs(*rightNeighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                rightNeighbour = rightNeighbour->neighbours[6];
                if(!rightNeighbour || rightNeighbour == &firstAttribute  ){
                    break;
                }

                (*rightNeighbour->patch)(3,3) = (*firstAttribute.patch)(3,3);

                if (!UpdateVBOs(*rightNeighbour)){
                    cout<<"Error at UpdateVBO"<<endl;
                    return GL_FALSE;
                }

                rightNeighbour = rightNeighbour->neighbours[4];
            }


            //end here



        }
        else if (firstDirection == E && secondDirection == S)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute.patch)(i, 3) = (*secondAttribute.patch)(3, i) = 0.5 * ((*firstAttribute.patch)(i, 2) + (*secondAttribute.patch)(2, i));
            }

            firstAttribute.neighbours[6] = &secondAttribute;
            secondAttribute.neighbours[4] = &firstAttribute;
        }
        else if (firstDirection == E && secondDirection == E)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute.patch)(i, 3) = (*secondAttribute.patch)(i, 3) = 0.5 * ((*firstAttribute.patch)(i, 2) + (*secondAttribute.patch)(i, 2));
            }

            firstAttribute.neighbours[6] = &secondAttribute;
            secondAttribute.neighbours[6] = &firstAttribute;
        }

        return UpdateVBOs(firstAttribute) && UpdateVBOs(secondAttribute);
    }

    std::ostream& operator <<(std::ostream& lhs, const BicubicCompositeSurface3& surface)
    {
        lhs << surface._iso_line_count << endl;
        lhs << surface._attributes.size() << endl;

        for (auto it = surface._attributes.begin(); it != surface._attributes.end(); ++it)
        {
            lhs << *it->patch << endl;
        }

        for (auto it = surface._attributes.begin(); it != surface._attributes.end(); ++it)
        {
            for (GLuint i=0; i<8; ++i)
            {
                GLint index = surface.IndexOfAttribute(*(it->neighbours[i]));
                lhs << index << " ";
            }
            lhs << endl;
        }

        return lhs;
    }

    std::istream& operator >>(std::istream& lhs, BicubicCompositeSurface3& surface)
    {
        GLuint n;

        lhs >> surface._iso_line_count;
        lhs >> n;

        surface._attributes.clear();
        for (GLuint i=0; i<n; ++i)
        {
            BicubicCompositeSurface3::PatchAttributes attr;
            surface._attributes.push_back(attr);
            BicubicCompositeSurface3::PatchAttributes &attribute = surface._attributes.back();

            attribute.patch = new BicubicBezierPatch();
            lhs >> *attribute.patch;

            surface.UpdateVBOs(attribute);
        }

        GLint index;
        for (GLuint i=0; i<n; ++i)
        {
            for (GLuint j=0; j<8; ++j)
            {
                lhs >> index;
                if (index == -1)
                {
                    surface._attributes[i].neighbours[j] = nullptr;
                }
                else
                {
                    surface._attributes[i].neighbours[j] = &surface._attributes[index];
                }
            }
        }

        return lhs;
    }
}
