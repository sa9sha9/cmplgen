# cmplgen
generate compiler

# test
PART : `make clean ; make && ./test.sh test01 && cat test01.result`
ALL  : `make clean ; make && ./testall.sh`

# ファイルを会津大学のディレクトリへ置く
1. `git pull`
1. `*.o` ファイルをgithubで扱わないので、tarファイルを自分で開く
`tar -xvf FILENAME.tar`