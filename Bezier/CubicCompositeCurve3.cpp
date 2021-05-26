#include "CubicCompositeCurve3.h"
#include <iostream>

using namespace std;

namespace cagd {

   CubicCompositeCurve3::ArcAttributes::ArcAttributes():arc(new CubicBezierArc3()), image(nullptr), previous(nullptr), next(nullptr)
   {
       color = new Color4(0.2f, 0.4f, 0.3f, 1.0f);
   }

   CubicCompositeCurve3::ArcAttributes::ArcAttributes(const ArcAttributes& arcAttribute)
       {
           if (arcAttribute.arc)
               this->arc = new CubicBezierArc3(*(arcAttribute.arc));
           else
               arc = nullptr;

           if (arcAttribute.color)
               color = new Color4(*(arcAttribute.color));
           else
               color = nullptr;

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

           color = new Color4(1.0f, 0.0f, 0.0f, 1.0f); // red
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
            }else
            {
                arc = nullptr;
                image = nullptr;
            }

            if (attribute.color)
            {
               color = new Color4(*(attribute.color));
            }else{
               color = nullptr;
            }

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
           if (color)
           {
               delete color;
               color = nullptr;
           }
           if (arc)
           {
               delete arc;
               arc = nullptr;
           }
       }

       CubicCompositeCurve3::CubicCompositeCurve3
}
