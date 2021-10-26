--apply configurations
config.plotHeight = 350




--temporal initialization of needed variables in main.lua
--in future main.lua should include this and the main code will be inside an update function

--for button "Print PRGA P(K_x = P) at experiment j"
x = 1
j = 3

--for button 'Print PRGA P(K_x = P) at firsts ' .. ZxAmount .. ' bytes in experiment' .. j
ZxAmount = 20


-- returns P(Z_x = E),P(Z_x = E| S_N[1]=0),P(Z_x = E| S_N[1]!=0) for experiment j
function PZxIsEven(j,x)
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
	return p,pS1eq0, pS1neq0
end