
tbverdir = ""
if (_ACTION == "vs2003") then tbverdir = "vc7.1"
elseif (_ACTION == "vs2005") then tbverdir = "vc8"
elseif (_ACTION == "vs2008") then tbverdir = "vc9"

elseif (_ACTION == "vs2010") then tbverdir = "vc10"

elseif (_ACTION == "codeblocks") then tbverdir = "cc4.1.0_libc2.4_kernel2.6.16.21"  end

vsVersion = "";
if (_ACTION == "vs2005") then vsVersion = "vs71"
elseif (_ACTION == "vs2008") then vsVersion = "vs8"
elseif (_ACTION == "vs2009") then vsVersion = "vs9"
elseif (_ACTION == "vs2010") then vsVersion = "vs10"
end