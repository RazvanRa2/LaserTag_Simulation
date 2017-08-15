#!/bin/bash

# Checher tema 2 SD 2017 - LaserTag

NO_TESTS=10
EXEC=tema2
TEST_POINTS=(5 5 5 5 9 9 9 9 12 12)
TIMEOUT_TIME=(1 1 1 1 1 1 1 1 1 1)
VALGRIND_TIMEOUT_TIME=(14 14 14 14 14 14 14 14 50 50)
BONUS_POINTS=(1 1 1 1 1 1 1 1 1 1) #valgrind
TOTAL=0
BONUS=0

ERRORS_NUMBER=0
CODING_STYLE_BONUS=0
CURRENT_DIRECTORY=`pwd`
ONE=1
TWO=2
MESSAGE_SIZE=12
PASSED_TESTS_GRADE=0

timeout_test()
{
	test_no=$1
    input_path=$2
    ref_path=$3
    out_path=$4
	
    # Get the input and ref files for the current test
	timeout=${TIMEOUT_TIME[$test_no]}
	(time timeout $timeout ./$EXEC <$input_path >$out_path) 2>time.err
	TEST_RESULT=$?
}

valgrind_test()
{
    test_no=$1
    input_path=$2
    ref_path=$3
    out_path=$4

	# Get the input and ref files for the current test
	timeout=${VALGRIND_TIMEOUT_TIME[$test_no]}

	(time timeout $timeout valgrind --leak-check=full --error-exitcode=1 -q ./$EXEC <$input_path >$out_path) 2>time.err
	TEST_RESULT=$?
}


# Check a single test and add the result to the total score. This function
# should _always_ receive a test number as the first and only argument
check_test()
{
    test_no=$1
    input_path="input/test"$test_no".in"
    ref_path="ref/test"$test_no".out"
    out_path="result.out"

    #echo
    echo -n "Test: $test_no ...................... "


	timeout_test $test_no $input_path $ref_path $out_path

    # Run the test, also consider the timeout
    if [ "$TEST_RESULT" == "124" ]; then
        echo "TIMEOUT [$timeout s]"
        rm -f result.out
        rm -f time.err
		return
	fi

    # Check the result
    diff -bB -i $ref_path $out_path 2>&1 1> my_diff
    if test $? -eq 0; then
        echo "PASS [${TEST_POINTS[$test_no]}p]"
		#if [ -f time.err  ]; then
			#cat time.err
		#fi
        TOTAL=$(expr $TOTAL + ${TEST_POINTS[$test_no]})
        PASSED_TESTS_GRADE=$(($PASSED_TESTS_GRADE+1));
    else
        echo "FAILED"
        echo "Diff result:"
        cat my_diff | tail -n 10
        #cat time.err
    fi

    # Clean up
    rm -f my_diff
    rm -f result.out
    rm -f time.err
}


check_valgrind_test()
{
    test_no=$1
    input_path="input/test"$test_no".in"
    ref_path="ref/test"$test_no".out"
    out_path="result.out"

    #echo
    echo -n "Test: $test_no ...................... "

    valgrind_test $test_no $input_path $ref_path $out_path

    if [ "$TEST_RESULT" != "0" ]; then
        echo "DETECTED MEMORY LEAKS"
        #tail -n 10 time.err
        rm -f result.out
        rm -f time.err
        return
    else
        echo "NO MEMORY LEAKS"
        rm -f result.out
        rm -f time.err
        BONUS=$(expr $BONUS + ${BONUS_POINTS[$test_no]})
        return
    fi

}

function checkBonus
{
    printf '%*s\n' "${COLUMNS:-$(($(tput cols) - $ONE))}" '' | tr ' ' -
    echo "" > checkstyle.txt
    
    echo -ne "Coding style Errors:\n"
    TargetDir=$1
    for entry in "$TargetDir"/*
    do
        if [[ -f $entry ]]; then
            ./cpplint.py "$entry" > checkstyle.txt 
            YOUR_BONUS=`cat checkstyle.txt | grep "Total errors found: "| tr -dc '0-9'`
            ERRORS_NUMBER=$(( $ERRORS_NUMBER + $YOUR_BONUS ))
        fi
    done

    if [[ $ERRORS_NUMBER != "0" ]]; then
        printf '\nBonus: %*sFAIL' "${COLUMNS:-$(($(tput cols) - $MESSAGE_SIZE))}" '' | tr ' ' -
        echo -ne "\n\t\tYou have a total of $ERRORS_NUMBER coding style errors.\n"

    else
        echo -ne "\n\tNone?\n\t\tWow, next level!"
        printf '\n\nBonus: %*sOK' "${COLUMNS:-$(($(tput cols) - $MESSAGE_SIZE + $TWO ))}" '' | tr ' ' -

        CODING_STYLE_BONUS=$(($PASSED_TESTS_GRADE * 2))
        CODING_STYLE_BONUS=$(($CODING_STYLE_BONUS))
    fi
}

function printBonus
{
    if [[ $ERRORS_NUMBER -gt 0 ]]; then
    printf '\n%*s' "${COLUMNS:-$(tput cols)}" '' | tr ' ' -
        echo -ne  "\n\tAnd you almost did it!\n"
    else
        echo -ne "\n\tDamn!\n"
    fi
    echo -ne "\n\t\tYou got a bonus of 0.$CODING_STYLE_BONUS/0.2.\n\n"
}

# Check if the the exec exist
if test ! -x $EXEC; then
    echo "Execf file missing"
    exit 1
fi

for ((i=0;i<$NO_TESTS;i++))
do
    check_valgrind_test $i
done
NO_NOTE=0
if [ $BONUS == 10 ]; then
    echo "RESULT: NO MEMORY LEAKS"
else
    echo "RESULT: MEMORY LEAKS DETECTED - NEED HUMAN ASSISTANCE"
    NO_NOTE=1
fi

if [ $NO_NOTE -eq 1 ]; then
    exit -1
fi;
echo ""
for ((i=0;i<$NO_TESTS;i++))
do
	check_test $i
done

# And the restul is ...
echo "TOTAL: $TOTAL/80"
echo ""

mkdir resources
cp archive.zip resources/archive.zip
cd resources
unzip archive.zip
cd ..
checkBonus resources
printBonus
rm -rf resources
