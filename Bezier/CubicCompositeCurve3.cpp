#include "CubicCompositeCurve3.h"
#include <iostream>
#include <QRandomGenerator>

using namespace std;

namespace cagd {

    CubicCompositeCurve3::ArcAttributes::ArcAttributes():
       arc(new CubicBezierArc3()),
       image(nullptr),
       previous(nullptr),
       next(nullptr)
    {
        colorInd = QRandomGenerator::global()->bounded(9);
    }

    CubicCompositeCurve3::ArcAttributes::ArcAttributes(const ArcAttributes& arcAttribute)
    {
        if (arcAttribute.arc)
           this->arc = new CubicBezierArc3(*(arcAttribute.arc));
        else
           arc = nullptr;

        colorInd = arcAttribute.colorInd;

        this->previous = arcAttribute.previous;
        this->next = arcAttribute.next;

        if (arcAttribute.image)
           image = new GenericCurve3(*(arcAttribute.image));
        else
           image = nullptr;
    }

    CubicCompositeCurve3::ArcAttributes::ArcAttributes(CubicBezierArc3 *arc)
    {
        this->arc = new CubicBezierArc3(*arc);

        image = this->arc->GenerateImage(2, 100); // 100 div points
        if (!image->UpdateVertexBufferObjects())
        {
           throw Exception("Could not update the VBO of arc");
        }

        colorInd = QRandomGenerator::global()->bounded(9);
        next = nullptr;
        previous = nullptr;
    }

    CubicCompositeCurve3::ArcAttributes& CubicCompositeCurve3::ArcAttributes::operator=(const ArcAttributes &attribute){
        if (&attribute == this)
        {
            return *this;
        }

        if (attribute.arc)
        {
            arc = new CubicBezierArc3(*attribute.arc);
            image = new GenericCurve3(*attribute.image);
        }
        else
        {
            arc = nullptr;
            image = nullptr;
        }

        colorInd = attribute.colorInd;

        previous = attribute.previous;
        next = attribute.next;

        return *this;
    }

    CubicCompositeCurve3::ArcAttributes::~ArcAttributes()
    {
        if (image)
        {
            delete image;
            image = nullptr;
        }
        if (arc)
        {
            delete arc;
            arc = nullptr;
        }
    }

    CubicCompositeCurve3::CubicCompositeCurve3(GLuint arcCount):
       _div_point_count(100)
    {
        _attributes.reserve(100);
        for (GLuint i = 0; i < arcCount; i++)
        {
            ArcAttributes* newAttr = new ArcAttributes;
            _attributes.push_back(*newAttr);
            _attributes[i].arc = InitializeArc();
            UpdateImageOfArc(i);
        }
    }

    CubicCompositeCurve3::~CubicCompositeCurve3()
    {
        _attributes.clear();
        _colors.clear();
    }

    CubicBezierArc3* CubicCompositeCurve3::InitializeArc()
    {
       CubicBezierArc3* arc = new CubicBezierArc3();

       (*arc)[0] = DCoordinate3(0.0, 1.0, 2.0);
       (*arc)[1] = DCoordinate3(0.0, 0.0, 0.0);
       (*arc)[2] = DCoordinate3(2.0, 0.0, 0.0);
       (*arc)[3] = DCoordinate3(0.0, 1.0, 0.0);

       if (!arc->UpdateVertexBufferObjectsOfData())
       {
            throw Exception("Could not update the VBO of data of the arc");
       }

       return arc;
    }

    GLboolean CubicCompositeCurve3::InsertNewArc()
    {
        try {
            ArcAttributes attribute;
            _attributes.push_back(attribute);
            _attributes.back().arc = InitializeArc();
            _attributes.back().image = _attributes.back().arc -> GenerateImage(2, _div_point_count);
            _attributes.back().image -> UpdateVertexBufferObjects();
            return GL_TRUE;
        }  catch (Exception e) {
            cout << "Error in inserting a new arc!" << endl;
            return GL_FALSE;
        }
    }

    GLboolean CubicCompositeCurve3::UpdateArc(
            const GLuint arcIndex,
            const GLuint pointIndex,
            const DCoordinate3 position)
    {
        if (arcIndex >= _attributes.size() || pointIndex > 3)
        {
            cout << "Invalid arc or control point index!" << endl;
            return GL_FALSE;
        }

        ArcAttributes* attribute = &_attributes[arcIndex];

        DCoordinate3 difference = (*attribute->arc)[pointIndex] - position;

        (*attribute->arc)[pointIndex] = position;

        if (pointIndex == 0 && attribute->previous)
        {
            if (attribute->previous->next == attribute) // previous - next
            {
                (*attribute->previous->arc)[3] = position;
                (*attribute->previous->arc)[2] -= difference;
                (*attribute->arc)[1] -= difference;
            }
            else // previous - previous
            {
                (*attribute->previous->arc)[0] = position;
                (*attribute->previous->arc)[1] -= difference;
                (*attribute->arc)[1] -= difference;
            }
        }
        else if (pointIndex == 1 && attribute->previous)
        {
            if (attribute->previous->next == attribute) // previous - next
            {
                (*attribute->previous->arc)[2] = 2 * (*attribute->arc)[0] - (*attribute->arc)[1];
            }
            else // previous - previous
            {
                (*attribute->previous->arc)[1] = 2 * (*attribute->arc)[0] - (*attribute->arc)[1];
            }
        }
        else if (pointIndex == 2 && attribute->next)
        {
            if (attribute->next->previous == attribute) // next - previous
            {
                (*attribute->next->arc)[1] = 2 * (*attribute->arc)[3] - (*attribute->arc)[2];
            }
            else // next - next
            {
                (*attribute->next->arc)[2] = 2 * (*attribute->arc)[3] - (*attribute->arc)[2];
            }
        }
        else if (pointIndex == 3 && attribute->next)
        {
            if (attribute->next->previous == attribute) // next - previous
            {
                (*attribute->next->arc)[0] = position;
                (*attribute->next->arc)[1] -= difference;
                (*attribute->arc)[2] -= difference;
            }
            else // next - next
            {
                (*attribute->next->arc)[3] = position;
                (*attribute->next->arc)[2] -= difference;
                (*attribute->arc)[2] -= difference;
            }
        }

        if (attribute->previous && (pointIndex == 0 || pointIndex == 1))
        {
            if (!attribute->previous->arc->UpdateVertexBufferObjectsOfData())
            {
                throw Exception("Could not update the VBO of data of arc!");
            }

            attribute->previous->image = attribute->previous->arc->GenerateImage(2, _div_point_count);
            if (!attribute->image)
            {
                throw Exception("Could not generate the image of arc!");
            }

            if (!attribute->previous->image->UpdateVertexBufferObjects())
            {
                throw Exception("Could not update the VBO of arc image");
            }
        }
        else if (attribute->next && (pointIndex == 2 || pointIndex == 3))
        {
            if (!attribute->next->arc->UpdateVertexBufferObjectsOfData())
            {
                throw Exception("Could not update the VBO of data of arc!");
            }

            attribute->next->image = attribute->next->arc->GenerateImage(2, _div_point_count);
            if (!attribute->image)
            {
                throw Exception("Could not generate the image of arc!");
            }

            if (!attribute->next->image->UpdateVertexBufferObjects())
            {
                throw Exception("Could not update the VBO of arc image");
            }
        }

        if (!attribute->arc->UpdateVertexBufferObjectsOfData())
        {
            throw Exception("Could not update the VBO of data of arc!");
        }

        attribute->image = attribute->arc->GenerateImage(2, _div_point_count);
        if (!attribute->image)
        {
            throw Exception("Could not generate the image of arc!");
        }

        if (!attribute->image->UpdateVertexBufferObjects())
        {
            throw Exception("Could not update the VBO of arc image");
        }

        return GL_TRUE;
    }

    GLboolean CubicCompositeCurve3::UpdateImageOfArc(const GLuint arcIndex)
    {
        ArcAttributes* attribute = &_attributes[arcIndex];
        if (!attribute->arc->UpdateVertexBufferObjectsOfData())
        {
            return GL_FALSE;
        }

        attribute->image = attribute->arc->GenerateImage(2, _div_point_count);
        if (!attribute->image || !attribute->image->UpdateVertexBufferObjects())
        {
            return GL_FALSE;
        }

        return GL_TRUE;
    }

    GLboolean CubicCompositeCurve3::UpdateImageOfAllArcs()
    {
        for (GLuint i = 0; i < _attributes.size(); i++)
            if (!UpdateImageOfArc(i))
                return GL_FALSE;
        return GL_TRUE;
    }

    int CubicCompositeCurve3::mouseOnCurve(DCoordinate3 mC)
    {
        int clickedArc = -1;
        GLdouble minDist = 0.1;
        GLdouble x = mC.x();
        GLdouble y = mC.y();
        for (GLuint j = 0; j < _attributes.size(); j++)
        {
            ArcAttributes* it = &_attributes[j];
            if (it ->image)
            {
                DCoordinate3 c;
                GLuint pointCount;
                pointCount = it -> image -> GetPointCount();
                for (GLuint i = 0; i < pointCount; i++)
                {
                    it -> image -> GetDerivative(0, i, c);
                    GLdouble tX = c.x();
                    GLdouble tY = c.y();
                    GLdouble distance = (x - tX) * (x - tX) + (y - tY) * (y - tY);
                    if (distance < minDist){
                        minDist = distance;
                        clickedArc = j;
                    }
                }
            }
        }

        return clickedArc;
    }

    int CubicCompositeCurve3::mouseOnCP(int arcInd, DCoordinate3 mC)
    {
        GLdouble minDist = 0.2;
        GLdouble x = mC.x();
        GLdouble y = mC.y();
        int clickedCP = -1;
        ArcAttributes* selectedArc = &_attributes[arcInd];
        DCoordinate3 c;
        for (GLuint i = 0; i < 4; i++)
        {
            selectedArc -> arc -> GetData(i, c);
            GLdouble tX = c.x();
            GLdouble tY = c.y();
            GLdouble distance = (x - tX) * (x - tX) + (y - tY) * (y - tY);
            cout << x << ' ' << tX << ' ' << y << ' ' << tY << endl;
            if (distance < minDist){
                minDist = distance;
                clickedCP = i;
            }
        }

        return clickedCP;
    }

    GLboolean CubicCompositeCurve3::mouseNotOnCurveOnCP(DCoordinate3 mC, int &arcInd, int &cp)
    {
        GLdouble minDist = 0.1;
        GLdouble x = mC.x();
        GLdouble y = mC.y();
        for (GLuint j = 0; j < _attributes.size(); j++)
        {
            ArcAttributes* selectedArc = &_attributes[j];
            DCoordinate3 c;
            for (GLuint i = 0; i < 4; i++)
            {
                selectedArc -> arc -> GetData(i, c);
                GLdouble tX = c.x();
                GLdouble tY = c.y();
                GLdouble distance = (x - tX) * (x - tX) + (y - tY) * (y - tY);
                cout << x << ' ' << tX << ' ' << y << ' ' << tY << endl;
                if (distance < minDist){
                    minDist = distance;
                    arcInd = j;
                    cp = i;
                }
            }
        }

        return GL_TRUE;
    }

    GLboolean CubicCompositeCurve3::JoinExistingArcs(
            const GLuint &firstArcIndex,
            Direction firstDirection,
            const GLuint &secondArcIndex,
            Direction secondDirection)
    {
        if (firstArcIndex >= _attributes.size() || secondArcIndex >= _attributes.size())
        {
            cout << "Invalid arc index(es)!" << endl;
            return GL_FALSE;
        }

        ArcAttributes &firstAttribute = _attributes[firstArcIndex];
        ArcAttributes &secondAttribute = _attributes[secondArcIndex];

        cout << firstDirection << secondDirection << endl;

        if ((firstDirection == LEFT && firstAttribute.previous) ||
            (firstDirection == RIGHT && firstAttribute.next) ||
            (secondDirection == LEFT && secondAttribute.previous) ||
            (secondDirection == RIGHT && secondAttribute.next))
        {
            cout << "One of the arcs is already joined in the given direction!" << endl;
            return GL_FALSE;
        }

        ArcAttributes attribute;
        _attributes.push_back(attribute);

        ArcAttributes &connectingAttribute = _attributes.back();

        connectingAttribute.arc = new CubicBezierArc3();
        CubicBezierArc3* arc = connectingAttribute.arc;

        if (firstDirection == LEFT)
        {
            (*arc)[0] = (*firstAttribute.arc)[0];
            (*arc)[1] = 2 * (*firstAttribute.arc)[0] - (*firstAttribute.arc)[1];

            firstAttribute.previous = &connectingAttribute;
        }
        else if (firstDirection == RIGHT)
        {
            (*arc)[0] = (*firstAttribute.arc)[3];
            (*arc)[1] = 2 * (*firstAttribute.arc)[3] - (*firstAttribute.arc)[2];

            firstAttribute.next = &connectingAttribute;
        }

        if (secondDirection == LEFT)
        {
            (*arc)[3] = (*secondAttribute.arc)[0];
            (*arc)[2] = 2 * (*secondAttribute.arc)[0] - (*secondAttribute.arc)[1];

            secondAttribute.previous = &connectingAttribute;
        }
        else if (secondDirection == RIGHT)
        {
            (*arc)[3] = (*secondAttribute.arc)[3];
            (*arc)[2] = 2 * (*secondAttribute.arc)[3] - (*secondAttribute.arc)[2];

            secondAttribute.next = &connectingAttribute;
        }

        if (!connectingAttribute.arc->UpdateVertexBufferObjectsOfData())
        {
            throw Exception("Could not update the VBO of data of the arc");
        }

        connectingAttribute.image = connectingAttribute.arc->GenerateImage(2, _div_point_count);
        if (!connectingAttribute.image)
        {
            throw Exception("Could not generate the image of arc!");
        }

        if (!connectingAttribute.image->UpdateVertexBufferObjects())
        {
            throw Exception("Could not update the VBO of arc image");
        }

        connectingAttribute.previous = &firstAttribute;
        connectingAttribute.next = &secondAttribute;
        return GL_TRUE;
    }

    GLboolean CubicCompositeCurve3::MergeExistingArcs(
            const GLuint &firstArcIndex,
            Direction firstDirection,
            const GLuint &secondArcIndex,
            Direction secondDirection)
    {
        if (firstArcIndex >= _attributes.size() || secondArcIndex >= _attributes.size())
        {
            cout << "Invalid arc index(es)!" << endl;
            return GL_FALSE;
        }

        ArcAttributes &firstAttribute = _attributes[firstArcIndex];
        ArcAttributes &secondAttribute = _attributes[secondArcIndex];

        if ((firstDirection == LEFT && firstAttribute.previous) ||
            (firstDirection == RIGHT && firstAttribute.next) ||
            (secondDirection == LEFT && secondAttribute.previous) ||
            (secondDirection == RIGHT && secondAttribute.next))
        {
            cout << "One of the arcs arlready has a neighbor in the given direction!" << endl;
            return GL_FALSE;
        }

        if (firstDirection == LEFT && secondDirection == LEFT)
        {
            (*firstAttribute.arc)[0] = (*secondAttribute.arc)[0] = 0.5 * ((*firstAttribute.arc)[1] + (*secondAttribute.arc)[1]);

            firstAttribute.previous = &secondAttribute;
            secondAttribute.previous = &firstAttribute;
        }
        else if (firstDirection == RIGHT && secondDirection == RIGHT)
        {
            (*firstAttribute.arc)[3] = (*secondAttribute.arc)[3] = 0.5 * ((*firstAttribute.arc)[2] + (*secondAttribute.arc)[2]);

            firstAttribute.next = &secondAttribute;
            secondAttribute.next = &firstAttribute;
        }
        else if (firstDirection == LEFT && secondDirection == RIGHT)
        {
            (*firstAttribute.arc)[0] = (*secondAttribute.arc)[3] = 0.5 * ((*firstAttribute.arc)[1] + (*secondAttribute.arc)[2]);

            firstAttribute.previous = &secondAttribute;
            secondAttribute.next = &firstAttribute;
        }
        else if (firstDirection == RIGHT && secondDirection == LEFT)
        {
            (*firstAttribute.arc)[3] = (*secondAttribute.arc)[0] = 0.5 * ((*firstAttribute.arc)[2] + (*secondAttribute.arc)[1]);

            firstAttribute.next = &secondAttribute;
            secondAttribute.previous = &firstAttribute;
        }

        if (!firstAttribute.arc->UpdateVertexBufferObjectsOfData())
        {
            throw Exception("Could not update the VBO of data of the arc");
        }
        if (!secondAttribute.arc->UpdateVertexBufferObjectsOfData())
        {
            throw Exception("Could not update the VBO of data of the arc");
        }

        firstAttribute.image = firstAttribute.arc->GenerateImage(2, _div_point_count);
        secondAttribute.image = secondAttribute.arc->GenerateImage(2, _div_point_count);
        if (!firstAttribute.image || !secondAttribute.image)
        {
            throw Exception("Could not generate the image of arc(s)!");
        }

        if (!firstAttribute.image->UpdateVertexBufferObjects())
        {
            throw Exception("Could not update the VBO of arc image");
        }
        if (!secondAttribute.image->UpdateVertexBufferObjects())
        {
            throw Exception("Could not update the VBO of arc image");
        }

        return GL_TRUE;
    }

    GLboolean CubicCompositeCurve3::ContinueExistingArc(const GLuint &arcIndex, Direction direction)
    {
        if (arcIndex >= _attributes.size())
        {
            cout << "Invalid arc index!" << endl;
            return GL_FALSE;
        }

        ArcAttributes* attribute = &_attributes[arcIndex];

        if ((direction == LEFT && attribute->previous) ||
            (direction == RIGHT && attribute->next))
        {
            cout << "The arc already has a neighbor in the given direction!" << endl;
            return GL_FALSE;
        }

        ArcAttributes newAttr;
        _attributes.push_back(newAttr);
        ArcAttributes* newAttribute = &_attributes.back();
        attribute = &_attributes[arcIndex];

        newAttribute->arc = new CubicBezierArc3();
        CubicBezierArc3* arc = newAttribute->arc;

        if (direction == LEFT)
        {
            (*arc)[3] = (*attribute->arc)[0];
            (*arc)[2] = 2 * (*attribute->arc)[0] - (*attribute->arc)[1];
            (*arc)[1] = 2 * (*arc)[2] - (*arc)[3];
            (*arc)[0] = 2 * (*arc)[1] - (*arc)[2];

            attribute->previous = newAttribute;
            newAttribute->next = attribute;
        }
        else if (direction == RIGHT)
        {
            (*arc)[0] = (*attribute->arc)[3];
            (*arc)[1] = 2 * (*attribute->arc)[3] - (*attribute->arc)[2];
            (*arc)[2] = 2 * (*arc)[1] - (*arc)[0];
            (*arc)[3] = 2 * (*arc)[2] - (*arc)[1];

            attribute->next = newAttribute;
            newAttribute->previous = attribute;
        }

        if (!newAttribute->arc->UpdateVertexBufferObjectsOfData())
        {
            throw Exception("Could not update the VBO of data of the arc");
        }

        newAttribute->image = newAttribute->arc->GenerateImage(2, _div_point_count);
        if (!newAttribute->image)
        {
            throw Exception("Could not generate the image of arc!");
        }

        if (!newAttribute->image->UpdateVertexBufferObjects())
        {
            throw Exception("Could not update the VBO of arc image");
        }

        return GL_TRUE;
    }

    GLint CubicCompositeCurve3::IndexOfAttribute(const ArcAttributes &attribute) const
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

    GLboolean CubicCompositeCurve3::RenderAllArcs()
    {
        glPointSize(6.0);

            for (auto it = _attributes.begin(); it != _attributes.end(); ++it)
            {
                if (it->image)
                {
                    glColor3f(_colors[it->colorInd].r(), _colors[it->colorInd].g(), _colors[it->colorInd].b());
                    it->image->RenderDerivatives(0, GL_LINE_STRIP);
                }
            }
        glPointSize(1.0);
        return GL_TRUE;
    }

    GLboolean CubicCompositeCurve3::RenderAllFirstOrderDerivatives()
    {
        glPointSize(6.0);

        for (auto it = _attributes.begin(); it != _attributes.end(); ++it)
        {
            if (it->image)
            {
                glColor3f(_colors[it->colorInd].r(), _colors[it->colorInd].g(), _colors[it->colorInd].b());
                it->image->RenderDerivatives(1, GL_LINES);
                it->image->RenderDerivatives(1, GL_POINTS);
            }
        }
        glPointSize(1.0);
        return GL_TRUE;
    }

    GLboolean CubicCompositeCurve3::RenderAllSecondOrderDerivatives()
    {
        glPointSize(6.0);

        for (auto it = _attributes.begin(); it != _attributes.end(); ++it)
        {
            if (it->image)
            {
                glColor3f(_colors[it->colorInd].r(), _colors[it->colorInd].g(), _colors[it->colorInd].b());
                it->image->RenderDerivatives(2, GL_LINES);
                it->image->RenderDerivatives(2, GL_POINTS);
            }
        }
        glPointSize(1.0);
        return GL_TRUE;
    }

    GLboolean CubicCompositeCurve3::RenderAllData(GLuint selectedCurveInd, GLuint selectedPointInd)
    {
        glPointSize(10.0f);
        glBegin(GL_POINTS);
            glVertex3dv(&(*_attributes[selectedCurveInd].arc)[selectedPointInd][0]);
        glEnd();
        glPointSize(1.0f);

        for (auto it = _attributes.begin(); it != _attributes.end(); ++it)
        {
            it->arc->RenderData();
        }

        return GL_TRUE;
    }

    GLboolean CubicCompositeCurve3::RenderHighlightedArcs(GLuint arcInd1, int arcInd2)
    {
        glColor3f(1.0f, 0.0f, 0.0f);
        _attributes[arcInd1].image->RenderDerivatives(0, GL_LINE_STRIP);
        glColor3f(0.0f, 1.0f, 0.0f);
        _attributes[arcInd2].image->RenderDerivatives(0, GL_LINE_STRIP);

        return GL_TRUE;
    }

    GLboolean CubicCompositeCurve3::GetDataPointValues(
            const GLuint &arcInd, const GLuint &dataPointInd, GLdouble &x, GLdouble &y, GLdouble &z)
    {
        DCoordinate3 dataPoint = (*_attributes[arcInd].arc)[dataPointInd];
        x = dataPoint.x();
        y = dataPoint.y();
        z = dataPoint.z();
        return GL_TRUE;
    }

    GLboolean CubicCompositeCurve3::GetDataPointValues(
            const GLuint &arcInd, const GLuint &dataPointInd, DCoordinate3 &p)
    {
        p = (*_attributes[arcInd].arc)[dataPointInd];
        return GL_TRUE;
    }

    GLboolean CubicCompositeCurve3::ChangeColor(GLuint arcInd, GLuint colorInd)
    {
        if (arcInd >= _attributes.size())
        {
            return GL_FALSE;
        }
        if (colorInd >= _colors.size())
        {
            return GL_FALSE;
        }
        _attributes[arcInd].colorInd = colorInd;

        return GL_TRUE;
    }

    GLuint CubicCompositeCurve3::GetColorInd(GLuint arcInd)
    {
        return _attributes[arcInd].colorInd;
    }

    int CubicCompositeCurve3::GetArcCount()
    {
        return _attributes.size();
    }

    std::ostream& operator << (std::ostream& lhs, const CubicCompositeCurve3& rhs)
    {
        lhs << rhs._div_point_count << endl;
        lhs << rhs._attributes.size() << endl;

        // attributes
        for (auto it = rhs._attributes.begin(); it != rhs._attributes.end(); ++it)
        {
            lhs << *it->arc;
            lhs << it->colorInd << endl;
        }

        // connections
        for (auto it = rhs._attributes.begin(); it != rhs._attributes.end(); ++it)
        {
            lhs << rhs.IndexOfAttribute(*it->previous) << " "
                << rhs.IndexOfAttribute(*it->next) << endl;
        }

        return lhs;
    }

    std::istream& operator >> (std::istream& lhs, CubicCompositeCurve3& rhs)
    {
        GLint n;

        lhs >> rhs._div_point_count;
        lhs >> n;

        rhs._attributes.resize(n);

        // attributes
        for (auto it = rhs._attributes.begin(); it != rhs._attributes.end(); ++it)
        {
            lhs >> *it->arc;
            lhs >> it->colorInd;
        }

        rhs.UpdateImageOfAllArcs();

        // connections
        for (auto it = rhs._attributes.begin(); it != rhs._attributes.end(); ++it)
        {
            lhs >> n;
            if (n == -1)
            {
                it->previous = nullptr;
            }
            else
            {
                it->previous = &rhs._attributes[n];
            }
            lhs >> n;
            if (n == -1)
            {
                it->next = nullptr;
            }
            else
            {
                it->next = &rhs._attributes[n];
            }
        }

        return lhs;
    }
}
