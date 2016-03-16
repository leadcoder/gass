
function debugMessage(message)
	--local script_man = GASS.LuaScriptManager_GetPtr()
	local dbmess = GASS.ToDebugPrintMessage(message)
	local name = dbmess:GetText()
	local newItem = CEGUI.createListboxTextItem(name, 0, nil, false, true)
    newItem:setSelectionBrushImage("TaharezLook", "GenericBrush")
	local winMgr = CEGUI.WindowManager:getSingleton()
    CEGUI.toListbox(winMgr:getWindow("Demo8/Window1/Listbox")):addItem(newItem)
end
-----------------------------------------
-- Handler to add an item to the box
-----------------------------------------
function clientHandler(args)
	local winMgr = CEGUI.WindowManager:getSingleton()
    local sim = GASS.SimEngine_GetPtr()
	local ssm =  sim:GetSimSystemManager()
	
	winMgr:getWindow("Network/ClientWindow/PortEditbox"):setText("1001")
	
	local client_mess = GASS.StartClientMessage("CEGUIClient",1002,1001)
	local mess = GASS.MessagePtr(client_mess)
	ssm:get():PostMessage(mess)
	
	winMgr:getWindow("Network/ClientWindow"):setVisible(true)
	winMgr:getWindow("Network/BootWindow"):setVisible(false)
	
end

function serverResponseHandler(message)

	local winMgr = CEGUI.WindowManager:getSingleton()
	local sr_mess = GASS.ToServerResponseMessage(message)
	local list_box = CEGUI.toListbox(winMgr:getWindow("Network/ClientWindow/ServerListbox"))
	
	local server_name = sr_mess:GetServerName()
	
	--check if server already exist?
	local new_item = true
	for i = 0 , (list_box:getItemCount()-1) do
		local item = list_box:getListboxItemFromIndex(i)
		if item:getText() == server_name then
			new_item = false
			break
		end
	end
	if new_item then
		local newItem = CEGUI.createListboxTextItem(server_name, 0, nil, false, true)
		newItem:setSelectionBrushImage("OgreTrayImages", "GenericBrush")
		list_box:addItem(newItem)
	end
end

function serverDisconnectedHandler(message)
	local winMgr = CEGUI.WindowManager:getSingleton()
	winMgr:getWindow("Network/ClientWindow"):setVisible(true)
	winMgr:getWindow("Network/ClientRuntimeWindow"):setVisible(false)
	updateClientDialog()
end

function serverHandler(args)
	local winMgr = CEGUI.WindowManager:getSingleton()
 	winMgr:getWindow("Network/ServerWindow/PortEditbox"):setText("1001")
	
	winMgr:getWindow("Network/ServerWindow"):setVisible(true)
	winMgr:getWindow("Network/BootWindow"):setVisible(false)
	
end

function backServerHandler(args)
	local winMgr = CEGUI.WindowManager:getSingleton()
 	
	winMgr:getWindow("Network/ServerWindow"):setVisible(false)
	winMgr:getWindow("Network/BootWindow"):setVisible(true)
end


function backClientHandler(args)
	local winMgr = CEGUI.WindowManager:getSingleton()
 	
	winMgr:getWindow("Network/ClientWindow"):setVisible(false)
	winMgr:getWindow("Network/BootWindow"):setVisible(true)
	
	local sim = GASS.SimEngine_GetPtr()
	local ssm =  sim:GetSimSystemManager()
	
	local client_mess = GASS.StopClientMessage()
	local mess = GASS.MessagePtr(client_mess)
	ssm:get():PostMessage(mess)
	
end


function clientConnectedHandler(message)
	local winMgr = CEGUI.WindowManager:getSingleton()
	local cc_mess = GASS.ToClientConnectedMessage(message)
	local list_box = CEGUI.toListbox(winMgr:getWindow("Network/ServerStatusWindow/ClientListbox"))
	local client_name = cc_mess:GetClientName()
	
	local new_item = true
	for i = 0 , list_box:getItemCount()-1 do
		local item = list_box:getListboxItemFromIndex(i)
		if item:getText() == client_name then
			new_item = false
			break
		end
	end
	if new_item then
		local newItem = CEGUI.createListboxTextItem(client_name, 0, nil, false, true)
		newItem:setSelectionBrushImage("OgreTrayImages", "GenericBrush")
		list_box:addItem(newItem)
	end
	
end

function clientDisconnectedHandler(message)
	local winMgr = CEGUI.WindowManager:getSingleton()
	local cc_mess = GASS.ToClientDisconnectedMessage(message)
	local list_box = CEGUI.toListbox(winMgr:getWindow("Network/ServerStatusWindow/ClientListbox"))
	local client_name = cc_mess:GetClientName()

	local client_item = nil
	for i = 0, list_box:getItemCount()-1 do
		local item = list_box:getListboxItemFromIndex(i)
		if item:getText() == client_name then
			list_box:removeItem(item)	
			break
		end
	end
end

function updateClientDialog()
	local winMgr = CEGUI.WindowManager:getSingleton()
	local list_box = CEGUI.toListbox(winMgr:getWindow("Network/ClientWindow/ServerListbox"))
	--clear server list
	list_box:resetList()
    local sim = GASS.SimEngine_GetPtr()
	local ssm =  sim:GetSimSystemManager()
	local server_port = winMgr:getWindow("Network/ClientWindow/PortEditbox"):getText()
	local ping_mess = GASS.PingRequestMessage(server_port)
	local mess = GASS.MessagePtr(ping_mess)
	ssm:get():PostMessage(mess)
end

function refreshHandler(args)
	updateClientDialog()
end

function startServerHandler(args)
	local winMgr = CEGUI.WindowManager:getSingleton()
    local sim = GASS.SimEngine_GetPtr()
	local ssm =  sim:GetSimSystemManager()
	
	local port = winMgr:getWindow("Network/ServerWindow/PortEditbox"):getText()
	local server_mess = GASS.StartServerMessage("CEGUIServer",port)
	local mess = GASS.MessagePtr(server_mess)
	ssm:get():PostMessage(mess)
	
	winMgr:getWindow("Network/ServerWindow"):setVisible(false)
	winMgr:getWindow("Network/ServerStatusWindow"):setVisible(true)
	winMgr:getWindow("ScenarioWindow"):setVisible(true)
	
end

function stopServerHandler(args)

	local winMgr = CEGUI.WindowManager:getSingleton()
    local sim = GASS.SimEngine_GetPtr()
	local ssm =  sim:GetSimSystemManager()
	local server_mess = GASS.StopServerMessage()
	local mess = GASS.MessagePtr(server_mess)
	ssm:get():PostMessage(mess)
	
	local list_box = CEGUI.toListbox(winMgr:getWindow("Network/ServerStatusWindow/ClientListbox"))
	list_box:resetList()

	winMgr:getWindow("Network/ServerWindow"):setVisible(true)
	winMgr:getWindow("Network/ServerStatusWindow"):setVisible(false)
	winMgr:getWindow("ScenarioWindow"):setVisible(false)
end

function connectClientHandler(args)
	local winMgr = CEGUI.WindowManager:getSingleton()
    local sim = GASS.SimEngine_GetPtr()
	local ssm =  sim:GetSimSystemManager()
	
	local list_box = CEGUI.toListbox(winMgr:getWindow("Network/ClientWindow/ServerListbox"))
	local item = list_box:getFirstSelectedItem()
	if not (item == nil) then
		local server_name = item:getText()
		local server_port = winMgr:getWindow("Network/ClientWindow/PortEditbox"):getText()
		local server_mess = GASS.ConnectToServerMessage(server_name,server_port)
		local mess = GASS.MessagePtr(server_mess)
		ssm:get():PostMessage(mess)
		winMgr:getWindow("Network/ClientWindow"):setVisible(false)
		winMgr:getWindow("Network/ClientRuntimeWindow"):setVisible(true)
	end
end


function disconnectClientHandler(args)
	local winMgr = CEGUI.WindowManager:getSingleton()
    local sim = GASS.SimEngine_GetPtr()
	local ssm =  sim:GetSimSystemManager()
	
	local client_mess = GASS.StopClientMessage()
	local mess = GASS.MessagePtr(client_mess)
	ssm:get():PostMessage(mess)
	local client_start_mess = GASS.StartClientMessage("CEGUIClient",1002,1001)
	ssm:get():PostMessage(GASS.MessagePtr(client_start_mess))
	
	winMgr:getWindow("Network/ClientWindow"):setVisible(true)
	winMgr:getWindow("Network/ClientRuntimeWindow"):setVisible(false)
	--local list_box = CEGUI.toListbox(winMgr:getWindow("Network/ClientWindow/ServerListbox"))
end


function startScenarioHandler(args)
	local sim = GASS.SimEngine_GetPtr()
	local ssm =  sim:GetSimSystemManager()
	local winMgr = CEGUI.WindowManager:getSingleton()
	local list_box = CEGUI.toListbox(winMgr:getWindow("ScenarioWindow/ScenarioListbox"))
	local item = list_box:getFirstSelectedItem()
	if not (item == nil) then
		local scenario_name = item:getText()
		scenario_name = "../data/scenarios/" .. scenario_name  
		--winMgr:getWindow("Network/ServerWindow"):setText(scenario_name)
		local scenario_mess = GASS.StartSceanrioRequestMessage(scenario_name)
		local mess = GASS.MessagePtr(scenario_mess)
		ssm:get():PostMessage(mess)
	end
end

-----------------------------------------
-- Script Entry Point
-----------------------------------------
local guiSystem = CEGUI.System:getSingleton()
local schemeMgr = CEGUI.SchemeManager:getSingleton()
local winMgr = CEGUI.WindowManager:getSingleton()

-- load our demo8 scheme
schemeMgr:create("OgreTray.scheme");
-- load our demo8 window layout
local root = winMgr:loadWindowLayout("GASSDemo1.layout")
-- set the layout as the root
guiSystem:setGUISheet(root)
-- set default mouse cursor
guiSystem:setDefaultMouseCursor("OgreTrayImages", "MouseArrow")
-- set the Tooltip type
guiSystem:setDefaultTooltip("OgreTray/Tooltip")


local newItem = CEGUI.createListboxTextItem("ogre_demo_scenario", 0, nil, false, true)
newItem:setSelectionBrushImage("OgreTrayImages", "GenericBrush")
CEGUI.toListbox(winMgr:getWindow("ScenarioWindow/ScenarioListbox")):addItem(newItem)


winMgr:getWindow("Network/ServerWindow"):setVisible(false)
winMgr:getWindow("Network/ClientWindow"):setVisible(false)
winMgr:getWindow("Network/ServerStatusWindow"):setVisible(false)
winMgr:getWindow("Network/ClientRuntimeWindow"):setVisible(false)
winMgr:getWindow("ScenarioWindow"):setVisible(false)

-- subscribe required events
winMgr:getWindow("Network/BootWindow/Controls/Client"):subscribeEvent("Clicked", "clientHandler")
winMgr:getWindow("Network/BootWindow/Controls/Server"):subscribeEvent("Clicked", "serverHandler")
winMgr:getWindow("Network/ServerWindow/StartServer"):subscribeEvent("Clicked", "startServerHandler")
winMgr:getWindow("Network/ServerWindow/Back"):subscribeEvent("Clicked", "backServerHandler")
winMgr:getWindow("Network/ClientWindow/Refresh"):subscribeEvent("Clicked", "refreshHandler")
winMgr:getWindow("Network/ClientWindow/Connect"):subscribeEvent("Clicked", "connectClientHandler")
winMgr:getWindow("Network/ClientWindow/Back"):subscribeEvent("Clicked", "backClientHandler")
winMgr:getWindow("Network/ServerStatusWindow/StopServer"):subscribeEvent("Clicked", "stopServerHandler")
winMgr:getWindow("Network/ClientRuntimeWindow/Disconnect"):subscribeEvent("Clicked", "disconnectClientHandler")
winMgr:getWindow("ScenarioWindow/StartScenario"):subscribeEvent("Clicked", "startScenarioHandler")

local sim = GASS.SimEngine_GetPtr()
local ssm =  sim:GetSimSystemManager()

local client_connected_mess = GASS.MessagePtr(GASS.ClientConnectedMessage("reg", 0))
ssm:get():RegisterForSystemMessage("clientConnectedHandler",client_connected_mess)
	
local client_disconnected_mess = GASS.MessagePtr(GASS.ClientDisconnectedMessage("reg", 0))
ssm:get():RegisterForSystemMessage("clientDisconnectedHandler",client_disconnected_mess )


local server_response_mess = GASS.MessagePtr(GASS.ServerResponseMessage("reg", 0, 0))
ssm:get():RegisterForSystemMessage("serverResponseHandler",server_response_mess)

local server_dc_mess = GASS.MessagePtr(GASS.ServerDisconnectedMessage("reg", 0))
ssm:get():RegisterForSystemMessage("serverDisconnectedHandler",server_dc_mess)


--local scene_loaded_mess = GASS.MessagePtr(GASS.ScenarioSceneLoadedNotifyMessage("reg", 0))
--ssm:get():RegisterForSystemMessage("sceneLoadedHandler",scene_loaded_mess)








