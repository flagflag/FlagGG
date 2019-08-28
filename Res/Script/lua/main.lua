require 'LAS.init'

log.info('finish loading LAS.')

las.tcp.opend = function()
    log.info('TCP ==> opend.')
end

las.tcp.closed = function()
    log.info('TCP ==> closed.')
end

las.tcp.error = function()
    log.info('TCP ==> error.')
end

las.tcp.message = function()
    log.info('TCP ==> message.')
end

local result = las.run(function()
    las.tcp.init()
    local result = las.await(las.tcp.connect)('127.0.0.1', 5000)
    log.info('connect result: ' .. result)
    log.info('connect state: ' .. las.tcp.is_active())
end)
if not result then
    log.info('las.run ==> failed.')
end

