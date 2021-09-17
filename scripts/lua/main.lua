

--this is not working right now
-- local opened = {false}
-- if imgui.CollapsingHeader('Test from lua', opened) then
--      imgui.Text('Hello from lua') -- some function!
-- end


-- imgui.Begin('Test Window',nil)
--     imgui.Text('Hello from lua') -- some function!
-- imgui.End()


if imgui.CollapsingHeaderSimple('Test from lua') then
    imgui.Text('Hello from lua') -- some function!
end