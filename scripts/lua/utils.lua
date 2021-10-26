
-- Returns a n x m array initialized to v
function new2Darray2v(n,m,v)
    grid = {}
    for i = 1, n do
        grid[i] = {}

        for j = 1, m do
            grid[i][j] = v -- Fill the values here
        end
    end
    return grid
end


function isNumeric(x)
    if tonumber(x) ~= nil then
        return true
    end
    return false
end



function isInt(n)
  return n==math.floor(n)
end



-- Returns full latex code to render a table, table columns in latex are equal to amount of elements in first index of array(#array[1])
-- so each element in array is a row in latex 
function latexTable(array, caption, label, inject,inject_on, floatPrecision)
    --[[
    array: 2d array of data
    caption: caption at bottom of table
    label: latex label of table
    inject: string between tabular lines, "\\line" for example
    inject_on: list of lines to inject string at the end
        - Bound is [2, rows - 1], nil adds inject string to all lines
        - Out of bound line numbers are ignored
        - The list is sorted automatically
    floatPrecision: precision to format float numbers to, ie: 0.8(8 decimal digits)
    ]]
    local res = [[
    \begin{table}[htbp]
        \begin{center}      
            \begin{tabular}
            ]] 

    for i=1,#array[1] do
        if i == 1 then
            res = res .. "{r"
        elseif i ~= #array[1] then
            res = res .. "c"
        else
            res = res .. "c}"
        end
    end
    res = res .. "\\hline"
    res = res .. tableToTeX(array,inject ,inject_on, floatPrecision)
    res = res .. string.format([[
                \\ \hline
                \end{tabular}
                \caption{%s}
                \label{%s}
            \end{center}
        \end{table}
    ]],caption,label)

    return res
end



--from "https://gist.github.com/calebreister/8dd7ab503c91dea4dd2c499b9d004231" Edited
function tableToTeX(array, inject, inject_on, floatPrecision)
    --[[
    array: the 2D array of data
    inject: string between tabular lines
    inject_on: list of lines to inject string at the end
            - Bound is [2, rows - 1], nil adds inject string to all lines
            - Out of bound line numbers are ignored
            - The list is sorted automatically
    For some reason, LuaLaTeX does not like it when I output newlines with
    \hlines. The output of this function is a continuous string.
    floatPrecision: precision to format float numbers to, ie: 0.8(8 decimal digits)
    ]]
    
    --Initial conditions
    local result = ""
    local line = 1 --keeps track of add_to index, not used if inject_on is nil
    if inject_on ~= nil then
        table.sort(inject_on)
    end
    
    --Insert data
    for y=1, #array do
        if inject ~= nil and y ~= 1 then
            if inject_on == nil or inject_on[line] == y then
                result = result .. inject .. ' '
                line = line + 1
            end
        end
        for x=1, #array[y] do
            if (isNumeric(array[y][x]) and not isInt(array[y][x]) ) then
                local a = "%"..floatPrecision.."f"
                result = result .. string.format(a,array[y][x])
            else
                result = result .. array[y][x]
            end

            if x < #array[y] then
                result = result .. " & "
            end
        end
        if y < #array then
            result = result .. " \\\\ "
        end
    end
    
    return result
end



-- Returns full latex code to render a bar chart, the array is array[y][x]
-- so there is a plot for each y, of x elements
function latexBarChart(array,caption,label,barWidth,width, height, xmin, xmax, ymin, ymax, floatPrecision, horizontalBars)
    --[[
    array: 2d array of data
    caption: caption at bottom of table
    label: latex label of table
    inject: string between tabular lines, "\\line" for example
    inject_on: list of lines to inject string at the end
        - Bound is [2, rows - 1], nil adds inject string to all lines
        - Out of bound line numbers are ignored
        - The list is sorted automatically
    floatPrecision: precision to format float numbers to, ie: 0.8(8 decimal digits)
    ]]
    local typeOfBar
    if horizontalBars then
        typeOfBar = "xbar"
    else
        typeOfBar = "ybar"
    end

    local res = string.format([[
\begin{figure}[htbp]
\centering
\begin{tikzpicture}
 
\begin{axis} [%s = .05cm,
    bar width = %fpt,
    width=%fcm,
    height=%fcm,
    xmin = %f, 
    xmax = %f,
    ymin = %f, 
    ymax = %f,  
    enlarge y limits = {abs = .8},
    enlarge x limits = {value = .25, upper},
]
]],typeOfBar, barWidth,width, height, xmin, xmax, ymin, ymax)

    --Insert data
    for y=1, #array do
        res = res .. "\\addplot coordinates {"
        for x=2, #array[y] do
            if (isNumeric(array[y][x]) and not isInt(array[y][x]) ) then
                local a = string.format("%"..floatPrecision.."f", array[y][x])
                --res = res .. string.format("\\bcbar[text=%s]{%f}", array[1][x], a)
            end
            if horizontalBars then
                res = res .. string.format("(%f,%f)", array[y][x], x-1 )
            else
                res = res .. string.format("(%f,%f)", x-1, array[y][x] )
            end

            if x == #array[y] then
                --nothing for now
            end
        end
        res = res .. "};"
    end

    res = res .. "\\legend {"  
    for y=1,#array do
        res = res .. array[y][1]
        if y ~= #array then
            res = res .. ","
        end
    end
    res = res .. "}"

    res = res .. string.format([[\end{axis}
\end{tikzpicture}
\caption{%s} \label{fig:%s}
\end{figure}
]],caption,label)

    return res
end