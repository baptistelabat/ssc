/*
 * Point.hpp
 *
 * \date 7 avr. 2014
 * \author cec
 */

#ifndef POINT_HPP_
#define POINT_HPP_

#include <Eigen/Dense>
#include <string>
#include <ostream>

namespace ssc
{
    namespace kinematics
    {
        /** \author cec
         *  \brief
         *  \details
         *  \ingroup kinematics
         *  \section ex1 Example
         *  \snippet kinematics/unit_tests/src/PointTest.cpp PointTest example
         *  \section ex2 Expected output
         *  \snippet kinematics/unit_tests/src/PointTest.cpp PointTest expected output
         */
        class Point
        {
            public:
                Point();
                Point(const std::string& frame);
                Point(const Point& P);
                Point(const std::string& frame, const Eigen::Vector3d& v);
                Point(const std::string& frame, const double x, const double y, const double z);
                Point& operator=(Point rhs);
                Eigen::Vector3d operator-(const Point& P) const;
                Point operator+(const Point& P) const;

                Eigen::Vector3d v;
                inline double& x() {return v[0];}
                inline double& y() {return v[1];}
                inline double& z() {return v[2];}
                inline double x() const {return v[0];}
                inline double y() const {return v[1];}
                inline double z() const {return v[2];}
                std::string get_frame() const;

                void swap(Point& other);

            private:
                std::string frame;
        };

        std::ostream& operator<<(std::ostream& os, const Point& P);

        void swap(Point& a, Point& b); // provide non-member for ADL
    }
}

#endif /* POINT_HPP_ */
