-- if imgui.CollapsingHeader(Test from lua')
--    imgui.Text('asd') -- some function!
--end
imgui.Begin('Test Window',nil)
    imgui.Text('Hello from lua') -- some function!
imgui.End()