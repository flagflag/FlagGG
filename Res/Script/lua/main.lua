require 'LAS.init'

log.info('finish loading LAS.')

las.tcp.opend = function()

end

las.tcp.closed = function()

end

las.tcp.error = function()

end

las.tcp.message = function()

end

las.run(function()
    las.tcp.init()
    las.wait(las.tcp.connect)('127.0.0.1', 5000)
    log.info('connect result: ' .. las.tcp.is_active())
end)



