/*******************************************************************************
*   Copyright 2013 EPFL                                                        *
*                                                                              *
*   This file is part of metroscope.                                           *
*                                                                              *
*   Metroscope is free software: you can redistribute it and/or modify         *
*   it under the terms of the GNU General Public License as                    *
*   published by the Free Software Foundation, either version 3 of the         *
*   License, or (at your option) any later version.                            *
*                                                                              *
*   Metroscope is distributed in the hope that it will be useful,              *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of             *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
*   GNU General Public License for more details.                               *
*                                                                              *
*   You should have received a copy of the GNU General Public License          *
*   along with Metroscope.  If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************/

/*
(***********************************************************************)
(*                                                                     *)
(* Wykobi Computational Geometry Library                               *)
(* Release Version 0.0.4                                               *)
(* http://www.wykobi.com                                               *)
(* Copyright (c) 2005-2009 Arash Partow, All Rights Reserved.          *)
(*                                                                     *)
(* The Wykobi computational geometry library and its components are    *)
(* supplied under the terms of the General Wykobi License agreement.   *)
(* The contents of the Wykobi computational geometry library and its   *)
(* components may not be copied or disclosed except in accordance with *)
(* the terms of that agreement.                                        *)
(*                                                                     *)
(* URL: http://www.wykobi.com/license.html                             *)
(*                                                                     *)
(***********************************************************************)
*/

#ifndef GEOMETRY_PRINTING_HPP
#define GEOMETRY_PRINTING_HPP

#include <iostream>
#include <iomanip>
#include <vector>

#include <wykobi/wykobi.hpp>
#include <wykobi/wykobi_matrix.hpp>

namespace Geometry
{
   template<typename Type>
   inline std::ostream& operator<<(std::ostream& os, const wykobi::point2d<Type>& point)
   {
      os << "(" << point.x << "," << point.y << ")";
      return os;
   }

   template<typename Type>
   inline std::ostream& operator<<(std::ostream& os, const wykobi::point3d<Type>& point)
   {
      os << "(" << point.x << "," << point.y << "," << point.z << ")";
      return os;
   }

   template<typename Type>
   inline std::ostream& operator<<(std::ostream& os, const wykobi::vector2d<Type>& v)
   {
      os << "(" << v.x << "," << v.y << ")";
      return os;
   }

   template<typename Type>
   inline std::ostream& operator<<(std::ostream& os, const wykobi::vector3d<Type>& v)
   {
      os << "(" << v.x << "," << v.y << "," << v.z << ")";
      return os;
   }

   template<typename Type>
   inline std::ostream& operator<<(std::ostream& os, const wykobi::ray<Type,2>& ray)
   {
      os << "(" << ray.origin.x << "," << ray.origin.y << "," << ray.direction.x << "," << ray.direction.y << ")";
      return os;
   }

   template<typename Type>
   inline std::ostream& operator<<(std::ostream& os, const wykobi::ray<Type,3>& ray)
   {
      os << "(" << ray.origin.x    << "," << ray.origin.y    << "," << ray.origin.z    << ","
                << ray.direction.x << "," << ray.direction.y << "," << ray.direction.z << ")";
      return os;
   }

   template<typename Type, size_t Dimension>
   inline std::ostream& operator<<(std::ostream& os, const wykobi::pointnd<Type,Dimension>& point)
   {
      os << "(";
      for(size_t i = 0; i < Dimension - 1; ++i)
      {
         os << point[i] << ",";
      }

      os << point[Dimension - 1] << ")";

      return os;
   }

   template<typename Type, size_t Dimension>
   inline std::ostream& operator<<(std::ostream& os, const wykobi::segment<Type,Dimension>& segment)
   {
      for(unsigned int i = 0; i < wykobi::segment<Type,Dimension>::PointCount; ++i)
      {
         os << segment[i];
      }
      return os;
   }

   template<typename Type, size_t Dimension>
   inline std::ostream& operator<<(std::ostream& os, const wykobi::line<Type,Dimension>& line)
   {
      for(unsigned int i = 0; i < wykobi::line<Type,Dimension>::PointCount; ++i)
      {
         os << line[i];
      }
      return os;
   }

   template<typename Type, size_t Dimension>
   inline std::ostream& operator<<(std::ostream& os, const wykobi::triangle<Type,Dimension>& triangle)
   {
      for(unsigned int i = 0; i < wykobi::triangle<Type,Dimension>::PointCount; ++i)
      {
         os << triangle[i];
      }
      return os;
   }

   template<typename Type>
   inline std::ostream& operator<<(std::ostream& os, const wykobi::rectangle<Type>& rectangle)
   {
      for(unsigned int i = 0; i < wykobi::rectangle<Type>::PointCount; ++i)
      {
         os << rectangle[i];
      }
      return os;
   }

   template<typename Type, size_t Dimension>
   inline std::ostream& operator<<(std::ostream& os, const wykobi::box<Type,Dimension>& box)
   {
      for(unsigned int i = 0; i < wykobi::box<Type,Dimension>::PointCount; ++i)
      {
         os << box[i];
      }
      return os;
   }

   template<typename Type, size_t Dimension>
   inline std::ostream& operator<<(std::ostream& os, const wykobi::quadix<Type,Dimension>& quadix)
   {
      for(unsigned int i = 0; i < wykobi::quadix<Type,Dimension>::PointCount; ++i)
      {
         os << quadix[i];
      }
      return os;
   }

   template<typename Type, size_t Dimension>
   inline std::ostream& operator<<(std::ostream& os, const wykobi::polygon<Type,Dimension>& polygon)
   {
	  for(unsigned int i = 0; i < polygon.size(); ++i)
	  {
		 os << polygon[i];
	  }
	  return os;
   }

   template<typename Type>
   inline std::ostream& operator<<(std::ostream& os, const wykobi::circle<Type>& circle)
   {
      os << "(" << circle.x << "," << circle.y << "," << circle.radius << ")";
      return os;
   }

   template<typename Type>
   inline std::ostream& operator<<(std::ostream& os, const wykobi::sphere<Type>& sphere)
   {
      os << "(" << sphere.x << "," << sphere.y << "," << sphere.z << "," << sphere.radius << ")";
      return os;
   }

   template<typename Type, size_t M, size_t N>
   inline std::ostream& operator<<(std::ostream& os, const wykobi::matrix<Type,M,N>& matrix)
   {
      for(size_t x = 0; x < M; x++)
      {
         for(size_t y = 0; y < N; y++)
         {
            os << matrix(x,y) << "\t";
         }
         os << std::endl;
      }
      return os;
   }

}

using Geometry::operator <<;

#endif
