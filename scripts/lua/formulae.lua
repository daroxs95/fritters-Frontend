--Constant functions

--Probability of S to hold value 1 at an even index after KSA
function KSAprobSEeq1()
    local result=0
    for i=1,256 do
        result = result + KSAprobSXeq1(i-1)
    end 
    result = (2/256)*result
    return result
end

--Probability of S to hold an even number at an even index after KSA
function KSAprobSEeqE()
    return 1-KSAprobSEeq1()
end

--Probability of S to hold value 0 at an odd index after KSA
function KSAprobSOeq0()
    local result=0
    for i=1,256 do
        result = result + KSAprobSXeq0(i-1)
    end 
    result = (2/256)*result
    return result
end

--Probability of S to hold an odd number at an odd index after KSA
function KSAprobSOeqO()
    return 1-KSAprobSOeq0()
end

--Probability of S[S[0]]=1 after KSA
function KSAprobS0eq1()
    return ( 1 - KSAprobSXeq1(0) ) * KSAprobSEeq1()
end


--Probability of S[S[0]]=0 after KSA
function KSAprobS0eq0()
    return KSAprobSXeq1(0) * KSAprobSXeq0(1)
end

--Probability of S[S[1]]=1 after KSA
function KSAprobS1eq1()
    return KSAprobSXeq0(1) * KSAprobSXeq1(0)
end

--Probability of S[S[1]]=0 after KSA
function KSAprobS1eq0()
    return (1 - KSAprobSXeq0(1) ) * KSAprobSOeq0()
end

--Probability of  P(S[S[1]]=0 and  P(S[S[0]]=1 after KSA
function KSAprobSS01eq10()
    return (1/128 * (126/128)^126 * 127)
end

--End of Constant functions



--Non constant functions

--Probability of S[x] = 1 after KSA
function KSAprobSXeq1(x)
    if x == 0 then
        return (1-1/128)^((256-2)/2)
    elseif x%2 == 0 then
        return 1/128*(1-1/128)^((256-x)/2)
    else
        return 0
    end
end


--Probability of S[x] = 0 after KSA
function KSAprobSXeq0(x)
    if x == 1 then
        return (1-1/128)^((256-2)/2)
    elseif x%2 == 0 then
        return 0
    else
        return 1/128*(1-1/128)^((256-x-1)/2)
    end
end

--End of non constant functions