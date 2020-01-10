local event_map = {}

function game.on_update(time_step)
    local func_array = event_map['on_update']
    if func_array ~= nil then
        for i, v in ipairs(func_array) do
            v(time_step)
        end
    end
end

local function register(name, func)
    if event_map[name] == nil then
        event_map[name] = {}
    end
    local func_array = event_map[name]
    func_array[#func_array + 1] = func
end

local function unregister(name, func)
    if event_map[name] ~= nil then
        if func == nil then
            event_map[name] = nil
        else
            local func_array = event_map[name]
            for i, v in ipairs(func_array) do
                if v == func then
                    table.remove(func_array, v)
                    break
                end
            end
        end
    end
end

return {
    register = register,
    unregister = unregister
}
