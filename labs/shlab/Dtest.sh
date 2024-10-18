#!/bin/bash

# 检查是否提供了输入参数
if [ $# -ne 1 ]; then
    echo "Usage: $0 <trace number>"
    exit 1
fi

# 获取输入的 trace 文件号
end_trace=$1

# 循环从 1 到 $end_trace
for num in $(seq 1 $end_trace); do
    # 如果 num 小于 10，则在前面补 0
    if [ $num -lt 10 ]; then
        i="0$num"
    else
        i="$num"
    fi
    echo "Testing trace${i}.txt..."

    # 执行 make rtest${i} 和 make test${i}，并去除第一行，然后进行比较
    diff \
    <(make rtest${i} | sed '1d' | sed 's/[0-9]\{4,\}/PID/g' | sed 's/[+-]//' | sed 's/ *$//' | sed '/^$/d') \
    <(make test${i} | sed '1d' | sed 's/[0-9]\{4,\}/PID/g' | sed 's/[+-]//' | sed 's/ *$//' | sed '/^$/d')


    # 检查 diff 的退出状态码
    if [ $? -eq 0 ]; then
        echo "trace${i}.txt test PASSED"
    else
        echo "trace${i}.txt test FAILED"
    fi
done
