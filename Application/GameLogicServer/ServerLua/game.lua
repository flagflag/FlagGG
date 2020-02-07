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

local main_hero = nil
local monsters = {}

local function start()
    print ('game start ==> create main hero.')
    main_hero = engine.unit:create()
    print ('set main hero attribute.')
    main_hero:set_name('TestUnit')
    main_hero:set_position(1.0, 0.0, 1.0)
    main_hero:set_rotation(0.0, 0.0, 0.0, 1.0)
    main_hero:set_scale(1.0, 1.0, 1.0)

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

return {
    start = start,
    stop = stop,
    pause = pause,
}
