require 'Game.init'
local context = GetContext()
local BaseTest = require 'UnitTest.BaseTest'

local HiZTest = class('HiZTest')

function HiZTest:ctor()

end

function HiZTest:run()
    local scene = gameplay.get_scene()

    -- 创建地形
    local terrain = context.Terrain.new()
    terrain:create(64)
	terrain:set_scale(1, 1, 0.2)
	terrain:set_position(0, -80, 0)
	terrain:set_name('Terrain')
    scene:add_child(terrain)

    -- 创建水体
    local water = context.ClientUnit.new()
    water:load('Unit/Water.ljson')
	water:set_position(500, 500 - 80, 1)
	water:set_rotation(-0.5, -0.5, -0.5, -0.5)
	water:set_scale(1000, 1000, 1000)
	water:set_name('Water')
    scene:add_child(water)

    -- 创建物体
    BaseTest.create_prefab('deco\\Building\\Arch_01\\model.prefab', 10, 0, 0) 

    -- 创建被遮挡的prefab
    BaseTest.create_prefab("deco\\Building\\Arch_01\\model.prefab", 50, 0, 5)
    BaseTest.create_prefab("deco\\Building\\Arch_01\\model.prefab", 60, 0, 5)
    BaseTest.create_prefab("deco\\Building\\Arch_01\\model.prefab", 70, 0, 5)
    
    for i = 1, 5 do
        BaseTest.create_prefab("deco\\Building\\Arch_01\\model.prefab", 70,  i * 10, 5)
        BaseTest.create_prefab("deco\\Building\\Arch_01\\model.prefab", 70, -i * 10, 5)
    end
end

BaseTest.run_unit_test(HiZTest)
