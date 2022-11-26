onInit = function(object)
	local component = object:GetComponent('OSGLocationComponent');
	component:LogMessage('hejsan');
	local name = component:GetStringAttribute('Name');
	component:LogMessage(name);
	--print 'print works!'
end