local context = GetContext()
local game = require 'server.game'
local event = require 'server.event'

local MyController = class('MyController', context.Controller)

function MyController:ctor(player)
    self.player = player
end

function MyController:start_direction_move(x, y, z)
    print ('start_direction_move')
    local control_unit = self.player:get_control_unit()
    self.movement = context.DirectionMovement.new()
    self.movement:set_move_direction(x, y, z)
    self.movement:start()
    control_unit:add_movement(self.movement)
end

function MyController:stop_direction_move()
    print ('stop_direction_move')
    local control_unit = self.player:get_control_unit()
    if control_unit then
        control_unit:remove_movement(self.movement)
        self.movement = nil
    end
end


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

event.register('on_player_into', function(user_id)
    local player = game.add_player(user_id)
    local controller = MyController.new(player)
    player:set_controller(controller)
    player.controller = controller
end)

event.register('on_player_info', function(user_id)
    game.remove_player(user_id)
end)
