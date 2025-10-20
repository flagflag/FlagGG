require 'Game.init'
local context = GetContext()
local BaseTest = require 'UnitTest.BaseTest'

local PhysicsTest = class('PhysicsTest')

function PhysicsTest:ctor()

end

function PhysicsTest:run()
    local scene = gameplay.get_scene()

    -- 创建平面
    local plane = context.ClientUnit.new()
    plane:load('Unit/WaterDown.ljson')
	plane:set_position(0, -5, 0)
	plane:set_rotation(-0.5, -0.5, -0.5, -0.5)
	plane:set_scale(100, 100, 100)
	plane:set_name("Plane")
    scene:add_child(plane)

    -- 创建物理Box
    BaseTest.create_prefab('engine/box/model.prefab', 10, 0, 0)

    -- 创建物理Sphere
    BaseTest.create_prefab('engine/shpere/model.prefab', 20, 0, 0)

    -- 创建物理Cylinder
    BaseTest.create_prefab('engine/cylinder/model.prefab', 30, 0, 0)
end
