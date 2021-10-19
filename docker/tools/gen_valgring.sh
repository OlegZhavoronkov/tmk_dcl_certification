#!/bin/bash
echo -e "#!/bin/bash\n(valgrind --tool=memcheck --leak-check=yes bin/tmk-first localhost 9999 2>&1) | tee first.log" > vfirst.sh
echo -e "#!/bin/bash\n(valgrind --tool=memcheck --leak-check=yes bin/tmk-middle 2>&1) | tee middle.log" > vmiddle.sh
chmod +x *.sh
