--author Jiang Hang
--title A simple example
--date 2015.06.25
#--bgcolor gray   
This is the abstract of this presentation.
It may consist of zero or more lines, and may be as long as you want.
--newpage agenda
--heading Agenda

  * Introduction

  * Concept

  * Implementation

  * Comparison with other implementations

  * Conclusions
--newpage intro
--heading Introduction

This is the introduction. And below, that's source code.

--beginoutput
#include <stdio.h>

int main(void) {
  puts("Hello World!");
  return 0;
}
--endoutput

--beginoutput
function main()
{
    clear
    echo "=============================\n"
    echo "c.创建私密仓库。                              create\n"
    echo "p.推送本地私密仓库到云端。                    push\n"
    echo "g.获取云端私密仓库。                          get\n"
    echo "d.解密当前目录下的私密仓库。                  decode\n"
    echo "e.加密当前目录下的私密仓库。                  encrypt\n"
    echo "q.退出。                                      quit\n"
    echo "=============================\n"
    read -t 15 -n 1 "请输入您的选择：" choose
    if [ "$choose"x = "c"x || "$choose"x = "C"x ]; then
        create()
    elif [ "$choose"x = "p"x || "$choose"x = "P"x ]; then
        push()
    elif [ "$choose"x = "g"x || "$choose"x = "G"x ]; then
        get()
    elif [ "$choose"x = "d"x || "$choose"x = "D"x ]; then
        decode()
    elif [ "$choose"x = "e"x || "$choose"x = "E"x ]; then
        encrypt()
    else
        exit 0
    fi
}
--endoutput
