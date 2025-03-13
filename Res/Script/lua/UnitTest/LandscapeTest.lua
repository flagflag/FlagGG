require 'Game.init'
local context = GetContext()
local BaseTest = require 'UnitTest.BaseTest'

local LandscapeTest = class('LandscapeTest')

function LandscapeTest:ctor()

end

function LandscapeTest:run()
    local scene = gameplay.get_scene()

    -- 创建地形
    local terrain = context.Terrain.new()
    terrain:create(64, { 64, 64, 64 }, 'Textures/PlaneHeightMap.png', 'Materials/TerrainLandscape.ljson')
	terrain:set_scale(1, 1, 1)
	terrain:set_position(0, 0, 0)
	terrain:set_name('Terrain')
    scene:add_child(terrain)

    local texture_brush_comp = context.TextureBrushComponent.new()
    game_builder.get_brush():attach_component(texture_brush_comp)

    local camera_node = scene:get_child('MainHero', true)
    camera_node:set_position(-64, 1100, 900)
    -- camera_node:set_rotation(0.948600411, 0, 0.316476375, 0)
    camera_node:set_rotation(0.944376230, 0, 0.328866959, 0)

    app.set_camera_move_speed(500.0)

    app.setup_web_ui()
    app.load_web_ui('file:///WebUI/GameBuilder.html')
    local web_view = app.get_web_view()
    self.brush_id = 1
    _G.register_js_function(web_view, 'AddBrushID', function(delta)
        self.brush_id = self.brush_id + delta
        local brush = game_builder.get_brush()
        local tex_comp = brush:get_component('TextureBrushComponent')
        tex_comp:set_brush_id(self.brush_id)
    end)
end

BaseTest.run_unit_test(LandscapeTest)
