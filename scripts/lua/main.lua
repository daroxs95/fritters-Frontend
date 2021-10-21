--this is not working right now
-- local opened = {false}
-- if imgui.CollapsingHeader('Test from lua', opened) then
--      imgui.Text('Hello from lua') -- some function!
-- end


-- imgui.Begin('Test Window',nil)
--     imgui.Text('Hello from lua') -- some function!
-- imgui.End()
require('formulae')

if imgui.CollapsingHeaderSimple('Lua Script',0) then
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

    if imgui.Button('Print P(S[x]=1)') then
        --print( KSAprobSXeq1(12) )
    end

    if imgui.Button('Print P(S[S[1]]=0)') then
        print(KSAprobSS01eq10())
    end

    if imgui.Button('Print PRGA P(K_x = P) at experiment j') then
        print(data[j].id)
        local p = 0
        local pS1eq0 = 0
        local pS1neq0 = 0
        for i=1,256 do
            if i%2 ~= 0 then
                p = p + data[j].PRGAoutputsProbabilities[x]:getOccurrenceProbability(i-1)
                pS1eq0 = pS1eq0 + data[j].PRGAoutputsProbabilitiesS1eq0[x]:getOccurrenceProbability(i-1)
                pS1neq0 = pS1neq0 + data[j].PRGAoutputsProbabilitiesS1neq0[x]:getOccurrenceProbability(i-1)
            end
        end
        print("P(K_x = P) ".. p)
        print("P(K_x = P | S[1]=0) ".. pS1eq0)
        print("P(K_x = P | S[1]!=0) ".. pS1neq0)
    end

    imgui.SameLine(0,-1)
    

    x = imgui.InputInt("x",x,1,100,0)
    j = imgui.InputInt("j",j,1,100,0)

end

if imgui.BeginMenuBar() then
    if imgui.BeginMenu('Experiments Probabilities', false) then
        --imgui.MenuItem('asd', nil, data[1].isActive)--this is crashing cause trying to modify data[1].isActive, neet to madify it as a return like using tuples in cpp wrapper
        imgui.EndMenu()
    end
    imgui.EndMenuBar()
end