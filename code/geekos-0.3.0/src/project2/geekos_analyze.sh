 #!/bin/sh
 calltree -np -b -dot ./src/*/*.c >geekos_project2.dot
 dot -T png size="6662,16210" geekos_project2.dot -o ./geekos_project2.png
