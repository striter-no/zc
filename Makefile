all:
	clang -std=c2x -Dmain_file=zc.c -D_GNU_SOURCE -O3 -o ~/Projects/.local/zc ./code/mds/subcode/main.c -I ./code/mds/include -fblocks -lBlocksRuntime
debug:
	clang -std=c2x -Dmain_file=zc.c -D_GNU_SOURCE -O0 -g -fsanitize=undefined,address -o ~/Projects/.local/zc ./code/mds/subcode/main.c -I ./code/mds/include -fblocks -lBlocksRuntime -Wall -Wextra -Wno-sign-compare -Wno-unused-parameter
test: # 
	clang -std=c2x -Dmain_file=test.c -DTESTING -D_GNU_SOURCE -fsanitize=undefined,address -O0 -g -o ./bin/test ./code/mds/subcode/main.c -I ./code/mds/include -fblocks -lBlocksRuntime -Wall -Wextra -Wno-sign-compare -Wno-unused-parameter