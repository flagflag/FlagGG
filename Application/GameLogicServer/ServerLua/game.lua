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

local function start()
    print ('game start ==> create main hero.')
    main_hero = engine.unit:create()
    print ('set main hero attribute.')
    main_hero:set_name('TestUnit')
    main_hero:set_position(1.0, 1.0, 1.0)
    main_hero:set_rotation(1.0, 1.0, 1.0)
    main_hero:set_scale(1.0, 1.0, 1.0)
end

local function stop()

end

local function pause()

end

return {
    start = start,
    stop = stop,
    pause = pause,
}
