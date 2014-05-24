/**
* 
* @file     unittest
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/14
*/
#ifndef HELIXXX_UNITTEST_HPP
#define HELIXXX_UNITTEST_HPP
#include <map>
#include <string>
#include <chrono>
#include <vector>
#include <iostream>
#include <typeinfo>
#include <stdexcept>
#include <lemoonxx/nocopyable.hpp>

namespace lemoon{namespace unittest{

	class check_failed : public std::runtime_error
	{
	public:
		
		check_failed(const char * msg, const char * file,size_t lines)
			:std::runtime_error(msg)
			,_file(file)
			,_lines(lines)
		{

		}

		const std::string& file() const { return _file; }

		
		size_t lines() const { return _lines; }

	private:
		std::string										_file;

		size_t											_lines;
	};

	inline void check(bool condition,const char * message,const char * file,int lines)
	{
		if(!condition)
		{
			throw check_failed(message,file,lines);
		}
	}


	//////////////////////////////////////////////////////////////////////////

	class test_case : private nocopyable
	{
	public:

		test_case(const char * name):_name(name){}

		virtual ~test_case(){}
	public:

		virtual void run(void * userData) = 0;

		const std::string & name() const{return _name;}

	private:

		std::string _name;
	};

	class test_suite : private nocopyable
	{
	public:

		typedef std::vector<test_case*>	test_cases;

		test_suite(const char * name):_name(name){}

		virtual ~test_suite(){}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// @fn	void AddTestCase(TestCase * testCase)
		///
		/// @brief	Adds a test case. 
		///
		/// @author	Yuki
		/// @date	2012/1/14
		///
		/// @param [in,out]	testCase	If non-null, the test case. 
		////////////////////////////////////////////////////////////////////////////////////////////////////

		void add_test_case(test_case * testCase){_children.push_back(testCase);}

		const test_cases & children() const{return _children;}

		const std::string & name() const{return _name;}

	public:

		virtual void* create() = 0;

		virtual void release(void * context) = 0;

	private:

		test_cases						_children;

		std::string						_name;
	};

	template<typename T>
	class test_suite_factory : public test_suite
	{
	public:
		test_suite_factory(const char * name):test_suite(name){}

	private:

		void* create()
		{
			return new T();
		}

		void release(void * context)
		{
			delete (T*)context;
		}
	};

	class package 
	{
	public:
		typedef std::map<const std::type_info*,test_suite*> test_suites;
	private:

		package(){}

	public:

		~package()
		{
			test_suites::const_iterator iter = _suites.begin(),end = _suites.end();

			for(;iter != end; ++ iter)
			{
				delete iter->second;
			}
		}

		static package * Singleton()
		{
			static package p;

			return &p;
		}


		const test_suites & Children() {return _suites;}

	public:

		template<typename SuiteType>
		void Register(const char * suiteName,test_case * testCase)
		{
			if(_suites.end() == _suites.find(&typeid(SuiteType)))
			{
				_suites[&typeid(SuiteType)] = new test_suite_factory<SuiteType>(suiteName);
			}

			_suites[&typeid(SuiteType)]->add_test_case(testCase);
		}

	private:

		test_suites _suites;
	};

	template<typename T>
	class test_case_factory : test_case
	{
	public:

		typedef typename T::testsuite_t						testsuite_t;

		test_case_factory(const char * testSuiteName,const char * name)
			:test_case(name)
		{
			package::Singleton()->Register<testsuite_t>(testSuiteName,this);
		}

		void run(void * userData)
		{
			T t(reinterpret_cast<testsuite_t*>(userData));

			t.run();
		}
	};

	class console_runner : private nocopyable
	{
	public:

		console_runner(const char * name,int , char ** )
			:_name(name)
		{}

		int run()
		{
			std::cout << "Lemon unittest console runner, version 1.9.1" << std::endl;

			std::cout << "Copyright (C) 2012  yayanyang All Rights Reserved" << std::endl;

			std::cout << "[Package] " << _name << std::endl;

			try
			{
				run_url();

				std::cout << "bingo !!!!!!!!!!!!!!!!" << std::endl;

				return 0; 
			}
			catch(const std::exception & e)
			{
				std::cerr << "error!!! " << e.what() << std::endl;

				return 2;
			}
		}

	private:
		void run_url()
		{
			package * p = package::Singleton();

			package::test_suites::const_iterator iter,end = p->Children().end();

			for(iter = p->Children().begin(); iter != end; ++ iter)
			{
				std::cout << "run test suite @" << iter->second->name() << std::endl;

				void * context = iter->second->create();

				try
				{
					test_suite::test_cases::const_iterator iter1,end1 = iter->second->children().end();

					for(iter1 = iter->second->children().begin(); iter1 != end1; ++ iter1)
					{
						std::cout << "run test case @@" << (*iter1)->name() << std::endl;

						using namespace std::chrono;

						steady_clock::time_point begin = steady_clock::now(),end;


						try
						{

							(*iter1)->run(context);

							end = steady_clock::now();
						}

						catch(...)
						{
							std::cout << "run test case @@" << (*iter1)->name() << " -- failed" << std::endl;

							throw;
						}


						std::cout << "run test case @@" << (*iter1)->name() 

							<< " -- success(" << duration_cast<duration<double>>(end - begin).count() << " s)" << std::endl;
					}
				}
				catch(...)
				{
					iter->second->release(context);

					std::cout << "run test suite @" << iter->second->name() << " -- failed" << std::endl;

					throw;
				}

				iter->second->release(context);

				std::cout << "run test suite @" << iter->second->name() << " -- success" << std::endl;
			}
		}
	
	private:

		//int					_args;

		//char				**_argv;

		std::string			_name;

		std::string			_url;

		//size_t				_times;
	};
}}

#define LEMON_UNITTEST_CASE(TestSuiteName,TestCaseName)\
class TestCaseName : private lemoon::nocopyable\
{\
public:\
	typedef TestSuiteName testsuite_t;\
	TestCaseName(testsuite_t* suite):_suite(suite){}\
	testsuite_t* Context(){return _suite;}\
	void run();\
private:\
	testsuite_t *_suite;\
};\
namespace{\
   lemoon::unittest::test_case_factory<TestCaseName> __object_##TestCaseName##_factory(#TestSuiteName,#TestCaseName);\
}\
void TestCaseName::run()


#define lemoon_check(condition)lemoon::unittest::check((bool)(condition),"condition check error --> "#condition,__FILE__,__LINE__)

#define lemoon_expect_exception(exp,exception) try{(exp);lemoon::unittest::check(false,"expect exception failed"#exception,__FILE__,__LINE__);}catch(const exception&){}

#endif //HELIXXX_UNITTEST_HPP
