--require 'LAS'

log.info('finish loading LAS.')

network.init(0, 
function()
    log.info('network opend.')
    network.send(0, '2333', 4)
end,
function()
    log.info('network closed.')
end,
function(error_code, error_message)
    log.info('network error[' .. error_code .. ',' .. error_message .. '].')
end,
function(buffer)
    log.info('network message[' .. buffer .. '].')
end)

network.connect(0, '127.0.0.1', 5000)


