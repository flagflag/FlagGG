
function las.await()

end

function las.coroutine(func)
    if type(func) ~= 'function' then
        return false
    end
    local co_func = coroutine.wrap(func)
    
    return true
end

