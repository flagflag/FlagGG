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
    -- map_builder:load_map('E:/GitProject/NE/Urho/vs_bgfx_editor/bin/Update/promotion2/scene/default/map.acmap')
    map_builder:load_map('E:/GitProject/NE/Res/maps/single_simple_ts_template_2/scene/default/map.acmap')

    local light_node = scene:get_child('MainLight', true)
    local light_comp = light_node:get_component('Light')
    light_comp:set_brightness(0.1)

    local probe_node = scene:get_child('MainProbe', true)
    local probe_comp = probe_node:get_component('Probe')
    probe_comp:set_diffuse_intensity(0.01)
    probe_comp:set_specular_intensity(0)

    local camera_node = scene:get_child('MainHero', true)
    -- camera_node:set_position(3389, 5420, 520)
    camera_node:set_position(1102, 2056, 1439)
    camera_node:set_rotation(0.948600411, 0, 0.316476375, 0)

    app.set_camera_move_speed(500.0)
end

BaseTest.run_unit_test(MapBuilderTest)
