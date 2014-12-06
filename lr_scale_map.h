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



/*!
   \brief A transformation between coordinate systems

   QwtScaleTransformation offers transformations from the coordinate system
   of a scale into the linear coordinate system of a paint device 
   and vice versa.
*/
class QwtScaleTransformation
{
public:
    //! Transformation type
    enum Type
    {
        //! Transformation between 2 linear scales
        Linear,

        //! Transformation between a linear and a logarithmic ( base 10 ) scale
        Log10,

        //! Any other type of transformation
        Other
    };

    QwtScaleTransformation( Type type );
    virtual ~QwtScaleTransformation();

    virtual double xForm( double x, double s1, double s2,
        double p1, double p2 ) const;
    virtual double invXForm( double x, double p1, double p2,
        double s1, double s2 ) const;

    Type type() const;

    virtual QwtScaleTransformation *copy() const;

private:
    QwtScaleTransformation();
    QwtScaleTransformation &operator=( const QwtScaleTransformation );

    const Type d_type;
};

//! \return Transformation type
inline QwtScaleTransformation::Type QwtScaleTransformation::type() const
{
    return d_type;
}
