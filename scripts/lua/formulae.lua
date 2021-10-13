--Probability of S to hold value 1 at an even index after KSA
function KSAprobEeq1()
    local result=0
    for i=1,256 do
        if i%2 ~= 0 then
            result = result + KSAprobXeq1(i-1)
        end
    end 
    result = (2/256)*result
    return result
end

--Probability of S to hold an even number at an even index after KSA
function KSAprobEeqE()
    return 1-KSAprobEeq1()
end

--Probability of S to hold value 0 at an odd index after KSA
function KSAprobOeq0()
    local result=0
    for i=1,256 do
        if i%2 == 0 then
            result = result + KSAprobXeq0(i-1)
        end
    end 
    result = (2/256)*result
    return result
end

--Probability of S to hold an odd number at an odd index after KSA
function KSAprobOeqO()
    return 1-KSAprobOeq0()
end


--Probability of S[x] = 1 after KSA
function KSAprobXeq1(x)
    if x == 0 then
        return (1-1/128)^((256-2)/2)
    elseif x%2 == 0 then
        return 1/128*(1-1/128)^((256-x)/2)
    else
        return 0
    end
end


--Probability of S[x] = 0 after KSA
function KSAprobXeq0(x)
    if x == 1 then
        return (1-1/128)^((256-2)/2)
    elseif x%2 == 0 then
        return 0
    else
        return 1/128*(1-1/128)^((256-x-1)/2)
    end
end




--Probability of S[S[0]]=1 after KSA
function KSAprobS0eq1()
    return ( 1 - KSAprobXeq1(0) ) * KSAprobEeq1()
end


--Probability of S[S[0]]=0 after KSA
function KSAprobS0eq0()
    return KSAprobXeq1(0) * KSAprobXeq0(1)
end

--Probability of S[S[1]]=1 after KSA
function KSAprobS1eq1()
    return KSAprobXeq0(1) * KSAprobXeq1(0)
end

--Probability of S[S[1]]=0 after KSA
function KSAprobS1eq0()
    return (1 - KSAprobXeq0(1) ) * KSAprobOeq0()
end