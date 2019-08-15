
function las.run(func, ...)
    if type(func) ~= 'function' then
        return false
    end
    local co_func = coroutine.wrap(func)
    pcall(co_func, 0, ...)
    return true
end

function las.await(func)
    local co, is_master = coroutine.running()
    if is_master then
        
        return
    end
    local cb = function(...)
        func(..., function(...)
            local result, error = coroutine.resume(co, ...)
            if not result then
                local error_msg = 'error code:' .. error .. ' error msg:' .. debug.traceback(co)
                log.error('error_msg')
            end
        end)
        return coroutine.yield()
    end
    return cb
end
