print("Lemon Program Option Compiler Version lua.1.0")
print("Copyright (C) 2012  yayanyang All Rights Reserved")

title = 
[==[
/**
 * this file is auto generate by programoptionc,do not modify it
 * @file     po.hpp
 * @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     ${DATE}
 */
]==]

class_def = 
[==[
	class ProgramOption : private lemon::nocopyable
	{
	public:
		ProgramOption();
		
		~ProgramOption();
	
		void Read(const lemon::String & path);
		
		void Write(const lemon::String & path);
	public:
		${Properties}
	private:
		${Fileds}
	};
]==]

class_impl = 

[==[

	class ProgramOption_lua : private lemon::nocopyable
	{
	public:
		ProgramOption_lua()
		{
			L = luaL_newstate();
		
			if(NULL == L)
			{	
				LEMON_DECLARE_ERRORINFO(errorCode);
		
				LEMON_USER_ERROR(errorCode,LEMON_SYS_OPEN_LUA_ERROR);

				throw lemon::Exception(errorCode);
			}
		
			luaL_openlibs(L);
		}
		
		~ProgramOption_lua()
		{
			lua_close(L);
		}
		
		operator lua_State * () {return L;}
		
	private:
		lua_State 	*L;
	};

	ProgramOption::ProgramOption()
	{
		${INIT}
	}
		
	ProgramOption::~ProgramOption()
	{
	}
	
	void ProgramOption_ThrowGetPropertyException(const char * key)
	{
		LEMON_DECLARE_ERRORINFO(errorCode);

		LEMON_USER_ERROR(errorCode,LEMON_SYS_PO_TYPE_ERROR);

		throw lemon::Exception(key,errorCode);
	}
	
	bool ProgramOption_GetProperty(const char * key,std::string &val,lua_State * _L)
	{
		lua_getglobal(_L,key);

		if(lua_isnil(_L,-1)) return false;

		if(!lua_isstring(_L,-1)) ProgramOption_ThrowGetPropertyException(key);

		val = lua_tostring(_L,-1);

		lua_pop(_L,1);
		
		return true;
	}
		
	bool ProgramOption_GetProperty(const char * key,double &val,lua_State * _L)
	{
		lua_getglobal(_L,key);

		if(lua_isnil(_L,-1)) return false;

		if(!lua_isnumber(_L,-1)) ProgramOption_ThrowGetPropertyException(key);
		
		val = lua_tonumber(_L,-1);

		lua_pop(_L,1);

		return true;
	}		
	
	void ProgramOption::Read(const lemon::String & path)
	{		
		ProgramOption_lua L;
		
		if(0 != luaL_dofile(L,lemon::to_locale(path).c_str()))
		{
			LEMON_DECLARE_ERRORINFO(errorCode);

			LEMON_USER_ERROR(errorCode,LEMON_SYS_LUA_DOFILE_ERROR);

			lemon::Exception e(lua_tostring(L,-1),errorCode);

			lua_pop(L,1);

			throw e;
		}
		
		${READ}
	}
	
	void ProgramOption::Write(const lemon::String & path)
	{
		std::ofstream stream(lemon::to_locale(path).c_str(),std::ios::trunc);

		if(!stream.is_open())
		{
			lemon::error_info errorCode;

			LEMON_USER_ERROR(errorCode,LEMON_SYS_CREATE_PO_SCRIPT_FILE_ERROR);

			errorCode.check_throw();
		}
		
		${WRITE}
	}

]==]



function trace(message)
   print("[lemon-poc] " .. message)
end

function propertyName(source,firstupper)
	
	toupper = firstupper
	
	return string.gsub(source,"([_%w])",
						function(s) 
							if toupper == true then
								toupper = false
								return string.upper(s) 
							else 
								if s == "_" then 
									toupper = true 
									return ""
								else
									return s
								end
							end
						end)
end

function write_properties(space,options)
	assert(type(options) == "table","the program_option.options is not table")
	
	codes = ""
	
	for k,v in pairs(options) do
	
		local name = propertyName(k,true)
		
		local name1 = propertyName(k,false)
		
		if(type(v.value) == "string") then 
			codes = codes .. space .. "void " .. name .. "(" .. "const std::string & val) { _" .. name1 .. " = val; }\n"  
			
			codes = codes .. space .. "const std::string & " .. name .. "() const { return _" .. name1 .. "; }\n"  
		end
		
		if(type(v.value) == "number") then  
			codes = codes .. space .. "void " .. name .. "(" .. "double val) { _" .. name1 .. " = val; }\n"  
			
			codes = codes .. space .. "double " .. name .. "() const { return _" .. name1 .. "; }\n"  
		end
		
	end
	
	return codes
end

function write_fields(space,options)
	assert(type(options) == "table","the program_option.options is not table")
	
	codes = ""
	
	for k,v in pairs(options) do
	
		local name = propertyName(k,false)
		
		codes = codes .. space
		
		if(type(v.value) == "string") then  codes = codes .. "std::string\t_" .. name .. ";\n"  end
		
		if(type(v.value) == "number") then  codes = codes .. "double \t_" .. name .. ";\n"  end
		
	end
	
	return codes
end

function write_fields_init(space,options)
	assert(type(options) == "table","the program_option.options is not table")
	
	local codes = ""
	
	for k,v in pairs(options) do
	
		local name = propertyName(k,false)
		
		codes = codes .. space
		
		if(type(v.value) == "string") then  codes = codes .. "_" .. name .. " = \"" .. v.value .. "\";\n" end
		
		if(type(v.value) == "number") then  codes = codes .. "_" .. name .. " = " .. v.value .. ";\n"  end
		
	end
	
	return codes
end

function write_write(space,prefix,options)
	
	assert(type(options) == "table","the program_option.options is not table")
	
	prefix = string.gsub(prefix,"[%.]","_")
	
	local codes = ""
	
	for k,v in pairs(options) do
		
		assert(k ~= nil,"parse po file error : program_option need define value filed")
	
		if(v.description ~= nil) then codes = codes .. space .. 'stream << "--' .. v.description .. '" << std::endl;' end
	
		codes = codes .. space .. 'stream << "' .. prefix .. "_" .. k .. ' = '
		
		if(type(v.value) == "string") then
			codes = codes .. '\\"' .. v.value  .. '\\"" << std::endl << std::endl;'
		end
		
		if(type(v.value) == "number") then
			codes = codes .. v.value .. '" << std::endl << std::endl;'
		end
	end
	
	return codes
end

function write_read(space,prefix,options)
	
	assert(type(options) == "table","the program_option.options is not table")
	
	prefix = string.gsub(prefix,"[%.]","_")
	
	local codes = ""
	
	for k,v in pairs(options) do
		--codes = codes .. space .. 'stream << "--' .. v.description .. '" << std::endl;'
		
		local name = propertyName(k,false)
	
		codes = codes .. space .. 'ProgramOption_GetProperty("' ..  prefix .. "_" .. k .. '", _'.. name .. ",L);\n"
	end
	
	return codes 
end

trace("load program option file :" .. arg[1])

dofile(arg[1])

trace("load program option file :" .. arg[1] .. " -- success")

assert(program_option ~= nil,"parse po file error : program_option is empty.")

assert(program_option.namespace ~= nil,"parse po file error : program_option.namespace is empty.")

trace("program option namespace :" .. program_option.namespace)

header = arg[2] .. "/po.hpp"

source = arg[2] .. "/po.cpp"

trace("open file to write native codes :" .. header)

headerFile = assert(io.open(header,"w+"),"can't open file to write :" .. header)

trace("open file to write native codes :" .. source)

sourceFile = assert(io.open(source,"w+"),"can't open file to write :" .. source)

guard = string.upper(string.gsub(program_option.namespace,"[%-%.]","_")) .. "_PO_HPP\n"

title = string.gsub(title,"${DATE}",os.date("%x", os.time()))

headerFile:write(title)

headerFile:write("#ifndef " .. guard)
headerFile:write("#define " .. guard .. "\n")

headerFile:write("#include <lemonxx/sys/sys.hpp>\n")
headerFile:write("#include <lemonxx/utility/utility.hpp>\n\n")

-- create begin namespace 
headerFile:write("namespace " .. string.gsub(program_option.namespace,"%.(%w+)","{ namespace %1") .. "{\n\n")

class_def = string.gsub(class_def,"(%s*)%${Properties}",
						function(space)
							return write_properties(space,program_option.options)
						end)
						
class_def = string.gsub(class_def,"(%s*)%${Fileds}",
						function(space)
							return write_fields(space,program_option.options)
						end)
					
headerFile:write(class_def)					

-- create end namespace 
headerFile:write(string.gsub(string.gsub(program_option.namespace,"%w+","}"),"%.","") .. "\n\n")

headerFile:write("#endif //" .. guard)

-- write sourceFile

sourceFile:write("#include \"po.hpp\"\n\n")
sourceFile:write("#include <fstream>\n\n")
sourceFile:write("#include <memory>\n\n")
sourceFile:write("#include <lemon/lua/lua.hpp>\n\n")
sourceFile:write("#include <lemon/sys/assembly.h>\n")

sourceFile:write("namespace " .. string.gsub(program_option.namespace,"%.(%w+)","{ namespace %1") .. "{\n\n")

class_impl = string.gsub(class_impl,"(%s*)%${INIT}",
						function(space)
							return write_fields_init(space,program_option.options)
						end)
						
class_impl = string.gsub(class_impl,"(%s*)%${WRITE}",
						function(space)
							return write_write(space,program_option.namespace,program_option.options)
						end)
						
class_impl = string.gsub(class_impl,"(%s*)%${READ}",
						function(space)
							return write_read(space,program_option.namespace,program_option.options)
						end)
						
sourceFile:write(class_impl)

sourceFile:write(string.gsub(string.gsub(program_option.namespace,"%w+","}"),"%.","") .. "\n\n")






