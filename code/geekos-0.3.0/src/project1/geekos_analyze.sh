 #!/bin/sh
 calltree -np -b -dot ./src/*/*.c >geekos.dot
 dot -T png geekos.dot -o ./geekos.png
