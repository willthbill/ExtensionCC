#pragma once

/*#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::FT FT;*/

/*#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
typedef CGAL::Exact_predicates_exact_constructions_kernel K;
typedef K::FT FT;*/

/*#include <CGAL/Fraction_traits.h>
#include <CGAL/Gmpz.h>
#include <CGAL/Gmpq.h>
#include <CGAL/Cartesian.h>
typedef CGAL::Cartesian<CGAL::Fraction_traits<CGAL::Gmpq>> K;
typedef K::FT FT;
typedef FT::Numerator_type Numerator_type;
typedef FT::Denominator_type Denominator_type;
typedef FT::Type FTType;*/

#include <CGAL/Fraction_traits.h>
#include <CGAL/Gmpz.h>
#include <CGAL/Gmpq.h>
#include <CGAL/Cartesian.h>
typedef CGAL::Cartesian<CGAL::Gmpq> K;
typedef CGAL::Fraction_traits<CGAL::Gmpq> FractionTraits;
typedef FractionTraits::Numerator_type Numerator_type;
typedef FractionTraits::Denominator_type Denominator_type;
typedef K::FT FT;

#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/Polygon_set_2.h>

// #include<bits/stdc++.h>
// #include <CGAL/IO/io.h>
// CGAL::IO::set_pretty_mode(std::cout);

typedef K::Point_2 Point;
typedef K::Line_2 Line;
typedef K::Direction_2 Direction;
typedef K::Vector_2 Vector;
typedef K::Orientation_2 Orientation;
typedef K::Segment_2 Segment;
typedef CGAL::Polygon_2<K> Polygon;
typedef CGAL::Polygon_with_holes_2<K> Polygon_with_holes;
typedef CGAL::Polygon_set_2<K> Polygon_set;
