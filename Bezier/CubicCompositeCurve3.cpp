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

       CubicCompositeCurve3::CubicCompositeCurve3(GLuint max_arc_count): _div_point_count(100)
       {
           _attributes.reserve(max_arc_count);
       }

       CubicCompositeCurve3::~CubicCompositeCurve3()
       {
               _attributes.clear();
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

      GLboolean CubicCompositeCurve3::UpdateArc(const GLuint arcIndex, const GLuint pointIndex, const DCoordinate3 position)
          {
              if (arcIndex >= _attributes.size() || pointIndex > 3)
              {
                  cout << "Invalid arc or control point index!" << endl;
                  return GL_FALSE;
              }

              ArcAttributes &attribute = _attributes[arcIndex];

              DCoordinate3 difference = (*attribute.arc)[pointIndex] - position;

              (*attribute.arc)[pointIndex] = position;

              if (pointIndex == 0 && attribute.previous)
              {
                  if (attribute.previous->next == &attribute) // previous - next
                  {
                      (*attribute.previous->arc)[3] = position;
                      (*attribute.previous->arc)[2] -= difference;
                      (*attribute.arc)[1] -= difference;
                  }
                  else // previous - previous
                  {
                      (*attribute.previous->arc)[0] = position;
                      (*attribute.previous->arc)[1] -= difference;
                      (*attribute.arc)[1] -= difference;
                  }
              }
              else if (pointIndex == 1 && attribute.previous)
              {
                  if (attribute.previous->next == &attribute) // previous - next
                  {
                      (*attribute.previous->arc)[2] = 2 * (*attribute.arc)[0] - (*attribute.arc)[1];
                  }
                  else // previous - previous
                  {
                      (*attribute.previous->arc)[1] = 2 * (*attribute.arc)[0] - (*attribute.arc)[1];
                  }
              }
              else if (pointIndex == 2 && attribute.next)
              {
                  if (attribute.next->previous == &attribute) // next - previous
                  {
                      (*attribute.next->arc)[1] = 2 * (*attribute.arc)[3] - (*attribute.arc)[2];
                  }
                  else // next - next
                  {
                      (*attribute.next->arc)[2] = 2 * (*attribute.arc)[3] - (*attribute.arc)[2];
                  }
              }
              else if (pointIndex == 3 && attribute.next)
              {
                  if (attribute.next->previous == &attribute) // next - previous
                  {
                      (*attribute.next->arc)[0] = position;
                      (*attribute.next->arc)[1] -= difference;
                      (*attribute.arc)[2] -= difference;
                  }
                  else // next - next
                  {
                      (*attribute.next->arc)[3] = position;
                      (*attribute.next->arc)[2] -= difference;
                      (*attribute.arc)[2] -= difference;
                  }
              }

              if (attribute.previous && (pointIndex == 0 || pointIndex == 1))
              {
                  if (!attribute.previous->arc->UpdateVertexBufferObjectsOfData())
                  {
                      throw Exception("Could not update the VBO of data of arc!");
                  }

                  attribute.previous->image = attribute.previous->arc->GenerateImage(2, _div_point_count);
                  if (!attribute.image)
                  {
                      throw Exception("Could not generate the image of arc!");
                  }

                  if (!attribute.previous->image->UpdateVertexBufferObjects())
                  {
                      throw Exception("Could not update the VBO of arc image");
                  }
              }
              else if (attribute.next && (pointIndex == 2 || pointIndex == 3))
              {
                  if (!attribute.next->arc->UpdateVertexBufferObjectsOfData())
                  {
                      throw Exception("Could not update the VBO of data of arc!");
                  }

                  attribute.next->image = attribute.next->arc->GenerateImage(2, _div_point_count);
                  if (!attribute.image)
                  {
                      throw Exception("Could not generate the image of arc!");
                  }

                  if (!attribute.next->image->UpdateVertexBufferObjects())
                  {
                      throw Exception("Could not update the VBO of arc image");
                  }
              }

              if (!attribute.arc->UpdateVertexBufferObjectsOfData())
              {
                  throw Exception("Could not update the VBO of data of arc!");
              }

              attribute.image = attribute.arc->GenerateImage(2, _div_point_count);
              if (!attribute.image)
              {
                  throw Exception("Could not generate the image of arc!");
              }

              if (!attribute.image->UpdateVertexBufferObjects())
              {
                  throw Exception("Could not update the VBO of arc image");
              }

              return GL_TRUE;
          }

          GLboolean CubicCompositeCurve3::JoinExistingArcs(const GLuint &firstArcIndex, Direction firstDirection, const GLuint &secondArcIndex, Direction secondDirection)
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

          GLboolean CubicCompositeCurve3::MergeExistingArcs(const GLuint &firstArcIndex, Direction firstDirection, const GLuint &secondArcIndex, Direction secondDirection)
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

              ArcAttributes &attribute = _attributes[arcIndex];

              if ((direction == LEFT && attribute.previous) ||
                  (direction == RIGHT && attribute.next))
              {
                  cout << "The arc already has a neighbor in the given direction!" << endl;
                  return GL_FALSE;
              }

              ArcAttributes newAttr;
              _attributes.push_back(newAttr);
              ArcAttributes &newAttribute = _attributes.back();

              newAttribute.arc = new CubicBezierArc3();
              CubicBezierArc3* arc = newAttribute.arc;

              if (direction == LEFT)
              {
                  (*arc)[3] = (*attribute.arc)[0];
                  (*arc)[2] = 2 * (*attribute.arc)[0] - (*attribute.arc)[1];
                  (*arc)[1] = 2 * (*arc)[2] - (*arc)[3];
                  (*arc)[0] = 2 * (*arc)[1] - (*arc)[2];

                  attribute.previous = &newAttribute;
                  newAttribute.next = &attribute;
              }
              else if (direction == RIGHT)
              {
                  (*arc)[0] = (*attribute.arc)[3];
                  (*arc)[1] = 2 * (*attribute.arc)[3] - (*attribute.arc)[2];
                  (*arc)[2] = 2 * (*arc)[1] - (*arc)[0];
                  (*arc)[3] = 2 * (*arc)[2] - (*arc)[1];

                  attribute.next = &newAttribute;
                  newAttribute.previous = &attribute;
              }

              if (!newAttribute.arc->UpdateVertexBufferObjectsOfData())
              {
                  throw Exception("Could not update the VBO of data of the arc");
              }

              newAttribute.image = newAttribute.arc->GenerateImage(2, _div_point_count);
              if (!newAttribute.image)
              {
                  throw Exception("Could not generate the image of arc!");
              }

              if (!newAttribute.image->UpdateVertexBufferObjects())
              {
                  throw Exception("Could not update the VBO of arc image");
              }

              return GL_TRUE;
          }

          GLboolean CubicCompositeCurve3::RenderAllArcs()
          {
              glPointSize(6.0);

              for (auto it = _attributes.begin(); it != _attributes.end(); ++it)
              {
                  if (it->image)
                  {
                      if(it->selected){
                          glColor3f(1.0, 1.0, 1.0);
                          it->image->RenderDerivatives(0, GL_LINE_STRIP);
                      }
                      else{
                          glColor3f(it->color->r(), it->color->g(), it->color->b());
                          it->image->RenderDerivatives(0, GL_LINE_STRIP);
                      }
                  }

              }
              glPointSize(1.0);
              return GL_TRUE;
          }
}
