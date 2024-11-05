all: tar

tar:
	rm -rf ccanada6_submission
	mkdir ccanada6_submission
	cp Makefile ccanada6_submission/
	cp README.md ccanada6_submission/
	cp -r part1 ccanada6_submission/
	cp -r part2 ccanada6_submission/
	cp -r part3 ccanada6_submission/

	tar -cvf ccanada6_assignment_3.tar.gz ccanada6_submission

clean:
	rm -f ccanada6_assignment_3.tar.gz
	rm -rf ccanada6_submission