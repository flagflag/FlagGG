game = {}

local event = require 'event'

event.register('on_update', function(time_step)
    
end)

local main_hero = nil

local function start()
    main_hero = engine.unit:create()
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
