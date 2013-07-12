/*
 * SignalContainer.hpp
 *
 * \date 18 mars 2013, 17:52:42
 *  \author cec
 */

#ifndef SIGNALCONTAINER_HPP_
#define SIGNALCONTAINER_HPP_


#include <map>
#include <typeinfo>
#include <vector>
#include "Exception.hpp"
#include "SignalContainerTypeLists.hpp"

//#include <cstdio>
//#include "test_macros.hpp"

class SignalContainerException : public Exception
{
    public:
        SignalContainerException(const char* s) :
                Exception(s)
        {
        }
};





/** \author cec
 * \ingroup data_source
 *  \brief This class was created to store signals of different types using boost::any
 *  \details It has the following responsibilities:
 *  - Store & retrieve signals by name
 *  - Convert a list of signal names (or all of its signals) to a vector of
 *    doubles (for Euler & RK integration)
 *  - Accept a visitor for serialization (eg. to an output file)
 *  \section ex1 Example
 *  \snippet data_source/unit_tests/src/SignalContainerTest.cpp SignalContainerTest example
 *  \section ex2 Expected output
 *  \snippet data_source/unit_tests/src/SignalContainerTest.cpp SignalContainerTest expected output
 */
class SignalContainer
{
    public:
        SignalContainer();

        template <typename T> void set(const std::string& signal_name, const T& value)
        {
            const TypedSignalName map_name(signal_name,typeid(T).name());
            signals[map_name] = value;
            get_iterator_list(value).push_back(signals.find(map_name));
        }

        template <typename T> T get(const std::string& signal_name) const
        {
            const TypedSignalName name(signal_name,typeid(T).name());
            ConstSignalIterator it = signals.find(name);
            if (it == signals.end())
            {
                std::string msg = "unable to find a signal named '";
                msg += name.get_signal_name();
                msg += "' of type '" + name.get_type_name() + "'";
                const std::string function_name = std::string(typeid(T).name()) + " SignalContainer::get(const std::string& signal_name) const";
                THROW(function_name, SignalContainerException, msg);
            }
            return boost::any_cast<T>(it->second);
        }

        template <class T> typename std::map<SignalName, T> get_all() const
        {
            std::map<SignalName, T> ret;
            for (ConstSignalIterator it = signals.begin() ; it != signals.end() ; ++it)
            {
                if (it->first.get_type_name() == typeid(T).name())
                {
                    ret.insert(std::make_pair(it->first.get_signal_name(),boost::any_cast<T>(it->second)));
                }
            }
            return ret;
        }

        /** \author cec
         *  \date 17 juin 2013, 12:23:08
         *  \brief Retrieves all doubles & vector of doubles in a single vector
         *  \details Values are returned by signal name. Vectors are stored
         *  contiguously.
         *  \returns Vector containing all doubles in the SignalContainer.
         *  \snippet data_source/unit_tests/src/SignalContainerTest.cpp SignalContainerTest enclosing_method_example
        */
        std::list<double> to_doubles() const;

        /** \author cec
         *  \date 18 juin 2013, 14:24:46
         *  \brief Sets all doubles & vector of doubles from list
         *  \returns Nothing
         *  \snippet data_source/unit_tests/src/SignalContainerTest.cpp SignalContainerTest enclosing_method_example
        */
        void from_doubles(std::list<double> l);



    private:
        /** \author cec
         *  \date 18 juin 2013, 10:33:02
         *  \brief Calls select_list_from_type for all scalars.
         *  \returns List of iterators taken from scalar_convertible_types
        */
        template <typename T> std::list<ConstSignalIterator>& get_iterator_list(const T& val)
        {
            (void) val;
            return select_list_from_type<T>(scalar_convertible_types);
        }

        /** \author cec
         *  \date 18 juin 2013, 10:33:02
         *  \brief Calls select_list_from_type for all vectors.
         *  \returns List of iterators taken from vector_convertible_types
        */
        template <typename T> std::list<ConstSignalIterator>& get_iterator_list(const std::vector<T>& val)
        {
            (void) val;
            return select_list_from_type<T>(vector_convertible_types);
        }

        template <typename T> void coerce_scalar(std::list<double>& ret) const
        {
            for (ConvertibleTypesIterator it  = begin<T>(scalar_convertible_types) ; it != end<T>(scalar_convertible_types) ; ++it)
            {
                coerce(ret, boost::any_cast<T>((*it)->second));
            }
        }

        template <typename T> void coerce_vector(std::list<double>& ret) const
        {
            for (ConvertibleTypesIterator it  = begin<T>(vector_convertible_types) ; it != end<T>(vector_convertible_types) ; ++it)
            {
                coerce(ret, boost::any_cast<std::vector<T> >((*it)->second));
            }
        }

        template <typename T> void coerce_type(std::list<double>& ret) const
        {
            coerce_scalar<T>(ret);
            coerce_vector<T>(ret);
        }

        template <typename T> void decoerce_scalar(std::list<double>& ret)
        {
            for (ConvertibleTypesIterator it  = begin<T>(scalar_convertible_types) ; it != end<T>(scalar_convertible_types) ; ++it)
            {
                T t = boost::any_cast<T>((*it)->second);

                decoerce(ret, t);
                signals[(*it)->first] = t;
            }
        }

        template <typename T> void decoerce_vector(std::list<double>& ret)
        {
            for (ConvertibleTypesIterator it  = begin<T>(vector_convertible_types) ; it != end<T>(vector_convertible_types) ; ++it)
            {
                std::vector<T> t = boost::any_cast<std::vector<T> >((*it)->second);
                decoerce(ret, t);
                signals[(*it)->first] = t;
            }
        }

        template <typename T> void decoerce_type(std::list<double>& ret)
        {
            decoerce_scalar<T>(ret);
            decoerce_vector<T>(ret);
        }

        Signals signals;
        ConvertibleTypes scalar_convertible_types;
        ConvertibleTypes vector_convertible_types;
};


template <> void SignalContainer::coerce_type<PhysicalQuantity>(std::list<double>& ret) const;

#endif /* SIGNALCONTAINER_HPP_ */