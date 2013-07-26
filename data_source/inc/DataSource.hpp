/*
 * DataSource.hpp
 *
 * \date 13 mars 2013, 15:31:43
 *  \author cec
 */

#ifndef DATASOURCE_HPP_
#define DATASOURCE_HPP_

#include <tr1/memory>
#include <string>
#include <set>
#include <tr1/unordered_map>

#include "DataSourceModule.hpp"
#include "SignalContainer.hpp"
#include "DataSourceException.hpp"

typedef std::tr1::shared_ptr<const DataSourceModule> ModulePtr;
typedef std::tr1::unordered_map<std::string,ModulePtr > FromName2Module;
typedef std::tr1::unordered_map<std::string,std::string> FromSignal2Module;
typedef std::tr1::unordered_map<std::string,std::set<std::string> > DependantModules;
typedef std::tr1::unordered_map<std::string,bool > UpdateState;


void append(DependantModules& map, const std::string& key, const std::string& value);

/** \author cec
 *  \brief Simplifies the creation of simulators using forward/reverse chaining.
 *  \details It provides models with the data they need (which may come from other models)
 *  \ingroup data_source
 *  \section ex1 Example
 *  \snippet data_source/unit_tests/src/DataSourceTest.cpp DataSourceTest example
 *  \section ex2 Expected output
 *  \snippet data_source/unit_tests/src/DataSourceTest.cpp DataSourceTest expected output
 */
class DataSource
{
    public:
        DataSource();

        /** \author cec
         *  \date 1 juil. 2013, 17:41:30
         *  \brief draws the DataSource's dependency graph
         *  \returns String containing the serialization of the DataSource
         *  \snippet data_source/unit_tests/src/DataSourceTest.cpp DataSourceTest draw_example
        */
        std::string draw() const;

        /** \author cec
         *  \date 17 juin 2013, 10:52:20
         *  \brief Adds a module to the DataSource.
         *  \details This method adds the module to the list, initializes it &
         *  retrieves all its dependencies.
         *  Another API which was considered earlier was add<M>(module_name) but
         *  this doesn't play well with mocks (you don't have access to
         *  the module because you leave its instantiation to the DataSource)
         *  \returns
         *  \snippet data_source/unit_tests/src/DataSourceTest.cpp DataSourceTest enclosing_method_example
        */
        void add(const DataSourceModule& module)
        {
            const ModulePtr m = add_module_if_not_already_present_and_return_clone(module);
            m->initialize();
            const bool read_only_bak = readonly;
            readonly = true; // We don't want the following call to module.update()
                              // to modify the signals in the DataSource: we just
                              // want to track module dependencies
            try
            {
                module_being_updated = m->get_name();
                m->update();

            }
            catch(DataSourceException& e)
            {
                throw(e);
            }
            catch(...)
            {
                // It's OK for m->update() to throw at this stage because we just want to retrieve its dependencies
            }
            current_module = "";
            readonly = read_only_bak;
            //set<bool>("read_only",read_only_bak);
        }

        /** \author cec
         *  \date 24 juil. 2013, 15:13:58
         *  \brief Remove a module from a data source (and from the lists of dependencies)
         *  \returns Nothing
         *  \snippet data_source/unit_tests/src/DataSourceTest.cpp DataSourceTest enclosing_method_example
        */
        template <typename T> void remove()
        {
            const auto module_to_remove = std::string(typeid(T).name()) + "auto-generated by DataSource";
            module2dependantmodules.erase(module_to_remove);
            name2module.erase(module_to_remove);
            std::vector<std::string> signals_to_remove;
            for (auto it = signal2module.begin() ; it != signal2module.end() ; ++it)
            {
                if (it->second == module_to_remove) signals_to_remove.push_back(it->first);
            }
            for (auto it = signals_to_remove.begin() ; it != signals_to_remove.end() ; ++it)
            {
                signal2module.erase(*it);
            }
            module2dependantmodules.erase(module_to_remove);
            module2requiredmodules.erase(module_to_remove);
            module2requiredsignals.erase(module_to_remove);
            update_dependencies();
        }

        /** \author cec
         *  \date 18 juin 2013, 22:19:54
         *  \brief Templated version of 'add'
         *  \details This is the version normally used to add a module because
         *  it automatically adds the present DataSource (*this) to the module's
         *  constructor.
         *  \returns Nothing
         *  \snippet data_source/unit_tests/src/DataSourceTest.cpp DataSourceTest enclosing_method_example
        */
        template <typename T> void add(const std::string& module_name)
        {
            if (module_name == "")
            {
                THROW(__PRETTY_FUNCTION__, DataSourceException, "Module name cannot be empty");
            }
            const T t(this, module_name);
            add(t);
        }

        /** \author cec
         *  \date 2 juil. 2013, 14:16:50
         *  \brief 'add' without specifying the name of the module (if there is only one of each type)
         *  \returns Nothing
         *  \snippet /unit_tests/src/DataSourceTest.cpp DataSourceTest enclosing_method_example
        */
        template <typename T> void add()
        {
            add<T>(std::string(typeid(T).name()) + "auto-generated by DataSource");
        }

        /** \author cec
         *  \date 17 juin 2013, 10:55:37
         *  \brief Retrieve the list of all modules currently in the DataSource
         *  \returns List of all present modules
         *  \snippet data_source/unit_tests/src/DataSourceTest.cpp DataSourceTest enclosing_method_example
        */
        FromName2Module get_modules() const;

        /** \author cec
         *  \date 17 juin 2013, 10:41:56
         *  \brief Removes all modules from a DataSource.
         *  \returns Nothing
         *  \snippet data_source/unit_tests/src/DataSourceTest.cpp DataSourceTest DataSource::clear_example
        */
        void clear();

        /** \author cec
         *  \date 18 juin 2013, 21:56:15
         *  \brief Adds/Updates a signal in the DataSource
         *  \returns Nothing
         *  \snippet data_source/unit_tests/src/DataSourceTest.cpp DataSourceTest set_example
        */
        template <typename T> void set(const std::string& signal_name, //<! Name of the signal to create or update
                                       const T& t)
        {
            if (readonly)
            {
                std::tr1::unordered_map<std::string, std::string>::const_iterator it =
                        signal2module.find(signal_name + typeid(T).name());
                if ((it != signal2module.end()) && (it->second != "")
                        && (it->second != current_module))
                {
                    THROW(__PRETTY_FUNCTION__, DataSourceException,
                            std::string("Attempting to add module '")
                                    + current_module + "' which sets signal '"
                                    + signal_name + "', but module '"
                                    + it->second
                                    + "' already sets it: need to remove one of them using DataSource::remove.");
                }
                else if (signal_name != "")
                {
                    signal2module[signal_name + typeid(T).name()] =
                            current_module;
                    update_dependencies();
                }
            }
            else
            {
                signals.set(signal_name + typeid(T).name(), t);
                DependantModules::const_iterator it = signal2dependantmodules
                        .find(signal_name + typeid(T).name());
                if (it != signal2dependantmodules.end())
                {
                    const std::set<std::string> S = it->second;
                    for (std::set<std::string>::const_iterator it = S.begin();
                            it != S.end(); ++it)
                    {
                        is_up_to_date[*it] = false;
                        const std::set<std::string> S2 =
                                module2dependantmodules[*it];
                        for (std::set<std::string>::const_iterator it2 = S2
                                .begin(); it2 != S2.end(); ++it2)
                        {
                            is_up_to_date[*it2] = false;
                        }
                    }
                }
            }
        }

        /** \author cec
         *  \date 18 juin 2013, 22:01:33
         *  \brief Retrieves a signal from the SignalContainer
         *  \returns Value of the signal
         *  \snippet data_source/unit_tests/src/DataSourceTest.cpp DataSourceTest get_example
         */
        template<typename T>
        T get(const std::string& signal_name)
        {
            if (readonly && (current_module != ""))
            {
                append(signal2dependantmodules, signal_name + typeid(T).name(),
                        current_module);
                append(module2requiredsignals, current_module,
                        signal_name + typeid(T).name());
                update_dependencies();
                return T();
            } else
            {
                const FromSignal2Module::const_iterator that_signal =
                        signal2module.find(signal_name + typeid(T).name());
                const bool computable = that_signal != signal2module.end();
                const bool stored = signals.has < T
                        > (signal_name + typeid(T).name());
                if (computable)
                {
                    const std::string module_name = that_signal->second;
                    if (not (is_up_to_date[module_name]))
                    {
                        module_being_updated = module_name;
                        name2module[module_name]->update();
                    }
                } else if (not (stored))
                {
                    THROW(__PRETTY_FUNCTION__, DataSourceException,
                            std::string("Unable to find signal '") + signal_name
                                    + "' required by module '" + module_being_updated + "'");
                }
            }
            return signals.get < T > (signal_name + typeid(T).name());
        }
        bool read_only() const;

    private:
        /** \author cec
         *  \date 17 juin 2013, 11:00:13
         *  \details Checks if a module with the same name exists in the DataSource.
         *  If it exists, throw an exception. Otherwise, add the module to name2module.
         *  \returns Pointer to the added module.
         *  \snippet /unit_tests/src/DataSourceTest.cpp DataSourceTest enclosing_method_example
         */
        ModulePtr add_module_if_not_already_present_and_return_clone(const DataSourceModule& module);
        std::set<std::string> get_dependencies(const std::string& module_name, std::set<std::string>& ret) const;
        std::set<std::string> get_dependencies(const std::string& ref_module, const std::string& current_module, std::set<std::string>& dependencies) const;
        bool a_module_depends_on_itself();
        void update_dependencies();
        void add_dependencies_and_dependent_modules(const std::set<std::string>& required_signals, const std::string& module_using_required_signals);
        FromName2Module name2module; //!< Map giving, for each module name, a (smart) pointer to the corresponding module
        bool readonly; //!< If this flag is set to true, DataSource::set will not modify the state of the DataSource. This is used to track dependencies between modules
        SignalContainer signals; //!< All signals currently in the DataSource
        std::string current_module; //!< Module currently adding signals to the DataSource (used to track if two different modules set the same signal)
        std::string module_being_updated; //!< Module currently getting signals from the DataSource (used to track which module is requiring a missing signal)
        FromSignal2Module signal2module; //!< Tracks which module sets which signal
        DependantModules module2dependantmodules; //!< For each module, stores the set of the names of the modules depending on it
        DependantModules module2requiredmodules; //!< For each module, stores the set of the names of the modules it depends on
        DependantModules module2requiredsignals; //!< For each module, stores the signals it depends on
        DependantModules signal2dependantmodules; //!< For each signal, stores the modules that depend on it
        UpdateState is_up_to_date;
};



#endif /* DATASOURCE_HPP_ */
