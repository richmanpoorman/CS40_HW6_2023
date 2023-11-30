 #! /bin/sh

# Midmark Testing
echo "------------"
echo "Midmark Test"
echo "------------"
echo "Timing:"
/usr/bin/time ./um Tests/midmark.um > /dev/null


# Advent Testing
echo "-----------"
echo "Advent Test"
echo "-----------"
echo "Timing:"
/usr/bin/time ./um Tests/advent.umz < Tests/adventPartialSolution.txt \
        > /dev/null

# Sandmark Testing
echo "-------------"
echo "Sandmark Test"
echo "-------------"
echo "Timing:"
/usr/bin/time ./um Tests/sandmark.umz > /dev/null



rm callgrind.*

# Instruction count
echo "Midmark Instruction Count:"
valgrind --tool=callgrind --dump-instr=yes ./um Tests/midmark.um > /dev/null

# qcachegrind callgrind.*