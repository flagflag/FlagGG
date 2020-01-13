
local game = require 'game'
local event = require 'event'

event.register('on_start', function()
    game.start()
end)

event.register('on_stop', function()
    game.stop()
end)

event.register('on_pause', function()
    game.pause()
end)

event.register('on_update', function(time_step)
    
end)
