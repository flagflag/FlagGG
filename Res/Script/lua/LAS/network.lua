
local NETWORK_TYPE = {
    TCP = 0,
    UDP = 1,
    WEB = 2,
}

las.tcp = {
    init = function()
        network.init(NETWORK_TYPE.TCP,
            function()
                if las.tcp.opend then
                    las.tcp.opend()
                    las.tcp.co_opend()
                end
            end,
            function()
                if las.tcp.closed then
                    las.tcp.closed()
                end
            end,
            function(error_code, error_message)
                if las.tcp.error then
                    las.tcp.error()
                end
            end,
            function()
                if las.tcp.message then
                    las.tcp.message()
                end
            end
        )
    end,

    connect = function(ip, port, callback)
        las.tcp.co_opend = callback
        network.connect(NETWORK_TYPE.TCP, ip, port)
    end,

    disconnect = function()
        network.disconnect(NETWORK_TYPE.TCP)
    end,

    send = function(buffer)
        network.send(NETWORK_TYPE.TCP, buffer, len(buffer))
    end

    is_active = function()
        return network.is_active(NETWORK_TYPE.TCP)
    end
}
