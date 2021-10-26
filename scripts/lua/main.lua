--this is not working right now
-- local opened = {false}
-- if imgui.CollapsingHeader('Test from lua', opened) then
--      imgui.Text('Hello from lua') -- some function!
-- end


-- imgui.Begin('Test Window',nil)
--     imgui.Text('Hello from lua') -- some function!
-- imgui.End()
require('formulae')
require('utils')

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
        a,b,c = PZxIsEven(j,x)
        print("P(K_x = P) ".. a)
        print("P(K_x = P | S[1]=0) ".. b)
        print("P(K_x = P | S[1]!=0) ".. c)
    end

    x = imgui.InputInt("x",x,1,100,0)
    --imgui.SameLine(0,-1)
    j = imgui.InputInt("j",j,1,100,0)



    if imgui.Button('Copy as latex table: PRGA P(K_x \\in P) at firsts ' .. ZxAmount .. ' bytes in experiment ' .. j) then
        local res = {}
        res[1] = {}
        res[1][1] = "$i$"
        res[1][2], res[1][3], res[1][4] = "$P(Z_i \\in P)$", "$P(Z_i \\in P| S_N[1]=0)$", "$P(Z_i \\in P| S_N[1] \\neq 0)$"
        for i=1,ZxAmount do
            res[i+1] = {}
            res[i+1][1] = i
            res[i+1][2], res[i+1][3], res[i+1][4] = PZxIsEven(j,i)
        end

        imgui.SetClipboardText(latexTable(res, "$P(Z_i \\in P)$ para llaves $\\{ 1,0,I,P,I,... \\}$, de tamaño par.", "PZiIsEven", "\\hline",{2}, .5))
        logger.info("Exported to clipboard latex table of: " .. data[j].id )
    end

    if imgui.Button('Copy as latex table: KSA P(S_N[u]=0) P(S_N[u]=1) for 0=<u<20 in experiment ' .. j) then
        local res = {}
        res[1] = {}
        res[1][1] = "$i$"
        res[1][2], res[1][3], res[1][4], res[1][5] = "$P(S_N[u]=0)$", "$P(S_N[u]=0)$ in practice", "$P(S_N[u]=1)$","$P(S_N[u]=1)$ in practice"
        for i=1,20 do
            res[i+1] = {}
            res[i+1][1] = i-1
            res[i+1][2] = KSAprobSXeq0(i-1)
            res[i+1][3] = data[j]:getOccurrenceProbability(i-1,0)  
            res[i+1][4] = KSAprobSXeq1(i-1)
            res[i+1][5] = data[j]:getOccurrenceProbability(i-1,1)
        end

        imgui.SetClipboardText(latexTable(res, "$P(S_N[u]=0), P(S_N[u]=1)$ te\\'oricas y pr\\'acticas.", "tab:KSAPSNuEq1o0", "\\hline",{2}, .5))
        logger.info("Exported to clipboard latex table  KSA P(S_N[u]=0) P(S_N[u]=1) for 0=<u<20 of experiment: " .. data[j].id )
    end

    if imgui.Button('Copy as latex bar chart: PRGA P(K_x \\in P) at firsts ' .. ZxAmount .. ' bytes in experiment ' .. j) then
        local res = {}
        res[1] = {}
        res[2] = {}
        res[3] = {}
        res[1][1], res[2][1], res[3][1] = "$P(Z_i \\in P)$", "$P(Z_i \\in P| S_N[1]=0)$", "$P(Z_i \\in P| S_N[1] \\neq 0)$"
        for i=1,ZxAmount do
            res[1][i+1], res[2][i+1], res[3][i+1] = PZxIsEven(j,i)
        end

        --imgui.SetClipboardText(latexBarChart(res,"$P(Z_i \\in P)$","barChartPZiEqE",2,13,20,0, 1,1, ZxAmount, 0.5, true))--horizontal
        imgui.SetClipboardText(latexBarChart(res,"$P(Z_i \\in P)$","barChartPZiEqE",2,15,5,1, ZxAmount,0, 1, 0.5, false))--vertical
        logger.info("Exported to clipboard latex bar chart of: " .. data[j].id )
    end


    ZxAmount = imgui.InputInt("ZxAmount",ZxAmount,1,100,0)

    if imgui.Button('Test multi sink logger') then
        logger.info("Yeahhh!!")
        logger.warn("Really?")
        logger.error("Fuuuuck!!!")
    end

    if imgui.Button('Test jArray') then
       print(jArrays[4]:getValue(50))
    end

    if imgui.Button('Copy as latex bar chart: PRGA P(S_i[u] = v) in experiment ' .. j) then
        local res = {}
        res[1] = {}
        res[2] = {}
        res[3] = {}
        res[1][1], res[2][1], res[3][1] = "$P(u = 1)$", "$u=15$", "$u=231$"
        for i=1,256 do
            res[1][i+1] = data[j]:getOccurrenceProbability(1,i-1)
            res[2][i+1] = data[j]:getOccurrenceProbability(15,i-1)
            res[3][i+1] = data[j]:getOccurrenceProbability(231,i-1)
        end

        --imgui.SetClipboardText(latexBarChart(res,"$P(Z_i \\in P)$","barChartPZiEqE",2,13,20,0, 1,1, ZxAmount, 0.5, true))--horizontal
        imgui.SetClipboardText(latexBarChart(res,"$P(Z_i \\in P)$","barChartPZiEqE",2,15,5,1, ZxAmount,0, 1, 0.5, false))--vertical
        logger.info("Exported to clipboard latex bar chart of: " .. data[j].id )
    end

end


if imgui.BeginMenuBar() then
    if imgui.BeginMenu('Experiments Probabilities', false) then
        --imgui.MenuItem('asd', nil, data[1].isActive)--this is crashing cause trying to modify data[1].isActive, neet to madify it as a return like using tuples in cpp wrapper
        imgui.EndMenu()
    end
    imgui.EndMenuBar()
end