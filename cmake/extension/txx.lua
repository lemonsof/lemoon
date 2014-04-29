-- arg[1] source file
-- arg[2] target file
-- arg[3] max length

function step_2(max,rep,l,u)
   if(l == "max") then  l = max end
   if(u == "max") then  u = max end
   result = ""

   for v = l,u,1 do
      result = result .. string.gsub(rep,"<i>",v)
   end

   return result
end

function step_1(max,rep,l,u)
   
   if(l == "max") then  l = max end
   if(u == "max") then  u = max end

   result = ""

   for v = l,u,1 do

      result = result .. string.gsub(rep,"<<i>>",v)
   end
   
   return result
end

function compile(max,source)
   
   local target,matchs = string.gsub(source,
				     "%$%$(.-)%$%$%(([^%$,]+),([^%$%)]+)%)",
				     function(rep,l,u)
					return step_1(max,rep,l,u)
				     end)

   target,matchs = string.gsub(target,
			       "%$([^%$]-)%$%(([^%$,]+),([^%$%)]+)%)",
			       function(rep,l,u)
				  return step_2(max,rep,l,u)
			       end)

   return target

end

-- get the template max length
maxLength = tonumber(arg[3])

print("compile ctg file :" .. arg[1])

sourcefile = assert(io.open(arg[1],"r"),"can't open file to read :" .. arg[1]):read("*a")

file = assert(io.open(arg[2],"w+"),"can't open file to write :" .. arg[2])

file:write(compile(maxLength,sourcefile))

file:close()