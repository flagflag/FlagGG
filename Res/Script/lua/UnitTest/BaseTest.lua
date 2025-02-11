local context = GetContext()

local BaseTest = class('BaseTest')

function BaseTest.create_prefab(prefab_path, x, y, z)
    local prefab = context.LoadPrefab(prefab_path)
    prefab:set_position(x, y, z)
    prefab:set_scale(0.01, 0.01, 0.01)

    local scene = gameplay.get_scene()
    scene:add_child(prefab)
end

function BaseTest.run_unit_test(unit_test_class)
    local inst = unit_test_class.new()
    inst:run()
end

return BaseTest
