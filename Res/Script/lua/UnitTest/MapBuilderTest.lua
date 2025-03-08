require 'Game.init'
local context = GetContext()
local BaseTest = require 'UnitTest.BaseTest'

local MapBuilderTest = class('MapBuilderTest')

function MapBuilderTest:ctor()

end

function MapBuilderTest:run()
    local scene = gameplay.get_scene()

    local map_builder = context.MapBuilder.new()
    map_builder:set_scene(scene)
    map_builder:load_map('E:/GitProject/NE/Urho/vs_bgfx_editor/bin/Update/promotion2/scene/default/map.acmap')

    local camera_node = scene:get_child('MainHero', true)
    camera_node:set_position(3389, 5420, 520)

    app.set_camera_move_speed(500.0)
end

BaseTest.run_unit_test(MapBuilderTest)
