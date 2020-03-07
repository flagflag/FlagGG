game = {}

local event = require 'event'

function game.on_update(time_step)
    event.call('on_update', time_step)
end

function game.on_start()
    event.call('on_start')
end

function game.on_stop()
    event.call('on_stop')
end

function game.on_pause()
    event.call('on_pause')
end

function game.on_player_into(user_id)
    event.call('on_player_into', user_id)
end

function game.on_player_leave(user_id)
    event.call('on_player_leave', user_id)
end

local main_hero = nil
local monsters = {}
local players = {}

local function start()
    print ('game start ==> create main hero.')
    main_hero = engine.unit:create()
    print ('set main hero attribute.')
    main_hero:set_name('TestUnit')
    main_hero:set_position(1.0, 0.0, 1.0)
    main_hero:set_rotation(0.0, 0.0, 0.0, 1.0)
    main_hero:set_scale(1.0, 1.0, 1.0)
    
    local player = players[0]
    if player then
        player:set_control_unit(main_hero)
    end

    -- 随机生成十个小怪
    for i = 0, 10 do
        local monster  = engine.unit:create()
        print ('create monster.')
        monster:set_name('monster')
        monster:set_position(i, 0.0, 2.0)
        monster:set_rotation(0.0, 0.0, 0.0, 1.0)
        monster:set_scale(i + 1, i + 1, i + 1)
        monsters[#monsters + 1] = monster
    end
end

local function stop()
    print ('game stop.')
    main_hero = nil
    monsters = {}
    collectgarbage('collect')
end

local function pause()

end

local add_player(user_id)
    local player = engine.player:create(user_id)
    players[user_id] = player
    return player
end

local remove_player(user_id)
    players[user_id] = nil
end

local get_player(usr_id)
    return players[user_id]
end

return {
    start = start,
    stop = stop,
    pause = pause,
    add_player = add_player,
    remove_player = remove_player,
    get_player = get_player,
}
