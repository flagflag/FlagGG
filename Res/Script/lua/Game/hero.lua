local context = GetContext()

local Hero = class('Hero', context.Unit)

function Hero:ctor()
    self.super.ctor()
end

function Hero:move_to(x, y)

end

return Hero
