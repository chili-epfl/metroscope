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

#ifndef Angle_HPP
#define Angle_HPP

#include <iostream>
#include <math.h>

template <typename T>
class Angle
{
	public:

		Angle();
		Angle(const T & pAngle);
		Angle (const Angle<T> & pAngle);

		bool isApproximatelyVertical() const {return ((mAngle>=45 && mAngle<135) || (mAngle>=225 && mAngle<315));}
		bool isApproximatelyHorizontal() const {return !isApproximatelyVertical();}

		bool isIn1stQuadrant() const {return *this<sc90;}
		bool isIn2ndQuadrant() const {return *this>=sc90 && *this<sc180;}
		bool isIn3rdQuadrant() const {return *this>=sc180 && *this<sc270;}
		bool isIn4thQuadrant() const {return *this>=sc270;}

		T getDegrees() const {return mAngle;}
		float radiants() const {return (float)(mAngle*scPI/sc180.getDegrees());}

		template <typename U> Angle<T> operator +(const Angle<U> & pAngle) const {return Angle<T>(in2PI(mAngle+pAngle.getDegrees()));}
		template <typename U> Angle<T> operator +(const U & pScalar) const {return Angle<T>(in2PI(mAngle+pScalar));}
		template <typename U> Angle<T> operator -(const Angle<U> & pAngle) const {return Angle<T>(in2PI(mAngle-pAngle.getDegrees()));}
		template <typename U> Angle<T> operator -(const U & pScalar) const {return Angle<T>(in2PI(mAngle-pScalar));}
		template <typename U> Angle<T> operator *(const Angle<U> & pAngle) const {return Angle<T>(in2PI(mAngle*pAngle.getDegrees()));}
		template <typename U> Angle<T> operator *(const U & pScalar) const {return Angle<T>(in2PI(mAngle*pScalar));}
		template <typename U> Angle<T> operator /(const Angle<U> & pAngle) const {return Angle<T>(in2PI(mAngle/pAngle.getDegrees()));}
		template <typename U> Angle<T> operator /(const U & pScalar) const {return Angle<T>(in2PI(mAngle/pScalar));}

		template <typename U> Angle<T> & operator +=(const Angle<U> & pAngle){
			mAngle+=(T)pAngle.getDegrees();
			mAngle=in2PI(mAngle);
			return *this;
		}
		template <typename U> Angle<T> & operator +=(const U & pScalar){
			mAngle+=(T)pScalar;
			mAngle=in2PI(mAngle);
			return *this;
		}
		template <typename U> Angle<T> & operator -=(const Angle<U> & pAngle){
			mAngle-=(T)pAngle.getDegrees();
			mAngle=in2PI(mAngle);
			return *this;
		}
		template <typename U> Angle<T> & operator -=(const U & pScalar){
			mAngle-=(T)pScalar;
			mAngle=in2PI(mAngle);
			return *this;
		}
		template <typename U> Angle<T> & operator *=(const Angle<U> & pAngle){
			mAngle*=(T)pAngle.getDegrees();
			mAngle=in2PI(mAngle);
			return *this;
		}
		template <typename U> Angle<T> & operator *=(const U & pScalar){
			mAngle*=(T)pScalar;
			mAngle=in2PI(mAngle);
			return *this;
		}
		template <typename U> Angle<T> & operator /=(const Angle<U> & pAngle){
			mAngle/=(T)pAngle.getDegrees();
			mAngle=in2PI(mAngle);
			return *this;
		}
		template <typename U> Angle<T> & operator /=(const U & pScalar){
			mAngle/=(T)pScalar;
			mAngle=in2PI(mAngle);
			return *this;
		}

		template <typename U> bool operator <(const Angle<U> & pAngle) const {return mAngle<pAngle.getDegrees();}
		template <typename U> bool operator <(const U & pScalar) const {return mAngle<pScalar;}
		template <typename U> bool operator >(const Angle<U> & pAngle) const {return mAngle>pAngle.getDegrees();}
		template <typename U> bool operator >(const U & pScalar) const {return mAngle>pScalar;}
		template <typename U> bool operator <=(const Angle<U> & pAngle) const {return mAngle<=pAngle.getDegrees();}
		template <typename U> bool operator <=(const U & pScalar) const {return mAngle<=pScalar;}
		template <typename U> bool operator >=(const Angle<U> & pAngle) const {return mAngle>=pAngle.getDegrees();}
		template <typename U> bool operator >=(const U & pScalar) const {return mAngle>=pScalar;}
		template <typename U> bool operator ==(const Angle<U> & pAngle) const {return mAngle==pAngle.getDegrees();}
		template <typename U> bool operator ==(const U & pScalar) const {return mAngle==pScalar;}
		template <typename U> bool operator !=(const Angle<U> & pAngle) const {return mAngle!=pAngle.getDegrees();}
		template <typename U> bool operator !=(const U & pScalar) const {return mAngle!=pScalar;}

        template <typename U> Angle<T> & operator =(const U & pScalar){
			mAngle=pScalar;
			mAngle=in2PI(mAngle);
			return *this;
		}
		template <typename U> Angle<T> & operator =(const Angle<U> & pAngle){
			mAngle=(T)pAngle.getDegrees();
			mAngle=in2PI(mAngle);
			return *this;
		}

		float sine() const {return sinf(radiants());}
		float cosine() const {return cosf(radiants());}
		float tangent() const {return tanf(radiants());}
		float cotangent() const {return cosf(radiants())/sinf(radiants());}

		static const Angle<T> sc0;
		static const Angle<T> sc45;
		static const Angle<T> sc90;
		static const Angle<T> sc135;
		static const Angle<T> sc180;
		static const Angle<T> sc270;
		static const float scPI;

		friend std::ostream& operator<<(std::ostream & os, const Angle<T> & pAngle)
		{
			os << pAngle.getDegrees();
			return os;
		}

	protected:
		T mAngle;

	private:
		T in2PI(const T & pTheta) const;
};

#endif
