gcc ex3.c -o ex3

./ex3 3 &
for i in {1..3}; do
    echo "pstree $i"
    pstree | grep ex3
    sleep 5
done

./ex3 5 &
for i in {1..5}; do
    echo "pstree $i"
    pstree | grep ex3
    sleep 5
done

rm ex3

