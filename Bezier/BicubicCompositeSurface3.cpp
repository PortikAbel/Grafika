#include "BicubicCompositeSurface3.h"
#include "../Core/Materials.h"
#include <Core/Exceptions.h>
#include <iostream>
#include <fstream>
#include <QRandomGenerator>

using namespace std;

namespace cagd
{
    // --------------------------------------------------------------------------------
    // PatchAttributess
    // --------------------------------------------------------------------------------

    GLvoid BicubicCompositeSurface3::_loadTextures()
    {
        for (GLuint i = 0; i < 10; i++)
        {
            QOpenGLTexture* tex = new QOpenGLTexture(QImage(QStringLiteral("Textures/periodic_texture_0%1.jpg").arg(i)));
            tex->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
            tex->setMagnificationFilter(QOpenGLTexture::Linear);
            _textures.push_back(tex);
        }
    }

    BicubicCompositeSurface3::PatchAttributes::PatchAttributes():
        patch(nullptr), image(nullptr), neighbours(8, nullptr), u_lines(nullptr), v_lines(nullptr)
    {
        matInd = QRandomGenerator::global()->bounded(4);
        texInd = QRandomGenerator::global()->bounded(4);
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

        matInd = attribute.matInd;
        texInd = attribute.texInd;

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
                symmetricColumn = 3 - column;
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
                symmetricRow = 3 - row;
                symmetricColumn = column;
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

            return;
        }

        if (direction == NW)
        {
            if (neighbours[NW]->neighbours[NE] == this)
            {
                symmetricRow = column;
                symmetricColumn = 3 - row;
            }
            else if (neighbours[NW]->neighbours[NW] == this)
            {
                symmetricRow = row;
                symmetricColumn = column;
            }
            else if (neighbours[NW]->neighbours[SW] == this)
            {
                symmetricRow = 3 - column;
                symmetricColumn = row;
            }
            else if (neighbours[NW]->neighbours[SE] == this)
            {
                symmetricRow = 3 - row;
                symmetricColumn = 3 - column;
            }

            return;
        }
        if (direction == NE)
        {
            if (neighbours[NE]->neighbours[NE] == this)
            {
                symmetricRow = row;
                symmetricColumn = column;
            }
            else if (neighbours[NE]->neighbours[NW] == this)
            {
                symmetricRow = 3 - column;
                symmetricColumn = row;
            }
            else if (neighbours[NE]->neighbours[SW] == this)
            {
                symmetricRow = 3 - row;
                symmetricColumn = 3 - column;
            }
            else if (neighbours[NE]->neighbours[SE] == this)
            {
                symmetricRow = column;
                symmetricColumn = 3 - row;
            }

            return;
        }
        if (direction == SE)
        {
            if (neighbours[SE]->neighbours[NE] == this)
            {
                symmetricRow = 3 - column;
                symmetricColumn = row;
            }
            else if (neighbours[SE]->neighbours[NW] == this)
            {
                symmetricRow = 3 - row;
                symmetricColumn = 3 - column;
            }
            else if (neighbours[SE]->neighbours[SW] == this)
            {
                symmetricRow = column;
                symmetricColumn = 3 - row;
            }
            else if (neighbours[SE]->neighbours[SE] == this)
            {
                symmetricRow = row;
                symmetricColumn = column;
            }

            return;
        }
        if (direction == SW)
        {
            if (neighbours[SW]->neighbours[NE] == this)
            {
                symmetricRow = 3 - row;
                symmetricColumn = 3 - column;
            }
            else if (neighbours[SW]->neighbours[NW] == this)
            {
                symmetricRow = column;
                symmetricColumn = 3 - row;
            }
            else if (neighbours[SW]->neighbours[SW] == this)
            {
                symmetricRow = row;
                symmetricColumn = column;
            }
            else if (neighbours[SW]->neighbours[SE] == this)
            {
                symmetricRow = 3 - column;
                symmetricColumn = row;
            }
        }
    }

    // --------------------------------------------------------------------------------

    BicubicCompositeSurface3::BicubicCompositeSurface3(GLuint patchCount):
            _u_iso_line_count(50), _v_iso_line_count(50)
    {
        _loadTextures();
        for (GLuint i = 0; i < patchCount; i++)
        {
            PatchAttributes* newAttr = new PatchAttributes;
            _attributes.push_back(newAttr);
            (*_attributes[i]).patch = InitializePatch();
            UpdateVBOs(_attributes[i]);
        }
    }

    GLvoid BicubicCompositeSurface3::UpdateUIsoLines(GLuint iso_line_count)
    {
        _u_iso_line_count = iso_line_count;
        for (GLuint i = 0; i < _attributes.size(); i++)
        {
            PatchAttributes *attribute = _attributes[i];
            UpdateVBOs(attribute);
        }
    }

    GLvoid BicubicCompositeSurface3::UpdateVIsoLines(GLuint iso_line_count)
    {
        _v_iso_line_count = iso_line_count;
        for (GLuint i = 0; i < _attributes.size(); i++)
        {
            PatchAttributes *attribute = _attributes[i];
            UpdateVBOs(attribute);
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

    GLboolean BicubicCompositeSurface3::UpdateVBOs(PatchAttributes *attribute)
    {
        if (!attribute->patch->UpdateVertexBufferObjectsOfData())
        {
            throw Exception("Could not update the VBO of data of the patch!");
        }

        attribute->u_lines = attribute->patch->GenerateUIsoparametricLines(_u_iso_line_count, 1, 30);
        attribute->v_lines = attribute->patch->GenerateVIsoparametricLines(_v_iso_line_count, 1, 30);
        for(GLuint i=0; i<attribute->u_lines->GetColumnCount(); ++i)
        {
            if((*attribute->u_lines)[i])
            {
                (*attribute->u_lines)[i]->UpdateVertexBufferObjects();
            }
        }
        for(GLuint i=0; i<attribute->v_lines->GetColumnCount(); ++i)
        {
            if((*attribute->v_lines)[i])
            {
                (*attribute->v_lines)[i]->UpdateVertexBufferObjects();
            }
        }

        attribute->image = attribute->patch->GenerateImage(_u_iso_line_count, _v_iso_line_count);
        if (!attribute->image)
        {
            throw Exception("Could not generate the image of patch!");
        }
        if (!attribute->image->UpdateVertexBufferObjects())
        {
            throw Exception("Could not update the VBO of patch image");
        }

        return GL_TRUE;
    }

    GLint BicubicCompositeSurface3::IndexOfAttribute(const PatchAttributes *attribute) const
    {
        for (GLuint i=0; i<_attributes.size(); ++i)
        {
            if (_attributes[i] == attribute)
            {
                return i;
            }
        }

        return -1;
    }

    GLboolean BicubicCompositeSurface3::RenderAllPatchesWithMaterials()
    {
        for (auto it = _attributes.begin(); it != _attributes.end(); ++it)
        {
            if ((*it)->image)
            {
                glEnable(GL_LIGHTING);
                glEnable(GL_NORMALIZE);

                    _materials[(*it)->matInd].Apply();
                    (*it)->image->Render();

                glDisable(GL_LIGHTING);
                glDisable(GL_NORMALIZE);
            }
        }

        return GL_TRUE;
    }

    GLboolean BicubicCompositeSurface3::RenderAllPatchesWithTextures()
    {
        for (auto it = _attributes.begin(); it != _attributes.end(); ++it)
        {
            if ((*it)->image)
            {
                glEnable(GL_TEXTURE_2D);
                _textures[(*it)->texInd]->bind();
                (*it)->image->Render();
                _textures[(*it)->texInd]->release();
                glDisable(GL_TEXTURE_2D);
            }
        }

        return GL_TRUE;
    }

    GLboolean BicubicCompositeSurface3::RenderAllPatchesIsoU() const
    {
        glPointSize(6.0);

        for (auto it = _attributes.begin(); it != _attributes.end(); ++it)
        {
            if ((*it)->u_lines)
            {
                glColor3f(1.0f, 0.0f, 0.0f); // red for iso lines

                for(GLuint i=0; i<(*it)->u_lines->GetColumnCount(); ++i)
                {
                    (*((*it)->u_lines))[i]->RenderDerivatives(0, GL_LINE_STRIP);
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
            if ((*it)->v_lines)
            {
                glColor3f(1.0f, 0.0f, 0.0f); // red for iso lines

                for(GLuint i=0; i<(*it)->v_lines->GetColumnCount(); ++i)
                {
                    (*((*it)->v_lines))[i]->RenderDerivatives(0, GL_LINE_STRIP);
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
            if ((*it)->u_lines)
            {
                glColor3f(0.0f, 0.5f, 0.0f); // green for first derivatives

                for(GLuint i=0; i<(*it)->u_lines->GetColumnCount(); ++i)
                {
                    (*((*it)->u_lines))[i]->RenderDerivatives(1, GL_POINTS);
                    (*((*it)->u_lines))[i]->RenderDerivatives(1, GL_LINES);
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
            if ((*it)->v_lines)
            {
                glColor3f(0.0f, 0.5f, 0.0f); // green for first derivatives

                for(GLuint i=0; i<(*it)->v_lines->GetColumnCount(); ++i)
                {
                    (*((*it)->v_lines))[i]->RenderDerivatives(1, GL_POINTS);
                    (*((*it)->v_lines))[i]->RenderDerivatives(1, GL_LINES);
                }
            }
        }

        glPointSize(1.0);
        return GL_TRUE;
    }

    GLboolean BicubicCompositeSurface3::RenderAllPatchesNormal() const
    {
        glPointSize(6.0);

        for (auto it = _attributes.begin(); it != _attributes.end(); ++it)
        {
            (*it)->image->RenderNormals();
        }

        glPointSize(1.0);
        return GL_TRUE;
    }

    GLboolean BicubicCompositeSurface3::RenderAllPatchesData(GLuint selectedPatchInd, GLuint selectedRowInd, GLuint selectedColInd) const
    {
        glColor3f(0.7f, 1.0f, 0.0f); // neongreen for control net

        glPointSize(10.0f);
        glBegin(GL_POINTS);
            glVertex3dv(&(*_attributes[selectedPatchInd]->patch)(selectedRowInd, selectedColInd)[0]);
        glEnd();

        glPointSize(6.0);

        for (auto it = _attributes.begin(); it != _attributes.end(); ++it)
        {
            if ((*it)->patch)
            {
                (*it)->patch->RenderData();
            }
        }

        glPointSize(1.0);
        return GL_TRUE;
    }

    GLboolean BicubicCompositeSurface3::RenderHighlightedPatches(GLuint patchInd1, GLuint patchInd2) const
    {
        if (!_attributes[patchInd1]->image || !_attributes[patchInd2]->image)
        {
            return GL_FALSE;
        }

        glEnable(GL_LIGHTING);
        glEnable(GL_NORMALIZE);

            MatFBGold.Apply();
            _attributes[patchInd1]->image->Render();
            MatFBSilver.Apply();
            _attributes[patchInd2]->image->Render();

        glDisable(GL_LIGHTING);
        glDisable(GL_NORMALIZE);

        return GL_TRUE;
    }

    int BicubicCompositeSurface3::GetPatchCount()
    {
        return _attributes.size();
    }

    GLuint BicubicCompositeSurface3::GetUIsoLineCount()
    {
        return _u_iso_line_count;
    }

    GLuint BicubicCompositeSurface3::GetVIsoLineCount()
    {
        return _v_iso_line_count;
    }

    GLboolean BicubicCompositeSurface3::GetDataPointValues(const GLuint patchIndex, const GLuint row, const GLuint column, DCoordinate3 &position)
    {
        (*_attributes[patchIndex]->patch).GetData(row,column,position);
        return GL_TRUE;
    }

    GLboolean BicubicCompositeSurface3::GetDataPointValues(const GLuint patchIndex, const GLuint row, const GLuint column, GLdouble &x, GLdouble &y, GLdouble &z)
    {
        (*_attributes[patchIndex]->patch).GetData(row,column,x,y,z);
        return GL_TRUE;
    }

    GLboolean BicubicCompositeSurface3::ChangeMaterial(GLuint patchInd, GLuint matInd)
    {
        if (patchInd >= _attributes.size())
        {
            return GL_FALSE;
        }
        if (matInd >= _materials.size())
        {
            return GL_FALSE;
        }
        _attributes[patchInd]->matInd = matInd;

        return GL_TRUE;
    }

    GLboolean BicubicCompositeSurface3::ChangeTexture(GLuint patchInd, GLuint texInd)
    {
        if (patchInd >= _attributes.size())
        {
            return GL_FALSE;
        }
        if (texInd >= _textures.size())
        {
            return GL_FALSE;
        }
        _attributes[patchInd]->texInd = texInd;

        return GL_TRUE;
    }

    GLuint BicubicCompositeSurface3::GetMatInd(GLuint patchInd)
    {
        return _attributes[patchInd]->matInd;
    }

    GLuint BicubicCompositeSurface3::GetTexInd(GLuint patchInd)
    {
        return _attributes[patchInd]->texInd;
    }

    GLboolean BicubicCompositeSurface3::InsertNewPatch()
    {
        PatchAttributes* attribute = new PatchAttributes;
        _attributes.push_back(attribute);

        try
        {
            attribute->patch = InitializePatch();
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

        for (auto it = _attributes.begin(); it != _attributes.end(); it++)
        {
            (*it)->updated = false;
        }

        std::vector<PointUpdate> points;
        points.push_back(PointUpdate(row, column, position));
        return UpdatePatch(_attributes[patchIndex], points);
    }

    GLboolean BicubicCompositeSurface3::UpdatePatch(PatchAttributes *attribute, std::vector<PointUpdate> points)
    {
        PatchAttributes *neighbour;
        std::vector<PointUpdate> neighbour_points[8];
        GLuint i, j;
        attribute->updated = true;

        for (auto it = points.begin(); it != points.end(); it++)
        {
            GLuint row = it->row;
            GLuint column = it->col;
            DCoordinate3 position = it->pos;

            DCoordinate3 diference = position - (*attribute->patch)(row, column);
            (*attribute->patch)(row, column) = position;

            if (row == 0)
            {
                if (attribute->neighbours[N] && !attribute->neighbours[N]->updated)
                {
                    neighbour = attribute->neighbours[N];
                    attribute->_GetSymmetricPointIndexes(row, column, N, i, j);
                    neighbour_points[N].push_back(PointUpdate(i, j, position));
                    attribute->_GetSymmetricPointIndexes(1, column, N, i, j);
                    neighbour_points[N].push_back(PointUpdate(i, j, 2 * position - (*attribute->patch)(1, column)));
                }
                if (column < 2 && attribute->neighbours[NW] && !attribute->neighbours[NW]->updated)
                {
                    neighbour = attribute->neighbours[NW];
                    attribute->_GetSymmetricPointIndexes(row, column, NW, i, j);
                    if (column == 0)
                    {
                        neighbour_points[NW].push_back(PointUpdate(i, j, position));
                        attribute->_GetSymmetricPointIndexes(1, 0, NW, i, j);
                        neighbour_points[NW].push_back(PointUpdate(i, j, 2 * position - (*attribute->patch)(1, 0)));
                        attribute->_GetSymmetricPointIndexes(0, 1, NW, i, j);
                        neighbour_points[NW].push_back(PointUpdate(i, j, 2 * position - (*attribute->patch)(0, 1)));
                    }
                    if (column == 1)
                    {
                        neighbour_points[NW].push_back(PointUpdate(i, j, 2 * (*attribute->patch)(0, 0) - position));
                    }
                }
                if (column > 1 && attribute->neighbours[NE] && !attribute->neighbours[NE]->updated)
                {
                    neighbour = attribute->neighbours[NE];
                    attribute->_GetSymmetricPointIndexes(row, column, NE, i, j);
                    if (column == 3)
                    {
                        neighbour_points[NE].push_back(PointUpdate(i, j, position));
                        attribute->_GetSymmetricPointIndexes(0, 2, NE, i, j);
                        neighbour_points[NE].push_back(PointUpdate(i, j, 2 * position - (*attribute->patch)(0, 2)));
                        attribute->_GetSymmetricPointIndexes(1, 3, NE, i, j);
                        neighbour_points[NE].push_back(PointUpdate(i, j, 2 * position - (*attribute->patch)(1, 3)));
                    }
                    if (column == 2)
                    {
                        neighbour_points[NE].push_back(PointUpdate(i, j, 2 * (*attribute->patch)(0, 3) - position));
                    }
                }
            }
            if (row == 1)
            {
                if (attribute->neighbours[N] && !attribute->neighbours[N]->updated)
                {
                    neighbour = attribute->neighbours[N];
                    attribute->_GetSymmetricPointIndexes(row, column, N, i, j);
                    neighbour_points[N].push_back(PointUpdate(i, j, 2 * (*attribute->patch)(0, column) - position));
                }
                if (column == 0 && attribute->neighbours[NW] && !attribute->neighbours[NW]->updated)
                {
                    neighbour = attribute->neighbours[NW];
                    attribute->_GetSymmetricPointIndexes(row, column, NW, i, j);
                    neighbour_points[N].push_back(PointUpdate(i, j, 2 * (*attribute->patch)(0, 0) - position));
                }
                if (column == 3 && attribute->neighbours[NE] && !attribute->neighbours[NE]->updated)
                {
                    neighbour = attribute->neighbours[NE];
                    attribute->_GetSymmetricPointIndexes(row, column, NE, i, j);
                    neighbour_points[N].push_back(PointUpdate(i, j, 2 * (*attribute->patch)(0, 3) - position));
                }
            }
            if (row == 2)
            {
                if (attribute->neighbours[S] && !attribute->neighbours[S]->updated)
                {
                    neighbour = attribute->neighbours[S];
                    attribute->_GetSymmetricPointIndexes(row, column, S, i, j);
                    neighbour_points[S].push_back(PointUpdate(i, j, 2 * (*attribute->patch)(3, column) - position));
                }
                if (column == 0 && attribute->neighbours[SW] && !attribute->neighbours[SW]->updated)
                {
                    neighbour = attribute->neighbours[SW];
                    attribute->_GetSymmetricPointIndexes(row, column, SW, i, j);
                    neighbour_points[SW].push_back(PointUpdate(i, j, 2 * (*attribute->patch)(3, 0) - position));
                }
                if (column == 3 && attribute->neighbours[SE] && !attribute->neighbours[SE]->updated)
                {
                    neighbour = attribute->neighbours[SE];
                    attribute->_GetSymmetricPointIndexes(row, column, SE, i, j);
                    neighbour_points[SE].push_back(PointUpdate(i, j, 2 * (*attribute->patch)(3, 3) - position));
                }
            }
            if (row == 3)
            {
                if (attribute->neighbours[S] && !attribute->neighbours[S]->updated)
                {
                    neighbour = attribute->neighbours[S];
                    attribute->_GetSymmetricPointIndexes(row, column, S, i, j);
                    neighbour_points[S].push_back(PointUpdate(i, j, position));
                    attribute->_GetSymmetricPointIndexes(2, column, S, i, j);
                    neighbour_points[S].push_back(PointUpdate(i, j, 2 * position - (*attribute->patch)(2, column)));
                }
                if (column < 2 && attribute->neighbours[SW] && !attribute->neighbours[SW]->updated)
                {
                    neighbour = attribute->neighbours[SW];
                    attribute->_GetSymmetricPointIndexes(row, column, SW, i, j);
                    if (column == 0)
                    {
                        neighbour_points[SW].push_back(PointUpdate(i, j, position));
                        attribute->_GetSymmetricPointIndexes(2, 0, SW, i, j);
                        neighbour_points[SW].push_back(PointUpdate(i, j, 2 * position - (*attribute->patch)(2, 0)));
                        attribute->_GetSymmetricPointIndexes(3, 1, SW, i, j);
                        neighbour_points[SW].push_back(PointUpdate(i, j, 2 * position - (*attribute->patch)(3, 1)));
                    }
                    if (column == 1)
                    {
                        neighbour_points[SW].push_back(PointUpdate(i, j, 2 * (*attribute->patch)(3, 0) - position));
                    }
                }
                if (column > 1 && attribute->neighbours[SE] && !attribute->neighbours[SE]->updated)
                {
                    neighbour = attribute->neighbours[SE];
                    attribute->_GetSymmetricPointIndexes(row, column, SE, i, j);
                    if (column == 3)
                    {
                        neighbour_points[SE].push_back(PointUpdate(i, j, position));
                        attribute->_GetSymmetricPointIndexes(2, 3, SE, i, j);
                        neighbour_points[SE].push_back(PointUpdate(i, j, 2 * position - (*attribute->patch)(2, 3)));
                        attribute->_GetSymmetricPointIndexes(3, 2, SE, i, j);
                        neighbour_points[SE].push_back(PointUpdate(i, j, 2 * position - (*attribute->patch)(3, 2)));
                    }
                    if (column == 2)
                    {
                        neighbour_points[SE].push_back(PointUpdate(i, j, 2 * (*attribute->patch)(3, 3) - position));
                    }
                }
            }

            if (column == 0 && attribute->neighbours[W] && !attribute->neighbours[W]->updated)
            {
                neighbour = attribute->neighbours[W];
                attribute->_GetSymmetricPointIndexes(row, column, W, i, j);
                neighbour_points[W].push_back(PointUpdate(i, j, position));
                attribute->_GetSymmetricPointIndexes(row, 1, W, i, j);
                neighbour_points[W].push_back(PointUpdate(i, j, 2 * position - (*attribute->patch)(row, 1)));
            }
            if (column == 1 && attribute->neighbours[W] && !attribute->neighbours[W]->updated)
            {
                neighbour = attribute->neighbours[W];
                attribute->_GetSymmetricPointIndexes(row, column, W, i, j);
                neighbour_points[W].push_back(PointUpdate(i, j, 2 * (*attribute->patch)(row, 0) - position));
            }
            if (column == 2 && attribute->neighbours[E] && !attribute->neighbours[E]->updated)
            {
                neighbour = attribute->neighbours[E];
                attribute->_GetSymmetricPointIndexes(row, column, E, i, j);
                neighbour_points[E].push_back(PointUpdate(i, j, 2 * (*attribute->patch)(row, 3) - position));
            }
            if (column == 3 && attribute->neighbours[E] && !attribute->neighbours[E]->updated)
            {
                neighbour = attribute->neighbours[E];
                attribute->_GetSymmetricPointIndexes(row, column, E, i, j);
                neighbour_points[E].push_back(PointUpdate(i, j, position));
                attribute->_GetSymmetricPointIndexes(row, 2, E, i, j);
                neighbour_points[E].push_back(PointUpdate(i, j, 2 * position - (*attribute->patch)(row, 2)));
            }
        }

        for (i = 0; i < 8; i++)
        {
            if (attribute->neighbours[i] && neighbour_points[i].size() > 0)
            {
                UpdatePatch(attribute->neighbours[i], neighbour_points[i]);
            }
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

        PatchAttributes *attribute = _attributes[patchIndex];

        for (GLuint i=0; i<=3; ++i)
        {
            for (GLuint j=0; j<=3; ++j)
            {
                (*attribute->patch)(i, j) += difference;
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

        PatchAttributes *attribute = _attributes[patchIndex];

        if (attribute->neighbours[direction])
        {
            cout << "The patch arlready has a neighbor in the given direction!" << endl;
            return GL_FALSE;
        }

        PatchAttributes* newAttribute = new PatchAttributes;
        _attributes.push_back(newAttribute);

        newAttribute->patch = new BicubicBezierPatch();
        BicubicBezierPatch* patch = newAttribute->patch;

        if (direction == N)
        {
            for(GLuint i=0; i<=3; ++i)
            {
               (*patch)(3, i) = (*attribute->patch)(0, i);
               (*patch)(2, i) = 2 * (*attribute->patch)(0, i) - (*attribute->patch)(1, i);
               (*patch)(1, i) = 2 * (*patch)(2, i) - (*patch)(3, i);
               (*patch)(0, i) = 2 * (*patch)(1, i) - (*patch)(2, i);
            }
            attribute->neighbours[N] = newAttribute;
            newAttribute->neighbours[S] = attribute;
        }
        else if (direction == NE)
        {
            for(int i=0; i<=3; i++) {
                for (int j=0; j<=3;j++) {
                    (*patch)(i,j) = 2*(*attribute->patch)(0,3) - (*attribute->patch)(3-i,3-j);
                }
            }

            attribute->neighbours[NE] = newAttribute;
            newAttribute->neighbours[SW] = attribute;
        }
        else if (direction == NW)
        {
            for(int i=0; i<=3; i++) {
                for (int j=0; j<=3;j++) {
                    (*patch)(i,j) = 2*(*attribute->patch)(0,0) - (*attribute->patch)(3-i,3-j);
                }
            }

            attribute->neighbours[NW] = newAttribute;
            newAttribute->neighbours[SE] = attribute;
        }
        else   if (direction == W)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*patch)(i, 3) = (*attribute->patch)(i, 0);
                (*patch)(i, 2) = 2 * (*attribute->patch)(i, 0) - (*attribute->patch)(i, 1);
                (*patch)(i, 1) = 2 * (*patch)(i, 2) - (*patch)(i, 3);
                (*patch)(i, 0) = 2 * (*patch)(i, 1) - (*patch)(i, 2);

            }
            attribute->neighbours[W] = newAttribute;
            newAttribute->neighbours[E] = attribute;
        }
        else   if (direction == S)
        {
        for(GLuint i=0; i<=3; ++i)
        {
            (*patch)(0, i) = (*attribute->patch)(3, i);
            (*patch)(1, i) = 2 * (*attribute->patch)(3, i) - (*attribute->patch)(2, i);
            (*patch)(2, i) = 2 * (*patch)(1, i) - (*patch)(0, i);
            (*patch)(3, i) = 2 * (*patch)(2, i) - (*patch)(1, i);
        }
        attribute->neighbours[S] = newAttribute;
        newAttribute->neighbours[N] = attribute;
        }
        else if (direction == SE)
        {

            for(int i=0; i<=3; i++) {
                for (int j=0; j<=3;j++) {
                    (*patch)(i,j) = 2*(*attribute->patch)(3,3) - (*attribute->patch)(3-i,3-j);
                }
            }

            attribute->neighbours[SE] = newAttribute;
            newAttribute->neighbours[NW] = attribute;
        }
        else if (direction == SW)
        {

            for(int i=0; i<=3; i++) {
                for (int j=0; j<=3;j++) {
                    (*patch)(i,j) = 2*(*attribute->patch)(3,0) - (*attribute->patch)(3-i,3-j);
                }
            }

           attribute->neighbours[SW] = newAttribute;
           newAttribute->neighbours[NE] = attribute;

        }
        else   if (direction == E)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*patch)(i, 0) = (*attribute->patch)(i, 3);
                (*patch)(i, 1) = 2 * (*attribute->patch)(i, 3) - (*attribute->patch)(i, 2);
                (*patch)(i, 2) = 2 * (*patch)(i, 1) - (*patch)(i, 0);
                (*patch)(i, 3) = 2 * (*patch)(i, 2) - (*patch)(i, 1);

            }
            attribute->neighbours[E] = newAttribute;
            newAttribute->neighbours[W] = attribute;
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

        PatchAttributes *firstAttribute = _attributes[firstPatchIndex];
        PatchAttributes *secondAttribute = _attributes[secondPatchIndex];

        if (firstAttribute->neighbours[firstDirection] || secondAttribute->neighbours[secondDirection])
        {
            cout << "One of the patches arlready has a neighbor in the given direction!" << endl;
            return GL_FALSE;
        }

        PatchAttributes *newAttribute = new PatchAttributes;
        _attributes.push_back(newAttribute);

        newAttribute->patch = new BicubicBezierPatch();

        //---------------------------------------------------------
        //                  FIRST DIRECTION
        //---------------------------------------------------------
        switch (firstDirection)
        {
        case N:
            for (GLuint i = 0; i <= 3; ++i)
            {
                (*newAttribute->patch)(0, i) = (*firstAttribute->patch)(0, i);
                (*newAttribute->patch)(1, i) = 2 * (*firstAttribute->patch)(0, i) - (*firstAttribute->patch)(1, i);
            }
            newAttribute->neighbours[N] = firstAttribute;
            break;
        case W:
            for (GLuint i = 0; i <= 3; ++i)
            {
                (*newAttribute->patch)(0, i) = (*firstAttribute->patch)(i, 0);
                (*newAttribute->patch)(1, i) = 2 * (*firstAttribute->patch)(i, 0) - (*firstAttribute->patch)(i, 1);
            }
            newAttribute->neighbours[N] = firstAttribute;
            break;
        case S:
            for (GLuint i = 0; i <= 3; ++i)
            {
                (*newAttribute->patch)(0,i) = (*firstAttribute->patch)(3,i);
                (*newAttribute->patch)(1,i) = 2 * (*firstAttribute->patch)(3,i) - (*firstAttribute->patch)(2,i);
            }
            newAttribute->neighbours[N] = firstAttribute;
            break;
        case E:
            for (GLuint i = 0; i <= 3; ++i)
            {
                (*newAttribute->patch)(0,i) = (*firstAttribute->patch)(i,3);
                (*newAttribute->patch)(1,i) = 2 * (*firstAttribute->patch)(i,3) - (*firstAttribute->patch)(i,2);
            }
            newAttribute->neighbours[N] = firstAttribute;
            break;
        case NW:
            for (GLuint i = 0; i <= 3; ++i)
            {
                (*newAttribute->patch)(0,i) = 2 * (*firstAttribute->patch)(0,0) - (*firstAttribute->patch)(0,i);
                (*newAttribute->patch)(1,i) = 2 * (*firstAttribute->patch)(0,0) - (*firstAttribute->patch)(1,i);
            }
            newAttribute->neighbours[NW] = firstAttribute;
            break;
        case NE:
            for (GLuint i = 0; i <= 3; ++i)
            {
                (*newAttribute->patch)(0,i) = 2 * (*firstAttribute->patch)(0,3) - (*firstAttribute->patch)(0,i);
                (*newAttribute->patch)(1,i) = 2 * (*firstAttribute->patch)(0,3) - (*firstAttribute->patch)(1,i);
            }
            newAttribute->neighbours[NW] = firstAttribute;
            break;
        case SW:
            for (GLuint i = 0; i <= 3; ++i)
            {
                (*newAttribute->patch)(0,i) = 2 * (*firstAttribute->patch)(3,0) - (*firstAttribute->patch)(3,i);
                (*newAttribute->patch)(1,i) = 2 * (*firstAttribute->patch)(3,0) - (*firstAttribute->patch)(2,i);
            }
            newAttribute->neighbours[NW] = firstAttribute;
            break;
        case SE:
            for (GLuint i = 0; i <= 3; ++i)
            {
                (*newAttribute->patch)(0,i) = 2 * (*firstAttribute->patch)(3,3) - (*firstAttribute->patch)(3,i);
                (*newAttribute->patch)(1,i) = 2 * (*firstAttribute->patch)(3,3) - (*firstAttribute->patch)(2,i);
            }
            newAttribute->neighbours[NW] = firstAttribute;
            break;
        }
        firstAttribute->neighbours[firstDirection] = newAttribute;

        //---------------------------------------------------------
        //                  SECOND DIRECTION
        //---------------------------------------------------------
        switch(secondDirection)
        {
        case N:
            for (GLuint i=0; i<=3; ++i)
            {
                (*newAttribute->patch)(3, i) = (*secondAttribute->patch)(0, i);
                (*newAttribute->patch)(2, i) = 2 * (*secondAttribute->patch)(0, i) - (*secondAttribute->patch)(1, i);
            }
            newAttribute->neighbours[S] = secondAttribute;
            break;
        case W:
            for (GLuint i=0; i<=3; ++i)
            {
                (*newAttribute->patch)(3, i) = (*secondAttribute->patch)(i, 0);
                (*newAttribute->patch)(2, i) = 2 * (*secondAttribute->patch)(i, 0) - (*secondAttribute->patch)(i, 1);
            }
            newAttribute->neighbours[S] = secondAttribute;
            break;
        case S:
            for (GLuint i = 0; i < 4; ++i)
            {
                (*newAttribute->patch)(3,i) = (*secondAttribute->patch)(3,i);
                (*newAttribute->patch)(2,i) = 2 * (*secondAttribute->patch)(3,i) - (*secondAttribute->patch)(2,i);
            }
            newAttribute->neighbours[S] = secondAttribute;
            break;
        case E:
            for (GLuint i = 0; i < 4; ++i)
            {
                (*newAttribute->patch)(3,i) = (*secondAttribute->patch)(i,3);
                (*newAttribute->patch)(2,i) = 2 * (*secondAttribute->patch)(3,i) - (*secondAttribute->patch)(i,2);
            }
            newAttribute->neighbours[S] = secondAttribute;
            break;
        case NW:
            for (GLuint i = 0; i <= 3; ++i)
            {
                (*newAttribute->patch)(3,i) = 2 * (*secondAttribute->patch)(0,0) - (*secondAttribute->patch)(0,i);
                (*newAttribute->patch)(2,i) = 2 * (*secondAttribute->patch)(0,0) - (*secondAttribute->patch)(1,i);
            }
            newAttribute->neighbours[SE] = secondAttribute;
            break;
        case NE:
            for (GLuint i = 0; i <= 3; ++i)
            {
                (*newAttribute->patch)(3,i) = 2 * (*secondAttribute->patch)(0,3) - (*secondAttribute->patch)(0,i);
                (*newAttribute->patch)(2,i) = 2 * (*secondAttribute->patch)(0,3) - (*secondAttribute->patch)(1,i);
            }
            newAttribute->neighbours[SE] = secondAttribute;
            break;
        case SW:
            for (GLuint i = 0; i <= 3; ++i)
            {
                (*newAttribute->patch)(3,i) = 2 * (*secondAttribute->patch)(3,0) - (*secondAttribute->patch)(3,i);
                (*newAttribute->patch)(2,i) = 2 * (*secondAttribute->patch)(3,0) - (*secondAttribute->patch)(2,i);
            }
            newAttribute->neighbours[SE] = secondAttribute;
            break;
        case SE:
            for (GLuint i = 0; i <= 3; ++i)
            {
                (*newAttribute->patch)(3,i) = 2 * (*secondAttribute->patch)(3,3) - (*secondAttribute->patch)(3,i);
                (*newAttribute->patch)(2,i) = 2 * (*secondAttribute->patch)(3,3) - (*secondAttribute->patch)(2,i);
            }
            newAttribute->neighbours[SE] = secondAttribute;
            break;
        }
        secondAttribute->neighbours[secondDirection] = newAttribute;

        return UpdateVBOs(newAttribute);
    }

    GLboolean BicubicCompositeSurface3::MergeExistingPatches(const GLuint &firstPatchIndex, Direction firstDirection, const GLuint &secondPatchIndex, Direction secondDirection)
    {
        if (firstPatchIndex >= _attributes.size() || secondPatchIndex >= _attributes.size())
        {
            cout << "Invalid patch index(es)!" << endl;
            return GL_FALSE;
        }

        PatchAttributes *firstAttribute = _attributes[firstPatchIndex];
        PatchAttributes *secondAttribute = _attributes[secondPatchIndex];

        if (firstAttribute->neighbours[firstDirection] || secondAttribute->neighbours[secondDirection])
        {
            cout << "One of the patches arlready has a neighbor in the given direction!" << endl;
            return GL_FALSE;
        }

        firstAttribute->neighbours[firstDirection] = secondAttribute;
        secondAttribute->neighbours[secondDirection] = firstAttribute;

        if (firstDirection == N && secondDirection == N)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute->patch)(0, i) = (*secondAttribute->patch)(0, i) = 0.5 * ((*firstAttribute->patch)(1, i) + (*secondAttribute->patch)(1, i));
            }
        }
        else if (firstDirection == N && secondDirection == W)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute->patch)(0, i) = (*secondAttribute->patch)(i, 0) = 0.5 * ((*firstAttribute->patch)(1, i) + (*secondAttribute->patch)(i, 1));
            }
        }
        else if (firstDirection == N && secondDirection == S)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute->patch)(0, i) = (*secondAttribute->patch)(3, i) = 0.5 * ((*firstAttribute->patch)(1, i) + (*secondAttribute->patch)(2, i));
            }
        }
        else if (firstDirection == N && secondDirection == E)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute->patch)(0, i) = (*secondAttribute->patch)(i, 3) = 0.5 * ((*firstAttribute->patch)(1, i) + (*secondAttribute->patch)(i, 2));
            }
        }
        else if (firstDirection == W && secondDirection == N)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute->patch)(i, 0) = (*secondAttribute->patch)(0, i) = 0.5 * ((*firstAttribute->patch)(i, 1) + (*secondAttribute->patch)(1, i));
            }
        }
        else if (firstDirection == W && secondDirection == W)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute->patch)(i, 0) = (*secondAttribute->patch)(i, 0) = 0.5 * ((*firstAttribute->patch)(i, 1) + (*secondAttribute->patch)(i, 1));
            }
        }
        else if (firstDirection == W && secondDirection == S)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute->patch)(i, 0) = (*secondAttribute->patch)(3, i) = 0.5 * ((*firstAttribute->patch)(i, 1) + (*secondAttribute->patch)(2, i));
            }
        }
        else if (firstDirection == W && secondDirection == E)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute->patch)(i, 0) = (*secondAttribute->patch)(i, 3) = 0.5 * ((*firstAttribute->patch)(i, 1) + (*secondAttribute->patch)(i, 2));
            }
        }
        else if (firstDirection == S && secondDirection == N)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute->patch)(3, i) = (*secondAttribute->patch)(0, i) = 0.5 * ((*firstAttribute->patch)(2, i) + (*secondAttribute->patch)(1, i));
            }
        }
        else if (firstDirection == S && secondDirection == W)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute->patch)(3, i) = (*secondAttribute->patch)(i, 0) = 0.5 * ((*firstAttribute->patch)(2, i) + (*secondAttribute->patch)(i, 1));
            }
        }
        else if (firstDirection == S && secondDirection == S)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute->patch)(3, i) = (*secondAttribute->patch)(3, i) = 0.5 * ((*firstAttribute->patch)(2, i) + (*secondAttribute->patch)(2, i));
            }
        }
        else if (firstDirection == S && secondDirection == E)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute->patch)(3, i) = (*secondAttribute->patch)(i, 3) = 0.5 * ((*firstAttribute->patch)(2, i) + (*secondAttribute->patch)(i, 2));
            }
        }
        else if (firstDirection == E && secondDirection == N)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute->patch)(i, 3) = (*secondAttribute->patch)(0, i) = 0.5 * ((*firstAttribute->patch)(i, 2) + (*secondAttribute->patch)(1, i));
            }
        }
        else if (firstDirection == E && secondDirection == W)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute->patch)(i, 3) = (*secondAttribute->patch)(i, 0) = 0.5 * ((*firstAttribute->patch)(i, 2) + (*secondAttribute->patch)(i, 1));
            }
        }
        else if (firstDirection == E && secondDirection == S)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute->patch)(i, 3) = (*secondAttribute->patch)(3, i) = 0.5 * ((*firstAttribute->patch)(i, 2) + (*secondAttribute->patch)(2, i));
            }
        }
        else if (firstDirection == E && secondDirection == E)
        {
            for(GLuint i=0; i<=3; ++i)
            {
                (*firstAttribute->patch)(i, 3) = (*secondAttribute->patch)(i, 3) = 0.5 * ((*firstAttribute->patch)(i, 2) + (*secondAttribute->patch)(i, 2));
            }
        }
        else if (firstDirection == NE && secondDirection == SW)
        {
            DCoordinate3 m1 = ((*firstAttribute->patch)(1,3) + (*secondAttribute->patch)(2,0)) * 0.5;
            DCoordinate3 m2 = ((*firstAttribute->patch)(0,2) + (*secondAttribute->patch)(3,1)) * 0.5;
            DCoordinate3 m = (m1 + m2) * 0.5;
            (*firstAttribute->patch)(0,3) = m;
            (*secondAttribute->patch)(3,0) = m;
            (*firstAttribute->patch)(1,3) += m - m1;
            (*secondAttribute->patch)(2,0) += m - m1;
            (*firstAttribute->patch)(0,2) += m - m2;
            (*secondAttribute->patch)(3,1) += m - m2;
        }
        else if (firstDirection == NE && secondDirection == SE)
        {
            DCoordinate3 m1 = ((*firstAttribute->patch)(1,3) + (*secondAttribute->patch)(3,2)) * 0.5;
            DCoordinate3 m2 = ((*firstAttribute->patch)(0,2) + (*secondAttribute->patch)(2,3)) * 0.5;
            DCoordinate3 m = (m1 + m2) * 0.5;
            (*firstAttribute->patch)(0,3) = m;
            (*secondAttribute->patch)(3,3) = m;
            (*firstAttribute->patch)(1,3) += m - m1;
            (*secondAttribute->patch)(3,2) += m - m1;
            (*firstAttribute->patch)(0,2) += m - m2;
            (*secondAttribute->patch)(2,3) += m - m2;
        }
        else if (firstDirection == NE && secondDirection == NE)
        {
            DCoordinate3 m1 = ((*firstAttribute->patch)(1,3) + (*secondAttribute->patch)(1,3)) * 0.5;
            DCoordinate3 m2 = ((*firstAttribute->patch)(0,2) + (*secondAttribute->patch)(0,2)) * 0.5;
            DCoordinate3 m = (m1 + m2) * 0.5;
            (*firstAttribute->patch)(0,3) = m;
            (*secondAttribute->patch)(0,3) = m;
            (*firstAttribute->patch)(1,3) += m - m1;
            (*secondAttribute->patch)(1,3) += m - m1;
            (*firstAttribute->patch)(0,2) += m - m2;
            (*secondAttribute->patch)(0,2) += m - m2;
        }
        else if (firstDirection == NE && secondDirection == NW)
        {
            DCoordinate3 m1 = ((*firstAttribute->patch)(1,3) + (*secondAttribute->patch)(0,1)) * 0.5;
            DCoordinate3 m2 = ((*firstAttribute->patch)(0,2) + (*secondAttribute->patch)(1,0)) * 0.5;
            DCoordinate3 m = (m1 + m2) * 0.5;
            (*firstAttribute->patch)(0,3) = m;
            (*secondAttribute->patch)(0,0) = m;
            (*firstAttribute->patch)(1,3) += m - m1;
            (*secondAttribute->patch)(0,1) += m - m1;
            (*firstAttribute->patch)(0,2) += m - m2;
            (*secondAttribute->patch)(1,0) += m - m2;
        }
        else if (firstDirection == SE && secondDirection == SW)
        {
            DCoordinate3 m1 = ((*firstAttribute->patch)(3,2) + (*secondAttribute->patch)(2,0)) * 0.5;
            DCoordinate3 m2 = ((*firstAttribute->patch)(2,3) + (*secondAttribute->patch)(3,1)) * 0.5;
            DCoordinate3 m = (m1 + m2) * 0.5;
            (*firstAttribute->patch)(3,3) = m;
            (*secondAttribute->patch)(3,0) = m;
            (*firstAttribute->patch)(3,2) += m - m1;
            (*secondAttribute->patch)(2,0) += m - m1;
            (*firstAttribute->patch)(2,3) += m - m2;
            (*secondAttribute->patch)(3,1) += m - m2;
        }
        else if (firstDirection == SE && secondDirection == SE)
        {
            DCoordinate3 m1 = ((*firstAttribute->patch)(3,2) + (*secondAttribute->patch)(3,2)) * 0.5;
            DCoordinate3 m2 = ((*firstAttribute->patch)(2,3) + (*secondAttribute->patch)(2,3)) * 0.5;
            DCoordinate3 m = (m1 + m2) * 0.5;
            (*firstAttribute->patch)(3,3) = m;
            (*secondAttribute->patch)(3,3) = m;
            (*firstAttribute->patch)(3,2) += m - m1;
            (*secondAttribute->patch)(3,2) += m - m1;
            (*firstAttribute->patch)(2,3) += m - m2;
            (*secondAttribute->patch)(2,3) += m - m2;
        }
        else if (firstDirection == SE && secondDirection == NE)
        {
            DCoordinate3 m1 = ((*firstAttribute->patch)(3,2) + (*secondAttribute->patch)(1,3)) * 0.5;
            DCoordinate3 m2 = ((*firstAttribute->patch)(2,3) + (*secondAttribute->patch)(0,2)) * 0.5;
            DCoordinate3 m = (m1 + m2) * 0.5;
            (*firstAttribute->patch)(3,3) = m;
            (*secondAttribute->patch)(0,3) = m;
            (*firstAttribute->patch)(3,2) += m - m1;
            (*secondAttribute->patch)(1,3) += m - m1;
            (*firstAttribute->patch)(2,3) += m - m2;
            (*secondAttribute->patch)(0,2) += m - m2;
        }
        else if (firstDirection == SE && secondDirection == NW)
        {
            DCoordinate3 m1 = ((*firstAttribute->patch)(3,2) + (*secondAttribute->patch)(0,1)) * 0.5;
            DCoordinate3 m2 = ((*firstAttribute->patch)(2,3) + (*secondAttribute->patch)(1,0)) * 0.5;
            DCoordinate3 m = (m1 + m2) * 0.5;
            (*firstAttribute->patch)(3,3) = m;
            (*secondAttribute->patch)(0,0) = m;
            (*firstAttribute->patch)(3,2) += m - m1;
            (*secondAttribute->patch)(0,1) += m - m1;
            (*firstAttribute->patch)(2,3) += m - m2;
            (*secondAttribute->patch)(1,0) += m - m2;
        }
        else if (firstDirection == SW && secondDirection == SW)
        {
            DCoordinate3 m1 = ((*firstAttribute->patch)(2,0) + (*secondAttribute->patch)(2,0)) * 0.5;
            DCoordinate3 m2 = ((*firstAttribute->patch)(3,1) + (*secondAttribute->patch)(3,1)) * 0.5;
            DCoordinate3 m = (m1 + m2) * 0.5;
            (*firstAttribute->patch)(3,0) = m;
            (*secondAttribute->patch)(3,0) = m;
            (*firstAttribute->patch)(2,0) += m - m1;
            (*secondAttribute->patch)(2,0) += m - m1;
            (*firstAttribute->patch)(3,1) += m - m2;
            (*secondAttribute->patch)(3,1) += m - m2;
        }
        else if (firstDirection == SW && secondDirection == SE)
        {
            DCoordinate3 m1 = ((*firstAttribute->patch)(2,0) + (*secondAttribute->patch)(3,2)) * 0.5;
            DCoordinate3 m2 = ((*firstAttribute->patch)(3,1) + (*secondAttribute->patch)(2,3)) * 0.5;
            DCoordinate3 m = (m1 + m2) * 0.5;
            (*firstAttribute->patch)(3,0) = m;
            (*secondAttribute->patch)(3,3) = m;
            (*firstAttribute->patch)(2,0) += m - m1;
            (*secondAttribute->patch)(3,2) += m - m1;
            (*firstAttribute->patch)(3,1) += m - m2;
            (*secondAttribute->patch)(2,3) += m - m2;
        }
        else if (firstDirection == SW && secondDirection == NE)
        {
            DCoordinate3 m1 = ((*firstAttribute->patch)(2,0) + (*secondAttribute->patch)(1,3)) * 0.5;
            DCoordinate3 m2 = ((*firstAttribute->patch)(3,1) + (*secondAttribute->patch)(0,2)) * 0.5;
            DCoordinate3 m = (m1 + m2) * 0.5;
            (*firstAttribute->patch)(3,0) = m;
            (*secondAttribute->patch)(0,3) = m;
            (*firstAttribute->patch)(2,0) += m - m1;
            (*secondAttribute->patch)(1,3) += m - m1;
            (*firstAttribute->patch)(3,1) += m - m2;
            (*secondAttribute->patch)(0,2) += m - m2;
        }
        else if (firstDirection == SW && secondDirection == NW)
        {
            DCoordinate3 m1 = ((*firstAttribute->patch)(2,0) + (*secondAttribute->patch)(0,1)) * 0.5;
            DCoordinate3 m2 = ((*firstAttribute->patch)(3,1) + (*secondAttribute->patch)(1,0)) * 0.5;
            DCoordinate3 m = (m1 + m2) * 0.5;
            (*firstAttribute->patch)(3,0) = m;
            (*secondAttribute->patch)(0,0) = m;
            (*firstAttribute->patch)(2,0) += m - m1;
            (*secondAttribute->patch)(0,1) += m - m1;
            (*firstAttribute->patch)(3,1) += m - m2;
            (*secondAttribute->patch)(1,0) += m - m2;
        }
        else if (firstDirection == NW && secondDirection == SW)
        {
            DCoordinate3 m1 = ((*firstAttribute->patch)(0,1) + (*secondAttribute->patch)(2,0)) * 0.5;
            DCoordinate3 m2 = ((*firstAttribute->patch)(1,0) + (*secondAttribute->patch)(3,1)) * 0.5;
            DCoordinate3 m = (m1 + m2) * 0.5;
            (*firstAttribute->patch)(0,0) = m;
            (*secondAttribute->patch)(3,0) = m;
            (*firstAttribute->patch)(0,1) += m - m1;
            (*secondAttribute->patch)(2,0) += m - m1;
            (*firstAttribute->patch)(1,0) += m - m2;
            (*secondAttribute->patch)(3,1) += m - m2;
        }
        else if (firstDirection == NW && secondDirection == SE)
        {
            DCoordinate3 m1 = ((*firstAttribute->patch)(0,1) + (*secondAttribute->patch)(3,2)) * 0.5;
            DCoordinate3 m2 = ((*firstAttribute->patch)(1,0) + (*secondAttribute->patch)(2,3)) * 0.5;
            DCoordinate3 m = (m1 + m2) * 0.5;
            (*firstAttribute->patch)(0,0) = m;
            (*secondAttribute->patch)(3,3) = m;
            (*firstAttribute->patch)(0,1) += m - m1;
            (*secondAttribute->patch)(3,2) += m - m1;
            (*firstAttribute->patch)(1,0) += m - m2;
            (*secondAttribute->patch)(2,3) += m - m2;
        }
        else if (firstDirection == NW && secondDirection == NE)
        {
            DCoordinate3 m1 = ((*firstAttribute->patch)(0,1) + (*secondAttribute->patch)(1,3)) * 0.5;
            DCoordinate3 m2 = ((*firstAttribute->patch)(1,0) + (*secondAttribute->patch)(0,2)) * 0.5;
            DCoordinate3 m = (m1 + m2) * 0.5;
            (*firstAttribute->patch)(0,0) = m;
            (*secondAttribute->patch)(0,3) = m;
            (*firstAttribute->patch)(0,1) += m - m1;
            (*secondAttribute->patch)(1,3) += m - m1;
            (*firstAttribute->patch)(1,0) += m - m2;
            (*secondAttribute->patch)(0,2) += m - m2;
        }
        else if (firstDirection == NW && secondDirection == NW)
        {
            DCoordinate3 m1 = ((*firstAttribute->patch)(0,1) + (*secondAttribute->patch)(0,1)) * 0.5;
            DCoordinate3 m2 = ((*firstAttribute->patch)(1,0) + (*secondAttribute->patch)(1,0)) * 0.5;
            DCoordinate3 m = (m1 + m2) * 0.5;
            (*firstAttribute->patch)(0,0) = m;
            (*secondAttribute->patch)(0,0) = m;
            (*firstAttribute->patch)(0,1) += m - m1;
            (*secondAttribute->patch)(0,1) += m - m1;
            (*firstAttribute->patch)(1,0) += m - m2;
            (*secondAttribute->patch)(1,0) += m - m2;
        }

        switch (firstDirection)
        {
        case N:
            if (firstAttribute->neighbours[NE])
            {
                PatchAttributes *neighbour = firstAttribute->neighbours[NE];
                (*neighbour->patch)(3,0) = (*firstAttribute->patch)(0,3);
                (*neighbour->patch)(3,1) = 2 * (*firstAttribute->patch)(0,3) - (*firstAttribute->patch)(0,2);
                (*neighbour->patch)(2,1) = 2 * (*firstAttribute->patch)(0,3) - (*firstAttribute->patch)(1,2);
                (*neighbour->patch)(2,0) = 2 * (*firstAttribute->patch)(0,3) - (*firstAttribute->patch)(1,3);
                UpdateVBOs(neighbour);
            }
            if (firstAttribute->neighbours[NW])
            {
                PatchAttributes *neighbour = firstAttribute->neighbours[NW];
                (*neighbour->patch)(3,3) = (*firstAttribute->patch)(0,0);
                (*neighbour->patch)(3,2) = 2 * (*firstAttribute->patch)(0,0) - (*firstAttribute->patch)(0,1);
                (*neighbour->patch)(2,3) = 2 * (*firstAttribute->patch)(0,0) - (*firstAttribute->patch)(1,0);
                (*neighbour->patch)(2,2) = 2 * (*firstAttribute->patch)(0,0) - (*firstAttribute->patch)(1,1);
                UpdateVBOs(neighbour);
            }
            if (firstAttribute->neighbours[E])
            {
                PatchAttributes *neighbour = firstAttribute->neighbours[E];
                (*neighbour->patch)(0,0) = (*firstAttribute->patch)(0,3);
                (*neighbour->patch)(0,1) = 2 * (*firstAttribute->patch)(0,3) - (*firstAttribute->patch)(0,2);
                UpdateVBOs(neighbour);
            }
            if (firstAttribute->neighbours[W])
            {
                PatchAttributes *neighbour = firstAttribute->neighbours[W];
                (*neighbour->patch)(0,3) = (*firstAttribute->patch)(0,0);
                (*neighbour->patch)(0,2) = 2 * (*firstAttribute->patch)(0,0) - (*firstAttribute->patch)(0,1);
                UpdateVBOs(neighbour);
            }
            break;
        case S:
            if (firstAttribute->neighbours[SE])
            {
                PatchAttributes *neighbour = firstAttribute->neighbours[SE];
                (*neighbour->patch)(0,0) = (*firstAttribute->patch)(3,3);
                (*neighbour->patch)(0,1) = 2 * (*firstAttribute->patch)(3,3) - (*firstAttribute->patch)(3,2);
                (*neighbour->patch)(1,1) = 2 * (*firstAttribute->patch)(3,3) - (*firstAttribute->patch)(2,2);
                (*neighbour->patch)(1,0) = 2 * (*firstAttribute->patch)(3,3) - (*firstAttribute->patch)(2,3);
                UpdateVBOs(neighbour);
            }
            if (firstAttribute->neighbours[SW])
            {
                PatchAttributes *neighbour = firstAttribute->neighbours[SW];
                (*neighbour->patch)(0,3) = (*firstAttribute->patch)(3,0);
                (*neighbour->patch)(0,2) = 2 * (*firstAttribute->patch)(3,0) - (*firstAttribute->patch)(3,1);
                (*neighbour->patch)(1,2) = 2 * (*firstAttribute->patch)(3,0) - (*firstAttribute->patch)(2,1);
                (*neighbour->patch)(1,3) = 2 * (*firstAttribute->patch)(3,0) - (*firstAttribute->patch)(2,0);
                UpdateVBOs(neighbour);
            }
            if (firstAttribute->neighbours[E])
            {
                PatchAttributes *neighbour = firstAttribute->neighbours[E];
                (*neighbour->patch)(3,0) = (*firstAttribute->patch)(3,3);
                (*neighbour->patch)(3,1) = 2 * (*firstAttribute->patch)(3,3) - (*firstAttribute->patch)(3,2);
                UpdateVBOs(neighbour);
            }
            if (firstAttribute->neighbours[W])
            {
                PatchAttributes *neighbour = firstAttribute->neighbours[W];
                (*neighbour->patch)(3,3) = (*firstAttribute->patch)(3,0);
                (*neighbour->patch)(3,2) = 2 * (*firstAttribute->patch)(3,0) - (*firstAttribute->patch)(3,1);
                UpdateVBOs(neighbour);
            }
            break;
        case E:
            if (firstAttribute->neighbours[SE])
            {
                PatchAttributes *neighbour = firstAttribute->neighbours[SE];
                (*neighbour->patch)(0,0) = (*firstAttribute->patch)(3,3);
                (*neighbour->patch)(0,1) = 2 * (*firstAttribute->patch)(3,3) - (*firstAttribute->patch)(3,2);
                (*neighbour->patch)(1,1) = 2 * (*firstAttribute->patch)(3,3) - (*firstAttribute->patch)(2,2);
                (*neighbour->patch)(1,0) = 2 * (*firstAttribute->patch)(3,3) - (*firstAttribute->patch)(2,3);
                UpdateVBOs(neighbour);
            }
            if (firstAttribute->neighbours[NE])
            {
                PatchAttributes *neighbour = firstAttribute->neighbours[NE];
                (*neighbour->patch)(3,0) = (*firstAttribute->patch)(0,3);
                (*neighbour->patch)(3,1) = 2 * (*firstAttribute->patch)(0,3) - (*firstAttribute->patch)(0,2);
                (*neighbour->patch)(2,1) = 2 * (*firstAttribute->patch)(0,3) - (*firstAttribute->patch)(1,2);
                (*neighbour->patch)(2,0) = 2 * (*firstAttribute->patch)(0,3) - (*firstAttribute->patch)(1,3);
                UpdateVBOs(neighbour);
            }
            if (firstAttribute->neighbours[N])
            {
                PatchAttributes *neighbour = firstAttribute->neighbours[N];
                (*neighbour->patch)(3,3) = (*firstAttribute->patch)(0,3);
                (*neighbour->patch)(2,3) = 2 * (*firstAttribute->patch)(0,3) - (*firstAttribute->patch)(1,3);
                UpdateVBOs(neighbour);
            }
            if (firstAttribute->neighbours[S])
            {
                PatchAttributes *neighbour = firstAttribute->neighbours[S];
                (*neighbour->patch)(0,3) = (*firstAttribute->patch)(3,3);
                (*neighbour->patch)(1,3) = 2 * (*firstAttribute->patch)(3,3) - (*firstAttribute->patch)(2,3);
                UpdateVBOs(neighbour);
            }
            break;
        case W:
            if (firstAttribute->neighbours[SW])
            {
                PatchAttributes *neighbour = firstAttribute->neighbours[SW];
                (*neighbour->patch)(0,3) = (*firstAttribute->patch)(3,0);
                (*neighbour->patch)(0,2) = 2 * (*firstAttribute->patch)(3,0) - (*firstAttribute->patch)(3,1);
                (*neighbour->patch)(1,2) = 2 * (*firstAttribute->patch)(3,0) - (*firstAttribute->patch)(2,1);
                (*neighbour->patch)(1,3) = 2 * (*firstAttribute->patch)(3,0) - (*firstAttribute->patch)(2,0);
                UpdateVBOs(neighbour);
            }
            if (firstAttribute->neighbours[NW])
            {
                PatchAttributes *neighbour = firstAttribute->neighbours[NW];
                (*neighbour->patch)(3,3) = (*firstAttribute->patch)(0,0);
                (*neighbour->patch)(3,2) = 2 * (*firstAttribute->patch)(0,0) - (*firstAttribute->patch)(0,1);
                (*neighbour->patch)(2,3) = 2 * (*firstAttribute->patch)(0,0) - (*firstAttribute->patch)(1,0);
                (*neighbour->patch)(2,2) = 2 * (*firstAttribute->patch)(0,0) - (*firstAttribute->patch)(1,1);
                UpdateVBOs(neighbour);
            }
            if (firstAttribute->neighbours[N])
            {
                PatchAttributes *neighbour = firstAttribute->neighbours[N];
                (*neighbour->patch)(3,0) = (*firstAttribute->patch)(0,0);
                (*neighbour->patch)(2,0) = 2 * (*firstAttribute->patch)(0,0) - (*firstAttribute->patch)(1,0);
                UpdateVBOs(neighbour);
            }
            if (firstAttribute->neighbours[S])
            {
                PatchAttributes *neighbour = firstAttribute->neighbours[S];
                (*neighbour->patch)(0,0) = (*firstAttribute->patch)(3,0);
                (*neighbour->patch)(1,0) = 2 * (*firstAttribute->patch)(3,0) - (*firstAttribute->patch)(2,0);
                UpdateVBOs(neighbour);
            }
            break;
        case NE:
            if (firstAttribute->neighbours[E])
            {
                PatchAttributes *neighbour = firstAttribute->neighbours[E];
                (*neighbour->patch)(0,0) = (*firstAttribute->patch)(0,3);
                (*neighbour->patch)(1,0) = (*firstAttribute->patch)(1,3);
                (*neighbour->patch)(0,1) = 2 * (*firstAttribute->patch)(0,3) - (*firstAttribute->patch)(0,2);
                (*neighbour->patch)(1,1) = 2 * (*firstAttribute->patch)(1,3) - (*firstAttribute->patch)(1,2);
                UpdateVBOs(neighbour);
            }
            if (firstAttribute->neighbours[N])
            {
                PatchAttributes *neighbour = firstAttribute->neighbours[N];
                (*neighbour->patch)(3,3) = (*firstAttribute->patch)(0,3);
                (*neighbour->patch)(3,2) = (*firstAttribute->patch)(0,2);
                (*neighbour->patch)(2,3) = 2 * (*firstAttribute->patch)(0,3) - (*firstAttribute->patch)(1,3);
                (*neighbour->patch)(2,2) = 2 * (*firstAttribute->patch)(0,2) - (*firstAttribute->patch)(1,2);
                UpdateVBOs(neighbour);
            }
            break;
        case SE:
            if (firstAttribute->neighbours[E])
            {
                PatchAttributes *neighbour = firstAttribute->neighbours[E];
                (*neighbour->patch)(3,0) = (*firstAttribute->patch)(3,3);
                (*neighbour->patch)(2,0) = (*firstAttribute->patch)(2,3);
                (*neighbour->patch)(3,1) = 2 * (*firstAttribute->patch)(3,3) - (*firstAttribute->patch)(3,2);
                (*neighbour->patch)(2,1) = 2 * (*firstAttribute->patch)(2,3) - (*firstAttribute->patch)(2,2);
                UpdateVBOs(neighbour);
            }
            if (firstAttribute->neighbours[S])
            {
                PatchAttributes *neighbour = firstAttribute->neighbours[S];
                (*neighbour->patch)(0,3) = (*firstAttribute->patch)(3,3);
                (*neighbour->patch)(0,2) = (*firstAttribute->patch)(3,2);
                (*neighbour->patch)(1,3) = 2 * (*firstAttribute->patch)(3,3) - (*firstAttribute->patch)(2,3);
                (*neighbour->patch)(1,2) = 2 * (*firstAttribute->patch)(3,2) - (*firstAttribute->patch)(2,2);
                UpdateVBOs(neighbour);
            }
            break;
        case SW:
            if (firstAttribute->neighbours[W])
            {
                PatchAttributes *neighbour = firstAttribute->neighbours[W];
                (*neighbour->patch)(3,3) = (*firstAttribute->patch)(3,0);
                (*neighbour->patch)(2,3) = (*firstAttribute->patch)(2,0);
                (*neighbour->patch)(3,2) = 2 * (*firstAttribute->patch)(3,0) - (*firstAttribute->patch)(3,1);
                (*neighbour->patch)(2,2) = 2 * (*firstAttribute->patch)(2,0) - (*firstAttribute->patch)(2,1);
                UpdateVBOs(neighbour);
            }
            if (firstAttribute->neighbours[S])
            {
                PatchAttributes *neighbour = firstAttribute->neighbours[S];
                (*neighbour->patch)(0,0) = (*firstAttribute->patch)(3,0);
                (*neighbour->patch)(0,1) = (*firstAttribute->patch)(3,1);
                (*neighbour->patch)(1,0) = 2 * (*firstAttribute->patch)(3,0) - (*firstAttribute->patch)(2,0);
                (*neighbour->patch)(1,1) = 2 * (*firstAttribute->patch)(3,1) - (*firstAttribute->patch)(2,1);
                UpdateVBOs(neighbour);
            }
            break;
        case NW:
            if (firstAttribute->neighbours[W])
            {
                PatchAttributes *neighbour = firstAttribute->neighbours[W];
                (*neighbour->patch)(0,3) = (*firstAttribute->patch)(0,0);
                (*neighbour->patch)(1,3) = (*firstAttribute->patch)(1,0);
                (*neighbour->patch)(0,2) = 2 * (*firstAttribute->patch)(0,0) - (*firstAttribute->patch)(0,1);
                (*neighbour->patch)(1,2) = 2 * (*firstAttribute->patch)(1,0) - (*firstAttribute->patch)(1,1);
                UpdateVBOs(neighbour);
            }
            if (firstAttribute->neighbours[N])
            {
                PatchAttributes *neighbour = firstAttribute->neighbours[N];
                (*neighbour->patch)(3,0) = (*firstAttribute->patch)(0,0);
                (*neighbour->patch)(3,1) = (*firstAttribute->patch)(0,1);
                (*neighbour->patch)(2,0) = 2 * (*firstAttribute->patch)(0,0) - (*firstAttribute->patch)(1,0);
                (*neighbour->patch)(2,1) = 2 * (*firstAttribute->patch)(0,1) - (*firstAttribute->patch)(1,1);
                UpdateVBOs(neighbour);
            }
            break;
        }

        switch (secondDirection)
        {
        case N:
            if (secondAttribute->neighbours[NE])
            {
                PatchAttributes *neighbour = secondAttribute->neighbours[NE];
                (*neighbour->patch)(3,0) = (*secondAttribute->patch)(0,3);
                (*neighbour->patch)(3,1) = 2 * (*secondAttribute->patch)(0,3) - (*secondAttribute->patch)(0,2);
                (*neighbour->patch)(2,1) = 2 * (*secondAttribute->patch)(0,3) - (*secondAttribute->patch)(1,2);
                (*neighbour->patch)(2,0) = 2 * (*secondAttribute->patch)(0,3) - (*secondAttribute->patch)(1,3);
                UpdateVBOs(neighbour);
            }
            if (secondAttribute->neighbours[NW])
            {
                PatchAttributes *neighbour = secondAttribute->neighbours[NW];
                (*neighbour->patch)(3,3) = (*secondAttribute->patch)(0,0);
                (*neighbour->patch)(3,2) = 2 * (*secondAttribute->patch)(0,0) - (*secondAttribute->patch)(0,1);
                (*neighbour->patch)(2,3) = 2 * (*secondAttribute->patch)(0,0) - (*secondAttribute->patch)(1,0);
                (*neighbour->patch)(2,2) = 2 * (*secondAttribute->patch)(0,0) - (*secondAttribute->patch)(1,1);
                UpdateVBOs(neighbour);
            }
            if (secondAttribute->neighbours[E])
            {
                PatchAttributes *neighbour = secondAttribute->neighbours[E];
                (*neighbour->patch)(0,0) = (*secondAttribute->patch)(0,3);
                (*neighbour->patch)(0,1) = 2 * (*secondAttribute->patch)(0,3) - (*secondAttribute->patch)(0,2);
                UpdateVBOs(neighbour);
            }
            if (secondAttribute->neighbours[W])
            {
                PatchAttributes *neighbour = secondAttribute->neighbours[W];
                (*neighbour->patch)(0,3) = (*secondAttribute->patch)(0,0);
                (*neighbour->patch)(0,2) = 2 * (*secondAttribute->patch)(0,0) - (*secondAttribute->patch)(0,1);
                UpdateVBOs(neighbour);
            }
            break;
        case S:
            if (secondAttribute->neighbours[SE])
            {
                PatchAttributes *neighbour = secondAttribute->neighbours[SE];
                (*neighbour->patch)(0,0) = (*secondAttribute->patch)(3,3);
                (*neighbour->patch)(0,1) = 2 * (*secondAttribute->patch)(3,3) - (*secondAttribute->patch)(3,2);
                (*neighbour->patch)(1,1) = 2 * (*secondAttribute->patch)(3,3) - (*secondAttribute->patch)(2,2);
                (*neighbour->patch)(1,0) = 2 * (*secondAttribute->patch)(3,3) - (*secondAttribute->patch)(2,3);
                UpdateVBOs(neighbour);
            }
            if (secondAttribute->neighbours[SW])
            {
                PatchAttributes *neighbour = secondAttribute->neighbours[SW];
                (*neighbour->patch)(0,3) = (*secondAttribute->patch)(3,0);
                (*neighbour->patch)(0,2) = 2 * (*secondAttribute->patch)(3,0) - (*secondAttribute->patch)(3,1);
                (*neighbour->patch)(1,2) = 2 * (*secondAttribute->patch)(3,0) - (*secondAttribute->patch)(2,1);
                (*neighbour->patch)(1,3) = 2 * (*secondAttribute->patch)(3,0) - (*secondAttribute->patch)(2,0);
                UpdateVBOs(neighbour);
            }
            if (secondAttribute->neighbours[E])
            {
                PatchAttributes *neighbour = secondAttribute->neighbours[E];
                (*neighbour->patch)(3,0) = (*secondAttribute->patch)(3,3);
                (*neighbour->patch)(3,1) = 2 * (*secondAttribute->patch)(3,3) - (*secondAttribute->patch)(3,2);
                UpdateVBOs(neighbour);
            }
            if (secondAttribute->neighbours[W])
            {
                PatchAttributes *neighbour = secondAttribute->neighbours[W];
                (*neighbour->patch)(3,3) = (*secondAttribute->patch)(3,0);
                (*neighbour->patch)(3,2) = 2 * (*secondAttribute->patch)(3,0) - (*secondAttribute->patch)(3,1);
                UpdateVBOs(neighbour);
            }
            break;
        case E:
            if (secondAttribute->neighbours[SE])
            {
                PatchAttributes *neighbour = secondAttribute->neighbours[SE];
                (*neighbour->patch)(0,0) = (*secondAttribute->patch)(3,3);
                (*neighbour->patch)(0,1) = 2 * (*secondAttribute->patch)(3,3) - (*secondAttribute->patch)(3,2);
                (*neighbour->patch)(1,1) = 2 * (*secondAttribute->patch)(3,3) - (*secondAttribute->patch)(2,2);
                (*neighbour->patch)(1,0) = 2 * (*secondAttribute->patch)(3,3) - (*secondAttribute->patch)(2,3);
                UpdateVBOs(neighbour);
            }
            if (secondAttribute->neighbours[NE])
            {
                PatchAttributes *neighbour = secondAttribute->neighbours[NE];
                (*neighbour->patch)(3,0) = (*secondAttribute->patch)(0,3);
                (*neighbour->patch)(3,1) = 2 * (*secondAttribute->patch)(0,3) - (*secondAttribute->patch)(0,2);
                (*neighbour->patch)(2,1) = 2 * (*secondAttribute->patch)(0,3) - (*secondAttribute->patch)(1,2);
                (*neighbour->patch)(2,0) = 2 * (*secondAttribute->patch)(0,3) - (*secondAttribute->patch)(1,3);
                UpdateVBOs(neighbour);
            }
            if (secondAttribute->neighbours[N])
            {
                PatchAttributes *neighbour = secondAttribute->neighbours[N];
                (*neighbour->patch)(3,3) = (*secondAttribute->patch)(0,3);
                (*neighbour->patch)(2,3) = 2 * (*secondAttribute->patch)(0,3) - (*secondAttribute->patch)(1,3);
                UpdateVBOs(neighbour);
            }
            if (secondAttribute->neighbours[S])
            {
                PatchAttributes *neighbour = secondAttribute->neighbours[S];
                (*neighbour->patch)(0,3) = (*secondAttribute->patch)(3,3);
                (*neighbour->patch)(1,3) = 2 * (*secondAttribute->patch)(3,3) - (*secondAttribute->patch)(2,3);
                UpdateVBOs(neighbour);
            }
            break;
        case W:
            if (secondAttribute->neighbours[SW])
            {
                PatchAttributes *neighbour = secondAttribute->neighbours[SW];
                (*neighbour->patch)(0,3) = (*secondAttribute->patch)(3,0);
                (*neighbour->patch)(0,2) = 2 * (*secondAttribute->patch)(3,0) - (*secondAttribute->patch)(3,1);
                (*neighbour->patch)(1,2) = 2 * (*secondAttribute->patch)(3,0) - (*secondAttribute->patch)(2,1);
                (*neighbour->patch)(1,3) = 2 * (*secondAttribute->patch)(3,0) - (*secondAttribute->patch)(2,0);
                UpdateVBOs(neighbour);
            }
            if (secondAttribute->neighbours[NW])
            {
                PatchAttributes *neighbour = secondAttribute->neighbours[NW];
                (*neighbour->patch)(3,3) = (*secondAttribute->patch)(0,0);
                (*neighbour->patch)(3,2) = 2 * (*secondAttribute->patch)(0,0) - (*secondAttribute->patch)(0,1);
                (*neighbour->patch)(2,3) = 2 * (*secondAttribute->patch)(0,0) - (*secondAttribute->patch)(1,0);
                (*neighbour->patch)(2,2) = 2 * (*secondAttribute->patch)(0,0) - (*secondAttribute->patch)(1,1);
                UpdateVBOs(neighbour);
            }
            if (secondAttribute->neighbours[N])
            {
                PatchAttributes *neighbour = secondAttribute->neighbours[N];
                (*neighbour->patch)(3,0) = (*secondAttribute->patch)(0,0);
                (*neighbour->patch)(2,0) = 2 * (*secondAttribute->patch)(0,0) - (*secondAttribute->patch)(1,0);
                UpdateVBOs(neighbour);
            }
            if (secondAttribute->neighbours[S])
            {
                PatchAttributes *neighbour = secondAttribute->neighbours[S];
                (*neighbour->patch)(0,0) = (*secondAttribute->patch)(3,0);
                (*neighbour->patch)(1,0) = 2 * (*secondAttribute->patch)(3,0) - (*secondAttribute->patch)(2,0);
                UpdateVBOs(neighbour);
            }
            break;
        case NE:
            if (secondAttribute->neighbours[E])
            {
                PatchAttributes *neighbour = secondAttribute->neighbours[E];
                (*neighbour->patch)(0,0) = (*secondAttribute->patch)(0,3);
                (*neighbour->patch)(1,0) = (*secondAttribute->patch)(1,3);
                (*neighbour->patch)(0,1) = 2 * (*secondAttribute->patch)(0,3) - (*secondAttribute->patch)(0,2);
                (*neighbour->patch)(1,1) = 2 * (*secondAttribute->patch)(1,3) - (*secondAttribute->patch)(1,2);
                UpdateVBOs(neighbour);
            }
            if (secondAttribute->neighbours[N])
            {
                PatchAttributes *neighbour = secondAttribute->neighbours[N];
                (*neighbour->patch)(3,3) = (*secondAttribute->patch)(0,3);
                (*neighbour->patch)(3,2) = (*secondAttribute->patch)(0,2);
                (*neighbour->patch)(2,3) = 2 * (*secondAttribute->patch)(0,3) - (*secondAttribute->patch)(1,3);
                (*neighbour->patch)(2,2) = 2 * (*secondAttribute->patch)(0,2) - (*secondAttribute->patch)(1,2);
                UpdateVBOs(neighbour);
            }
            break;
        case SE:
            if (secondAttribute->neighbours[E])
            {
                PatchAttributes *neighbour = secondAttribute->neighbours[E];
                (*neighbour->patch)(3,0) = (*secondAttribute->patch)(3,3);
                (*neighbour->patch)(2,0) = (*secondAttribute->patch)(2,3);
                (*neighbour->patch)(3,1) = 2 * (*secondAttribute->patch)(3,3) - (*secondAttribute->patch)(3,2);
                (*neighbour->patch)(2,1) = 2 * (*secondAttribute->patch)(2,3) - (*secondAttribute->patch)(2,2);
                UpdateVBOs(neighbour);
            }
            if (secondAttribute->neighbours[S])
            {
                PatchAttributes *neighbour = secondAttribute->neighbours[S];
                (*neighbour->patch)(0,3) = (*secondAttribute->patch)(3,3);
                (*neighbour->patch)(0,2) = (*secondAttribute->patch)(3,2);
                (*neighbour->patch)(1,3) = 2 * (*secondAttribute->patch)(3,3) - (*secondAttribute->patch)(2,3);
                (*neighbour->patch)(1,2) = 2 * (*secondAttribute->patch)(3,2) - (*secondAttribute->patch)(2,2);
                UpdateVBOs(neighbour);
            }
            break;
        case SW:
            if (secondAttribute->neighbours[W])
            {
                PatchAttributes *neighbour = secondAttribute->neighbours[W];
                (*neighbour->patch)(3,3) = (*secondAttribute->patch)(3,0);
                (*neighbour->patch)(2,3) = (*secondAttribute->patch)(2,0);
                (*neighbour->patch)(3,2) = 2 * (*secondAttribute->patch)(3,0) - (*secondAttribute->patch)(3,1);
                (*neighbour->patch)(2,2) = 2 * (*secondAttribute->patch)(2,0) - (*secondAttribute->patch)(2,1);
                UpdateVBOs(neighbour);
            }
            if (secondAttribute->neighbours[S])
            {
                PatchAttributes *neighbour = secondAttribute->neighbours[S];
                (*neighbour->patch)(0,0) = (*secondAttribute->patch)(3,0);
                (*neighbour->patch)(0,1) = (*secondAttribute->patch)(3,1);
                (*neighbour->patch)(1,0) = 2 * (*secondAttribute->patch)(3,0) - (*secondAttribute->patch)(2,0);
                (*neighbour->patch)(1,1) = 2 * (*secondAttribute->patch)(3,1) - (*secondAttribute->patch)(2,1);
                UpdateVBOs(neighbour);
            }
            break;
        case NW:
            if (secondAttribute->neighbours[W])
            {
                PatchAttributes *neighbour = secondAttribute->neighbours[W];
                (*neighbour->patch)(0,3) = (*secondAttribute->patch)(0,0);
                (*neighbour->patch)(1,3) = (*secondAttribute->patch)(1,0);
                (*neighbour->patch)(0,2) = 2 * (*secondAttribute->patch)(0,0) - (*secondAttribute->patch)(0,1);
                (*neighbour->patch)(1,2) = 2 * (*secondAttribute->patch)(1,0) - (*secondAttribute->patch)(1,1);
                UpdateVBOs(neighbour);
            }
            if (secondAttribute->neighbours[N])
            {
                PatchAttributes *neighbour = secondAttribute->neighbours[N];
                (*neighbour->patch)(3,0) = (*secondAttribute->patch)(0,0);
                (*neighbour->patch)(3,1) = (*secondAttribute->patch)(0,1);
                (*neighbour->patch)(2,0) = 2 * (*secondAttribute->patch)(0,0) - (*secondAttribute->patch)(1,0);
                (*neighbour->patch)(2,1) = 2 * (*secondAttribute->patch)(0,1) - (*secondAttribute->patch)(1,1);
                UpdateVBOs(neighbour);
            }
            break;
        }

        return UpdateVBOs(firstAttribute) && UpdateVBOs(secondAttribute);
    }

    std::ostream& operator <<(std::ostream& lhs, const BicubicCompositeSurface3& surface)
    {
        lhs << surface._u_iso_line_count << endl;
        lhs << surface._v_iso_line_count << endl;
        lhs << surface._attributes.size() << endl;

        for (auto it = surface._attributes.begin(); it != surface._attributes.end(); ++it)
        {
            lhs << *(*it)->patch << endl;
        }

        for (auto it = surface._attributes.begin(); it != surface._attributes.end(); ++it)
        {
            for (GLuint i=0; i<8; ++i)
            {
                GLint index = surface.IndexOfAttribute((*it)->neighbours[i]);
                lhs << index << " ";
            }
            lhs << endl;
        }

        return lhs;
    }

    std::istream& operator >>(std::istream& lhs, BicubicCompositeSurface3& surface)
    {
        GLuint n;

        lhs >> surface._u_iso_line_count >> surface._v_iso_line_count;
        lhs >> n;

        surface._attributes.clear();
        for (GLuint i=0; i<n; ++i)
        {
            BicubicCompositeSurface3::PatchAttributes *attribute = new BicubicCompositeSurface3::PatchAttributes;
            surface._attributes.push_back(attribute);

            attribute->patch = new BicubicBezierPatch();
            lhs >> *attribute->patch;

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
                    surface._attributes[i]->neighbours[j] = nullptr;
                }
                else
                {
                    surface._attributes[i]->neighbours[j] = surface._attributes[index];
                }
            }
        }

        return lhs;
    }

    int BicubicCompositeSurface3::MouseOnPatch(DCoordinate3 mC)
        {
            int clickedPatch = -1;
            GLdouble minDist = 0.2;
            GLdouble x = mC.x();
            GLdouble y = mC.y();

            GLboolean patchFound = GL_FALSE;

            for (GLuint j = 0; j < _attributes.size(); j++)
            {
                PatchAttributes *it = _attributes[j];
                if (it ->image)
                {
                    DCoordinate3 c;
                    GLuint pointCount;
                    pointCount = it -> image -> VertexCount();
                    for (GLuint i = 0; i < pointCount; i++)
                    {
                        it -> image -> GetVertex(i, c);
                        GLdouble tX = c.x();
                        GLdouble tY = c.y();
                        GLdouble tZ = c.z();
                        GLdouble distance = (x - tX) * (x - tX) + (y - tY) * (y - tY) + tZ * tZ;
                        if (distance < minDist){
                            minDist = distance;
                            clickedPatch = j;
                        }
                    }
                    if (patchFound == GL_TRUE)
                    {
                        break;
                    }
                }
            }

            return clickedPatch;
        }

    GLboolean BicubicCompositeSurface3::MouseOnCP(int patchInd, DCoordinate3 mC, int &cpX, int &cpY)
    {
        cpX = -1;
        cpY = -1;
        GLdouble minDist = 0.2;
        GLdouble x = mC.x();
        GLdouble y = mC.y();

        PatchAttributes *selectedPatch = _attributes[patchInd];
        DCoordinate3 c;

        for (GLuint i = 0; i < 4; i++)
        {
            for (GLuint j = 0; j < 4; j++)
            {
                selectedPatch -> patch -> GetData(i, j, c);
                GLdouble tX = c.x();
                GLdouble tY = c.y();
                GLdouble tZ = c.z();
                GLdouble distance = (x - tX) * (x - tX) + (y - tY) * (y - tY) + tZ * tZ;

                if (distance < minDist){
                    minDist = distance;
                    cpX = i;
                    cpY = j;
                }
            }
        }

        return GL_TRUE;

    }

    GLboolean BicubicCompositeSurface3::MouseNotOnPatchOnCP(DCoordinate3 mC, int &patchInd, int &cpX, int &cpY)
    {
        GLdouble minDist = 0.2;
        GLdouble x = mC.x();
        GLdouble y = mC.y();

        for (GLuint k = 0; k < _attributes.size(); k++)
        {
            PatchAttributes *selectedPatch = _attributes[k];
            DCoordinate3 c;
            for (GLuint i = 0; i < 4; i++)
            {
                for (GLuint j = 0; j < 4; j++)
                {
                    selectedPatch -> patch -> GetData(i, j, c);
                    GLdouble tX = c.x();
                    GLdouble tY = c.y();
                    GLdouble tZ = c.z();
                    GLdouble distance = (x - tX) * (x - tX) + (y - tY) * (y - tY) + tZ * tZ;

                    if (distance < minDist){
                        minDist = distance;
                        cpX = i;
                        cpY = j;
                        patchInd = k;
                    }
                }
            }

        }

        return GL_TRUE;
    }

}
