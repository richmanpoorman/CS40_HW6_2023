 #! /bin/sh
shopt -s nullglob
TESTS=($(ls *.um | sed -e 's/\.um$//'))

make

./makeTests

for test in "${TESTS[@]}"; do
        echo TEST: $test
        
        inputFile="/dev/null"
        if [ -f $test.0 ]; then
                echo FOUND $test.0
                $inputFile=$test.0
        fi 
        touch $test.1
        um $test.um < $inputFile > $test.1
        
        touch $test.testOut1
        ./um $test.um < $inputFile > $test.testOut1
        DIFF=$(diff $test.testOut1 $test.1)
        if [ "$DIFF" != "" ]; then 
                echo "    TEST FAILED: "
                echo $DIFF
        else 
                echo "    TEST PASSED:"
                cat -v $test.testOut1
                echo
        fi
        rm $test.testOut1
        echo
done 