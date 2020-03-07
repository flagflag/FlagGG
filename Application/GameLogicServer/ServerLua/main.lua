
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
    game.update(time_step)
end)

event.register('on_player_into', function(user_id)
    game.add_player(user_id)
end)

event.register('on_player_info', function(user_id)
    game.remove_player(user_id)
end)

event.register('start_direction_move', function(user_id, x, y, z)
    local player = game.get_player(user_id)
    local control_unit = player:get_control_unit()
    local movement = engine.movement:create()
    control_unit:add_movement(movement)
end)

event.register('stop_direction_move', function(user_id)

end)
