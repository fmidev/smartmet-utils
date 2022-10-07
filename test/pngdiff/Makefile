PNGDIFF := ../../smartpngdiff

# We do not want failed tests here in case when newbase is installed and configured
# to use WGS84 branch
export IGNORE_WGS84 := Y

all:

test:   test-different-1 test-different-2 test-identical

#Identical images (same image twice): smartpngdiff should pass
test-identical:
	$(PNGDIFF) 1.png 1.png diff-1-1.png

# Images similar enough: smartpngdiff should pass with warning
test-different-1:
	$(PNGDIFF) 1.png 2.png diff-1-2.png

# Unrelated images: smartpngdiff must fail
test-different-2:
	if ! $(PNGDIFF) 1.png 3.png diff-1-3.png; then true; else false; fi

clean:
	rm -f diff-*.png
