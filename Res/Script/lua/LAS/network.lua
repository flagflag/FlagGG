
local NETWORK_TYPE = {
    TCP = 0,
    UDP = 1,
    WEB = 2,
}

local function create_network(net_type)
    local object = {}
    object.init = function()
        network.init(net_type,
            function()
                if object.opend then
                    object.opend()
                end
                if object.co_opend then
                    object.co_opend(true)
                    object.co_opend = nil
                end
            end,
            function()
                if object.closed then
                    object.closed()
                end
            end,
            function(error_code, error_message)
                if object.error then
                    object.error()
                end
                if object.co_opend then
                    object.co_opend(false)
                    object.co_opend = nil
                end
            end,
            function()
                if object.message then
                    object.message()
                end
            end
        )
    end
    
    object.connect = function(ip, port, callback)
        if object.co_opend then
            log.info('is connecting...')
            return false
        end
        object.co_opend = callback
        network.connect(net_type, ip, port)
        return true
    end
    
    object.disconnect = function()
        network.disconnect(net_type)
    end
    
    object.send = function(buffer)
        network.send(net_type, buffer, len(buffer))
    end
    
    object.is_active = function()
        return network.is_active(net_type)
    end

    return object
end

las.tcp = create_network(NETWORK_TYPE.TCP)
las.udp = create_network(NETWORK_TYPE.UDP)
las.web = create_network(NETWORK_TYPE.WEB)
