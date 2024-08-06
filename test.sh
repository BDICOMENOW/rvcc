#set -x
assert() {
  expected="$1"
  
  input="$2"
  # 运行程序，传入期待值，将生成结果写入tmp.s汇编文件。
  # 如果运行不成功，则会执行exit退出。成功时会短路exit操作
  ./rvcc "$input" > tmp.s || exit
  # 编译rvcc产生的汇编文件
  # clang -o tmp tmp.s
  riscv64-linux-gnu-gcc tmp.s -o tmp 

  # 运行生成出来目标文件
  # ./tmp
  #qemu-riscv64 -L $RISCV/sysroot ./tmp

  qemu-riscv64 ./tmp
  # $RISCV/bin/spike --isa=rv64gc $RISCV/riscv64-unknown-linux-gnu/bin/pk ./tmp
  
  actual="$?"
  
  if [ "$actual" = "$expected" ]; then
    echo "$input -> $actual"
  else
    echo "$input -> $expected expected, but got $actual"
    exit 1
  fi
}


#assert 0 0
#assert 42 42
#assert 34 12-34+56
assert 10 ' 10 - 30 + 30'

echo ok
