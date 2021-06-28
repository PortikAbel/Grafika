#ifndef CUBICCOMPOSITECURVE3_H
#define CUBICCOMPOSITECURVE3_H

#endif // CUBICCOMPOSITECURVE3_H
#pragma once

#include "CubicBezierArcs3.h"
#include <Core/Colors4.h>
#include <Core/Constants.h>
#include <Core/Exceptions.h>


namespace cagd
{
    class CubicCompositeCurve3;

    std::ostream& operator << (std::ostream& lhs, const CubicCompositeCurve3& rhs);
    std::istream& operator >> (std::istream& lhs, CubicCompositeCurve3& rhs);

    class CubicCompositeCurve3
    {        
        friend std::ostream& operator << (std::ostream& lhs, const CubicCompositeCurve3& rhs);
        friend std::istream& operator >> (std::istream& lhs, CubicCompositeCurve3& rhs);

    public:
        enum Direction {LEFT, RIGHT};

        class ArcAttributes
        {
        public:
            CubicBezierArc3   *arc;
            GenericCurve3   *image;
            //Color4          *color;
            ArcAttributes   *previous, *next;
            GLuint          colorInd;

            ArcAttributes();
            ArcAttributes(CubicBezierArc3 *arc);
            ArcAttributes(const ArcAttributes&);
            ArcAttributes& operator=(const ArcAttributes&);
            ~ArcAttributes();
        };
    protected:
        std::vector<ArcAttributes> _attributes;
        std::vector<Color4>        _colors{ darkRed, yellow, darkGreen, green, darkBlue, blue, purple, pink, orange, grey };
        GLuint _div_point_count;

    public:
        CubicCompositeCurve3(GLuint arcCount = 0);
        ~CubicCompositeCurve3();

        CubicBezierArc3* InitializeArc();
        GLboolean UpdateArc(const GLuint arcIndex, const GLuint pointIndex, const DCoordinate3 position);
        GLboolean UpdateImageOfArc(const GLuint arcIndex);
        GLboolean UpdateImageOfAllArcs();
        GLboolean InsertNewArc();
        GLboolean DeleteArc(const GLuint &arcInd);

        GLint     IndexOfAttribute(const ArcAttributes &attribute) const;

        GLboolean ContinueExistingArc(const GLuint &index, Direction dir);
        GLboolean JoinExistingArcs(const GLuint &arc_ind1, Direction dir1, const GLuint &arc_ind2, Direction dir2);
        GLboolean MergeExistingArcs(const GLuint &arc_ind1, Direction dir1, const GLuint &arc_ind2, Direction dir2);

        GLboolean RenderAllArcs();
        GLboolean RenderAllFirstOrderDerivatives();
        GLboolean RenderAllSecondOrderDerivatives();
        GLboolean RenderAllData(GLuint selectedCurveInd, GLuint selectedPointInd);
        GLboolean RenderHighlightedArcs(GLuint arcInd1, int arcInd2);

        GLboolean GetDataPointValues(const GLuint &arcInd, const GLuint &dataPointInd, DCoordinate3 &p);
        GLboolean GetDataPointValues(const GLuint &arcInd, const GLuint &dataPointInd, GLdouble &x, GLdouble &y, GLdouble &z);
        GLboolean ChangeDataPointValue(const GLuint &arcInd, const GLuint &dataPointInd, const GLuint &pointComponentInd, const GLdouble &val);

        int       GetArcCount();

        GLboolean ChangeColor(GLuint arcInd, GLuint colorInd);
        GLuint    GetColorInd(GLuint arcInd);
        int       mouseOnCurve(DCoordinate3 mC);
        int       mouseOnCP(int arcInd, DCoordinate3 mC);
        GLboolean mouseNotOnCurveOnCP(DCoordinate3 mC, int &arcInd, int &cp);

    };
}
