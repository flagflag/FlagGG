require 'LAS.init'

log.info('finish loading LAS.')

las.udp.opend = function()
    log.info('TCP ==> opend.')
end

las.udp.closed = function()
    log.info('TCP ==> closed.')
end

las.udp.error = function()
    log.info('TCP ==> error.')
end

las.udp.message = function()
    log.info('TCP ==> message.')
end

local result = las.run(function()
    las.udp.init()
    local result = las.await(las.udp.connect)('127.0.0.1', 5000)
    if result then
        log.info('connect result: true')
        gameplay.start_game()
    else
        log.info('connect result: false')
    end
end)
if not result then
    log.info('las.run ==> failed.')
end
