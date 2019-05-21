/*
 * Velocity.hpp
 *
 * \date 7 avr. 2014
 * \author cec
 */

#ifndef VELOCITY_HPP_
#define VELOCITY_HPP_

#include "ssc/kinematics/Point.hpp"

namespace ssc
{
    namespace kinematics
    {
        typedef Point AngularVelocityVector;
        typedef Point TranslationVelocityVector;


        /** \author cec
         *  \brief
         *  \details
         *  \ingroup kinematics
         */
        class Velocity
        {
            public:
                Velocity(const Point& p, const AngularVelocityVector& w);
                Velocity(const Point& p, const TranslationVelocityVector& t, const AngularVelocityVector& w);
                Velocity& operator=(Velocity rhs);
                Velocity(const Velocity& rhs);
                std::string get_frame() const;
                Point get_point() const;
                AngularVelocityVector get_angular_velocity() const;
                TranslationVelocityVector get_translation_velocity() const;
                Velocity change_point(const Point& P) const;

                inline double& u() {return vP.x();}
                inline double& v() {return vP.y();}
                inline double& w() {return vP.z();}
                inline double& p() {return omega.x();}
                inline double& q() {return omega.y();}
                inline double& r() {return omega.z();}
                inline double u() const {return vP.x();}
                inline double v() const {return vP.y();}
                inline double w() const {return vP.z();}
                inline double p() const {return omega.x();}
                inline double q() const {return omega.y();}
                inline double r() const {return omega.z();}

                void swap(Velocity& other);

            private:
                Velocity();
                Point P;
                TranslationVelocityVector vP;
                AngularVelocityVector omega;
        };

        void swap(Velocity& a, Velocity& b); // provide non-member for ADL
    }
}

#endif // VELOCITY_HPP_
