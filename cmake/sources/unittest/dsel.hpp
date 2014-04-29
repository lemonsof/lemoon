/**
* 
* @file     desl
* @brief    Copyright (C) 2011  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2011/12/05
*/
#ifndef LEMON_CXX_UNITTEST_DESL_HPP
#define LEMON_CXX_UNITTEST_DESL_HPP
#include <map>
#include <vector>
#include <typeinfo>
#include <iostream>
#include <stdexcept>
#include <cosmos/sys/abi.h>
#include <cosmos/assembly.h>

namespace cosmos{namespace unittest{namespace cxx{

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @class	ConditionException
	///
	/// @brief	Exception for  condition errors. 
	///
	/// @author	Yayanyang
	/// @date	2011/12/14
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class ConditionException : public std::runtime_error
	{
	public:

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// @fn	ConditionException(const char * message,LemonErrorCode errorCode)
		///
		/// @brief	Constructor. 
		///
		/// @author	Yayanyang
		/// @date	2011/12/14
		///
		/// @param [in,out]	message	If non-null, the message. 
		/// @param	errorCode		The error code. 
		////////////////////////////////////////////////////////////////////////////////////////////////////

		ConditionException(const char * message,const CosmosErrorInfo & errorCode)
			:std::runtime_error(message)
			,_errorCode(errorCode)
		{

		}

		~ConditionException() throw(){}

	        const CosmosErrorInfo & ErrorCode() const{return _errorCode;}

	private:

		std::string   				_message;

		CosmosErrorInfo				_errorCode;
	};

	inline void Check(bool condition,const char * message,const char * file,int lines)
	{
		if(!condition)
		{
			COSMOS_DECLARE_ERRORINFO(errorCode);

			COSMOS_USER_ERROR(errorCode,COSMOS_UNITTEST_FAILED);

			errorCode.File = file;

			errorCode.Lines = lines;

			throw ConditionException(message,errorCode);
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @class	TestCase
	///
	/// @brief	Test case. 
	///
	/// @author	Yuki
	/// @date	2012/1/14
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class TestCase 
	{
	protected:
		TestCase(const char * name):_name(name){}
		virtual ~TestCase(){}
	public:

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// @fn	virtual void Run(void * userData) = 0
		///
		/// @brief	Runs. 
		///
		/// @author	Yuki
		/// @date	2012/1/14
		///
		/// @param [in,out]	userData	If non-null, information describing the user. 
		////////////////////////////////////////////////////////////////////////////////////////////////////

		virtual void Run(void * userData) = 0;

		const std::string & Name() const{return _name;}
	private:

		std::string _name;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @class	TestSuite
	///
	/// @brief	Test suite. 
	///
	/// @author	Yuki
	/// @date	2012/1/14
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class TestSuite 
	{
	public:

		typedef std::vector<TestCase*>	TestCases;

		TestSuite(const char * name):_name(name){}

		virtual ~TestSuite(){}

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

		void AddTestCase(TestCase * testCase){_children.push_back(testCase);}

		const TestCases & Children() const{return _children;}

		const std::string &  Name() const{return _name;}

	public:

		virtual void* Create() = 0;

		virtual void Release(void * context) = 0;

	private:

		TestCases						_children;

		std::string						_name;
	};

	template<typename T>
	class TestSuiteFactory : public TestSuite
	{
	public:
		TestSuiteFactory(const char * name):TestSuite(name){}

	private:

		void* Create()
		{
			return new T();
		}

		void Release(void * context)
		{
			delete (T*)context;
		}
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @class	Package
	///
	/// @brief	Package. 
	///
	/// @author	Yuki
	/// @date	2012/1/14
	////////////////////////////////////////////////////////////////////////////////////////////////////

	class Package
	{
	public:
		typedef std::map<const std::type_info*,TestSuite*> TestSuites;
	private:

		Package(){}

	public:

		~Package()
		{
			TestSuites::const_iterator iter = _suites.begin(),end = _suites.end();

			for(;iter != end; ++ iter)
			{
				delete iter->second;
			}
		}

		static Package * Singleton()
		{
			static Package *package = new Package();

			return package;
		}


		const TestSuites & Children() {return _suites;}

	public:

		template<typename SuiteType>
		void Register(const char * suiteName,TestCase * testCase)
		{
			if(_suites.end() == _suites.find(&typeid(SuiteType)))
			{
				_suites[&typeid(SuiteType)] = new TestSuiteFactory<SuiteType>(suiteName);
			}

			_suites[&typeid(SuiteType)]->AddTestCase(testCase);
		}

	private:

		TestSuites _suites;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @property	template<typename T> class TestCaseFactory
	///
	/// @brief	 the test case factory. 
	///
	/// @return	The test case factory. 
	////////////////////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	class TestCaseFactory : TestCase
	{
	public:

		typedef typename T::testsuite_t						testsuite_t;

		TestCaseFactory(const char * testSuiteName,const char * name)
			:TestCase(name)
		{
			Package::Singleton()->Register<testsuite_t>(testSuiteName,this);
		}

		void Run(void * userData)
		{
			T t(reinterpret_cast<testsuite_t*>(userData));

			t.Run();
		}
	};

}}}

///////////////////////////////////////////////////////////////////////////
//the unittest macro defined
// 
#define COSMOS_UNITTEST_CASE(TestSuiteName,TestCaseName)\
class TestCaseName : private lemon::nocopyable\
{\
public:\
	typedef TestSuiteName testsuite_t;\
	TestCaseName(testsuite_t* suite):_suite(suite){}\
	testsuite_t* Context(){return _suite;}\
	void Run();\
private:\
	testsuite_t *_suite;\
};\
namespace{\
	cosmos::unittest::cxx::TestCaseFactory<TestCaseName> __object_##TestCaseName##_factory(#TestSuiteName,#TestCaseName);\
}\
void TestCaseName::Run()

#define COSMOS_CHECK(condition) cosmos::unittest::cxx::Check((bool)(condition),"condition check error --> "#condition,__FILE__,__LINE__)

#define COSMOS_UNITTEST_CHECK(condition) cosmos::unittest::cxx::Check((bool)(condition),"condition check error --> "#condition,__FILE__,__LINE__)

#define COSMOS_UNITTEST_EXPECT_EXCEPTION(exp,Expression) try{(exp);cosmos::unittest::cxx::Check(false,"expect exception failed"#Expression,__FILE__,__LINE__);}catch(const Expression&){}

#endif //LEMON_CXX_UNITTEST_DESL_HPP
