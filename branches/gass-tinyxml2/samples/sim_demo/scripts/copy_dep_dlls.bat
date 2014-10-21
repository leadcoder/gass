rem ECHO Copy dependencies dll files
IF "%1" == "Debug" (
	::copying 
 	copy "%2\tbb_debug.dll" "%3"
	copy "%2\ois_d.dll" "%3"
	copy "%2\OpenAL32.dll" "%3"
)
IF "%1" == "Release" (
	::copying 
 	copy "%2\tbb.dll" "%3"
	copy "%2\ois.dll" "%3"
	copy "%2\OpenAL32.dll" "%3"
)

pause
