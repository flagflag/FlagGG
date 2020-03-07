local event = require 'event'

controler = {}

function controler.start_direction_move(user_id, x, y, z)
    event.call('start_direction_move', user_id, x, y, z)
end

function controler.stop_direction_move(user_id)
    event.call('stop_direction_move', user_id)
end
