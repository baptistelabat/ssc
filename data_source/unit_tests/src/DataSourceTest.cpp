/*
 * DataSourceTest.cpp
 *
 * \date 13 mars 2013, 15:19:45
 *  \author cec
 */

#include "DataSourceTest.hpp"
#include "DataSource.hpp"
#include "gmock/gmock.h"
#include "DataSourceModule.hpp"
#include "DataSourceModuleMock.hpp"
#include "DataSourceException.hpp"
#include "TestModules.hpp"

#include <set>

static DataGenerator a_(21);
static DataSource* ds_;

using ::testing::Return;
using ::testing::NiceMock;
using ::testing::StrictMock;
using ::testing::InSequence;
MockableDataSourceModule::MockableDataSourceModule() : DataSourceModule(ds_, a_.random<std::string>())
{

}

DataSourceTest::DataSourceTest() : a(DataGenerator(654))
{
    ds_ = new DataSource();
}

DataSourceTest::~DataSourceTest()
{
    delete ds_;
}

void DataSourceTest::SetUp()
{
}

void DataSourceTest::TearDown()
{
}

TEST_F(DataSourceTest, example)
{
//! [DataSourceTest example]
    std::tr1::shared_ptr<DataSource> ds(new DataSource);
    NiceMock<MockDataSourceModule>* mock = new NiceMock<MockDataSourceModule>();
    ON_CALL(*mock, clone()).WillByDefault(Return(mock));
    EXPECT_CALL(*mock, initialize()).Times(1);
    ds->add(*mock);
//! [DataSourceTest example]
//! [DataSourceTest expected output]
//! [DataSourceTest expected output]
}


TEST_F(DataSourceTest, all_mock_modules_generated_for_tests_should_have_different_names)
{
    std::set<std::string> module_names;
    const size_t nb_of_mocks = 1000;
    for (size_t i = 0 ; i < nb_of_mocks ; ++i)
    {
        MockDataSourceModule mock;
        module_names.insert(mock.get_name());
    }
    ASSERT_EQ(nb_of_mocks, module_names.size());
}

TEST_F(DataSourceTest, should_be_able_to_clear_a_datasource_to_empty_it)
{
    ds_->clear();
    ASSERT_EQ(0,ds_->get_modules().size());
}

TEST_F(DataSourceTest, should_be_able_to_add_a_module_and_get_the_list_of_modules)
{
    DataSource ds;
    // Using NiceMock because we only want to specify the behavior of the method
    // of interest
    // There's a 'new' but no 'delete' because the clone() method will return
    // this pointer, therefore it will be garbage-collected by std::tr1::shared_ptr
    NiceMock<MockDataSourceModule>* mock_1 = new NiceMock<MockDataSourceModule>();
    NiceMock<MockDataSourceModule>* mock_2 = new NiceMock<MockDataSourceModule>();

    ON_CALL(*mock_1, clone()).WillByDefault(Return(mock_1));
    ON_CALL(*mock_2, clone()).WillByDefault(Return(mock_2));
    ASSERT_EQ(0,ds.get_modules().size());
    ds.add(*mock_1);
    ds.add(*mock_2);
    const FromName2Module modules = ds.get_modules();
    ASSERT_EQ(2,modules.size());
    std::set<std::string> module_names;
    std::set<std::string> expected_module_names;
    for (FromName2Module::const_iterator it = modules.begin() ; it != modules.end() ; ++it)
    {
        module_names.insert(it->first);
    }
    expected_module_names.insert(mock_1->get_name());
    expected_module_names.insert(mock_2->get_name());
    ASSERT_EQ(expected_module_names, module_names);
}

TEST_F(DataSourceTest, check_that_the_correct_functions_are_called_for_add_module)
{
    DataSource ds;
    // We want the calls to occur in an exact order
    InSequence s;
    // We're using a StrictMock because we want to capture the behavior of
    // DataSource::add exactly (i.e. there should not be *any* uninteresting
    // calls)
    // There's a 'new' but no 'delete' because the clone() method will return
    // this pointer, therefore it will be garbage-collected by std::tr1::shared_ptr
    StrictMock<MockDataSourceModuleWithGetName>* mock = new StrictMock<MockDataSourceModuleWithGetName>();
    ON_CALL(*mock, clone()).WillByDefault(Return(mock));
    EXPECT_CALL(*mock, get_name()).Times(1);
    EXPECT_CALL(*mock, clone()).Times(1);
    EXPECT_CALL(*mock, get_name()).Times(1);
    EXPECT_CALL(*mock, initialize()).Times(1);
    EXPECT_CALL(*mock, get_name()).Times(1);
    EXPECT_CALL(*mock, update()).Times(1);
    ds.add(*mock);
}

TEST_F(DataSourceTest, should_not_be_able_to_add_two_modules_with_the_same_name)
{
    DataSource ds;
    // Using NiceMock because we only want to specify the behavior of the method
    // of interest
    NiceMock<MockDataSourceModuleWithGetName>* mock = new NiceMock<MockDataSourceModuleWithGetName>();
    ON_CALL(*mock, clone()).WillByDefault(Return(mock));
    ds.add(*mock);
    ASSERT_THROW(ds.add(*mock),DataSourceException);
}

TEST_F(DataSourceTest, should_be_able_to_set_and_retrieve_a_constant)
{
    DataSource ds;
    for (size_t i = 0 ; i < 10000 ; ++i)
    {
        const double d = a.random<double>();
        ds.set("d", d);
        ASSERT_DOUBLE_EQ(d, ds.get<double>("d"));
    }
}

TEST_F(DataSourceTest, a_module_cannot_depend_on_itself)
{
    DataSource ds;
    ASSERT_THROW(ds.add<ModuleDependingOnItself>(a.random<std::string>()), DataSourceException);
}

TEST_F(DataSourceTest, should_throw_if_module_name_is_empty)
{
    DataSource ds;
    ASSERT_THROW(ds.add<TestModule>(""), DataSourceException);
}

TEST_F(DataSourceTest, should_throw_if_there_are_any_circular_dependencies)
{
    DataSource ds;
    ds.add<ModuleA>("Module A");
    ds.add<ModuleB>("Module B");
    ASSERT_THROW(ds.add<ModuleC>("Module C"), DataSourceException);
}

TEST_F(DataSourceTest, should_throw_if_attempting_to_retrieve_a_value_not_in_DataSource)
{
    DataSource ds;
    ASSERT_THROW(ds.get<double>(a.random<std::string>()), DataSourceException);
}

TEST_F(DataSourceTest, should_throw_if_not_all_dependencies_are_met)
{
    DataSource ds;
    ds.add<TestModule>(a.random<std::string>());
    ASSERT_THROW(ds.get<size_t>("nb_of_updates"), DataSourceException);
    ds.set<size_t>("nb_of_updates", 0);
    ASSERT_NO_THROW(ds.get<size_t>("nb_of_updates"));
}

TEST_F(DataSourceTest, should_throw_if_two_modules_set_the_same_signal)
{
    DataSource ds;
    ds.add<TestModule>(a.random<std::string>());
    ASSERT_THROW(ds.add<TestModule>(a.random<std::string>()), DataSourceException);
}

TEST_F(DataSourceTest, test_module_should_work_properly)
{
    DataSource ds;
    ds.add<TestModule>(a.random<std::string>());
    const size_t i = a.random<size_t>();
    ds.set<size_t>("nb_of_updates", i);
    ASSERT_EQ(i+1, ds.get<size_t>("nb_of_updates2"));
}

TEST_F(DataSourceTest, data_source_should_not_update_unnecessarily)
{
    DataSource ds;
    ds.add<TestModule>(a.random<std::string>());
    const size_t i = a.random<size_t>();
    ds.set<size_t>("nb_of_updates", i);
    for (size_t k = 0 ; k < 10000 ; ++k)
    {
        ASSERT_EQ(i+1, ds.get<size_t>("nb_of_updates2"));
    }
    ds.set<size_t>("nb_of_updates", i+1);
    ASSERT_EQ(i+2, ds.get<size_t>("nb_of_updates2"));
}

TEST_F(DataSourceTest, two_signals_with_same_name_but_different_types_do_not_create_a_circular_dependencies)
{
    DataSource ds;
    ASSERT_NO_THROW(ds.add<ModuleD>("Module D"));
}

MODULE(M1,ds->set<double>("s1",123))
MODULE(M2,const double s1 = ds->get<double>("s1");\
          ds->set<double>("s2", s1))
MODULE(M3,ds->get<double>("s2"))

TEST_F(DataSourceTest, can_add_a_module_without_specifying_its_name_but_only_once_per_type)
{
    DataSource ds;
    ASSERT_NO_THROW(ds.add<M1>());
    ASSERT_THROW(ds.add<M1>(), DataSourceException);
    ASSERT_EQ(123, ds.get<double>("s1"));
    ASSERT_NO_THROW(ds.add<M2>());
    ASSERT_THROW(ds.add<M2>(), DataSourceException);
    ASSERT_EQ(123, ds.get<double>("s2"));
}

TEST_F(DataSourceTest, bug_detected_in_EONAV_for_cyclic_dependency_check)
{
    DataSource data_source;
    data_source.add<M1>("m1");
    data_source.add<M2>("m2");
    ASSERT_NO_THROW(data_source.add<M3>("m3"));
}

TEST_F(DataSourceTest, should_be_able_to_remove_a_module)
{
    DataSource data_source;
    data_source.add<M1>();
    ASSERT_THROW(data_source.add<M1>(),DataSourceException);
    data_source.remove<M1>();
    ASSERT_NO_THROW(data_source.add<M1>());
}

/*
 *
 */
TEST_F(DataSourceTest, can_declare_and_define_a_module_using_a_macro)
{
    MODULE(modulename,const double x1 = ds->get<double>("x1");\
                      const double x2 = ds->get<double>("x2");\
                      ds->set<double>("y1",x1+x2);\
                      ds->set<double>("y2",x1*x2))
}

MODULE(ode,const double x = ds->get<double>("x");\
           ds->set<double>("dx_dt",2*x))

MODULE(ode2,const double x = ds->get<double>("x");\
            ds->set<double>("dy_dt",x*x);\
            ds->set<double>("dz_dt",sin(x)))

MODULE(ode3,const double x = ds->get<double>("x");\
            const double y = ds->get<double>("y");\
            ds->set<double>("dx_dt",x*y);\
            ds->set<double>("dy_dt",sin(x)))

TEST_F(DataSourceTest, should_be_able_to_define_a_state_derivative)
{
    DataSource data_source;
    data_source.add<ode>();
    data_source.define_derivative("x", "dx_dt");
}

TEST_F(DataSourceTest, can_retrieve_derivatives)
{
    DataSource data_source;
    data_source.add<ode>();
    data_source.add<ode2>();
    data_source.define_derivative("x", "dx_dt");
    data_source.define_derivative("y", "dy_dt");
    data_source.define_derivative("z", "dz_dt");
    for (size_t i = 0 ; i < 1000 ; ++i)
    {
        const double x = a.random<double>();
        data_source.set<double>("x",x);
        std::vector<double> dx(3,0);
        data_source.get_derivatives(dx);
        ASSERT_DOUBLE_EQ(2*x, dx[0]);
        ASSERT_DOUBLE_EQ(x*x, dx[1]);
        ASSERT_DOUBLE_EQ(sin(x), dx[2]);
    }
}

TEST_F(DataSourceTest, can_retrieve_derivatives_out_of_order)
{
    DataSource data_source;
    data_source.add<ode3>();
    std::vector<double> v;
    ASSERT_NO_THROW(data_source.get_derivatives(v));
    data_source.define_derivative("y", "dy_dt");
    data_source.define_derivative("x", "dx_dt");
    for (size_t i = 0 ; i < 1000 ; ++i)
    {
        const double x = a.random<double>();
        const double y = a.random<double>();
        data_source.set<double>("x",x);
        data_source.set<double>("y",y);
        std::vector<double> dx(2,0);
        data_source.get_derivatives(dx);
        ASSERT_DOUBLE_EQ(y*x, dx[1]);
        ASSERT_DOUBLE_EQ(sin(x), dx[0]);
    }
}

TEST_F(DataSourceTest, get_derivatives_should_throw_if_input_vector_is_not_the_right_size)
{
    DataSource data_source;
    data_source.add<ode3>();
    data_source.define_derivative("x", "dx_dt");
    data_source.define_derivative("y", "dy_dt");
    std::vector<double> empty;
    std::vector<double> one_element(1,0);
    ASSERT_THROW(data_source.get_derivatives(empty), DataSourceException);
    ASSERT_THROW(data_source.get_derivatives(one_element), DataSourceException);
    for (size_t i = 2 ; i < 1002 ; ++i)
    {
        std::vector<double> dx_dt(i,0);
        ASSERT_THROW(data_source.get_derivatives(dx_dt), DataSourceException);
    }
}

TEST_F(DataSourceTest, get_derivatives_should_work_even_if_there_are_no_derivatives_to_get)
{
    DataSource data_source;
    std::vector<double> v;
    ASSERT_NO_THROW(data_source.get_derivatives(v));
}

TEST_F(DataSourceTest, cant_define_the_same_derivative_twice)
{
    DataSource data_source;
    data_source.add<ode>();
    const std::string state_name = a.random<std::string>();
    const std::string derivative_name = a.random<std::string>();
    data_source.define_derivative(state_name, derivative_name);
    ASSERT_THROW(data_source.define_derivative(state_name, a.random<std::string>()), DataSourceException);
    ASSERT_THROW(data_source.define_derivative(a.random<std::string>(), derivative_name), DataSourceException);
    ASSERT_NO_THROW(data_source.define_derivative(a.random<std::string>(), a.random<std::string>()));
}

TEST_F(DataSourceTest, cant_add_the_same_derivative_twice)
{
    DataSource data_source;
    const std::string a_name = a.random<std::string>();
    data_source.define_derivative("x", a_name);
    ASSERT_THROW(data_source.define_derivative("x", a_name), DataSourceException);
}

TEST_F(DataSourceTest, can_set_the_states_to_an_arbitrary_value)
{
    DataSource data_source;
    const size_t nb_of_states = a .random<size_t>().between(0,500);
    const std::vector<double> x = a.random_vector_of<double>().of_size(nb_of_states);
    const std::vector<std::string> state_names = a.random_vector_of<std::string>().of_size(nb_of_states);
    for (size_t i = 0 ; i < nb_of_states ; ++i)
    {
        data_source.define_derivative(state_names[nb_of_states-i-1], a.random<std::string>());
    }
    data_source.set_states(x);
    for (size_t i = 0 ; i < nb_of_states ; ++i)
    {
        ASSERT_DOUBLE_EQ(x[i],data_source.get<double>(state_names[nb_of_states-i-1]));
    }
}

TEST_F(DataSourceTest, set_states_should_throw_if_vector_has_incorrect_size)
{
    DataSource data_source;
    const size_t nb_of_states = a .random<size_t>().between(0,500);
    const std::vector<double> x = a.random_vector_of<double>().of_size(a.random<size_t>().between(0,500).but_not(nb_of_states));
    for (size_t i = 0 ; i < nb_of_states ; ++i)
    {
        data_source.define_derivative(a.random<std::string>(), a.random<std::string>());
    }
    ASSERT_THROW(data_source.set_states(x), DataSourceException);
}

TEST_F(DataSourceTest, can_retrieve_a_list_containing_the_names_of_all_states)
{
    //! [DataSourceTest get_state_names_example]
    DataSource data_source;
    data_source.add<ode3>();
    data_source.define_derivative("y", "dy_dt");
    data_source.define_derivative("x", "dx_dt");
    const std::vector<std::string> state_names = data_source.get_state_names();
    ASSERT_EQ(2, state_names.size());
    ASSERT_EQ("y", state_names[0]);
    ASSERT_EQ("x", state_names[1]);
    //! [DataSourceTest get_state_names_example]
}

TEST_F(DataSourceTest, when_copying_a_data_source_the_references_to_it_should_be_updated_inside_the_modules)
{
    // When copying a DataSource, if nothing is done, all the modules will maintain
    // a pointer to the copied DataSource: we need to update the pointer maintained
    // by each module to point to the new DataSource. This bug was discovered
    // when creating DataSourceDerivatives class.
    DataSource data_source;
    data_source.add<ode>();
    DataSource ds(data_source);
    const double x = a.random<double>();
    ds.set<double>("x", x);
    ASSERT_NO_THROW(ds.get<double>("dx_dt"));
    ASSERT_DOUBLE_EQ(ds.get<double>("dx_dt"), 2*x);
}

TEST_F(DataSourceTest, a_copied_DataSource_should_be_independant_from_its_copy)
{
    DataSource data_source;
    data_source.add<ode>();
    DataSource ds(data_source);
    const double x1 = a.random<double>();
    const double x2 = a.random<double>();
    data_source.set<double>("x", x1);
    ds.set<double>("x", x2);
    ASSERT_DOUBLE_EQ(data_source.get<double>("dx_dt"), 2*x1);
    ASSERT_DOUBLE_EQ(ds.get<double>("dx_dt"), 2*x2);
}
