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
    class CubicCompositeCurve3
    {
    public:
        enum Direction {LEFT, RIGHT};

        class ArcAttributes
        {
        public:
            CubicBezierArc3   *arc;
            GenericCurve3   *image;
            Color4          *color;
            ArcAttributes   *previous, *next;

            ArcAttributes();
            ArcAttributes(CubicBezierArc3 *arc);
            ArcAttributes(const ArcAttributes&);
            ArcAttributes& operator=(const ArcAttributes&);
            ~ArcAttributes();

            GLboolean selected;
        };
    protected:
        std::vector<ArcAttributes> _attributes;
        GLuint _div_point_count = 100;

    public:
        CubicCompositeCurve3(const size_t min_arc_count = 100);
        ~CubicCompositeCurve3();
        GLboolean InsertNewArc();
        GLboolean JoinExistingArcs(const GLuint &arc_ind1, Direction dir1, const GLuint &arc_ind2, Direction dir2);
        GLboolean ContinueExistingArc(const GLuint &index, Direction dir);
        GLboolean MergeExistingArcs(const GLuint &arc_ind1, Direction dir1,
                                    const GLuint &arc_ind2, Direction dir2);
        GLvoid RenderAllArcs(const int &d1, const int &d2, int arcInd, int render_data);
        GLvoid RenderSelectedArc(const int &d1, const int &d2, GLuint arcInd, int render_data);
        GLboolean ChangeDataPointValue(const GLuint &arcInd, const GLuint &dataPointInd, const GLuint &pointComponentInd, const GLdouble &val);
        GLboolean GetDataPointValues(const GLuint &arcInd, const GLuint &dataPointInd, GLdouble &x, GLdouble &y, GLdouble &z);
        GLboolean GetColorComponents(const GLuint &arcInd, GLdouble &r, GLdouble &g, GLdouble &b);
        GLboolean ChangeColorComponentValue(const GLuint &arcInd, const GLuint &colorComponentInd, const GLdouble &val);
        GLboolean DeleteArc(const GLuint &arcInd);
    };
}
