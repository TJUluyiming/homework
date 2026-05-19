<#
C-- 编译器自动测试脚本
功能：批量运行所有测试用例，自动对比输出，生成测试报告
#>

# ==================== 配置项 ====================
$COMPILER_CMD = "g++ -std=c++17 -Wall -g -static lexer.cpp parser.cpp ir_visitor.cpp main.cpp -o mycompiler.exe"
$MAIN_FILE = "main.cpp"
$TEST_RESULTS_DIR = "test_results"
$TEMP_MAIN_FILE = "main_temp.cpp"

# ==================== 测试用例定义（全部使用单引号here-string） ====================
$TEST_CASES = @(
    # 1. 基础功能测试
    [PSCustomObject]@{
        Name = "01_空程序"
        Code = @'
int main() { return 0; }
'@
        ExpectedOutput = @'
>>> Phase 1: Lexical Analysis
int     <KW,1>
main    <KW,5>
(       <SE,20>
)       <SE,21>
{       <SE,22>
return  <KW,3>
0       <INT,0>
;       <SE,24>
}       <SE,23>

>>> Symbol Table

>>> Phase 2: Syntax Analysis
1       #int    reduction
2       compUnit#int    move
3       int#Ident       reduction
4       funcType#Ident  move
5       Ident#( move
6       (#)     move
7       )#{     move
8       {#return        reduction
9       blockItems#return       move
10      return#IntConst move
11      IntConst#;      reduction
12      number#;        reduction
13      primaryExp#;    reduction
14      unaryExp#;      reduction
15      mulExp#;        reduction
16      addExp#;        reduction
17      exp#;   move
18      ;#}     reduction
19      stmt#}  reduction
20      blockItem#}     reduction
21      blockItems#}    move
22      }#$     reduction
23      block#$ reduction
24      funcDef#$       reduction
25      compUnit#$      reduction
26      Program#$       accept

>>> Phase 3: Intermediate Code Generation
declare i32 @getint()
declare i32 @getch()
declare i32 @getarray(i32*)
declare void @putint(i32)
declare void @putch(i32)
declare void @putarray(i32, i32*)
declare void @starttime()
declare void @stoptime()

define i32 @main() {
main_ENTRY:
  ret i32 0
}
'@
    },

    [PSCustomObject]@{
        Name = "02_全局变量声明与初始化"
        Code = @'
int a;
int b = 100;
const int c = 200;
float d;
float e = 3.14159;
const float f = 2.71828;

int main() {
    return 0;
}
'@
        ExpectedOutput = @'
>>> Phase 1: Lexical Analysis
int     <KW,1>
a       <IDN,a>
;       <SE,24>
int     <KW,1>
b       <IDN,b>
=       <OP,11>
100     <INT,100>
;       <SE,24>
const   <KW,4>
int     <KW,1>
c       <IDN,c>
=       <OP,11>
200     <INT,200>
;       <SE,24>
float   <KW,6>
d       <IDN,d>
;       <SE,24>
float   <KW,6>
e       <IDN,e>
=       <OP,11>
3.14159 <FLOAT,3.14159>
;       <SE,24>
const   <KW,4>
float   <KW,6>
f       <IDN,f>
=       <OP,11>
2.71828 <FLOAT,2.71828>
;       <SE,24>
int     <KW,1>
main    <KW,5>
(       <SE,20>
)       <SE,21>
{       <SE,22>
return  <KW,3>
0       <INT,0>
;       <SE,24>
}       <SE,23>

>>> Symbol Table
a       <IDN,a>
b       <IDN,b>
c       <IDN,c>
d       <IDN,d>
e       <IDN,e>
f       <IDN,f>

>>> Phase 2: Syntax Analysis
1       #int    reduction
2       compUnit#int    move
3       int#Ident       reduction
4       bType#Ident     move
5       Ident#; reduction
6       varDef#;        reduction
7       varDefs#;       move
8       ;#int   reduction
9       varDecl#int     reduction
10      decl#int        reduction
11      compUnit#int    move
12      int#Ident       reduction
13      bType#Ident     move
14      Ident#= move
15      =#IntConst      move
16      IntConst#;      reduction
17      number#;        reduction
18      primaryExp#;    reduction
19      unaryExp#;      reduction
20      mulExp#;        reduction
21      addExp#;        reduction
22      exp#;   reduction
23      initVal#;       reduction
24      varDef#;        reduction
25      varDefs#;       move
26      ;#const reduction
27      varDecl#const   reduction
28      decl#const      reduction
29      compUnit#const  move
30      const#int       move
31      int#Ident       reduction
32      bType#Ident     move
33      Ident#= move
34      =#IntConst      move
35      IntConst#;      reduction
36      number#;        reduction
37      primaryExp#;    reduction
38      unaryExp#;      reduction
39      mulExp#;        reduction
40      addExp#;        reduction
41      constExp#;      reduction
42      constInitVal#;  reduction
43      constDef#;      reduction
44      constDefs#;     move
45      ;#float reduction
46      constDecl#float reduction
47      decl#float      reduction
48      compUnit#float  move
49      float#Ident     reduction
50      bType#Ident     move
51      Ident#; reduction
52      varDef#;        reduction
53      varDefs#;       move
54      ;#float reduction
55      varDecl#float   reduction
56      decl#float      reduction
57      compUnit#float  move
58      float#Ident     reduction
59      bType#Ident     move
60      Ident#= move
61      =#floatConst    move
62      floatConst#;    reduction
63      number#;        reduction
64      primaryExp#;    reduction
65      unaryExp#;      reduction
66      mulExp#;        reduction
67      addExp#;        reduction
68      exp#;   reduction
69      initVal#;       reduction
70      varDef#;        reduction
71      varDefs#;       move
72      ;#const reduction
73      varDecl#const   reduction
74      decl#const      reduction
75      compUnit#const  move
76      const#float     move
77      float#Ident     reduction
78      bType#Ident     move
79      Ident#= move
80      =#floatConst    move
81      floatConst#;    reduction
82      number#;        reduction
83      primaryExp#;    reduction
84      unaryExp#;      reduction
85      mulExp#;        reduction
86      addExp#;        reduction
87      constExp#;      reduction
88      constInitVal#;  reduction
89      constDef#;      reduction
90      constDefs#;     move
91      ;#int   reduction
92      constDecl#int   reduction
93      decl#int        reduction
94      compUnit#int    move
95      int#Ident       reduction
96      funcType#Ident  move
97      Ident#( move
98      (#)     move
99      )#{     move
100     {#return        reduction
101     blockItems#return       move
102     return#IntConst move
103     IntConst#;      reduction
104     number#;        reduction
105     primaryExp#;    reduction
106     unaryExp#;      reduction
107     mulExp#;        reduction
108     addExp#;        reduction
109     exp#;   move
110     ;#}     reduction
111     stmt#}  reduction
112     blockItem#}     reduction
113     blockItems#}    move
114     }#$     reduction
115     block#$ reduction
116     funcDef#$       reduction
117     compUnit#$      reduction
118     Program#$       accept

>>> Phase 3: Intermediate Code Generation
declare i32 @getint()
declare i32 @getch()
declare i32 @getarray(i32*)
declare void @putint(i32)
declare void @putch(i32)
declare void @putarray(i32, i32*)
declare void @starttime()
declare void @stoptime()

@a = global i32 0
@b = global i32 100
@c = global i32 200
@d = global float 0.000000
@e = global float 3.141590
@f = global float 2.718280
define i32 @main() {
main_ENTRY:
  ret i32 0
}
'@
    },

    [PSCustomObject]@{
        Name = "03_局部变量声明与赋值"
        Code = @'
int main() {
    int x;
    int y = 50;
    float z = 1.23;
    x = 20;
    y = x;
    return 0;
}
'@
        ExpectedOutput = @'
>>> Phase 1: Lexical Analysis
int     <KW,1>
main    <KW,5>
(       <SE,20>
)       <SE,21>
{       <SE,22>
int     <KW,1>
x       <IDN,x>
;       <SE,24>
int     <KW,1>
y       <IDN,y>
=       <OP,11>
50      <INT,50>
;       <SE,24>
float   <KW,6>
z       <IDN,z>
=       <OP,11>
1.23    <FLOAT,1.23>
;       <SE,24>
x       <IDN,x>
=       <OP,11>
20      <INT,20>
;       <SE,24>
y       <IDN,y>
=       <OP,11>
x       <IDN,x>
;       <SE,24>
return  <KW,3>
0       <INT,0>
;       <SE,24>
}       <SE,23>

>>> Symbol Table
x       <IDN,x>
y       <IDN,y>
z       <IDN,z>

>>> Phase 2: Syntax Analysis
1       #int    reduction
2       compUnit#int    move
3       int#Ident       reduction
4       funcType#Ident  move
5       Ident#( move
6       (#)     move
7       )#{     move
8       {#int   reduction
9       blockItems#int  move
10      int#Ident       reduction
11      bType#Ident     move
12      Ident#; reduction
13      varDef#;        reduction
14      varDefs#;       move
15      ;#int   reduction
16      varDecl#int     reduction
17      blockItem#int   reduction
18      blockItems#int  move
19      int#Ident       reduction
20      bType#Ident     move
21      Ident#= move
22      =#IntConst      move
23      IntConst#;      reduction
24      number#;        reduction
25      primaryExp#;    reduction
26      unaryExp#;      reduction
27      mulExp#;        reduction
28      addExp#;        reduction
29      exp#;   reduction
30      initVal#;       reduction
31      varDef#;        reduction
32      varDefs#;       move
33      ;#float reduction
34      varDecl#float   reduction
35      blockItem#float reduction
36      blockItems#float        move
37      float#Ident     reduction
38      bType#Ident     move
39      Ident#= move
40      =#floatConst    move
41      floatConst#;    reduction
42      number#;        reduction
43      primaryExp#;    reduction
44      unaryExp#;      reduction
45      mulExp#;        reduction
46      addExp#;        reduction
47      exp#;   reduction
48      initVal#;       reduction
49      varDef#;        reduction
50      varDefs#;       move
51      ;#Ident reduction
52      varDecl#Ident   reduction
53      blockItem#Ident reduction
54      blockItems#Ident        move
55      Ident#= move
56      =#IntConst      move
57      IntConst#;      reduction
58      number#;        reduction
59      primaryExp#;    reduction
60      unaryExp#;      reduction
61      mulExp#;        reduction
62      addExp#;        reduction
63      exp#;   move
64      ;#Ident reduction
65      stmt#Ident      reduction
66      blockItem#Ident reduction
67      blockItems#Ident        move
68      Ident#= move
69      =#Ident move
70      Ident#; reduction
71      lVal#;  reduction
72      primaryExp#;    reduction
73      unaryExp#;      reduction
74      mulExp#;        reduction
75      addExp#;        reduction
76      exp#;   move
77      ;#return        reduction
78      stmt#return     reduction
79      blockItem#return        reduction
80      blockItems#return       move
81      return#IntConst move
82      IntConst#;      reduction
83      number#;        reduction
84      primaryExp#;    reduction
85      unaryExp#;      reduction
86      mulExp#;        reduction
87      addExp#;        reduction
88      exp#;   move
89      ;#}     reduction
90      stmt#}  reduction
91      blockItem#}     reduction
92      blockItems#}    move
93      }#$     reduction
94      block#$ reduction
95      funcDef#$       reduction
96      compUnit#$      reduction
97      Program#$       accept

>>> Phase 3: Intermediate Code Generation
declare i32 @getint()
declare i32 @getch()
declare i32 @getarray(i32*)
declare void @putint(i32)
declare void @putch(i32)
declare void @putarray(i32, i32*)
declare void @starttime()
declare void @stoptime()

define i32 @main() {
main_ENTRY:
  %x = alloca i32
  %y = alloca i32
  %z = alloca float
  store i32 50, i32* %y
  store float 1.230000, float* %z
  store i32 20, i32* %x
  %tmp0 = load i32, i32* %x
  store i32 %tmp0, i32* %y
  ret i32 0
}
'@
    },

    # 2. 表达式运算测试
    [PSCustomObject]@{
        Name = "04_单目运算"
        Code = @'
int main() {
    int a = -10;
    int b = +20;
    int c = !0;
    int d = !1;
    return 0;
}
'@
        ExpectedOutput = @'
>>> Phase 1: Lexical Analysis
int     <KW,1>
main    <KW,5>
(       <SE,20>
)       <SE,21>
{       <SE,22>
int     <KW,1>
a       <IDN,a>
=       <OP,11>
-       <OP,7>
10      <INT,10>
;       <SE,24>
int     <KW,1>
b       <IDN,b>
=       <OP,11>
+       <OP,6>
20      <INT,20>
;       <SE,24>
int     <KW,1>
c       <IDN,c>
=       <OP,11>
!       <OP,26>
0       <INT,0>
;       <SE,24>
int     <KW,1>
d       <IDN,d>
=       <OP,11>
!       <OP,26>
1       <INT,1>
;       <SE,24>
return  <KW,3>
0       <INT,0>
;       <SE,24>
}       <SE,23>

>>> Symbol Table
a       <IDN,a>
b       <IDN,b>
c       <IDN,c>
d       <IDN,d>

>>> Phase 2: Syntax Analysis
1       #int    reduction
2       compUnit#int    move
3       int#Ident       reduction
4       funcType#Ident  move
5       Ident#( move
6       (#)     move
7       )#{     move
8       {#int   reduction
9       blockItems#int  move
10      int#Ident       reduction
11      bType#Ident     move
12      Ident#= move
13      =#-     move
14      -#IntConst      move
15      IntConst#;      reduction
16      number#;        reduction
17      primaryExp#;    reduction
18      unaryExp#;      reduction
19      mulExp#;        reduction
20      addExp#;        reduction
21      exp#;   reduction
22      initVal#;       reduction
23      varDef#;        reduction
24      varDefs#;       move
25      ;#int   reduction
26      varDecl#int     reduction
27      blockItem#int   reduction
28      blockItems#int  move
29      int#Ident       reduction
30      bType#Ident     move
31      Ident#= move
32      =#+     move
33      +#IntConst      move
34      IntConst#;      reduction
35      number#;        reduction
36      primaryExp#;    reduction
37      unaryExp#;      reduction
38      mulExp#;        reduction
39      addExp#;        reduction
40      exp#;   reduction
41      initVal#;       reduction
42      varDef#;        reduction
43      varDefs#;       move
44      ;#int   reduction
45      varDecl#int     reduction
46      blockItem#int   reduction
47      blockItems#int  move
48      int#Ident       reduction
49      bType#Ident     move
50      Ident#= move
51      =#!     move
52      !#IntConst      move
53      IntConst#;      reduction
54      number#;        reduction
55      primaryExp#;    reduction
56      unaryExp#;      reduction
57      mulExp#;        reduction
58      addExp#;        reduction
59      exp#;   reduction
60      initVal#;       reduction
61      varDef#;        reduction
62      varDefs#;       move
63      ;#int   reduction
64      varDecl#int     reduction
65      blockItem#int   reduction
66      blockItems#int  move
67      int#Ident       reduction
68      bType#Ident     move
69      Ident#= move
70      =#!     move
71      !#IntConst      move
72      IntConst#;      reduction
73      number#;        reduction
74      primaryExp#;    reduction
75      unaryExp#;      reduction
76      mulExp#;        reduction
77      addExp#;        reduction
78      exp#;   reduction
79      initVal#;       reduction
80      varDef#;        reduction
81      varDefs#;       move
82      ;#return        reduction
83      varDecl#return  reduction
84      blockItem#return        reduction
85      blockItems#return       move
86      return#IntConst move
87      IntConst#;      reduction
88      number#;        reduction
89      primaryExp#;    reduction
90      unaryExp#;      reduction
91      mulExp#;        reduction
92      addExp#;        reduction
93      exp#;   move
94      ;#}     reduction
95      stmt#}  reduction
96      blockItem#}     reduction
97      blockItems#}    move
98      }#$     reduction
99      block#$ reduction
100     funcDef#$       reduction
101     compUnit#$      reduction
102     Program#$       accept

>>> Phase 3: Intermediate Code Generation
declare i32 @getint()
declare i32 @getch()
declare i32 @getarray(i32*)
declare void @putint(i32)
declare void @putch(i32)
declare void @putarray(i32, i32*)
declare void @starttime()
declare void @stoptime()

define i32 @main() {
main_ENTRY:
  %a = alloca i32
  %b = alloca i32
  %c = alloca i32
  %d = alloca i32
  %tmp0 = sub i32 0, 10
  store i32 %tmp0, i32* %a
  store i32 20, i32* %b
  %tmp1 = xor i32 0, 1
  store i32 %tmp1, i32* %c
  %tmp2 = xor i32 1, 1
  store i32 %tmp2, i32* %d
  ret i32 0
}
'@
    },

    [PSCustomObject]@{
        Name = "05_二元算术运算"
        Code = @'
int main() {
    int a = 3 + 4 * 5;
    int b = (10 - 2) / 3;
    int c = 15 % 4;
    return 0;
}
'@
        ExpectedOutput = @'
>>> Phase 1: Lexical Analysis
int     <KW,1>
main    <KW,5>
(       <SE,20>
)       <SE,21>
{       <SE,22>
int     <KW,1>
a       <IDN,a>
=       <OP,11>
3       <INT,3>
+       <OP,6>
4       <INT,4>
*       <OP,8>
5       <INT,5>
;       <SE,24>
int     <KW,1>
b       <IDN,b>
=       <OP,11>
(       <SE,20>
10      <INT,10>
-       <OP,7>
2       <INT,2>
)       <SE,21>
/       <OP,9>
3       <INT,3>
;       <SE,24>
int     <KW,1>
c       <IDN,c>
=       <OP,11>
15      <INT,15>
%       <OP,10>
4       <INT,4>
;       <SE,24>
return  <KW,3>
0       <INT,0>
;       <SE,24>
}       <SE,23>

>>> Symbol Table
a       <IDN,a>
b       <IDN,b>
c       <IDN,c>

>>> Phase 2: Syntax Analysis
1       #int    reduction
2       compUnit#int    move
3       int#Ident       reduction
4       funcType#Ident  move
5       Ident#( move
6       (#)     move
7       )#{     move
8       {#int   reduction
9       blockItems#int  move
10      int#Ident       reduction
11      bType#Ident     move
12      Ident#= move
13      =#IntConst      move
14      IntConst#+      move
15      +#IntConst      move
16      IntConst#*      move
17      *#IntConst      move
18      IntConst#;      reduction
19      number#;        reduction
20      primaryExp#;    reduction
21      unaryExp#;      reduction
22      mulExp#;        reduction
23      addExp#;        reduction
24      exp#;   reduction
25      initVal#;       reduction
26      varDef#;        reduction
27      varDefs#;       move
28      ;#int   reduction
29      varDecl#int     reduction
30      blockItem#int   reduction
31      blockItems#int  move
32      int#Ident       reduction
33      bType#Ident     move
34      Ident#= move
35      =#(     move
36      (#IntConst      move
37      IntConst#-      move
38      -#IntConst      move
39      IntConst#)      move
40      )#/     move
41      /#IntConst      move
42      IntConst#;      reduction
43      number#;        reduction
44      primaryExp#;    reduction
45      unaryExp#;      reduction
46      mulExp#;        reduction
47      addExp#;        reduction
48      exp#;   reduction
49      initVal#;       reduction
50      varDef#;        reduction
51      varDefs#;       move
52      ;#int   reduction
53      varDecl#int     reduction
54      blockItem#int   reduction
55      blockItems#int  move
56      int#Ident       reduction
57      bType#Ident     move
58      Ident#= move
59      =#IntConst      move
60      IntConst#%      move
61      %#IntConst      move
62      IntConst#;      reduction
63      number#;        reduction
64      primaryExp#;    reduction
65      unaryExp#;      reduction
66      mulExp#;        reduction
67      addExp#;        reduction
68      exp#;   reduction
69      initVal#;       reduction
70      varDef#;        reduction
71      varDefs#;       move
72      ;#return        reduction
73      varDecl#return  reduction
74      blockItem#return        reduction
75      blockItems#return       move
76      return#IntConst move
77      IntConst#;      reduction
78      number#;        reduction
79      primaryExp#;    reduction
80      unaryExp#;      reduction
81      mulExp#;        reduction
82      addExp#;        reduction
83      exp#;   move
84      ;#}     reduction
85      stmt#}  reduction
86      blockItem#}     reduction
87      blockItems#}    move
88      }#$     reduction
89      block#$ reduction
90      funcDef#$       reduction
91      compUnit#$      reduction
92      Program#$       accept

>>> Phase 3: Intermediate Code Generation
declare i32 @getint()
declare i32 @getch()
declare i32 @getarray(i32*)
declare void @putint(i32)
declare void @putch(i32)
declare void @putarray(i32, i32*)
declare void @starttime()
declare void @stoptime()

define i32 @main() {
main_ENTRY:
  %a = alloca i32
  %b = alloca i32
  %c = alloca i32
  %tmp0 = mul i32 4, 5
  %tmp1 = add i32 3, %tmp0
  store i32 %tmp1, i32* %a
  %tmp2 = sub i32 10, 2
  %tmp3 = sdiv i32 %tmp2, 3
  store i32 %tmp3, i32* %b
  %tmp4 = srem i32 15, 4
  store i32 %tmp4, i32* %c
  ret i32 0
}
'@
    },

    # 3. 函数相关测试
    [PSCustomObject]@{
        Name = "06_无参数函数调用"
        Code = @'
int foo() {
    return 100;
}

int main() {
    int a = foo();
    return 0;
}
'@
        ExpectedOutput = @'
>>> Phase 1: Lexical Analysis
int     <KW,1>
foo     <IDN,foo>
(       <SE,20>
)       <SE,21>
{       <SE,22>
return  <KW,3>
100     <INT,100>
;       <SE,24>
}       <SE,23>
int     <KW,1>
main    <KW,5>
(       <SE,20>
)       <SE,21>
{       <SE,22>
int     <KW,1>
a       <IDN,a>
=       <OP,11>
foo     <IDN,foo>
(       <SE,20>
)       <SE,21>
;       <SE,24>
return  <KW,3>
0       <INT,0>
;       <SE,24>
}       <SE,23>

>>> Symbol Table
foo     <IDN,foo>
a       <IDN,a>

>>> Phase 2: Syntax Analysis
1       #int    reduction
2       compUnit#int    move
3       int#Ident       reduction
4       funcType#Ident  move
5       Ident#( move
6       (#)     move
7       )#{     move
8       {#return        reduction
9       blockItems#return       move
10      return#IntConst move
11      IntConst#;      reduction
12      number#;        reduction
13      primaryExp#;    reduction
14      unaryExp#;      reduction
15      mulExp#;        reduction
16      addExp#;        reduction
17      exp#;   move
18      ;#}     reduction
19      stmt#}  reduction
20      blockItem#}     reduction
21      blockItems#}    move
22      }#int   reduction
23      block#int       reduction
24      funcDef#int     reduction
25      compUnit#int    move
26      int#Ident       reduction
27      funcType#Ident  move
28      Ident#( move
29      (#)     move
30      )#{     move
31      {#int   reduction
32      blockItems#int  move
33      int#Ident       reduction
34      bType#Ident     move
35      Ident#= move
36      =#Ident move
37      Ident#( move
38      (#)     move
39      )#;     reduction
40      unaryExp#;      reduction
41      mulExp#;        reduction
42      addExp#;        reduction
43      exp#;   reduction
44      initVal#;       reduction
45      varDef#;        reduction
46      varDefs#;       move
47      ;#return        reduction
48      varDecl#return  reduction
49      blockItem#return        reduction
50      blockItems#return       move
51      return#IntConst move
52      IntConst#;      reduction
53      number#;        reduction
54      primaryExp#;    reduction
55      unaryExp#;      reduction
56      mulExp#;        reduction
57      addExp#;        reduction
58      exp#;   move
59      ;#}     reduction
60      stmt#}  reduction
61      blockItem#}     reduction
62      blockItems#}    move
63      }#$     reduction
64      block#$ reduction
65      funcDef#$       reduction
66      compUnit#$      reduction
67      Program#$       accept

>>> Phase 3: Intermediate Code Generation
declare i32 @getint()
declare i32 @getch()
declare i32 @getarray(i32*)
declare void @putint(i32)
declare void @putch(i32)
declare void @putarray(i32, i32*)
declare void @starttime()
declare void @stoptime()

define i32 @foo() {
foo_ENTRY:
  ret i32 100
}

define i32 @main() {
main_ENTRY:
  %a = alloca i32
  %tmp0 = call i32 @foo()
  store i32 %tmp0, i32* %a
  ret i32 0
}
'@
    },

    [PSCustomObject]@{
        Name = "07_多参数函数调用"
        Code = @'
int add(int x, int y) {
    return x + y;
}

int main() {
    int a = add(3, 4);
    return 0;
}
'@
        ExpectedOutput = @'
>>> Phase 1: Lexical Analysis
int     <KW,1>
add     <IDN,add>
(       <SE,20>
int     <KW,1>
x       <IDN,x>
,       <SE,25>
int     <KW,1>
y       <IDN,y>
)       <SE,21>
{       <SE,22>
return  <KW,3>
x       <IDN,x>
+       <OP,6>
y       <IDN,y>
;       <SE,24>
}       <SE,23>
int     <KW,1>
main    <KW,5>
(       <SE,20>
)       <SE,21>
{       <SE,22>
int     <KW,1>
a       <IDN,a>
=       <OP,11>
add     <IDN,add>
(       <SE,20>
3       <INT,3>
,       <SE,25>
4       <INT,4>
)       <SE,21>
;       <SE,24>
return  <KW,3>
0       <INT,0>
;       <SE,24>
}       <SE,23>

>>> Symbol Table
add     <IDN,add>
x       <IDN,x>
y       <IDN,y>
a       <IDN,a>

>>> Phase 2: Syntax Analysis
1       #int    reduction
2       compUnit#int    move
3       int#Ident       reduction
4       funcType#Ident  move
5       Ident#( move
6       (#int   move
7       int#Ident       reduction
8       bType#Ident     move
9       Ident#, move
10      ,#int   move
11      int#Ident       reduction
12      bType#Ident     move
13      Ident#) move
14      )#{     move
15      {#return        reduction
16      blockItems#return       move
17      return#Ident    move
18      Ident#+ move
19      +#Ident move
20      Ident#; reduction
21      lVal#;  reduction
22      primaryExp#;    reduction
23      unaryExp#;      reduction
24      mulExp#;        reduction
25      addExp#;        reduction
26      exp#;   move
27      ;#}     reduction
28      stmt#}  reduction
29      blockItem#}     reduction
30      blockItems#}    move
31      }#int   reduction
32      block#int       reduction
33      funcDef#int     reduction
34      compUnit#int    move
35      int#Ident       reduction
36      funcType#Ident  move
37      Ident#( move
38      (#)     move
39      )#{     move
40      {#int   reduction
41      blockItems#int  move
42      int#Ident       reduction
43      bType#Ident     move
44      Ident#= move
45      =#Ident move
46      Ident#( move
47      (#IntConst      move
48      IntConst#,      move
49      ,#IntConst      move
50      IntConst#)      move
51      )#;     reduction
52      unaryExp#;      reduction
53      mulExp#;        reduction
54      addExp#;        reduction
55      exp#;   reduction
56      initVal#;       reduction
57      varDef#;        reduction
58      varDefs#;       move
59      ;#return        reduction
60      varDecl#return  reduction
61      blockItem#return        reduction
62      blockItems#return       move
63      return#IntConst move
64      IntConst#;      reduction
65      number#;        reduction
66      primaryExp#;    reduction
67      unaryExp#;      reduction
68      mulExp#;        reduction
69      addExp#;        reduction
70      exp#;   move
71      ;#}     reduction
72      stmt#}  reduction
73      blockItem#}     reduction
74      blockItems#}    move
75      }#$     reduction
76      block#$ reduction
77      funcDef#$       reduction
78      compUnit#$      reduction
79      Program#$       accept

>>> Phase 3: Intermediate Code Generation
declare i32 @getint()
declare i32 @getch()
declare i32 @getarray(i32*)
declare void @putint(i32)
declare void @putch(i32)
declare void @putarray(i32, i32*)
declare void @starttime()
declare void @stoptime()

define i32 @add(i32 %param_x, i32 %param_y) {
add_ENTRY:
  %x = alloca i32
  %y = alloca i32
  store i32 %param_x, i32* %x
  store i32 %param_y, i32* %y
  %tmp0 = load i32, i32* %x
  %tmp1 = load i32, i32* %y
  %tmp2 = add i32 %tmp0, %tmp1
  ret i32 %tmp2
}

define i32 @main() {
main_ENTRY:
  %a = alloca i32
  %tmp3 = call i32 @add(i32 3, i32 4)
  store i32 %tmp3, i32* %a
  ret i32 0
}
'@
    },

    [PSCustomObject]@{
        Name = "08_库函数调用"
        Code = @'
int main() {
    int a = getint();
    putint(a);
    return 0;
}
'@
        ExpectedOutput = @'
>>> Phase 1: Lexical Analysis
int     <KW,1>
main    <KW,5>
(       <SE,20>
)       <SE,21>
{       <SE,22>
int     <KW,1>
a       <IDN,a>
=       <OP,11>
getint  <IDN,getint>
(       <SE,20>
)       <SE,21>
;       <SE,24>
putint  <IDN,putint>
(       <SE,20>
a       <IDN,a>
)       <SE,21>
;       <SE,24>
return  <KW,3>
0       <INT,0>
;       <SE,24>
}       <SE,23>

>>> Symbol Table
a       <IDN,a>
getint  <IDN,getint>
putint  <IDN,putint>

>>> Phase 2: Syntax Analysis
1       #int    reduction
2       compUnit#int    move
3       int#Ident       reduction
4       funcType#Ident  move
5       Ident#( move
6       (#)     move
7       )#{     move
8       {#int   reduction
9       blockItems#int  move
10      int#Ident       reduction
11      bType#Ident     move
12      Ident#= move
13      =#Ident move
14      Ident#( move
15      (#)     move
16      )#;     reduction
17      unaryExp#;      reduction
18      mulExp#;        reduction
19      addExp#;        reduction
20      exp#;   reduction
21      initVal#;       reduction
22      varDef#;        reduction
23      varDefs#;       move
24      ;#Ident reduction
25      varDecl#Ident   reduction
26      blockItem#Ident reduction
27      blockItems#Ident        move
28      Ident#( move
29      (#Ident move
30      Ident#) move
31      )#;     reduction
32      unaryExp#;      reduction
33      mulExp#;        reduction
34      addExp#;        reduction
35      exp#;   move
36      ;#return        reduction
37      stmt#return     reduction
38      blockItem#return        reduction
39      blockItems#return       move
40      return#IntConst move
41      IntConst#;      reduction
42      number#;        reduction
43      primaryExp#;    reduction
44      unaryExp#;      reduction
45      mulExp#;        reduction
46      addExp#;        reduction
47      exp#;   move
48      ;#}     reduction
49      stmt#}  reduction
50      blockItem#}     reduction
51      blockItems#}    move
52      }#$     reduction
53      block#$ reduction
54      funcDef#$       reduction
55      compUnit#$      reduction
56      Program#$       accept

>>> Phase 3: Intermediate Code Generation
declare i32 @getint()
declare i32 @getch()
declare i32 @getarray(i32*)
declare void @putint(i32)
declare void @putch(i32)
declare void @putarray(i32, i32*)
declare void @starttime()
declare void @stoptime()

define i32 @main() {
main_ENTRY:
  %a = alloca i32
  %tmp0 = call i32 @getint()
  store i32 %tmp0, i32* %a
  %tmp1 = load i32, i32* %a
  call void @putint(i32 %tmp1)
  ret i32 0
}
'@
    },

    # 4. 进阶压力测试
    [PSCustomObject]@{
        Name = "09_分支控制与嵌套If_Else"
        Code = @'
int main() {
    int a = 5;
    int b = 10;
    if (a < b) {
        if (a == 5) { return 1; }
        else { return 2; }
    } else {
        return 3;
    }
}
'@
        ExpectedOutput = ""
    },

    [PSCustomObject]@{
        Name = "10_复合逻辑与短路求值"
        Code = @'
int main() {
    int a = 1;
    int b = 0;
    if (a == 1 || b == 1) {
        return 10;
    }
    return 20;
}
'@
        ExpectedOutput = ""
    },

    [PSCustomObject]@{
        Name = "11_作用域遮蔽"
        Code = @'
int x = 100;
int main() {
    int x = 10;
    {
        int x = 1;
        putint(x); 
    }
    putint(x); 
    return 0;
}
'@
        ExpectedOutput = ""
    },

    [PSCustomObject]@{
        Name = "12_复杂运算符优先级混合"
        Code = @'
int main() {
    int res = 5 + 4 * 3 - 2 == 15 && !0;
    return res; 
}
'@
        ExpectedOutput = ""
    },

    [PSCustomObject]@{
        Name = "13_极端注释与空白"
        Code = @'
// ===================================
//  这是一个单行注释
// ===================================
/* 这是一个
   跨越了多行的
   块注释 */   int main(  ) { 
    return /* 内嵌注释 */ 0 ; 
}
'@
        ExpectedOutput = ""
    }
)

# ==================== 主脚本逻辑 ====================
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "      C-- 编译器自动测试脚本（修复版）" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# 创建测试结果目录
if (Test-Path $TEST_RESULTS_DIR) {
    Remove-Item $TEST_RESULTS_DIR -Recurse -Force
}
New-Item -ItemType Directory -Path $TEST_RESULTS_DIR | Out-Null

# 备份原始main.cpp
Copy-Item $MAIN_FILE "$MAIN_FILE.bak" -Force
Write-Host "已备份原始 $MAIN_FILE 为 $MAIN_FILE.bak"
Write-Host ""

$passed = 0
$failed = 0
$failed_cases = @()

# 遍历所有测试用例
foreach ($test in $TEST_CASES) {
    Write-Host "正在测试: $($test.Name)" -ForegroundColor Yellow
    
    # 1. 生成临时main.cpp（使用单引号替换，避免转义问题）
    $main_content = Get-Content $MAIN_FILE -Raw
    $new_main_content = $main_content -replace 'std::string code = ".*?";', "std::string code = @`"`n$($test.Code)`n`"@;"
    Set-Content $TEMP_MAIN_FILE -Value $new_main_content -Encoding UTF8
    
    # 2. 编译
    Write-Host "  正在编译..."
    $compile_output = & cmd /c $COMPILER_CMD 2>&1
    if ($LASTEXITCODE -ne 0) {
        Write-Host "  ❌ 编译失败" -ForegroundColor Red
        $compile_output | Out-File "$TEST_RESULTS_DIR\$($test.Name)_compile_error.txt"
        $failed++
        $failed_cases += $test.Name
        continue
    }
    
    # 3. 运行程序并捕获输出
    Write-Host "  正在运行..."
    $actual_output = & .\mycompiler.exe 2>&1
    $actual_output_str = $actual_output -join "`n"
    
    # 4. 保存实际输出
    $actual_output_str | Out-File "$TEST_RESULTS_DIR\$($test.Name)_actual.txt" -Encoding UTF8
    $test.ExpectedOutput | Out-File "$TEST_RESULTS_DIR\$($test.Name)_expected.txt" -Encoding UTF8
    
    # 5. 对比输出（忽略行尾空格和换行差异）
    $actual_lines = $actual_output_str -split "`n" | ForEach-Object { $_.TrimEnd() }
    $expected_lines = $test.ExpectedOutput -split "`n" | ForEach-Object { $_.TrimEnd() }
    
    if ($actual_lines -join "`n" -eq $expected_lines -join "`n") {
        Write-Host "  ✅ 测试通过" -ForegroundColor Green
        $passed++
    } else {
        Write-Host "  ❌ 测试失败" -ForegroundColor Red
        Write-Host "    实际输出已保存到: $TEST_RESULTS_DIR\$($test.Name)_actual.txt"
        Write-Host "    预期输出已保存到: $TEST_RESULTS_DIR\$($test.Name)_expected.txt"
        $failed++
        $failed_cases += $test.Name
    }
    
    Write-Host ""
}

# 恢复原始main.cpp
Copy-Item "$MAIN_FILE.bak" $MAIN_FILE -Force
Remove-Item "$MAIN_FILE.bak" -Force
Remove-Item $TEMP_MAIN_FILE -Force
Remove-Item "mycompiler.exe" -Force -ErrorAction SilentlyContinue

# 生成测试报告
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "            测试结果汇总" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "总测试用例数: $($TEST_CASES.Count)"
Write-Host "通过: $passed" -ForegroundColor Green
Write-Host "失败: $failed" -ForegroundColor Red
Write-Host ""

if ($failed -gt 0) {
    Write-Host "失败的测试用例:" -ForegroundColor Red
    foreach ($case in $failed_cases) {
        Write-Host "  - $case" -ForegroundColor Red
    }
    Write-Host ""
    Write-Host "请查看 $TEST_RESULTS_DIR 目录下的输出文件进行对比调试。"
} else {
    Write-Host "🎉 所有测试用例全部通过！" -ForegroundColor Green
}

Write-Host ""
Write-Host "测试完成。"