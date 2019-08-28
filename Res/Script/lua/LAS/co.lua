
function las.run(func, ...)
    local co_func = coroutine.wrap(func)
    xpcall(co_func, log.error, ...)
    return true
end

function las.await(func)
    if type(func) ~= 'function' then
        return nil
    end
    local co, is_master = coroutine.running()
    if is_master then
        return func
    end
    local awake = function(...)
        local result, error = coroutine.resume(co, ...)
        if not result then
            local error_msg = 'error code:' .. error .. ' error msg:' .. debug.traceback(co)
            log.error(error_msg)
        end
    end
    local wrap_func = function(...)
        local args = {...}
        args[#args + 1] = awake
        func(table.unpack(args))
        return coroutine.yield()
    end
    return wrap_func
end
