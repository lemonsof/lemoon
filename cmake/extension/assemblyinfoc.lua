-- assemblyinfoc (The Assembly Information Compiler)
-- arg[1] ; the assembly information source file
-- arg[2] ; the default version string
-- arg[3] ; the generate assembly information metadata c/c++ source files's output directory
-- arg[4] ; the assembly name
-- compile the assembly information source file to binary type,and generate metadata c/c++ source files

title = 
[==[
/**
 * this file is auto generate by assemblyinfoc,do not modify it
 * @file     assembly.h
 * @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     2012/01/14
 */
]==]

rc=
[==[
#define APSTUDIO_READONLY_SYMBOLS
/////////////////////////////////////////////////////////////////////////////
//
// Generated from the TEXTINCLUDE 2 resource.
//
#include "afxres.h"

/////////////////////////////////////////////////////////////////////////////
#undef APSTUDIO_READONLY_SYMBOLS

/////////////////////////////////////////////////////////////////////////////
// Chinese (Simplified, PRC) resources

#if !defined(AFX_RESOURCE_DLL) || defined(AFX_TARG_CHS)
LANGUAGE LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED

#ifdef APSTUDIO_INVOKED
/////////////////////////////////////////////////////////////////////////////
//
// TEXTINCLUDE
//

1 TEXTINCLUDE 
BEGIN
    "resource1.h\0"
END

2 TEXTINCLUDE 
BEGIN
    "#include ""afxres.h""\r\n"
    "\0"
END

3 TEXTINCLUDE 
BEGIN
    "\r\n"
    "\0"
END

#endif    // APSTUDIO_INVOKED


/////////////////////////////////////////////////////////////////////////////
//
// Version
//

VS_VERSION_INFO VERSIONINFO
 FILEVERSION ${version}
 PRODUCTVERSION ${version}
 FILEFLAGSMASK 0x3fL
#ifdef _DEBUG
 FILEFLAGS 0x1L
#else
 FILEFLAGS 0x0L
#endif
 FILEOS 0x40004L
 FILETYPE 0x0L
 FILESUBTYPE 0x0L
BEGIN
    BLOCK "StringFileInfo"
    BEGIN
        BLOCK "080404b0"
        BEGIN
            VALUE "CompanyName", "${company}"
            VALUE "FileDescription", "${description}"
            VALUE "FileVersion", "${version_string}"
            VALUE "InternalName", "${name}"
            VALUE "LegalCopyright", "Copyright (C) ${YEAR} ${company} All Rights Reserved."
            VALUE "OriginalFilename", "${name}"
            VALUE "ProductName", "${name}"
            VALUE "ProductVersion", "${version_string}"
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
        VALUE "Translation", 0x804, 1200
    END
END

#endif    // Chinese (Simplified, PRC) resources
/////////////////////////////////////////////////////////////////////////////



#ifndef APSTUDIO_INVOKED
/////////////////////////////////////////////////////////////////////////////
//
// Generated from the TEXTINCLUDE 3 resource.
//


/////////////////////////////////////////////////////////////////////////////
#endif    // not APSTUDIO_INVOKED
]==]

-- the log function
function Log(message)
   print("[assembly info compiler] " .. message)
end

-- @description replace originVersion string's "*" to auto generate vesion number
-- function name :GetRealVersion(orginVersion)
-- @args originVersion,the origin version string
-- @return the realversion string
function GetRealVersion(originVersion)

   print("parse version string: " .. originVersion)

   local version = {}

   version[0],version[1],version[2],version[3] = originVersion:match("(%d+)%.(%d+)%.(%d+)%.(%d+)$")

   if(nil == version[0]) then
      version[0],version[1],version[2],version[3] = originVersion:match("(%d+)%.(%d+)%.(%d+)%.%*$")
      if(nil == version[0]) then
	 version[0],version[1],version[2],version[3] = originVersion:match("(%d+)%.(%d+)%.%*$")
      end
   end
   
   if(nil == version[0]) then assert(false,"invalid version string :" .. originVersion) end

   version[0] = assert(tonumber(version[0]),"invalid version string :" .. originVersion)

   version[1] = assert(tonumber(version[1]),"invalid version string :" .. originVersion)

   t = os.date("*t",os.difftime(os.time(),os.time{year=2010, month=1, day=1}))

   if(nil == version[2]) then
      version[2] = t.yday
   else
      version[2] = assert(tonumber(version[2]),"invalid version string :" .. originVersion)
   end

   if(nil == version[3]) then
      version[3] = math.floor((t.hour * 3600 + t.min * 60 + t.sec) / 2)
   else
      version[3] = assert(tonumber(version[3]),"invalid version string :" .. originVersion)
   end

   print("real version string: " .. version[0] .. "." .. version[1] .. "." .. version[2] .. "." .. version[3])

   return version

end

-- load and run the assembly information source lua file
Log("load assembly information define file :" .. arg[1])
dofile(arg[1])
Log("load assembly information define file :" .. arg[1] .. " -- success")
assert(assembly ~= nil,"assembly information is empty :" .. arg[1])
--assert(assembly.name ~= nil,"assembly name must be set :" .. arg[1])
assert(assembly.guid ~= nil,"assembly guid must be set :" .. arg[1])

-- get the prefix string
prefix = string.upper(string.gsub(arg[4],"[%-.]","_"))
prefix = string.gsub(prefix , "%+","X")
-- get the version
version = GetRealVersion(arg[2])
-- get the guid string
guid = assembly.guid
-- now generate the assembly.h assembly.cpp files

assembly_h = arg[3] .. "/assembly.h"

assembly_cpp = arg[3] .. "/assembly.cpp"

errorcode_h = arg[3] .. "/errorcode.h"

assembly_rc = arg[3] .. "/assembly.rc"

headFile = assert(io.open(assembly_h,"w+"),"can't open file to write :" .. assembly_h)

sourceFile = assert(io.open(assembly_cpp,"w+"),"can't open file to write :" .. assembly_cpp)

--errorFile = assert(io.open(errorcode_h,"w+"),"can't open file to write :" .. errorcode_h)

rcFile = assert(io.open(assembly_rc,"w+"),"can't open file to write :" .. assembly_rc)

headFile:write(title)

headFile:write("#ifndef " .. prefix .. "_ASSEMBLY_H\n")
headFile:write("#define " .. prefix .. "_ASSEMBLY_H\n")


headFile:write("#include \"configure.h\"\n")
headFile:write("#include <helix/abi.h>\n\n")
sourceFile:write("#include \"assembly.h\"\n\n")

headFile:write(prefix .. "_API const helix_uuid_t " .. prefix .. "_GUID;\n\n")
headFile:write(prefix .. "_API const helix_version_t " .. prefix .. "_VERSION;\n\n")
sourceFile:write("const helix_uuid_t " .. prefix .. "_GUID = " .. guid .. ";\n\n")
sourceFile:write("const helix_version_t " .. prefix .. "_VERSION = {" .. version[0] .. "," .. version[1] .. "," .. version[2] .. "," .. version[3] .. "};\n\n")

index = 0

if(nil ~= assembly.errorcode) then

   for k,v in ipairs(assembly.errorcode) do 
      headFile:write(prefix .. "_API const helix_errno_t " .. prefix .. "_" .. v.name .. ";\n\n")
      sourceFile:write("const helix_errno_t " .. prefix .. "_" .. v.name .. " = {&" .. prefix .. "_GUID," .. k .. "};\n\n")
   end

end

headFile:write("#endif //" .. string.upper(prefix) .. "_ASSEMBLY_H\n")

rc = string.gsub(rc,"${version}",version[0] .. "," .. version[1] .. "," .. version[2] .. "," .. version[3])

rc = string.gsub(rc,"${version_string}",version[0] .. "." .. version[1] .. "." .. version[2] .. "." .. version[3])

rc = string.gsub(rc,"${YEAR}",os.date("%Y",os.time()))

if(nil ~= assembly.company) then rc = string.gsub(rc,"${company}",assembly.company) end

if(nil ~= assembly.description) then rc = string.gsub(rc,"${description}",assembly.description) end

if(nil ~= assembly.name) then rc = string.gsub(rc,"${name}",assembly.name) end

rcFile:write(rc)

headFile:close()
sourceFile:close()

rcFile:close()


