#!/bin/bash

# 检查是否提供了输入参数
if [ $# -ne 1 ]; then
    echo "Usage: $0 <trace number>"
    exit 1
fi

# 获取输入的 trace 文件号
end_trace=$1

# 定义函数进行统一的 sed 处理，忽略路径和无关信息
clean_output() {
    sed '1d' | \
    sed 's/[0-9]\{4,\}/PID/g' | \
    sed 's/[+-]//' | \
    sed 's/ *$//' | \
    sed '/^$/d' | \
    sed 's:/[^ ]*/[^ ]*/[^ ]*/[^ ]*/::g' | \
    sed 's/\.\/tshref/\.\/tsh/g' | \
    sed 's/rtest/test/g' | \
    grep -v "cpuUsage.sh"
}

# 循环从 1 到 $end_trace
for num in $(seq 1 $end_trace); do
    # 如果 num 小于 10，则在前面补 0
    if [ $num -lt 10 ]; then
        i="0$num"
    else
        i="$num"
    fi
    echo "Testing trace${i}.txt..."

    # 获取 rtest 和 test 的输出
    actual_output=$(make rtest${i} 2>/dev/null | clean_output)
    expected_output=$(make test${i} 2>/dev/null | clean_output)

    # 如果输出为空，打印调试信息
    if [ -z "$actual_output" ] || [ -z "$expected_output" ]; then
        echo "Warning: actual_output or expected_output is empty for trace${i}.txt"
    fi

    # 使用 diff 比较输出，带有颜色显示
    diff --color=always <(echo "$expected_output") <(echo "$actual_output")

    # 检查 diff 的退出状态码
    if [ $? -eq 0 ]; then
        echo -e "\e[32mtrace${i}.txt test PASSED\e[0m"
    else
        echo -e "\e[31mtrace${i}.txt test FAILED\e[0m"
    fi
done
