require 'Game.init'
local context = GetContext()

local function create_prefab(prefab_path, x, y, z)
    local prefab = context.LoadPrefab(prefab_path)
    prefab:set_position(x, y, z)
    prefab:set_scale(0.01, 0.01, 0.01)

    local scene = gameplay.get_scene()
    scene:add_child(prefab)
end

create_prefab("deco\\Building\\Arch_01\\model.prefab", 50, 0, 5)
create_prefab("deco\\Building\\Arch_01\\model.prefab", 60, 0, 5)
create_prefab("deco\\Building\\Arch_01\\model.prefab", 70, 0, 5)

for i = 1, 5 do
    create_prefab("deco\\Building\\Arch_01\\model.prefab", 70,  i * 10, 5)
    create_prefab("deco\\Building\\Arch_01\\model.prefab", 70, -i * 10, 5)
end

