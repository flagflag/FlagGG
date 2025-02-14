require 'Game.init'
local context = GetContext()
local BaseTest = require 'UnitTest.BaseTest'

local WaterTest = class('WaterTest')

function WaterTest:ctor()

end

function WaterTest:run()
    local scene = gameplay.get_scene()

    -- 创建平面
    local plane = context.ClientUnit.new()
    plane:load('Unit/WaterDown.ljson')
	plane:set_position(0, -5, 0)
	plane:set_rotation(-0.5, -0.5, -0.5, -0.5)
	plane:set_scale(100, 100, 100)
	plane:set_name("Plane")
    scene:add_child(plane)

    -- 创建水体
    local water = context.ClientUnit.new()
    water:load('Unit/Water.ljson')
	water:set_position(0, -5, 0)
	water:set_rotation(-0.5, -0.5, -0.5, -0.5)
	water:set_scale(100, 100, 100)
	water:set_name('Water')
    scene:add_child(water)

    -- 创建物体
    BaseTest.create_prefab('deco\\Building\\Arch_01\\model.prefab', 10, 0, 0) 
end

BaseTest.run_unit_test(WaterTest)
