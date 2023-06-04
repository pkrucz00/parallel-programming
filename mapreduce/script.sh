for size in 1 5 10
do
        input="books-input/${size}g.txt"
        for test_case_num in {1..3}
        do
                { time hadoop jar /usr/lib/hadoop/hadoop-streaming.jar \
                                -files mapper.py,reducer.py\
                                 -mapper mapper.py -reducer reducer.py \
                                -input ${input} -output books-output ; } 2>>time.txt
                hdfs dfs -rm -r books-output
        done
done