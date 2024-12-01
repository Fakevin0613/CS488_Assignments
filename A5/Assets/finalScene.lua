mat_green = gr.material({0.0, 0.5, 0.0}, {0.3, 0.3, 0.3}, 10)  -- Brighter green
mat_brown = gr.material({0.6, 0.3, 0.1}, {0.4, 0.4, 0.4}, 25) -- Wooden cushions
mat_black = gr.material({0.1, 0.1, 0.1}, {0.1, 0.1, 0.1}, 5)  -- Pockets

local table_length = 9  -- Reduced from 12
local table_width = 4.5   -- Reduced from 6
local cushion_height = 0.3  -- Height of cushions
local cushion_width = 0.5   -- Width of cushions
local cushion_offset = 0.3  -- Increased gap for pockets

scene_root = gr.node('root')

-- Create a table group to handle rotation
table_group = gr.node('table_group')
scene_root:add_child(table_group)
table_group:rotate('X', -60)

-- Main table surface
table = gr.plane('table', {0, 0, 0}, {0, 0, 1}, table_length, table_width)
table_group:add_child(table)
table:set_material(mat_green)

-- Pockets
local pocket_radius = 0.25  -- Slightly larger pockets
local pocket_depth = 0.1   -- Slight depth below table surface
local pocket_offset = 0.1   -- Slight gap between pockets

-- Middle pockets
local middle_positions = {
    {-table_width/2 - cushion_width/2 + pocket_offset, 0, pocket_depth},  -- Left middle
    {table_width/2 + cushion_width/2 - pocket_offset, 0, pocket_depth},   -- Right middle
}

for i, pos in ipairs(middle_positions) do
    local pocket = gr.nh_sphere('pocket_middle_' .. i, {pos[1], pos[2], pos[3]}, pocket_radius)
    table_group:add_child(pocket)
    pocket:set_material(mat_black)
end

-- Corner pockets
local corner_positions = {
    {-table_width/2 - cushion_width/2 + pocket_offset, -table_length/2 + cushion_offset + 2 * pocket_offset, pocket_depth},  -- Bottom left
    {table_width/2 + cushion_width/2 - pocket_offset, -table_length/2 + cushion_offset + 2 * pocket_offset, pocket_depth},   -- Bottom right
    {-table_width/2 - cushion_width/2 + pocket_offset, table_length/2 + cushion_offset - 2 * pocket_offset, pocket_depth},   -- Top left
    {table_width/2 + cushion_width/2 - pocket_offset, table_length/2 + cushion_offset - 2 * pocket_offset, pocket_depth},    -- Top right
}

for i, pos in ipairs(corner_positions) do
    local pocket = gr.nh_sphere('pocket_corner_' .. i, {pos[1], pos[2], pos[3]}, pocket_radius)
    table_group:add_child(pocket)
    pocket:set_material(mat_black)
end

-- Cushions (wooden rails)
-- Long sides
cushion_left_top = gr.cube('cushion_left_top')
table_group:add_child(cushion_left_top)
cushion_left_top:scale(cushion_width, table_length / 2, cushion_height)
cushion_left_top:translate(-table_width / 2 - cushion_width, 0, 0)
cushion_left_top:set_material(mat_brown)

cushion_left_bottom = gr.cube('cushion_left_bottom')
table_group:add_child(cushion_left_bottom)
cushion_left_bottom:scale(cushion_width, -table_length / 2, cushion_height)
cushion_left_bottom:translate(-table_width / 2 - cushion_width, 0, 0)
cushion_left_bottom:set_material(mat_brown)

cushion_right_top = gr.cube('cushion_right_top')
table_group:add_child(cushion_right_top)
cushion_right_top:scale(cushion_width, table_length / 2, cushion_height)
cushion_right_top:translate(table_width / 2, 0, 0)
cushion_right_top:set_material(mat_brown)

cushion_right_bottom = gr.cube('cushion_right_bottom')
table_group:add_child(cushion_right_bottom)
cushion_right_bottom:scale(cushion_width, -table_length / 2, cushion_height)
cushion_right_bottom:translate(table_width / 2, 0, 0)
cushion_right_bottom:set_material(mat_brown)

-- Short sides
cushion_top = gr.cube('cushion_top')
table_group:add_child(cushion_top)
cushion_top:scale(table_width + cushion_width * 2, cushion_width, cushion_height)
cushion_top:translate(-table_width / 2 - cushion_width, table_length / 2, 0)
cushion_top:set_material(mat_brown)

cushion_bottom = gr.cube('cushion_bottom')
table_group:add_child(cushion_bottom)
cushion_bottom:scale(table_width + cushion_width * 2, cushion_width, cushion_height)
cushion_bottom:translate(-table_width / 2 - cushion_width, -table_length / 2, 0)
cushion_bottom:set_material(mat_brown)


-- Ball materials
mat_white = gr.material({1.0, 1.0, 1.0}, {0.8, 0.8, 0.8}, 50)    -- Cue ball
mat_red = gr.material({0.9, 0.1, 0.1}, {0.8, 0.8, 0.8}, 50)      -- Red balls
mat_pink = gr.material({0.9, 0.4, 0.6}, {0.8, 0.8, 0.8}, 50)     -- Pink ball
mat_black = gr.material({0.1, 0.1, 0.1}, {0.8, 0.8, 0.8}, 50)    -- Black ball
mat_blue = gr.material({0.1, 0.1, 0.9}, {0.8, 0.8, 0.8}, 50)     -- Blue ball
mat_brown = gr.material({0.6, 0.3, 0.1}, {0.8, 0.8, 0.8}, 50)    -- Brown ball
mat_green = gr.material({0.1, 0.7, 0.1}, {0.8, 0.8, 0.8}, 50)    -- Green ball
mat_yellow = gr.material({0.9, 0.9, 0.1}, {0.8, 0.8, 0.8}, 50)   -- Yellow ball

-- Ball properties
local ball_radius = 0.15
local ball_spacing = ball_radius * 2.2  -- Slight gap between balls

-- Create balls group
balls_group = gr.node('balls_group')
table_group:add_child(balls_group)

-- Function to create a ball
function create_ball(name, position, material)
    local ball = gr.nh_sphere(name, position, ball_radius)
    balls_group:add_child(ball)
    ball:set_material(material)
    return ball
end

-- Add cue ball (white)
create_ball('cue_ball', {0, -table_length/4, 0.1}, mat_white)

-- Add colored balls in triangle formation
-- Pink ball (front of triangle)
create_ball('pink_ball', {0, table_length/4, 0.1}, mat_pink)

-- Add red balls in triangle formation
local start_x = 0
local start_y = table_length/4 + ball_spacing
local rows = 5

for row = 0, rows-1 do
    for col = 0, row do
        local x = start_x + (col - row/2) * ball_spacing
        local y = start_y + row * ball_spacing * 0.866 -- sqrt(3)/2 for equilateral triangle
        create_ball('red_ball_' .. row .. '_' .. col, {x, y, 0.1}, mat_red)
    end
end

-- Add colored balls in their specific positions
create_ball('black_ball', {0, table_length/2 - ball_spacing, 0.1}, mat_black)
create_ball('blue_ball', {0, 0, 0.1}, mat_blue)
create_ball('brown_ball', {0, -table_length/4 + ball_spacing, 0.1}, mat_brown)
create_ball('green_ball', {-table_width/4, -table_length/4 + ball_spacing, 0.1}, mat_green)
create_ball('yellow_ball', {table_width/4, -table_length/4 + ball_spacing, 0.1}, mat_yellow)


local foot_radius = 0.2  -- Thinner feet
local foot_height = 2   -- Shorter feet
local foot_positions = {
    {-table_width/2 - cushion_width/2, -table_length/2 + cushion_offset},  -- Bottom left
    {table_width/2 + cushion_width/2, -table_length/2 + cushion_offset},   -- Bottom right
    {-table_width/2 - cushion_width/2, table_length/2 + cushion_offset},   -- Top left
    {table_width/2 + cushion_width/2, table_length/2 + cushion_offset}     -- Top right
}

for i, pos in ipairs(foot_positions) do
    local foot = gr.cylinder('table_foot_' .. i, {pos[1], pos[2], -foot_height}, {0, 0, 1}, foot_radius, foot_height)
    table_group:add_child(foot)
    foot:set_material(mat_brown)
end

-- Adjusted Lights
white_light = gr.light({-10.0, 10.0, 10.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

-- Render with adjusted camera
gr.render(scene_root, 'snooker-table.png', 1024, 1024,
          {0, 0, 10}, {0, 0, -10}, {0, 1, 0}, 50,
          {0.3, 0.3, 0.3}, {white_light})