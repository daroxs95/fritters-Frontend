--this is not working right now
-- local opened = {false}
-- if imgui.CollapsingHeader('Test from lua', opened) then
--      imgui.Text('Hello from lua') -- some function!
-- end


-- imgui.Begin('Test Window',nil)
--     imgui.Text('Hello from lua') -- some function!
-- imgui.End()

if imgui.CollapsingHeaderSimple('Lua Script') then
    imgui.Text('Hello from lua') -- some function!
    if imgui.TreeNode('Print Probabilities for {0,1}') then
        if imgui.Button(data[1].id) then
            print(data[1].id)
            print('P[0]=0', data[1]:getOccurrenceProbability(0,0))
            print('P[0]=1', data[1]:getOccurrenceProbability(0,1))
            print('P[1]=0', data[1]:getOccurrenceProbability(1,0))
            print('P[1]=1', data[1]:getOccurrenceProbability(1,1))
        end
        if imgui.Button(data[7].id) then
            print(data[7].id)
            print('P[0]=0', data[7]:getOccurrenceProbability(0,0))
            print('P[0]=1', data[7]:getOccurrenceProbability(0,1))
            print('P[1]=0', data[7]:getOccurrenceProbability(1,0))
            print('P[1]=1', data[7]:getOccurrenceProbability(1,1))
        end
        if imgui.Button(data[3].id) then
            print(data[3].id)
            print('P[0]=0', data[3]:getOccurrenceProbability(0,0))
            print('P[0]=1', data[3]:getOccurrenceProbability(0,1))
            print('P[1]=0', data[3]:getOccurrenceProbability(1,0))
            print('P[1]=1', data[3]:getOccurrenceProbability(1,1))
        end
        imgui.TreePop()
    end
    if imgui.Button('Toggle'..data[1].id) then
        data[1].isActive = false
    end
end

if imgui.BeginMenuBar() then
    if imgui.BeginMenu('Experiments Probabilities', false) then
        --imgui.MenuItem('asd', nil, data[1].isActive)--this is crashing cause trying to modify data[1].isActive, neet to madify it as a return like using tuples in cpp wrapper
        imgui.EndMenu()
    end
    imgui.EndMenuBar()
end