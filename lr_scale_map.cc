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

#include "lr_scale_map.h"

#include "lr_math.h"



#define qExp(x) ::exp(x)


//! Smallest allowed value for logarithmic scales: 1.0e-150
//double QwtScaleMap::LogMin = 1.0e-150;

//! Largest allowed value for logarithmic scales: 1.0e150
//double QwtScaleMap::LogMax = 1.0e150;

//! Constructor for a linear transformation
QwtScaleTransformation::QwtScaleTransformation( Type type ):
    d_type( type )
{
}

//! Destructor
QwtScaleTransformation::~QwtScaleTransformation()
{
}

//! Create a clone of the transformation
QwtScaleTransformation *QwtScaleTransformation::copy() const
{
    return new QwtScaleTransformation( d_type );
}

/*!
  \brief Transform a value from the coordinate system of a scale
         into the coordinate system of the paint device

  \param s  Value related to the coordinate system of the scale
  \param s1 First border of the coordinate system of the scale
  \param s2 Second border of the coordinate system of the scale
  \param p1 First border of the coordinate system of the paint device
  \param p2 Second border of the coordinate system of the paint device
  \return
  <dl>
  <dt>linear mapping:<dd>p1 + (p2 - p1) / (s2 - s1) * (s - s1);</dd>
  </dl>
  <dl>
  <dt>log10 mapping: <dd>p1 + (p2 - p1) / log(s2 / s1) * log(s / s1);</dd>
  </dl>
*/

double QwtScaleTransformation::xForm(
    double s, double s1, double s2, double p1, double p2 ) const
{
    if ( d_type == Log10 )
        return p1 + ( p2 - p1 ) / log( s2 / s1 ) * log( s / s1 );
    else
        return p1 + ( p2 - p1 ) / ( s2 - s1 ) * ( s - s1 );
}

/*!
  \brief Transform a value from the coordinate system of the paint device
         into the coordinate system of a scale.

  \param p Value related to the coordinate system of the paint device 
  \param p1 First border of the coordinate system of the paint device
  \param p2 Second border of the coordinate system of the paint device
  \param s1 First border of the coordinate system of the scale
  \param s2 Second border of the coordinate system of the scale
  \return
  <dl>
  <dt>linear mapping:<dd>s1 + ( s2 - s1 ) / ( p2 - p1 ) * ( p - p1 );</dd>
  </dl>
  <dl>
  <dt>log10 mapping:<dd>exp((p - p1) / (p2 - p1) * log(s2 / s1)) * s1;</dd>
  </dl>
*/

double QwtScaleTransformation::invXForm( double p, double p1, double p2,
    double s1, double s2 ) const
{
    if ( d_type == Log10 )
        return qExp( ( p - p1 ) / ( p2 - p1 ) * log( s2 / s1 ) ) * s1;
    else
        return s1 + ( s2 - s1 ) / ( p2 - p1 ) * ( p - p1 );
}

