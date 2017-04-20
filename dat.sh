CUR="0"
datran() {
	# repeating for different random seeds
	for i in `seq 5`
	do
		echo $CUR
		./datran -s $i $1 $2 $3 >`printf %02d $CUR`
		CUR="`expr $CUR + 1`"
	done
}
datcnf() {
	echo $CUR
	./datcnf $1 >`printf %02d $CUR`
	CUR="`expr $CUR + 1`"
}
datpts() {
	echo $CUR
	./datpts $1 | ./datgen >`printf %02d $CUR`
	CUR="`expr $CUR + 1`"
}

echo "Random linear hypergraphs"
datran 100 100  3
datran 100 100  5
datran 100 100  8
datran 100 100 10
datran 200  20 10
datran 200 100 10
datran 200 100 15
datran 200 500 50
datran 500  10 90
datran 500 100 15

echo "Predefined point positions"
datpts 0
datpts 1
datpts 2
datpts 3
datpts 4
datpts 5
datpts 6
datpts 7
datpts 8

echo "Predefined hypergraphs"
datcnf 0
datcnf 1
datcnf 2
datcnf 3
