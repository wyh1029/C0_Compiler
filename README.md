# C0_Compiler

## 一．需求说明

### 1．文法说明

＜加法运算符＞ ::= +｜-

＜乘法运算符＞  ::= *｜/

＜关系运算符＞  ::=  <｜<=｜>｜>=｜!=｜==

＜字母＞   ::= ＿｜a｜．．．｜z｜A｜．．．｜Z

＜数字＞   ::= ０｜＜非零数字＞

＜非零数字＞  ::= １｜．．．｜９

＜字符＞    ::=  '＜加法运算符＞'｜'＜乘法运算符＞'｜'＜字母＞'｜'＜数字＞'

＜字符串＞   ::=  "｛十进制编码为32,33,35-126的ASCII字符｝"

＜程序＞    ::= ［＜常量说明＞］［＜变量说明＞］{＜有返回值函数定义＞|＜无返回值函数定义＞}＜主函数＞

＜常量说明＞ ::=  const＜常量定义＞;{ const＜常量定义＞;}

＜常量定义＞   ::=   int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞}

​                               | char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}

＜无符号整数＞  ::= ＜非零数字＞｛＜数字＞｝| 0

＜整数＞        ::= ［＋｜－］＜无符号整数＞

＜标识符＞    ::=  ＜字母＞｛＜字母＞｜＜数字＞｝

＜声明头部＞   ::=  int＜标识符＞ |char＜标识符＞

＜变量说明＞  ::= ＜变量定义＞;{＜变量定义＞;}

＜变量定义＞  ::= ＜类型标识符＞(＜标识符＞|＜标识符＞'['＜无符号整数＞']'){,(＜标识符＞|＜标识符＞'['＜无符号整数＞']' )} //＜无符号整数＞表示数组元素的个数，其值需大于0

＜常量＞   ::=  ＜整数＞|＜字符＞

＜类型标识符＞      ::=  int | char

＜有返回值函数定义＞  ::=  ＜声明头部＞'('＜参数表＞')' '{'＜复合语句＞'}'

＜无返回值函数定义＞  ::= void＜标识符＞'('＜参数表＞')''{'＜复合语句＞'}'

＜复合语句＞   ::=  ［＜常量说明＞］［＜变量说明＞］＜语句列＞

＜参数表＞    ::= ＜参数＞{,＜参数＞}| ＜空>

＜参数＞    ::=  ＜类型标识符＞＜标识符＞

＜主函数＞    ::= void main'('')''{'＜复合语句＞'}'

＜表达式＞    ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}   //[+|-]只作用于第一个<项>

＜项＞     ::= ＜因子＞{＜乘法运算符＞＜因子＞}

＜因子＞    ::= ＜标识符＞｜＜标识符＞'['＜表达式＞']'|'('＜表达式＞')'｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞         

＜语句＞    ::= ＜条件语句＞｜＜循环语句＞| '{'＜语句列＞'}'| ＜有返回值函数调用语句＞;  |＜无返回值函数调用语句＞;｜＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;｜＜空＞;|＜情况语句＞｜＜返回语句＞;

＜赋值语句＞   ::=  ＜标识符＞＝＜表达式＞|＜标识符＞'['＜表达式＞']'=＜表达式＞

＜条件语句＞  ::=  if '('＜条件＞')'＜语句＞

＜条件＞    ::=  ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞ //表达式为0条件为假，否则为真

＜循环语句＞   ::=  while '('＜条件＞')'＜语句＞

＜情况语句＞  ::=  switch '('＜表达式＞')' '{'＜情况表＞＜缺省＞ '}'

＜情况表＞   ::=  ＜情况子语句＞{＜情况子语句＞}

＜情况子语句＞  ::=  case＜常量＞：＜语句＞

＜缺省＞   ::=  default : ＜语句＞|＜空＞

＜有返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'

＜无返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')'

＜值参数表＞   ::= ＜表达式＞{,＜表达式＞}｜＜空＞

＜语句列＞   ::= ｛＜语句＞｝

＜读语句＞    ::=  scanf '('＜标识符＞{,＜标识符＞}')'

＜写语句＞    ::= printf '(' ＜字符串＞,＜表达式＞ ')'| printf '('＜字符串＞ ')'| printf '('＜表达式＞')'

＜返回语句＞   ::=  return['('＜表达式＞')'] 

 

附加说明：

（1）char类型的变量或常量，用字符的ASCII码对应的整数参加运算

（2）标识符区分大小写字母

（3）写语句中，字符串原样输出，单个char类型的变量或常量输出字符，其他表达式按整型输出

（4）情况语句中，switch后面的表达式和case后面的常量只允许出现int和char类型；每个情况子语句执行完毕后，不继续执行后面的情况子语句

（5）数组的下标从0开始

### 2．目标代码说明

32位mips汇编程序。用到的指令包括(部分)：

| 指令       | 助记符  | 示例                |
| ---------- | ------- | ------------------- |
| 加         | addu    | add $1,$2,$3        |
| 立即数加   | addi    | addi $1,   $2, 1    |
| 立即数减   | subi    | subi $1,   $2, 1    |
| 减         | subu    | sub $1,$2,$3        |
| 乘         | mul     | mult $1, $2, $3     |
| 除         | div     | div  $1, $2, $3     |
| 取字       | lw      | lw $t,   4($s)      |
| 存字       | sw      | sw $t,   offset($s) |
| 条件分支   | beq     | beq $s,   $t, label |
| 条件分支   | bne     | bne $s,   $t, label |
| 无条件跳转 | j       | j label             |
| 无条件跳转 | jal     | jal label           |
| 跳转并返回 | jr      | Ja $ra              |
| 系统调用   | syscall | syscall             |
| 移位       | sll     | sll $1, $2, 2       |
| 取地址     | la      | la $v0, $s0         |
| 移动值     | move    | move $t0, $t0       |

 

### 3. 优化方案*

1. 缩短寄存器使用周期。

2. 对对于生成没有影响的代码进行删除。

3. 调用函数时仅对使用中的变量进行存储。

## 二．详细设计

### 1．程序结构

本程序采用c语言进行编写，只存在一个.c文件，程序从main函数开始，调用语法分析程序，语法分析程序从词法分析程序中获得读入的字符类别并进行语法分析，在分析的同时建立符号表并生成中间代码；如果发生错误，则输出错误信息并进行跳读。然后调用生成目标代码程序，先调用.data段生成程序，然后调用.text段生成程序，然后即可输出目标代码。最后进行优化，调用不同的优化程序对目标代码进行优化，然后输出优化后的目标代码程序。

### 2．类/方法/函数功能

#### **2.1 main()****函数**

获取文件路径，如果路径不正确则报错，退出程序，如果正确则进入分析程序。将各种生成的代码输出到指定文件。

#### **2.2** **词法分析函数**

##### 2.2.1 word_Recongize(FILE* fp);

​        根据读入的字符分析其属于什么词法成分。

##### 2.2.2 int isletter(char c);

​        判断一个字符是否是字母，字母包括A-Z, a-z, _;

##### 2.2.3 int isnumber(char c);

​        判断一个字符是否是数字，数字包括0-9；

##### 2.2.4 int isrightstring(char* token);

判断字符串中的字符是否是要求中的字符，即ASCII码在32，33，35-127之

间；

##### 2.2.5 int isrightchar(char c);

​        判断字符是否是合法字符，即字母或数字或加法或乘法标识符；

##### 2.2.6 int getwordSym(char* token);

​        判断字符是否是保留字，如果是的话就返回序号，如果不是的话就返回0；

#### **2.3** **语法分析函数**

#####  2.3.1 void setence_Recongize();

​                 语法分析程序，采用递归子程序分析法；

##### 2.3.2 int const_dcl(int sym, int level);

​                 常量声明分析程序；

##### 2.3.3 int const_def(int sym, int level);

​        常量定义分析程序；

##### 2.3.4 int int_var_dcl(int sym, int level);

​        数字返回值函数声明；

##### 2.3.5 int char_var_dcl(int sym, int level);

​        字符返回值函数声明；

##### 2.3.6 int int_re_func_def(int sym, int level);

​        数字返回值函数定义；

##### 2.3.7 int char_re_func_def(int sym, int level);

​        字符返回值函数定义；

##### 2.3.8 int no_re_func_def(int sym, int level);

​        无返回值函数定义；

##### 2.3.9 int para_list(int sym, int level);

​        参数表分析；

##### 2.3.10 int parameter(int sym, int level);

​        参数分析；

##### 2.3.11 int main_func_def(int sym, int level);

​		Main函数定义分析；

##### 2.3.12 int com_statements(int sym, int level);

​        复合语句分析；

##### 2.3.13 int statements(int sym); int statement(int sym);

​        语句分析；

##### 2.3.14 

int if_statement(int sym);   int while_statement(int sym); 

int func_call(int sym);            int func_call_factor(int sym); 

int ass_statement(int sym);    int scanf_statement(int sym);

int printf_statement(int sym); int switch_statement(int sym);

int return_statement(int sym); int condition(int sym);

int case_table(int sym, char* end_label, int switch_type);

int default_statement(int sym);

各类不同的语句分析；

##### 2.3.15

int value_para_list(int sym, int paranum, int index);

值参数表分析；

##### 2.3.16

int expression(int sym);       int term(int sym);    int factor(int sym);

表达式分析；

##### 2.3.17

int isfirst_statement(int sym);      int isop(int sym);

int isfirst_default(int sym);     int isconst(int sym);

各类语句的first集合。

#### **2.4** **中间代码生成分析**

##### 2.4.1 char * get_tReg();

获得一个空的t寄存器

##### 2.4.2 char * get_sReg();

获得一个空的s寄存器

##### 2.4.3 void free_tReg(char reg[]);

释放一个不用的t寄存器；

##### 2.4.4 void free_sReg(char reg[]);

释放一个不用的s寄存器；

##### 2.4.5 void Deal();            

表达式处理；

##### 2.4.6 int getSymIndex_intercode(char * name);

索引id在符号表中的位置；

##### 2.4.7 int id_use_find(char * name);

使用id时在符号表中查找是否定义；

#### **2.5** **目标代码生成**

##### 2.5.1 void MIPS_create_data();

 .data段目标代码生成；

##### 2.5.2 void MIPS_create_text();

.text段目标代码生成；

##### 2.5.3 int div_factor(char code[]);

代码个数分离；

##### 2.5.4 char * getop(int sym);

获得条件中的字符；

##### 2.5.5 int getSymIndex_mips(char * name);

获得id在符号表中的位置；

##### 2.5.6 char * load_reg(char* idname);

将值从相应的位置取出；

##### 2.5.7 char * save_reg(char * idname, char * value);

将值存入相应的位置；

##### 2.5.8 int FindRegType(char Reg[20], int i);

获取寄存器中所存值的种类；

####  **2.6** **优化**

##### 2.6.1 void delete_move();

删除形如move $t0, $t0的表达式。

### 3．调用依赖关系

### ![img](file:///C:/Users/wangy/AppData/Local/Temp/msohtmlclip1/01/clip_image002.jpg)

### 4．符号表管理方案

\#define IDENTNAMESIZE 100

struct SymRec

{

​    char name[IDENTNAMESIZE];     //标识符名称

​    int kind;            //标识符种类：0-Const、1-Var、2-Function、3-Parameter

​    int type;            //标识符类型：0-Void、1-Int、2-Char、3-String

​    int isarray;     //是否为数组

​    int lev;         //标识符层级：0-全局、n-第n个函数内

​    int value;           //标识符内容/数组大小/该函数名的层级/第？个字符串

​    int index;           //内存栈中地址

};

### 5．存储分配方案

假设在main函数中调用名为f的函数，其栈式存储如下：

|      栈顶       |
| :-------------: |
|  f.局部数据区   |
|    f.参数区     |
|    f.prevabp    |
|      f.ret      |
|      f.abp      |
|    Main.abp     |
| Main.局部数据区 |
|  Main.prevabp   |
|    Main.ret     |
|    main.abp     |
|   函数声明区    |
|   全局变量区    |
|   全局常量区    |

 

### 6. 四元式设计*

| 四元式                               | 含义                                   |
| ------------------------------------ | -------------------------------------- |
| label:                               | 标签                                   |
| ret.                                 | 函数返回                               |
| $t0                                  | 根据寄存器值判断条件，若为1则真为0则假 |
| GOTO   label                         | 跳转到标签所在地址                     |
| scan   idname                        | 读入键盘输入存储到相应的标识符中       |
| push   parameter                     | 函数传参                               |
| call   func                          | 调用函数                               |
| print   string/print expr            | 输出字符串/表达式                      |
| ret   expr                           | 函数返回值                             |
| int   func()/char func()/void func() | int/char/void   函数定义               |
| a   = b                              | 赋值语句                               |
| a   == b                             | 等于条件判断                           |
| a   >= b                             | 大于等于条件判断                       |
| a   <= b                             | 小于等于条件判断                       |
| a   != b                             | 不等于条件判断                         |
| a   < b                              | 小于条件判断                           |
| a   > b                              | 大于条件判断                           |
| BZ   label                           | 若条件为假则跳转                       |
| BNZ   label                          | 若条件为真则跳转                       |
| a   = b + c                          | 加减乘除运算                           |
| a   = b - c                          |                                        |
| a   = b * c                          |                                        |
| a   = b / c                          |                                        |

 

### 7. 目标代码生成方案*

​        

| 四元式                                     | 目标代码                                                     |
| ------------------------------------------ | ------------------------------------------------------------ |
| label:                                     | label:                                                       |
| ret.                                       | jr  $ra                                                      |
| $t0   BZ   label                           | $t0   beq   $t0,$0,label                                     |
| $t0   BNZ   label                          | $t0   bne   $t0,$0,label                                     |
| GOTO   label                               | j   label                                                    |
| scan   idname                              | li   $v0,5    //int   syscall   li   $v0,12    //char   syscall |
| push   parameter                           | subi   $sp, $sp, 4   sw   $t0,0($sp)                         |
| call   func                                | sw   $t0,0($sp)   ……（按需存储）……   sw   $t9,0($sp)   sw    $ra, 0($sp)   sw    $s7, 0($sp)   jal   %s   move   $sp, $s7   lw   $t0,0($sp)   ……（按需取回）……   lw   $t9,0($sp)   lw   $ra, 0($sp)   lw   $s7, 0($sp) |
| print   string/print expr                  | (按照不同的print类型将print的数值取出)   li   $v0,1    //int   syscall   li   $v0,11    //char   syscall   li   $v0,4    //string   syscall |
| ret   expr                                 | move   $v0, $t0   jr   $ra                                   |
| int   func()/char func()/void func()       | func:   subi   $sp, $sp, 0//符号栈   addi   $s7, $sp, 0//符号栈 |
| a   = b                                    | move   $t0, $t1                                              |
| a   == b   BZ   label   (其它条件判断同理) | 将a，b值分别存入$t0,$t1   bne   $t0, $t1, label              |
| a   = b + c                                | 将b，c值分别存入$t0,$t1   addu/subu/mul/div   $t2, $t0, $t1   将$t2值存入a |
| a   = b - c                                |                                                              |
| a   = b * c                                |                                                              |
| a   = b / c                                |                                                              |

 

### 8. 优化方案*

   删除形如move $t0,$t0 的无实际意义的指令。

### 9. 出错处理

**9.1** **文件错误处理**

​        0： 文件不存在；

**9.2** **词法分析错误处理**

​        1： 存在0开头的数字；

​        2： 字符串或字符缺少后一半引号；

​        3： 字符串中存在非法字符；

​        4： 字符的‘’中存在非法字符或‘’中的字符数量大于1；

​        5： ！单独出现；

​        6： 出现不该出现的符号，例如%&等；

**9.3** **语法分析错误处理**

​        7： 在跳过过程中，跳到文档最后依然没有匹配到对应字符；

​        8： 语法分析中匹配到不正确的成分；

**9.4** **语义分析错误处理**

​        9： 赋值语句中出现在等号左侧的不是变量；

​        10：函数调用传参的时候类型不匹配；

​        11：函数调用传参的时候个数不匹配；

​        12：switch语句与case语句的类型不匹配；

​		13：赋值语句等号两边类型不匹配；

​		14：条件中条件表达式不是整型；

​		15：因子为函数调用时调用了没有返回值的函数；

​		16：函数定义时的类型与return时的类型不匹配；

​		17：标识符重复定义；

​		18：使用了没有定义的标识符；

​		19：数组使用时下标越界；

**9.5** **寄存器使用错误处理**

​         20：t0~t9寄存器均在使用时申请新的寄存器；

​         21：释放了未使用的t0~t9寄存器；

​         22：s0~s7寄存器均在使用时申请新的寄存器；

​         23：释放了未使用的s0~s7寄存器； 

## 三．操作说明

### 1．运行环境

编译代码运行环境：VS2017；

生成MIPS代码运行环境：MARS4.5

### 2．操作步骤

1. 打开工程文件；

2. 创建一个新的工程文件；

3. 将原工程文件中的代码复制粘贴到新的工程文件中；

4. 点击运行；

5. 在控制台输入测试代码的路径；

6. 按照运行提示输入所需内容，即可查看输出。

## 四．测试报告

### 1．测试程序及测试结果

**1.1 test scanandprint**

void test_printstring(){

​        printf("ABCDEFGhijklmnOPQrstUVWxyz!@#$%^&*(){}");   

}

void test_printstringandexpr(int i){

​        printf("the result is :",i);

}

void test_printexpr(char c){

​        printf(c);

}

void main(){

​        int a;

​        char b;

​        scanf(a,b);

​        test_printstring();

​        test_printstringandexpr(a);

​        test_printexpr(b);

}

输入：1 a

输出：ABCDEFGhijklmnOPQrstUVWxyz!@#$%^&*(){} the result is :1a

**1.2 test ifandexpr**

void main(){

​        int a;

​        int expr;

​        scanf(a);

​        if (a==0){

​                 expr = a+1*2+3-4+(5/5)-(1*3)+100-99;

​                 printf(expr);

​        }

​        if (a>0){

​                 if (a>100){

​                         expr = (a-100)/100;

​                         printf(expr);    

​                 }

​                 if (a<100){

​                         expr = a+100;

​                         printf(expr);

​                 }

​                 if (a == 100){

​                         {{{{{;}}}}}

​                         expr = -a+100+a;

​                         printf(expr);

​                 }

​        }

​        if (a<0){

​                 printf(a*(-1)+a+a);

​        }

}

输入：1          100          -90

输出：1          100          -90

**1.3 test switchandwhile**

void main(){

​        int a;

​        int sum;

​        char c;

​        scanf(a);

​        scanf(c);

​        while(a<5){

​        switch(a){

​                 case 0: sum = a;

​                 case 1: sum = sum+1;

​                 case 2: sum = sum +2;

​                 case 3: sum = sum + 3;

​                 case 4: sum = sum +4;

​                 default:{

​                         ;

​                 }

​        }

​        a = a+1;

​        }

​        printf("sum is :",sum);

​        switch(c){

​                 case '+':   printf("add");

​                 case '-':    printf("sub");

​                case '*':  printf("mul");

​                 case '/': printf("div");

​                 default: printf("error");

​        }

}

输入：-100                     +

输出：sum is :10   add

**1.4 test global**

const int num_1 = -1,num_2 = -2;

conost char char_a = 'a', char_z = 'z',char_A = 'A',chat_Z ='Z';

void change(){

​        const int num_1 =1;

​        const char char_a = 'A';

​        printf(num_1); 

​        printf(char_A);

}

void main(){

​        printf(num_1);

​        printf(char_a);

​        change();

}

输出：-1a1A

**1.5 test digui**

int k[10];

int digui(int i){

​        if (i>1)

​                 return i*digui(i-1);

​        if (i==1)

​                 return 1;

}

void main(){

​        int a;

​        scanf(a);

​        digui(a);

​        printf(digui(a));

​        a=0;

​        while(a<10){

​        k[a] = a;

​        printf(k[a]);

​        }

}

输入：6

输出：7200123456789

1.6 test6

void function(int a,char b){}

 

void main(){

​        int a;

​        char b

​        scanf(a,b);

​        function(b);

​        function(b,a);

}

输出：参数个数不对

​                 参数类型不对

1.7 test7

int n[10];

 

void main(){

​        const char c = 'c';

​        n[20]=1;

​        n[2] = c;

​        n[0] = 3;

​        n[9] = n[20];

​        n[8] = 1+n[6];

​        if (c){

​                 printf(c);

​        }

输出：条件应为整型

1.8 test8

int digui(int i){

​        if (i>1)

​                 return i*digui(i-1);

​        if (i==1)

​                 return 1;

}

void main(){

​        int a;

​        scanf(a);

​        digui(a);

​        printf(digui(a));

​        a=0;

​        while(a<10){

​        k[a] = a;

​        printf(k[a]);

​        }

}

输出：未定义k

1.9 test9

void fun(){

​        if (a!00000000)

​        return 0;

}

void main(){

​        a = fun();

}

输出：非法字符

​                 前置0

​                 RETURN error

​                 调用void函数

1.10 test10

void main(){

​        int int;

}

输出：应该是idname；

### 2．测试结果分析

##### 2.1 正确测试程序对文法的覆盖

​        覆盖语句：if，while，switch-case-default，printf，scanf，return；

​        覆盖数据结构：数字，字符，数组；

​        覆盖函数类型：void，int，char；

​        覆盖变量区域：全局变量，局部变量；

##### 2.2 错误测试程序对错误类型的覆盖（类型见上）

​        词法错误：1，2， 4，5，6

​        语法错误：7，8

​        语义错误：9，10，11，12，13，14，1516，17，18，19