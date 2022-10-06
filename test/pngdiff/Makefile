all:

test:   test-different-1 test-different-2 test-identical

test-identical:
	../../smartpngdiff.sh 1.png 1.png diff-1-1.png

test-different-1:
	../../smartpngdiff.sh 1.png 2.png diff-1-2.png

test-different-2:
	if ! ../../smartpngdiff.sh 1.png 3.png diff-1-3.png; then true; else false; fi

clean:
	rm -f diff-*.png
