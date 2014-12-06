/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0

  * @file lr_interval.cc
  * modified by: @author garipova.nailya@tgtoil.com
  * @author Copyright(C) 2012 TGT Oil & Gas Services. All right reserved.

 *****************************************************************************/

#ifndef QWT_SCALE_DIV_H
#define QWT_SCALE_DIV_H

//#include "qwt_global.h"
#include "lr_interval.h"
#include <vector>

class QwtInterval;

/*!
  \brief A class representing a scale division

  A scale division consists of its limits and 3 list
  of tick values qualified as major, medium and minor ticks.

  In most cases scale divisions are calculated by a QwtScaleEngine.

  \sa subDivideInto(), subDivide()
*/

class QwtScaleDiv
{
public:
    //! Scale tick types
    enum TickType
    {
        //! No ticks
        NoTick = -1,

        //! Minor ticks
        MinorTick,

        //! Medium ticks
        MediumTick,

        //! Major ticks
        MajorTick,

        //! Number of valid tick types
        NTickTypes
    };

    explicit QwtScaleDiv();
	explicit QwtScaleDiv( const QwtInterval &, std::vector< std::vector<double> >);
    explicit QwtScaleDiv( 
        double lowerBound, double upperBound, std::vector< std::vector<double> >);

    bool operator==( const QwtScaleDiv &s ) const;
    bool operator!=( const QwtScaleDiv &s ) const;

    void setInterval( double lowerBound, double upperBound );
    void setInterval( const QwtInterval & );
    QwtInterval interval() const;

    double lowerBound() const;
    double upperBound() const;
    double range() const;

    bool contains( double v ) const;

    void setTicks( int type, std::vector<double>  );
	std::vector<double> ticks( int type ) const;

    void invalidate();
    bool isValid() const;

    void invert();

private:
    double d_lowerBound;
    double d_upperBound;
	std::vector< std::vector<double> > d_ticks;

    bool d_isValid;
};

//Q_DECLARE_TYPEINFO(QwtScaleDiv, Q_MOVABLE_TYPE);

/*!
   Change the interval
   \param lowerBound lower bound
   \param upperBound upper bound
*/
inline void QwtScaleDiv::setInterval( double lowerBound, double upperBound )
{
    d_lowerBound = lowerBound;
    d_upperBound = upperBound;
}

/*!
  \return lowerBound -> upperBound
*/
inline QwtInterval QwtScaleDiv::interval() const
{
    return QwtInterval( d_lowerBound, d_upperBound );
}

/*!
  \return lower bound
  \sa upperBound()
*/
inline double QwtScaleDiv::lowerBound() const
{
    return d_lowerBound;
}

/*!
  \return upper bound
  \sa lowerBound()
*/
inline double QwtScaleDiv::upperBound() const
{
    return d_upperBound;
}

/*!
  \return upperBound() - lowerBound()
*/
inline double QwtScaleDiv::range() const
{
    return d_upperBound - d_lowerBound;
}
#endif
