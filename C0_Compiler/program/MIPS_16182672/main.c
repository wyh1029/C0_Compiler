#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ListMax 40
#define IDENTNAMESIZE 100
#define SYMLISTMAX 1000
#define CODELISTMAX 5000
#define MIPSDATAMAX 1000
#define MIPSTEXTMAX 10000
#define MAXSREG 6
#define MAXTREG	9

////////////词法分析///////////////////////
int word_Recongize();
int isletter(char c);
int isnumber(char c);
int isrightstring(char* token);
int isrightchar(char c);
int getwordSym(char* token);
//词法分析错误处理
int SymError(char c);
int skipsym(int n);

///////////语法分析///////////////////////
void setence_Recongize();
//常量说明
int const_dcl(int sym, int level);
int const_def(int sym, int level);
//变量说明
int int_var_dcl(int sym, int level);
int char_var_dcl(int sym, int level);
//有返回值函数
int int_re_func_def(int sym, int level);
int char_re_func_def(int sym, int level);
//无返回值函数
int no_re_func_def(int sym, int level);
int para_list(int sym, int level);
int parameter(int sym, int level);
//main函数
int main_func_def(int sym, int level);
//复合语句
int com_statements(int sym, int level);
//语句列
int statements(int sym);
int isfirst_statement(int sym);
//语句
int statement(int sym);
//各类语句
int if_statement(int sym);
int while_statement(int sym);
int func_call(int sym);
int func_call_factor(int sym);
int ass_statement(int sym);
int scanf_statement(int sym);
int printf_statement(int sym);
int switch_statement(int sym);
int return_statement(int sym);
//条件
int condition(int sym);
int isop(int sym);
//值参数表
int value_para_list(int sym, int paranum, int index);
//表达式
int expression(int sym);
int term(int sym);
int factor(int sym);
int isfirst_expression(int sym);
//switch
int case_table(int sym, char* end_label, int switch_type);
int default_statement(int sym);
int isfirst_default(int sym);
int isconst(int sym);
//语法分析错误处理
int extra_stc(int sym);
//语义分析
int id_def_check(char *idname, int level);
char* get_kind(int i);				//符号表种类

//中间代码生成
char * get_tReg();
char * get_sReg();
void free_tReg(char reg[]);
void free_sReg(char reg[]);

void Deal();		//表达式处理

//目标代码生成
void MIPS_create_data();
void MIPS_create_text();
int div_factor(char code[]);
char * getop(int sym);
int getSymIndex_mips(char * name);
int getSymIndex_intercode(char * name);
int id_use_find(char * name);
char * load_reg(char* idname);
char * save_reg(char * idname, char * value);
int FindRegType(char Reg[20], int i);

//优化
void delete_move();



char* wordList[ListMax] =  {"NULL","const","int","char","void","main",
							"if","while","switch","case","default",
							"scanf","printf","return",
							"+","-","*","/",
							"<","<=",">",">=","!=","==",
							":",";",",","=","'","\"",
							"(",")","[","]","{","}",
							"ident","number","character","string"};

char* wordListSym[ListMax] = {"NULL","constSym","intSym","charSym","voidSym","mainSym",
							"ifSym","whileSym","switchSym","caseSym","defaultSym",
							"scanfSym","printfSym","returnSym",
							"addSym","subSym","mulSym","divSym",
							"ltSym","lteSym","mtSym","mteSym","neqSym","eqSym",
							"colSym","semSym","comSym","assSym","sqmSym","dqmSym",
							"lspSym","rspSym","lmpSym","rmpSym","lbpSym","rbpSym",
							"idSym","numSym","charSym","strSym"};


struct SymRec
{
	char name[IDENTNAMESIZE];		//标识符名称
	int kind;			//标识符种类：0-Const、1-Var、2-Function、3-Parameter
	int type;			//标识符类型：0-Void、1-Int、2-Char、3-String
	int isarray;		//是否为数组
	int lev;			//标识符层级：0-全局、n-第n个函数内
	int value;			//标识符内容/数组大小/该函数名的层级
	int index;			//内存栈中地址
};

char token[100] = { 0 };     //词法分析，读入的token
int NUM0 = 0;				//词法分析，数值
int line = 1;				//总行数
int ERROR_num = 0;			//总错误数
FILE *fp;			//文件指针
struct SymRec SymList[SYMLISTMAX];		//符号表
int SymList_top = 0;		//符号表位置

int FuncStackTop[100] = { 0 };			//函数栈
int Level_top = 0;				//第？个函数
int currentlev;
int FuncParaNum[100] = { 0 };		//第？个函数有？个参数
char CodeList[CODELISTMAX][50];			//中间代码表
int CodeList_top;
int expression_type;	//表达式类型
int returnflag;			//return检查

char StrList[400][200] = { 0 };		//字符串表
int StrList_top = 0;				//字符串序号

int LabelNum = 0;			//标签序号递增

char OPList[200][20] = { 0 };			//运算列表
int OPList_top;						

char MIPS_data[MIPSDATAMAX][200];		//mips代码数据段
int MIPS_data_top;

char MIPS_text[MIPSTEXTMAX][200];		//代码段
int MIPS_text_top;

int tReg[10] = { 0 };			//寄存器
int sReg[8] = { 0 };

int r2type;
int main() {
	int i;
	char filename[100] = {0};
	char c = 0;
	scanf("%s",filename);
	fp = fopen(filename, "r");
	FILE * out;
	if (fp == NULL)
		printf("error 0: can't open the file!\n");
	else {
		setence_Recongize();
	}
	if (ERROR_num == 0) {
		printf("compile successful!\n");
		out = fopen("intercode.txt", "w");
		for (i = 0; i < CodeList_top; i++)
		{
			fprintf(out, "%s\n", CodeList[i]);
		}
		fclose(out);
		out = fopen("intercodeafter.txt", "w");
		for (i = 0; i < CodeList_top; i++)
		{
			fprintf(out, "%s\n", CodeList[i]);
		}
		fclose(out);
		printf("create inter code successful!\n");
		MIPS_create_data();
		MIPS_create_text();
		out = fopen("firstcode.txt", "w");
		for (i = 0; i < MIPS_data_top; i++)
		{
			fprintf(out, "%s\n", MIPS_data[i]);
		}
		for (i = 0; i < MIPS_text_top; i++)
		{
			fprintf(out, "%s\n", MIPS_text[i]);
		}
		fclose(out);
		printf("create MIPS code successful!\n");
		delete_move();
		out = fopen("secondcode.txt", "w");
		for (i = 0; i < MIPS_data_top; i++)
		{
			fprintf(out, "%s\n", MIPS_data[i]);
		}
		for (i = 0; i < MIPS_text_top; i++)
		{
			fprintf(out, "%s\n", MIPS_text[i]);
		}
		fclose(out);
		printf("create second code successful!\n");
	}
	else {
		printf("Totally there are %d errors in this project!\nExit Compile!\n",ERROR_num);
	}
	return 0;
}

int word_Recongize() {
	char c;
	int i = 0;
	int j = 0;
	int num = 0;

	c = fgetc(fp);
	if (c == '\n') {
		line++;
		return word_Recongize();
	}
	//\t ' '什么都不做
	else if ((c == '\t') || (c == ' '))
	{
		//fseek(fp,1,SEEK_CUR);
		return word_Recongize();
	}
	//字母
	else if (isletter(c)) {
		i = 0;
		token[i++] = c;
		c = fgetc(fp);
		while ((isletter(c)) || (isnumber(c))) {
			token[i++] = c;
			c = fgetc(fp);
		}
		token[i] = '\0';
		if (c != EOF)
			fseek(fp, -1, SEEK_CUR);
		if (getwordSym(token)) {
		//	printf("%s %s\n", wordListSym[getwordSym(token)], token);
			return getwordSym(token);
		}
		else {
			//printf("idSym %s\n", token);
			return 36;
		}

	}
	//数字
	else if (isnumber(c)) {
		if (c != '0') {
			num = c - '0';
			c = fgetc(fp);
			while (isnumber(c)) {
				num = num * 10 + c - '0';
				c = fgetc(fp);
			}
			if (c != EOF)
				fseek(fp, -1, SEEK_CUR);
		//	printf("numSym %d\n", num);
			NUM0 = num;
		//	token[1] = '\0';
			return 37;
		}
		else if (c == '0') {
			c = fgetc(fp);
			if (isnumber(c)) {
				fseek(fp, -1, SEEK_CUR);
				return SymError('0');
			}	
			else {
				fseek(fp, -1, SEEK_CUR);
		//		printf("numSym %d\n", 0);
				NUM0 = 0;
				token[1] = '\0';
				return 37;
			}
		}
	}
	//字符串
	else if (c == '\"') {
		i = 0;
		//printf("dqmSym \"\n");
		token[i++] = '\"';
		c = fgetc(fp);
		while (c != '\"') {
			if (c != EOF) {
				token[i++] = c;
				c = fgetc(fp);
			}
			else
			{
				return SymError('#');
			}
		}
		token[i++] = '\"';
		token[i] = '\0';
		if (isrightstring(token)) {
		//	printf("strSym %s\n", token);
			return 39;
		}
		else
			return SymError('\"');
	}
		//字符
		else if (c == '\'') {
			token[0] = '\'';
			c = fgetc(fp);
			token[1] = c;
			c = fgetc(fp);
			if (c == '\'') {
				token[2] = '\'';
				token[3] = '\0';
				if (isrightchar(token[1])) {
		//			printf("charSym %s\n", token);
					return 38;
				}
				else {
					fseek(fp, -1, SEEK_CUR);
					return SymError('\'');
				}
					
			}
			else if (c == EOF)
				return SymError('#');
			else
				return SymError('\'');
		}
		//符号:;,
	else if (c == ':') {
		return 24;
	}
	else if (c == ';') {
		return 25;
	}
	else if (c == ',') {
		return 26;
	}
		//符号+-*/
	else if (c == '+'){
		return 14;
	}
	else if (c == '-'){
		return 15;
	}
	else if (c == '*'){
		return 16;	
	}
	else if (c == '/') {
		return 17;
	}
		//符号> >= < <= != == = 
	else if (c == '<') {
		c = fgetc(fp);
		if (c == EOF) {
			return 18;
		}
		else if (c == '=') {
			return 19;		
		}		
		else {
			fseek(fp, -1, SEEK_CUR);
			return 18;
		}
	}
	else if (c == '>') {
		c = fgetc(fp);
		if (c == EOF) {
			return 20;
		}	
		else if (c == '=') {
			return 21;
		}
		else {
			fseek(fp, -1, SEEK_CUR);
			return 20;
		}
	}
	else if (c == '=') {
		c = fgetc(fp);
		if (c == EOF) {
			return 27;
		}
		else if (c == '=') {
			return 23;
		}
		else {
			fseek(fp, -1, SEEK_CUR);
			return 27;
		}
	}
	else if (c == '!') {
		if ((c = fgetc(fp)) == '=') {
			return 22;
		}
		else {
			fseek(fp, -1, SEEK_CUR);
			return SymError('!');
		}
	}
		//符号(){}[]
	else if (c == '('){
		return 30;
	}
	else if (c == ')') {
		return 31;
	}
	else if (c == '[') {
		return 32;
	}
	else if (c == ']') {
		return 33;
	}
	else if (c == '{') {
		return 34;
	}
	else if (c == '}') {
		return 35;
	}
	else if (c == EOF) {
		return 0;
	}
	else
		return SymError(c);
}

int isletter(char c) {
	if ((c >= 'A') && (c <= 'Z'))
		return 1;
	else if ((c >= 'a') && (c <= 'z'))
		return 1;
	else if (c == '_')
		return 1;
	else
		return 0;
}

int isnumber(char c) {
	if ((c >= '0') && (c <= '9'))
		return 1;
	else
		return 0;
}

int isrightstring(char* token)
{
	int length = strlen(token);
	int i = 0;
	while (i < length) {
		if ((token[i] >= 32) && (token[i] <= 126))
			i++;
		else
			return 0;
	}
	return 1;
}

int isrightchar(char c)
{

		if (c == '+')
			return 1;
		else if (c == '-')
			return 1;
		else if (c == '*')
			return 1;
		else if (c == '/')
			return 1;
		else if (isnumber(c))
			return 1;
		else if (isletter(c))
			return 1;
		else
			return 0;
	return 1;
}

int getwordSym(char* token) {
	int i = 0;
	for ( i = 1; i < 14; i++)
	{
		if (!strcmp(token, wordList[i])) {
			return i;
		}
	}
	return 0;
}

int SymError(char c) {
	ERROR_num++;
//	printf("there is an error in symbol recongize!------------------------------\n");
	if (c == '0') {
		printf("error 1: the number start with 0 in %d line!\n",line);
		return skipsym(1);
	}
	else if (c == '#') {
		printf("error 2: the string can't end in %d line!\n", line);
		exit(-1);
	}
	else if (c == '\"') {
		printf("error 3: the string have a unillgal symbol in %d line!\n", line);
		return skipsym(0);
	}
	else if (c == '\'') {
		printf("error 4: the character have a problem in %d line!\n", line);
		return skipsym(3);
	}
	else if (c == '!') {
		printf("error 5: the symbol\"!\"shouldn't appear only in %d line!\n", line);
		return skipsym(0);
	}
	else {
		printf("error 6: the symbol \"%c\"shouldn't appear in %d line!\n", c, line);
		return skipsym(0);
	}
}

int skipsym(int n) {
	char c;
	switch (n) {
	case 0: {
		return word_Recongize();
	}
	case 1: {
		c = fgetc(fp);
		while (c == '0')
			c = fgetc(fp);
		fseek(fp, -1, SEEK_CUR);
		return word_Recongize();
	}
	case 2: {
		c = fgetc(fp);
		while (c != ')')
			c = fgetc(fp);
		fseek(fp, -1, SEEK_CUR);
		return word_Recongize();
	}
	case 3: {
		c = fgetc(fp);
		while (c != '\'')
			c = fgetc(fp);
		return word_Recongize();
	}
	}
}

void setence_Recongize(){
	int sym = 0;
	char c;
	sym = word_Recongize();
	
	//常量说明		
	while (sym == 1) {					//const
		sym = const_dcl(sym, 0);
	}

	while (sym != 0) {	
		//先不分变量声明和返回函数
		if (sym == 2) {					//int
			sym = word_Recongize();
			if (sym == 36) {				//id
				sym = word_Recongize();
				if (sym == 30) {			//(
					Level_top++;
					sym = int_re_func_def(sym, Level_top);
				}
				else if ((sym == 32) ||(sym == 26) || (sym == 25)) {		//,;[
					sym = int_var_dcl(sym, 0);
				}
				else {
					printf("error 8: the symbol is wrong, the right symbol should be ; or , or [ in line %d!\n",line);
					ERROR_num++;
					c = fgetc(fp);
					while (c != ';') {
						if (c == EOF) {
							printf("error 7: Can't find an right symbol before end!\n");
							system("pause");
						}
						else
							c = fgetc(fp);
					}
					//fseek(fp, -1, SEEK_CUR);
					return word_Recongize();
				}
			}
			else {
				printf("error 8: the symbol is wrong, the right symbol should be a idname in line %d!\n", line);
				ERROR_num++;
				c = fgetc(fp);
				while (c != ';') {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						system("pause");
					}
					else
						c = fgetc(fp);
				}
				//fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
		}
		else if (sym == 3) {					//char
			sym = word_Recongize();
			if (sym == 36) {			//id
				sym = word_Recongize();
				if (sym == 30) {		//(
					Level_top++;
					sym = char_re_func_def(sym, Level_top);
				}
				else if ((sym == 32) || (sym == 26) || (sym == 25)) {	//,;[
					sym = char_var_dcl(sym, 0);
				}
				else {
					printf("error 8: the symbol is wrong, the right symbol should be ; or , or [ in line %d!\n", line);
					ERROR_num++;
					c = fgetc(fp);
					while (c != ';') {
						if (c == EOF) {
							printf("error 7: Can't find an right symbol before end!\n");
							system("pause");
						}
						else
							c = fgetc(fp);
					}
					//fseek(fp, -1, SEEK_CUR);
					return word_Recongize();
				}
			}
			else {
				printf("error 8: the symbol is wrong, the right symbol should be a idname in line %d!\n", line);
				ERROR_num++;
				c = fgetc(fp);
				while ((c != ';') && (c !='}')) {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						system("pause");
					}
					else
						c = fgetc(fp);
				}
				//fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
		}

		else if (sym == 4) {				//void
			sym = word_Recongize();
			if (sym == 5) {				//main
				Level_top++;
				sym = main_func_def(sym, Level_top);
			}
			else if (sym == 36) {		//无返回值函数
				Level_top++;
				sym = no_re_func_def(sym, Level_top);
			}
			else {
				printf("error 8: the symbol is wrong, the right symbol should be a idname or main in line %d!\n", line);
				ERROR_num++;
				c = fgetc(fp);
				while (c != '}') {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						exit(0);
					}
						
					else
						c = fgetc(fp);
				}
				//fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
		}
		else {
			sym = extra_stc(sym);
		}
	}
}

int const_dcl(int sym, int level) {
	char c;
	while (sym == 1) {
		sym = word_Recongize();
		sym = const_def(sym, level);
		if (sym == 25) {
		//	printf("this is a const declare!\n");
			sym = word_Recongize();
		}
		else {
			printf("error 8: the symbol is wrong, the right symbol should be ; in line %d!\n", line);
			ERROR_num++;
			c = fgetc(fp);
			while (c!=';') {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			return word_Recongize();
		}
	}
	return sym;
}
int const_def(int sym, int level) {
	int num;
	char idname[IDENTNAMESIZE] = { 0 };
	char c;
	if (sym == 2) {				//int
		//printf("constint\n");
		sym = word_Recongize();
		if (sym == 36) {		//idSym
			strcpy(idname, token);
			sym = word_Recongize();
			if (sym == 27) {		//=
				sym = word_Recongize();
				if (sym == 14) {		//+		
					sym = word_Recongize();
					if (sym == 37) {			//num
						num = NUM0;
					}
					else {
						printf("error 8: the symbol is wrong, the right symbol should be a number in line %d!\n", line);
						ERROR_num++;
						c = fgetc(fp);
						while (c != ';') {
							if (c == EOF) {
								printf("error 7: Can't find an right symbol before end!\n");
								system("pause");
							}
							else
								c = fgetc(fp);
						}
						fseek(fp, -1, SEEK_CUR);
						return word_Recongize();
					}
				}
				else if (sym == 15) {			//-
					sym = word_Recongize();
					if (sym == 37) {			//num
						num = NUM0 * (-1);
					}
					else {
						printf("error 8: the symbol is wrong, the right symbol should be a symbol in line %d!\n", line);
						ERROR_num++;
						c = fgetc(fp);
						while (c != ';') {
							if (c == EOF) {
								printf("error 7: Can't find an right symbol before end!\n");
								system("pause");
							}
							else
								c = fgetc(fp);
						}
						fseek(fp, -1, SEEK_CUR);
						return word_Recongize();
					}
				}
				else if (sym == 37) {			//num
					num = NUM0;
				}
				else {
					printf("error 8: the symbol is wrong, the right symbol should be a number in line %d!\n", line);
					ERROR_num++;
					c = fgetc(fp);
					while (c != ';') {
						if (c == EOF) {
							printf("error 7: Can't find an right symbol before end!\n");
							system("pause");
						}
						else
							c = fgetc(fp);
					}
					fseek(fp, -1, SEEK_CUR);
					return word_Recongize();
				}
				////////////填入符号表////////////
				if (id_def_check(idname, level) == 0) {
					strcpy(SymList[SymList_top].name,idname);
					SymList[SymList_top].kind = 0;
					SymList[SymList_top].type = 1;
					SymList[SymList_top].isarray = 0;
					SymList[SymList_top].lev = level;
					SymList[SymList_top].value = num;
					if (level == 0) {
						SymList[SymList_top].index = 0;
					}
					else {
						SymList[SymList_top].index = FuncStackTop[level];
						FuncStackTop[level] += 4;
					}
					SymList_top++;
				}
				else {
					printf("error 17: the name %s in line %d has defined in symlist!\n", idname, line);
					ERROR_num++;
					c = fgetc(fp);
					while (c != ';') {
						c = fgetc(fp);
					}
					return 25;
				}
				sym = word_Recongize();
				while (sym == 26) {				//,
					sym = word_Recongize();
					if (sym == 36) {			//id
						strcpy(idname, token);
						sym = word_Recongize();
						if (sym == 27) {				//=
							sym = word_Recongize();
							if (sym == 14) {				//+
								sym = word_Recongize();
								if (sym == 37) {
									num = NUM0;
								}
								else {
									printf("error 8: the symbol is wrong, the right symbol should be a number in line %d!\n", line);
									ERROR_num++;
									c = fgetc(fp);
									while (c != ';') {
										if (c == EOF) {
											printf("error 7: Can't find an right symbol before end!\n");
											system("pause");
										}
										else
											c = fgetc(fp);
									}
									fseek(fp, -1, SEEK_CUR);
									return word_Recongize();
								}
							}
							else if (sym == 15) {			//-
								sym = word_Recongize();
								if (sym == 37) {
									num = -1*NUM0;
								}
								else {
									printf("error 8: the symbol is wrong, the right symbol should be a number in line %d!\n", line);
									ERROR_num++;
									c = fgetc(fp);
									while (c != ';') {
										if (c == EOF) {
											printf("error 7: Can't find an right symbol before end!\n");
											system("pause");
										}
										else
											c = fgetc(fp);
									}
									fseek(fp, -1, SEEK_CUR);
									return word_Recongize();
								}
							}
							else if (sym == 37) {			//num
								num = NUM0;
							}
							else {
								printf("error 8: the symbol is wrong, the right symbol should be a number in line %d!\n", line);
								ERROR_num++;
								c = fgetc(fp);
								while (c != ';') {
									if (c == EOF) {
										printf("error 7: Can't find an right symbol before end!\n");
										system("pause");
									}
									else
										c = fgetc(fp);
								}
								fseek(fp, -1, SEEK_CUR);
								return word_Recongize();
							}
							////////////填入符号表////////////
							if (id_def_check(idname, level) == 0) {
								strcpy(SymList[SymList_top].name, idname);
								SymList[SymList_top].kind = 0;
								SymList[SymList_top].type = 1;
								SymList[SymList_top].isarray = 0;
								SymList[SymList_top].lev = level;
								SymList[SymList_top].value = num;
								if (level == 0) {
									SymList[SymList_top].index = 0;
								}
								else {
									SymList[SymList_top].index = FuncStackTop[level];
									FuncStackTop[level] += 4;
								}
								SymList_top++;
							}
							else {
								printf("error 17: the name %s in line %d has defined in symlist!\n", idname, line);
								ERROR_num++;
								c = fgetc(fp);
								while (c != ';') {
									c = fgetc(fp);
								}
								return 25;
							}
							///////////////////
							sym = word_Recongize();
							//printf("const int\n");
						}
						else {
							printf("error 8: the symbol is wrong, the right symbol should be = in line %d!\n", line);
							ERROR_num++;
							c = fgetc(fp);
							while (c != ';') {
								if (c == EOF) {
									printf("error 7: Can't find an right symbol before end!\n");
									system("pause");
								}
								else
									c = fgetc(fp);
							}
							fseek(fp, -1, SEEK_CUR);
							return word_Recongize();
						}
					}
					else {
						printf("error 8: the symbol is wrong, the right symbol should be an idname in line %d!\n", line);
						ERROR_num++;
						c = fgetc(fp);
						while (c != ';') {
							if (c == EOF) {
								printf("error 7: Can't find an right symbol before end!\n");
								system("pause");
							}
							else
								c = fgetc(fp);
						}
						fseek(fp, -1, SEEK_CUR);
						return word_Recongize();
					}
				}
				if (sym == 25) {
					//printf("this is a const declare!\n");
					return 25;
				}
				else {
					printf("error 8: the symbol is wrong, the right symbol should be ; in line %d!\n", line);
					ERROR_num++;
					c = fgetc(fp);
					while (c != ';') {
						if (c == EOF) {
							printf("error 7: Can't find an right symbol before end!\n");
							system("pause");
						}
						else
							c = fgetc(fp);
					}
					fseek(fp, -1, SEEK_CUR);
					return word_Recongize();
				}
			}
			else {
				printf("error 8: the symbol is wrong, the right symbol should be = in line %d!\n", line);
				ERROR_num++;
				c = fgetc(fp);
				while (c != ';') {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						system("pause");
					}
					else
						c = fgetc(fp);
				}
				fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
		}
		else {
			printf("error 8: the symbol is wrong, the right symbol should be an idname in line %d!\n", line);
			ERROR_num++;
			c = fgetc(fp);
			while (c != ';') {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
	}
	else if (sym == 3) {			//char
		//printf("constchar\n");
		sym = word_Recongize();
		if (sym == 36) {			//id
			strcpy(idname, token);
			sym = word_Recongize();
			if (sym == 27) {			//=
				sym = word_Recongize();
				if (sym == 38) {
					//////////////填入符号表//////////////////
					if (id_def_check(idname, level) == 0) {
						strcpy(SymList[SymList_top].name, idname);
						SymList[SymList_top].kind = 0;
						SymList[SymList_top].type = 2;
						SymList[SymList_top].isarray = 0;
						SymList[SymList_top].lev = level;
						SymList[SymList_top].value = token[1];
						if (level == 0) {
							SymList[SymList_top].index = 0;
						}
						else {
							SymList[SymList_top].index = FuncStackTop[level];
							FuncStackTop[level] += 4;
						}
						SymList_top++;
					}
					else {
						printf("error 17: the name %s in line %d has defined in symlist!\n", idname, line);
						ERROR_num++;
						c = fgetc(fp);
						while (c != ';') {
							c = fgetc(fp);
						}
						return 25;
					}
					///////////////////
					sym = word_Recongize();
					while (sym == 26) {
						sym = word_Recongize();
						if (sym == 36) {
							strcpy(idname, token);			//id
							sym = word_Recongize();
							if (sym == 27) {
								sym = word_Recongize();
								if (sym == 38) {
									//////////////填入符号表//////////////////
									if (id_def_check(idname, level) == 0) {
										strcpy(SymList[SymList_top].name, idname);
										SymList[SymList_top].kind = 0;
										SymList[SymList_top].type = 2;
										SymList[SymList_top].isarray = 0;
										SymList[SymList_top].lev = level;
										SymList[SymList_top].value = token[1];
										if (level == 0) {
											SymList[SymList_top].index = 0;
										}
										else {
											SymList[SymList_top].index = FuncStackTop[level];
											FuncStackTop[level] += 4;
										}
										SymList_top++;
									}
									else {
										printf("error 17: the name %s in line %d has defined in symlist!\n", idname, line);
										ERROR_num++;
										c = fgetc(fp);
										while (c != ';') {
											c = fgetc(fp);
										}
										return 25;
									}
									///////////////////
							//		printf("const char\n");
									sym = word_Recongize();
								}
								else {
									printf("error 8: the symbol is wrong, the right symbol should be a character in line %d!\n", line);
									ERROR_num++;
									c = fgetc(fp);
									while (c != ';') {
										if (c == EOF) {
											printf("error 7: Can't find an right symbol before end!\n");
											system("pause");
										}
										else
											c = fgetc(fp);
									}
									fseek(fp, -1, SEEK_CUR);
									return word_Recongize();
								}
							}
							else {
								printf("error 8: the symbol is wrong, the right symbol should be = in line %d!\n", line);
								ERROR_num++;
								c = fgetc(fp);
								while (c != ';') {
									if (c == EOF) {
										printf("error 7: Can't find an right symbol before end!\n");
										system("pause");
									}
									else
										c = fgetc(fp);
								}
								fseek(fp, -1, SEEK_CUR);
								return word_Recongize();
							}
						}
						else {
							printf("error 8: the symbol is wrong, the right symbol should be a idname in line %d!\n", line);
							ERROR_num++;
							c = fgetc(fp);
							while (c != ';') {
								if (c == EOF) {
									printf("error 7: Can't find an right symbol before end!\n");
									system("pause");
								}
								else
									c = fgetc(fp);
							}
							fseek(fp, -1, SEEK_CUR);
							return word_Recongize();
						}
					}
					if (sym == 25) {
						return 25;
					}
					else {
						printf("error 8: the symbol is wrong, the right symbol should be ; in line %d!\n", line);
						ERROR_num++;
						c = fgetc(fp);
						while (c != ';') {
							if (c == EOF) {
								printf("error 7: Can't find an right symbol before end!\n");
								system("pause");
							}
							else
								c = fgetc(fp);
						}
						fseek(fp, -1, SEEK_CUR);
						return word_Recongize();
					}
				}
				else {
					printf("error 8: the symbol is wrong, the right symbol should be a character in line %d!\n", line);
					ERROR_num++;
					c = fgetc(fp);
					while (c != ';') {
						if (c == EOF) {
							printf("error 7: Can't find an right symbol before end!\n");
							system("pause");
						}
						else
							c = fgetc(fp);
					}
					fseek(fp, -1, SEEK_CUR);
					return word_Recongize();
				}
			}
			else {
				printf("error 8: the symbol is wrong, the right symbol should be = in line %d!\n", line);
				ERROR_num++;
				c = fgetc(fp);
				while (c != ';') {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						system("pause");
					}
					else
						c = fgetc(fp);
				}
				fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
		}
		else {
			printf("error 8: the symbol is wrong, the right symbol should be an idname in line %d!\n", line);
			ERROR_num++;
			c = fgetc(fp);
			while (c != ';') {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
	}
	else {
		;/////////////error//////////////不是int/char
	}
}

int int_var_dcl(int sym, int level) {
	char idname[100] = { 0 };
	char c;
	strcpy(idname, token);
	if (sym == 32){			//[
		sym = word_Recongize();
		if (sym == 37) {			//数组元素个数
			if (NUM0 > 0) {
				sym = word_Recongize();
				if (sym == 33) {			//]
					//////////////填入符号表//////////////////
					if (id_def_check(idname, level) == 0) {
						strcpy(SymList[SymList_top].name, idname);
						SymList[SymList_top].kind = 1;
						SymList[SymList_top].type = 1;
						SymList[SymList_top].isarray = 1;
						SymList[SymList_top].lev = level;
						SymList[SymList_top].value = NUM0;
						if (level == 0) {
							SymList[SymList_top].index = 0;
						}
						else {
							SymList[SymList_top].index = FuncStackTop[level];
							FuncStackTop[level] += 4*NUM0;
						}
						SymList_top++;
					}
					else {
						printf("error 17: the name %s in line %d has defined in symlist!\n", idname, line);
						ERROR_num++;
						c = fgetc(fp);
						while (c != ';') {
							c = fgetc(fp);
						}
						return 25;
					}
					///////////////////
					sym = word_Recongize();
				}
				else {
					printf("error 8: the symbol is wrong, the right symbol should be ] in line %d!\n", line);
					ERROR_num++;
					c = fgetc(fp);
					while (c != ';') {
						if (c == EOF) {
							printf("error 7: Can't find an right symbol before end!\n");
							system("pause");
						}
						else
							c = fgetc(fp);
					}
					//fseek(fp, -1, SEEK_CUR);
					return word_Recongize();
				}
			}
			else {
				;///////error 数组有0个
			}
		}
		else {
			printf("error 8: the symbol is wrong, the right symbol should be a number in line %d!\n", line);
			ERROR_num++;
			c = fgetc(fp);
			while (c != ';') {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			//fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
	}
	else if ((sym == 26)||(sym == 25)) {
		//////////////写入符号表////////////////
		if (id_def_check(idname, level) == 0) {
			strcpy(SymList[SymList_top].name, idname);
			SymList[SymList_top].kind = 1;
			SymList[SymList_top].type = 1;
			SymList[SymList_top].isarray = 0;
			SymList[SymList_top].lev = level;
		//	SymList[SymList_top].value = num;
			if (level == 0) {
				SymList[SymList_top].index = 0;
			}
			else {
				SymList[SymList_top].index = FuncStackTop[level];
				FuncStackTop[level] += 4;
			}
			SymList_top++;
		}
		else {
			printf("error 17: the name %s in line %d has defined in symlist!\n", idname, line);
			ERROR_num++;
			c = fgetc(fp);
			while (c != ';') {
				c = fgetc(fp);
			}
			return 25;
		}
		///////////////////

		//printf("int var\n");
	}
	while (sym == 26) {						//，
		sym = word_Recongize();
		if (sym == 36) {				//id
			strcpy(idname, token);
			sym = word_Recongize();
			if (sym == 32) {
				sym = word_Recongize();
				if (sym == 37) {
					if (NUM0 > 0) {
						sym = word_Recongize();
						if (sym == 33) {
							//////////////填入符号表//////////////////
							if (id_def_check(idname, level) == 0) {
								strcpy(SymList[SymList_top].name, idname);
								SymList[SymList_top].kind = 1;
								SymList[SymList_top].type = 1;
								SymList[SymList_top].isarray = 1;
								SymList[SymList_top].lev = level;
								SymList[SymList_top].value = NUM0;
								if (level == 0) {
									SymList[SymList_top].index = 0;
								}
								else {
									SymList[SymList_top].index = FuncStackTop[level];
									FuncStackTop[level] += 4 * NUM0;
								}
								SymList_top++;
							}
							else {
								printf("error 17: the name %s in line %d has defined in symlist!\n", idname, line);
								ERROR_num++;
								c = fgetc(fp);
								while (c != ';') {
									c = fgetc(fp);
								}
								return 25;
							}
							///////////////////
							//printf("int array\n");
							sym = word_Recongize();
						}
						else {
							printf("error 8: the symbol is wrong, the right symbol should be ] in line %d!\n", line);
							ERROR_num++;
							c = fgetc(fp);
							while (c != ';') {
								if (c == EOF) {
									printf("error 7: Can't find an right symbol before end!\n");
									system("pause");
								}
								else
									c = fgetc(fp);
							}
							fseek(fp, -1, SEEK_CUR);
							return word_Recongize();
						}
					}
					else {
						;///////error 数组有0个
					}
				}
				else {
					printf("error 8: the symbol is wrong, the right symbol should be a number in line %d!\n", line);
					ERROR_num++;
					c = fgetc(fp);
					while (c != ';') {
						if (c == EOF) {
							printf("error 7: Can't find an right symbol before end!\n");
							system("pause");
						}
						else
							c = fgetc(fp);
					}
					//fseek(fp, -1, SEEK_CUR);
					return word_Recongize();
				}
			}
			else if ((sym == 26)||(sym == 25)) {
				//////////////////写入符号表/////////////////
				if (id_def_check(idname, level) == 0) {
					strcpy(SymList[SymList_top].name, idname);
					SymList[SymList_top].kind = 1;
					SymList[SymList_top].type = 1;
					SymList[SymList_top].isarray = 0;
					SymList[SymList_top].lev = level;
					if (level == 0) {
						SymList[SymList_top].index = 0;
					}
					else {
						SymList[SymList_top].index = FuncStackTop[level];
						FuncStackTop[level] += 4;
					}
					SymList_top++;
				}
				else {
					printf("error 17: the name %s in line %d has defined in symlist!\n", idname, line);
					ERROR_num++;
					c = fgetc(fp);
					while (c != ';') {
						c = fgetc(fp);
					}
					return 25;
				}
				///////////////////
				//printf("int var\n");
			}
		}
		else {
			printf("error 8: the symbol is wrong, the right symbol should be an idname in line %d!\n", line);
			ERROR_num++;
			c = fgetc(fp);
			while (c != ';') {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			//fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
	}
	if (sym == 25) {		//分号
		return word_Recongize();
	}
	else {
		printf("error 8: the symbol is wrong, the right symbol should be ; in line %d!\n", line);
		ERROR_num++;
		c = fgetc(fp);
		while (c != ';') {
			if (c == EOF) {
				printf("error 7: Can't find an right symbol before end!\n");
				system("pause");
			}
			else
				c = fgetc(fp);
		}
		//fseek(fp, -1, SEEK_CUR);
		return word_Recongize();
	}
}
int char_var_dcl(int sym, int level) {
	char idname[IDENTNAMESIZE] = { 0 };
	char c;
	strcpy(idname, token);
	if (sym == 32) {
		sym = word_Recongize();
		if (sym == 37) {
			if (NUM0 > 0) {
				sym = word_Recongize();
				if (sym == 33) {
					//////////////////写入符号表/////////////////
					if (id_def_check(idname, level) == 0) {
						strcpy(SymList[SymList_top].name, idname);
						SymList[SymList_top].kind = 1;
						SymList[SymList_top].type = 2;
						SymList[SymList_top].isarray = 1;
						SymList[SymList_top].lev = level;
						SymList[SymList_top].value = NUM0;
						if (level == 0) {
							SymList[SymList_top].index = 0;
						}
						else {
							SymList[SymList_top].index = FuncStackTop[level];
							FuncStackTop[level] += 4 * NUM0;
						}
						SymList_top++;
					}
					else {
						printf("error 17: the name %s in line %d has defined in symlist!\n", idname, line);
						ERROR_num++;
						c = fgetc(fp);
						while (c != ';') {
							c = fgetc(fp);
						}
						return 25;
					}
					sym = word_Recongize();
				}
				else {
					printf("error 8: the symbol is wrong, the right symbol should be ]!\n");
					c = fgetc(fp);
					while (c != ';') {
						if (c == EOF) {
							printf("error 7: Can't find an right symbol before end!\n");
							system("pause");
						}
						else
							c = fgetc(fp);
					}
					//fseek(fp, -1, SEEK_CUR);
					return word_Recongize();
				}
			}
			else {
				;///////error 数组有0个
			}
		}
		else {
			printf("error 8: the symbol is wrong, the right symbol should be a number in line %d!\n", line);
			ERROR_num++;
			c = fgetc(fp);
			while (c != ';') {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			//fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
	}
	else if ((sym == 26)||(sym == 25)) {
		//////////////////写入符号表/////////////////
		if (id_def_check(idname, level) == 0) {
			strcpy(SymList[SymList_top].name, idname);
			SymList[SymList_top].kind = 1;
			SymList[SymList_top].type = 2;
			SymList[SymList_top].isarray = 0;
			SymList[SymList_top].lev = level;
			//	SymList[SymList_top].value = num;
			if (level == 0) {
				SymList[SymList_top].index = 0;
			}
			else {
				SymList[SymList_top].index = FuncStackTop[level];
				FuncStackTop[level] += 4;
			}
			SymList_top++;
		}
		else {
			printf("error 17: the name %s in line %d has defined in symlist!\n", idname, line);
			ERROR_num++;
			c = fgetc(fp);
			while (c != ';') {
				c = fgetc(fp);
			}
			return 25;
		}
		///////////////////
		//printf("char var\n");
	}
	while (sym == 26) {						//，
		sym = word_Recongize();
		if (sym == 36) {				//id
			strcpy(idname, token);
			sym = word_Recongize();
			if (sym == 32) {
				sym = word_Recongize();
				if (sym == 37) {
					if (NUM0 > 0) {
						sym = word_Recongize();
						if (sym == 33) {
							//////////////////写入符号表/////////////////
							if (id_def_check(idname, level) == 0) {
								strcpy(SymList[SymList_top].name, idname);
								SymList[SymList_top].kind = 1;
								SymList[SymList_top].type = 2;
								SymList[SymList_top].isarray = 1;
								SymList[SymList_top].lev = level;
								SymList[SymList_top].value = NUM0;
								if (level == 0) {
									SymList[SymList_top].index = 0;
								}
								else {
									SymList[SymList_top].index = FuncStackTop[level];
									FuncStackTop[level] += 4 * NUM0;
								}
								SymList_top++;
							}
							else {
								printf("error 17: the name %s in line %d has defined in symlist!\n", idname, line);
								ERROR_num++;
								c = fgetc(fp);
								while (c != ';') {
									c = fgetc(fp);
								}
								return 25;
							}
							///////////////////
//							printf("char array\n");
							sym = word_Recongize();
						}
						else {
							printf("error 8: the symbol is wrong, the right symbol should be ] in line %d!\n", line);
							ERROR_num++;
							c = fgetc(fp);
							while (c != ';') {
								if (c == EOF) {
									printf("error 7: Can't find an right symbol before end!\n");
									system("pause");
								}
								else
									c = fgetc(fp);
							}
							//fseek(fp, -1, SEEK_CUR);
							return word_Recongize();
						}
					}
					else {
						;///////error 数组有0个
					}
				}
				else {
					printf("error 8: the symbol is wrong, the right symbol should be a number! in line %d!\n", line);
					ERROR_num++;
					c = fgetc(fp);
					while (c != ';') {
						if (c == EOF) {
							printf("error 7: Can't find an right symbol before end!\n");
							system("pause");
						}
						else
							c = fgetc(fp);
					}
					//fseek(fp, -1, SEEK_CUR);
					return word_Recongize();
				}
			}
			else if ((sym == 26)||(sym == 25)) {
				//////////////////写入符号表/////////////////
				if (id_def_check(idname, level) == 0) {
					strcpy(SymList[SymList_top].name, idname);
					SymList[SymList_top].kind = 1;
					SymList[SymList_top].type = 2;
					SymList[SymList_top].isarray = 0;
					SymList[SymList_top].lev = level;
					//	SymList[SymList_top].value = num;
					if (level == 0) {
						SymList[SymList_top].index = 0;
					}
					else {
						SymList[SymList_top].index = FuncStackTop[level];
						FuncStackTop[level] += 4;
					}
					SymList_top++;
				}
				else {
					printf("error 17: the name %s in line %d has defined in symlist!\n", idname, line);
					ERROR_num++;
					c = fgetc(fp);
					while (c != ';') {
						c = fgetc(fp);
					}
					return 25;
				}
			}

		}
		else {
			printf("error 8: the symbol is wrong, the right symbol should be an idname in line %d!\n", line);
			ERROR_num++;
			c = fgetc(fp);
			while (c != ';') {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			//fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
	}

	if (sym == 25) {		//分号
		return word_Recongize();
	}
	else {
		printf("error 8: the symbol is wrong, the right symbol should be ; in line %d!\n", line);
		ERROR_num++;
		c = fgetc(fp);
		while (c != ';') {
			if (c == EOF) {
				printf("error 7: Can't find an right symbol before end!\n");
				system("pause");
			}
			else
				c = fgetc(fp);
		}
		//fseek(fp, -1, SEEK_CUR);
		return word_Recongize();
	}
}

int int_re_func_def(int sym, int level) {
	char funcname[IDENTNAMESIZE] = { 0 };
	char c;
	strcpy(funcname, token);
	returnflag = 1;
	//////////////////填入符号表////////////////
	if (!id_def_check(funcname, level)) {
		strcpy(SymList[SymList_top].name, funcname);
		SymList[SymList_top].kind = 2;
		SymList[SymList_top].type = 1;
		SymList[SymList_top].isarray = 0;
		SymList[SymList_top].lev = 0;
		SymList[SymList_top].value = level;
		SymList[SymList_top].index = 0;
		SymList_top++;
	}
	else {
		printf("error 17: the name %s in line %d has defined in symlist!\n", funcname, line);
		ERROR_num++;
		c = fgetc(fp);
		while (c != ';') {
			c = fgetc(fp);
		}
		return 25;
	}
	///////////////填入中间代码////////////////
	sprintf(CodeList[CodeList_top++], "int %s()", funcname);
	///////////////
	sym = word_Recongize();
	if ((sym == 2) || (sym == 3)) {			//int,char
		sym = para_list(sym, level);
	}
	if (sym == 31) {		//)
		sym = word_Recongize();
		if (sym == 34) {			//{
			sym = word_Recongize();
			sym = com_statements(sym, level);
			
			if (sym == 35) {
				//printf("This is a int return function definition!\n");
				sprintf(CodeList[CodeList_top++], "ret.");
				return word_Recongize();
			}
			else {
				printf("error 8: the symbol is wrong, the right symbol should be } in line %d!\n", line);
				ERROR_num++;
				c = fgetc(fp);
				while (c != '}') {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						system("pause");
					}
					else
						c = fgetc(fp);
				}
				//fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
		}
		else {
			printf("error 8: the symbol is wrong, the right symbol should be { in line %d!\n", line);
			ERROR_num++;
			c = fgetc(fp);
			while (c != '}') {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			//fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
	}
	else {
		printf("error 8: the symbol is wrong, the right symbol should be ) in line %d!\n", line);
		ERROR_num++;
		c = fgetc(fp);
		while (c != '}') {
			if (c == EOF) {
				printf("error 7: Can't find an right symbol before end!\n");
				system("pause");
			}
			else
				c = fgetc(fp);
		}
		//fseek(fp, -1, SEEK_CUR);
		return word_Recongize();
	}
	
}
int char_re_func_def(int sym, int level) {
	char funcname[IDENTNAMESIZE] = { 0 };
	char c;
	strcpy(funcname, token);
	returnflag = 2;
	//////////////////填入符号表////////////////
	if (!id_def_check(funcname, level)) {
		strcpy(SymList[SymList_top].name, funcname);
		SymList[SymList_top].kind = 2;
		SymList[SymList_top].type = 2;
		SymList[SymList_top].isarray = 0;
		SymList[SymList_top].lev = 0;
		SymList[SymList_top].value = level;
		SymList[SymList_top].index = 0;
		SymList_top++;
	}
	else {
		printf("error 17: the name %s in line %d has defined in symlist!\n", funcname, line);
		ERROR_num++;
		c = fgetc(fp);
		while (c != ';') {
			c = fgetc(fp);
		}
		return 25;
	}
	///////////////
	sprintf(CodeList[CodeList_top++], "char %s()", funcname);
	sym = word_Recongize();
	if ((sym == 2) || (sym == 3)) {			//int,char
		sym = para_list(sym, level);
	}
	if (sym == 31) {		//)
		sym = word_Recongize();
		if (sym == 34) {			//{
			sym = word_Recongize();
			sym = com_statements(sym, level);
			if (sym == 35) {
				//printf("This is a char return function definition!\n");
				sprintf(CodeList[CodeList_top++], "ret.");
				return word_Recongize();
			}
			else {
				printf("error 8: the symbol is wrong, the right symbol should be } in line %d!\n", line);
				ERROR_num++;
				c = fgetc(fp);
				while (c != '}') {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						system("pause");
					}
					else
						c = fgetc(fp);
				}
				//fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
		}
		else {
			printf("error 8: the symbol is wrong, the right symbol should be { in line %d!\n", line);
			ERROR_num++;
			c = fgetc(fp);
			while (c != '}') {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			//fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
	}
	else {
		printf("error 8: the symbol is wrong, the right symbol should be ) in line %d!\n", line);
		ERROR_num++;
		c = fgetc(fp);
		while (c != '}') {
			if (c == EOF) {
				printf("error 7: Can't find an right symbol before end!\n");
				system("pause");
			}
			else
				c = fgetc(fp);
		}
	//	fseek(fp, -1, SEEK_CUR);
		return word_Recongize();
	}
}
int no_re_func_def(int sym, int level) {
	char funcname[IDENTNAMESIZE] = { 0 };
	char c;
	strcpy(funcname, token);
	returnflag = 0;
	//////////////////填入符号表////////////////
	if (!id_def_check(funcname, level)) {
		strcpy(SymList[SymList_top].name, funcname);
		SymList[SymList_top].kind = 2;
		SymList[SymList_top].type = 0;
		SymList[SymList_top].isarray = 0;
		SymList[SymList_top].lev = 0;
		SymList[SymList_top].value = level;
		SymList[SymList_top].index = 0;
		SymList_top++;
	}
	else {
		printf("error 17: the name %s in line %d has defined in symlist!\n", funcname, line);
		ERROR_num++;
		c = fgetc(fp);
		while (c != ';') {
			c = fgetc(fp);
		}
		return 25;
	}
	///////////////
	sprintf(CodeList[CodeList_top++], "void %s()", funcname);
	////////////////
	sym = word_Recongize();
	if (sym == 30){				//(
		sym = word_Recongize();
		if ((sym == 2) || (sym == 3)) {			//int,char
			sym = para_list(sym, level);
		}
		if (sym == 31) {		//)
			sym = word_Recongize();
			if (sym == 34) {			//{
				sym = word_Recongize();
				sym = com_statements(sym, level);
				if (sym == 35) {
					//printf("This is a void return function definition!\n");
					sprintf(CodeList[CodeList_top++], "ret.");
					return word_Recongize();
				}
				else {
					ERROR_num++;
					printf("error 8: the symbol is wrong, the right symbol should be } in line %d!\n", line);
					ERROR_num++;
					c = fgetc(fp);
					while (c != '}') {
						if (c == EOF) {
							printf("error 7: Can't find an right symbol before end!\n");
							system("pause");
						}
						else
							c = fgetc(fp);
					}
					//fseek(fp, -1, SEEK_CUR);
					return word_Recongize();
				}
			}
			else {
				printf("error 8: the symbol is wrong, the right symbol should be { in line %d!\n", line);
				ERROR_num++;
				c = fgetc(fp);
				while (c != '}') {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						system("pause");
					}
					else
						c = fgetc(fp);
				}
				//fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
		}
		else {
			printf("error 8: the symbol is wrong, the right symbol should be ) in line %d!\n", line);
			ERROR_num++;
			c = fgetc(fp);
			while (c != '}') {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			//fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
	}
}

int para_list(int sym, int level)
{
	sym = parameter(sym, level);
	while (sym == 26) {
		sym = word_Recongize();
		sym = parameter(sym, level);
	}
	return sym;
}
int parameter(int sym, int level)
{
	char c;
	if (sym == 2) {
		sym = word_Recongize();
		if (sym == 36) {
			////////////////////填表////////////////
			if (id_def_check(token, level) == 0) {
				strcpy(SymList[SymList_top].name, token);
				SymList[SymList_top].kind = 3;
				SymList[SymList_top].type = 1;
				SymList[SymList_top].isarray = 0;
				SymList[SymList_top].lev = level;
				//SymList[SymList_top].value = ;
				SymList[SymList_top].index = FuncStackTop[level];
				FuncStackTop[level] += 4;
				FuncParaNum[level] += 1;
				SymList_top++;
			}
			else {
				printf("error 17: the name %s in line %d has defined in symlist!\n", token , line);
				ERROR_num++;
				c = fgetc(fp);
				while (c != ';') {
					c = fgetc(fp);
				}
				return 25;
			}
			sprintf(CodeList[CodeList_top++], "para int %s",token);
			return word_Recongize();
		}
		//////////////////////////////////////
		else {
			printf("error 8: the symbol is wrong, the right symbol should be an idname in line %d!\n", line);
			ERROR_num++;
			c = fgetc(fp);
			while ((c != ',')&&(c!=')')) {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
	}
	else if (sym == 3) {
		sym = word_Recongize();
		if (sym == 36) {
			////////////////////填表////////////////
			if (id_def_check(token, level) == 0) {
				strcpy(SymList[SymList_top].name, token);
				SymList[SymList_top].kind = 3;
				SymList[SymList_top].type = 2;
				SymList[SymList_top].isarray = 0;
				SymList[SymList_top].lev = level;
				//SymList[SymList_top].value = ;
				SymList[SymList_top].index = FuncStackTop[level];
				FuncStackTop[level] += 4;
				FuncParaNum[level] += 1;
				SymList_top++;
			}
			else {
				printf("error 17: the name %s in line %d has defined in symlist!\n", token, line);
				ERROR_num++;
				c = fgetc(fp);
				while (c != ';') {
					c = fgetc(fp);
				}
				return 25;
			}
			/////////////
			sprintf(CodeList[CodeList_top++], "para char %s", token);
			return word_Recongize();
		}
		else {
			printf("error 8: the symbol is wrong, the right symbol should be an idname in line %d!\n", line);
			ERROR_num++;
			c = fgetc(fp);
			while (c != '}') {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
	}
}

int main_func_def(int sym, int level) {
	char c;
	sym = word_Recongize();
	if (sym == 30) {
		sym = word_Recongize();			
		if (sym == 31) {				//)
			sprintf(CodeList[CodeList_top++], "void main()");
			strcpy(SymList[SymList_top].name, "main");
			SymList[SymList_top].kind = 2;
			SymList[SymList_top].type = 0;
			SymList[SymList_top].isarray = 0;
			SymList[SymList_top].lev = 0;
			SymList[SymList_top].value = level;
			SymList[SymList_top].index = 0;
			SymList_top++;
			sym = word_Recongize();
			if (sym == 34) {			//{
				sym = word_Recongize();
				sym = com_statements(sym, level);
				if (sym == 35) {
					///////////////////
//					printf("This is a main function!\n");
					strcpy(CodeList[CodeList_top++], "ret.");
					return word_Recongize();
				}
				else {
					printf("error 8: the symbol is wrong, the right symbol should be } in line %d!\n", line);
					ERROR_num++;
					c = fgetc(fp);
					while (c != '}') {
						if (c == EOF) {
							printf("error 7: Can't find an right symbol before end!\n");
							system("pause");
						}
							
						else
							c = fgetc(fp);
					}
					fseek(fp, -1, SEEK_CUR);
					return word_Recongize();
				}
			}
			else {
				printf("error 8: the symbol is wrong, the right symbol should be { in line %d!\n", line);
				ERROR_num++;
				c = fgetc(fp);
				while (c != '}') {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						system("pause");
					}
					else
						c = fgetc(fp);
				}
				fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
		}
		else {
			printf("error 8: the symbol is wrong, the right symbol should be ) in line %d!\n", line);
			ERROR_num++;
			c = fgetc(fp);
			while (c != '}') {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
	}
	else {
		printf("error 8: the symbol is wrong, the right symbol should be ( in line %d!\n", line);
		ERROR_num++;
		c = fgetc(fp);
		while (c != '}') {
			if (c == EOF) {
				printf("error 7: Can't find an right symbol before end!\n");
				system("pause");
			}
			else
				c = fgetc(fp);
		}
		fseek(fp, -1, SEEK_CUR);
		return word_Recongize();
	}
}

int com_statements(int sym, int level)
{
	char c;
	while (sym == 1) {				//const
			sym = const_dcl(sym, level);
	}
	while ((sym == 2) || (sym == 3)) {
		if (sym == 2) {			//int
			sym = word_Recongize();
			if (sym == 36) {		//id
				sym = word_Recongize();
				sym = int_var_dcl(sym, level);
			}
			else {
				printf("error 8: the symbol is wrong, the right symbol should be an idname in line %d!\n", line);
				ERROR_num++;
				c = fgetc(fp);
				while (c != ';') {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						system("pause");
					}
					else
						c = fgetc(fp);
				}
				//fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
		}

		else {				//char
			sym = word_Recongize();
			if (sym == 36) {		//id
				sym = word_Recongize();
				sym = char_var_dcl(sym, level);
			}
			else {
				printf("error 8: the symbol is wrong, the right symbol should be an idname in line %d!\n", line);
				ERROR_num++;
				c = fgetc(fp);
				while (c != ';') {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						system("pause");
					}
					else
						c = fgetc(fp);
				}
			//	fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
		}
	}
	sym = statements(sym);
	return sym;
}

int statements(int sym)
{
	while (isfirst_statement(sym)) {
		sym = statement(sym);
	}	
	return sym;	
}

int isfirst_statement(int sym)
{
	if ((sym == 6) || (sym == 7) || (sym == 8) || (sym == 11) || (sym == 12) || (sym == 13) || (sym == 25) || (sym == 34) || (sym == 36))
		return 1;
	else
		return 0;
}

int statement(int sym)
{
	char c;
	//	while(sym != 0){
	if (sym == 6) {				//if
		sym = if_statement(sym);
	}
	else if (sym == 7) {		//while
		sym = while_statement(sym);
	}
	else if (sym == 34) {		//{语句列
		sym = word_Recongize();
		sym = statements(sym);
		if (sym == 35) {			//}
			sym = word_Recongize();
		}
		else {
			;//////////////error  没有}
		}
	}
	else if (sym == 36) {		//赋值，函数调用
		sym = word_Recongize();
		if (sym == 30) {		//(
			sym = func_call(sym);
			if (sym == 25) {
				sym = word_Recongize();
			}
			else {
				printf("error 8: the symbol is wrong, the right symbol should be ; in line %d!\n", line);
				ERROR_num++;
				c = fgetc(fp);
				while (c != ';') {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						system("pause");
					}
						
					else
						c = fgetc(fp);
				}
				//fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
		}
		else if ((sym == 27) || (sym == 32)) {	//=
			sym = ass_statement(sym);
			if (sym == 25) {
				sym = word_Recongize();
			}
			else {
				printf("error 8: the symbol is wrong, the right symbol should be ; in line %d!\n", line);
				ERROR_num++;
				c = fgetc(fp);
				while (c != ';') {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						system("pause");
					}
					else
						c = fgetc(fp);
				}
				//fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
		}
	}
	else if (sym == 11) {		//scanf
		sym = scanf_statement(sym);
		if (sym == 25) {
			sym = word_Recongize();
		}
		else {
			printf("error 8: the symbol is wrong, the right symbol should be ; in line %d!\n", line);
			ERROR_num++;
			c = fgetc(fp);
			while (c != ';') {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
		//	fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
	}
	else if (sym == 12) {		//printf
		sym = printf_statement(sym);
		if (sym == 25) {
			sym = word_Recongize();
		}
		else {
			printf("error 8: the symbol is wrong, the right symbol should be ; in line %d!\n", line);
			ERROR_num++;
			c = fgetc(fp);
			while (c != ';') {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			//fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
	}
	else if (sym == 8) {		//switch
		sym = switch_statement(sym);
	}
	else if (sym == 13) {		//return
		sym = return_statement(sym);
		if (sym == 25) {
			sym = word_Recongize();
		}
		else {
			printf("error 8: the symbol is wrong, the right symbol should be ; in line %d!\n", line);
			ERROR_num++;
			c = fgetc(fp);
			while (c != ';') {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			//fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
	}
	else if (sym == 25) {		//;
//			printf("this is a null statement!\n");
		sym = word_Recongize();
	}
	return sym;
}

int if_statement(int sym)
{
	char c;
	char label[20] = { 0 };
	sprintf(label, "label%d", LabelNum++);
	sym = word_Recongize();
	if (sym == 30) {		//（
		sym = word_Recongize();
		sym = condition(sym);
		if (sym == 31) {		//)
			sprintf(CodeList[CodeList_top++], "BZ %s", label);
			sym = word_Recongize();
			sym = statement(sym);
			sprintf(CodeList[CodeList_top++], "%s:", label);
			return sym;
		}
		else {
			printf("error 8: the symbol is wrong, the right symbol should be ) in line %d!\n", line);
			ERROR_num++;
			c = fgetc(fp);
			while ((c != ';')&&(c!=')')) {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			//fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
	}
	else {
		printf("error 8: the symbol is wrong, the right symbol should be ( in line %d!\n", line);
		ERROR_num++;
		c = fgetc(fp);
		while ((c != ';') && (c != ')')) {
			if (c == EOF) {
				printf("error 7: Can't find an right symbol before end!\n");
				system("pause");
			}
			else
				c = fgetc(fp);
		}
		//fseek(fp, -1, SEEK_CUR);
		return word_Recongize();
	}
}

int while_statement(int sym)
{
	char c;
	char label1[20] = { 0 };
	char label2[20] = { 0 };
	sprintf(label1, "label%d", LabelNum++);
	sprintf(label2, "label%d", LabelNum++);
	sprintf(CodeList[CodeList_top++], "%s:", label1);
	sym = word_Recongize();
	if (sym == 30) {		//（
		sym = word_Recongize();
		sym = condition(sym);
		if (sym == 31) {		//)
			sprintf(CodeList[CodeList_top++], "BZ %s", label2);

			sym = word_Recongize();
			sym = statement(sym);
			sprintf(CodeList[CodeList_top++], "GOTO %s", label1);
			sprintf(CodeList[CodeList_top++], "%s:", label2);

			return sym;
		}
		else {
			printf("error 8: the symbol is wrong, the right symbol should be ) in line %d!\n", line);
			ERROR_num++;
			c = fgetc(fp);
			while ((c != ';') && (c != ')')) {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			//fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
	}
	else {
		printf("error 8: the symbol is wrong, the right symbol should be ( in line %d!\n", line);
		ERROR_num++;
		c = fgetc(fp);
		while ((c != ';') && (c != ')')) {
			if (c == EOF) {
				printf("error 7: Can't find an right symbol before end!\n");
				system("pause");
			}
			else
				c = fgetc(fp);
		}
		//fseek(fp, -1, SEEK_CUR);
		return word_Recongize();
	}
}


int func_call(int sym)
{
	char funcname[IDENTNAMESIZE] = { 0 };
	strcpy(funcname, token);
	
	sym = word_Recongize();
	sym = value_para_list(sym, FuncParaNum[SymList[getSymIndex_intercode(funcname)].value], getSymIndex_intercode(funcname));
	if (sym == 31) {			//)
		sprintf(CodeList[CodeList_top++], "call %s", funcname);
		return word_Recongize();
	}
}

int func_call_factor(int sym)
{
	char c;
	char funcname[IDENTNAMESIZE] = { 0 };
	strcpy(funcname, token);
	if (SymList[getSymIndex_intercode(funcname)].kind == 0) {
		ERROR_num++;
		printf("error 15: the function call in factor shouold have return!\n", line);
		c = fgetc(fp);
		while (c != ')') {
			if (c == EOF) {
				printf("error 7: Can't find an right symbol before end!\n");
				system("pause");
			}
			else
				c = fgetc(fp);
		}
		//fseek(fp, -1, SEEK_CUR);
		return word_Recongize();
	}
	sym = word_Recongize();
	sym = value_para_list(sym, FuncParaNum[SymList[getSymIndex_intercode(funcname)].value], getSymIndex_intercode(funcname));
	if (sym == 31) {			//)
		sprintf(CodeList[CodeList_top++], "call %s", funcname);
		strcpy(OPList[OPList_top++], get_tReg());
		sprintf(CodeList[CodeList_top++], "%s = RET", OPList[OPList_top - 1]);
		return word_Recongize();
	}
}
int ass_statement(int sym)
{
	char c;
	char idname[100];
	char reg[20];
	int left_type, right_type;
	int index;
	strcpy(idname, token);
	index = getSymIndex_intercode(idname);
	left_type = SymList[index].type;
	if (id_use_find(idname) == 0) {
		printf("error 18: the idname \"%s\" in line %d is not defined in Symlist!\n", idname, line);
		ERROR_num++;
		c = fgetc(fp);
		while (c != ';') {
			if (c == EOF) {
				printf("error 7: Can't find an right symbol before end!\n");
				system("pause");
			}
			else
				c = fgetc(fp);
		}
		fseek(fp, -1, SEEK_CUR);
		return word_Recongize();
	}
	else if ((SymList[getSymIndex_intercode(idname)].kind != 1) && (SymList[getSymIndex_intercode(idname)].kind != 3)) {
		ERROR_num++;
		printf("error 9: the left symbol in assign statement should be a variable in line %d!\n", line);
		c = fgetc(fp);
		while (c != ';') {
			if (c == EOF) {
				printf("error 7: Can't find an right symbol before end!\n");
				system("pause");
			}
			else
				c = fgetc(fp);
		}
		fseek(fp, -1, SEEK_CUR);
		return word_Recongize();
	}
	if (sym == 32) {			//[
		sym = word_Recongize();
		if (isfirst_expression(sym)) {
			sym = expression(sym);
			if (sym == 33) {			//]
				sprintf(idname, "%s[%s]", idname, OPList[OPList_top - 1]);
				if (isnumber(OPList[OPList_top - 1][0])) {
					NUM0 = atoi(OPList[OPList_top - 1]);
					if (NUM0 >= SymList[index].value) {
						printf("error 19: the array is not so big in line %d!\n", line);
						ERROR_num++;
						c = fgetc(fp);
						while (c != ';') {
							if (c == EOF) {
								printf("error 7: Can't find an right symbol before end!\n");
								system("pause");
							}
							else
								c = fgetc(fp);
						}
						fseek(fp, -1, SEEK_CUR);
						return word_Recongize();
					}
				}
				if ((OPList[OPList_top - 1][0] == '$') && (OPList[OPList_top - 1][1] == 't'))
					strcpy(reg, OPList[OPList_top - 1]);
				OPList_top--;
				sym = word_Recongize();
			}
			else {
				ERROR_num++;
				printf("error 8: the symbol is wrong, the right symbol should be ] in line %d!\n", line);
				ERROR_num++;
				c = fgetc(fp);
				while (c != ';') {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						system("pause");
					}
					else
						c = fgetc(fp);
				}
				fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
		}
		else {
			ERROR_num++;
			printf("error 8: the symbol is wrong, the right symbol should be an expression in line %d!\n", line);
			ERROR_num++;
			c = fgetc(fp);
			while (c != ';') {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
	}
	if (sym == 27) {
		sym = word_Recongize();
		if (isfirst_expression(sym)) {
			sym = expression(sym);
			OPList_top--;
			right_type = expression_type;
			if (left_type != right_type) {
				ERROR_num++;
				printf("error 13: the left type and the right type in assign statement can't match in line %d!\n", line);
				fseek(fp, -1, SEEK_CUR);
				c = fgetc(fp);
				while (c != ';') {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						system("pause");
					}
					else
						c = fgetc(fp);
				}
				fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
			sprintf(CodeList[CodeList_top++], "%s = %s", idname, OPList[OPList_top]);
			if ((OPList[OPList_top][0] == '$') && (OPList[OPList_top][1] == 't'))
				free_tReg(OPList[OPList_top]);
			if ((reg[0] == '$') && (reg[1] == 't'))
				free_tReg(reg);
			return sym;
		}
		else {
			ERROR_num++;
			printf("error 8: the symbol is wrong, the right symbol should be an expression in line %d!\n", line);
			ERROR_num++;
			c = fgetc(fp);
			while (c != ';') {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
	}
	else {
		ERROR_num++;
		printf("error 8: the symbol is wrong, the right symbol should be a number in line %d!\n", line);
		ERROR_num++;
		c = fgetc(fp);
		while ((c != ';') && (c != '(')) {
			if (c == EOF) {
				printf("error 7: Can't find an right symbol before end!\n");
				system("pause");
			}
			else
				c = fgetc(fp);
		}
		fseek(fp, -1, SEEK_CUR);
		return word_Recongize();
	}
}

int scanf_statement(int sym)
{
	char c;
	char idname[IDENTNAMESIZE] = { 0 };
	sym = word_Recongize();
	if (sym == 30) {			//(
		sym = word_Recongize();
		if (sym == 36) {		//id
			sprintf(CodeList[CodeList_top++], "scan %s", token);
			sym = word_Recongize();
			while (sym == 26) {			//,
				sym = word_Recongize();
				if (sym == 36) {		//id
					sprintf(CodeList[CodeList_top++], "scan %s", token);
					sym = word_Recongize();
				}
				else {
					printf("error 8: the symbol is wrong, the right symbol should be an idname in line %d!\n", line);
					ERROR_num++;
					c = fgetc(fp);
					while ((c != ';')&&(c!=')')) {
						if (c == EOF) {
							printf("error 7: Can't find an right symbol before end!\n");
							system("pause");
						}
						else
							c = fgetc(fp);
					}
					fseek(fp, -1, SEEK_CUR);
					return word_Recongize();
				}
			}
			if (sym == 31) {		//)
				return word_Recongize();
			}
			else {
				printf("error 8: the symbol is wrong, the right symbol should be ) in line %d!\n", line);
				ERROR_num++;
				c = fgetc(fp);
				while ((c != ';') && (c != ')')) {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						system("pause");
					}
					else
						c = fgetc(fp);
				}
				fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
		}
		else {
			printf("error 8: the symbol is wrong, the right symbol should be an idname in line %d!\n", line);
			ERROR_num++;
			c = fgetc(fp);
			while ((c != ';') && (c != ')')) {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
	}
	else {
		printf("error 8: the symbol is wrong, the right symbol should be ( in line %d!\n", line);
		ERROR_num++;
		c = fgetc(fp);
		while ((c != ';') && (c != ')')) {
			if (c == EOF) {
				printf("error 7: Can't find an right symbol before end!\n");
				system("pause");
			}
			else
				c = fgetc(fp);
		}
		fseek(fp, -1, SEEK_CUR);
		return word_Recongize();
	}
}


int printf_statement(int sym)
{
	char c;
	if (sym == 12) {
		sym = word_Recongize();
		if (sym == 30) {		//(
			sym = word_Recongize();
			if (sym == 39) {		//string
				///////////
				sprintf(SymList[SymList_top].name, "String%d", StrList_top);
				SymList[SymList_top].kind = 0;
				SymList[SymList_top].type = 3;
				SymList[SymList_top].isarray = 0;
				SymList[SymList_top].lev = 0;
				SymList[SymList_top].value = StrList_top;
				SymList[SymList_top].index = 0;
				strcpy(StrList[StrList_top++], token);
				/////////////////
				sprintf(CodeList[CodeList_top++], "print %s", SymList[SymList_top].name);
				SymList_top++;
				sym = word_Recongize();
				if (sym == 26) {		//,
					sym = word_Recongize();
					if (isfirst_expression(sym)) {		//expression
						sym = expression(sym);
						if (sym == 31) {
							sprintf(CodeList[CodeList_top++], "print %s", OPList[--OPList_top]);
							if (OPList[OPList_top][0] == '$'&&OPList[OPList_top][1] == 't')
								free_tReg(OPList[OPList_top]);
							return word_Recongize();
						}
						else {
							printf("error 8: the symbol is wrong, the right symbol should be } in line %d!\n", line);
							ERROR_num++;
							c = fgetc(fp);
							while (c != ';') {
								if (c == EOF) {
									printf("error 7: Can't find an right symbol before end!\n");
									system("pause");
								}
								else
									c = fgetc(fp);
							}
							fseek(fp, -1, SEEK_CUR);
							return word_Recongize();
						}
					}
					else {
						printf("error 8: the symbol is wrong, the right symbol should be an expression in line %d!\n", line);
						ERROR_num++;
						c = fgetc(fp);
						while ((c != ';')&&(c!=')')) {
							if (c == EOF) {
								printf("error 7: Can't find an right symbol before end!\n");
								system("pause");
							}
							else
								c = fgetc(fp);
						}
						if (c==';')
							fseek(fp, -1, SEEK_CUR);
						return word_Recongize();
					}
				}
				else if (sym == 31) {		//)
					return word_Recongize();
				}
				else {
					printf("error 8: the symbol is wrong, the right symbol should be ) in line %d!\n", line);
					ERROR_num++;
					c = fgetc(fp);
					while (c != ';') {
						if (c == EOF) {
							printf("error 7: Can't find an right symbol before end!\n");
							system("pause");
						}
						else
							c = fgetc(fp);
					}
					fseek(fp, -1, SEEK_CUR);
					return word_Recongize();
				}
			}
			else if (isfirst_expression(sym)) {			//expression
				sym = expression(sym);
				sprintf(CodeList[CodeList_top++], "print %s", OPList[--OPList_top]);
				if (OPList[OPList_top][0] == '$'&&OPList[OPList_top][1] == 't')
					free_tReg(OPList[OPList_top]);
				if (sym == 31) {
					return word_Recongize();
				}
				else {
					printf("error 8: the symbol is wrong, the right symbol should be } in line %d!\n", line);
					ERROR_num++;
					c = fgetc(fp);
					while (c != ';') {
						if (c == EOF) {
							printf("error 7: Can't find an right symbol before end!\n");
							system("pause");
						}
						else
							c = fgetc(fp);
					}
					fseek(fp, -1, SEEK_CUR);
					return word_Recongize();
				}
			}
			else {
				printf("error 8: the symbol is wrong, the right symbol should be an expression in line %d!\n", line);
				ERROR_num++;
				c = fgetc(fp);
				while ((c != ';')&&(c!=')')) {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						system("pause");
					}
					else
						c = fgetc(fp);
				}
				if (c==';')
					fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
		}
		else {
			printf("error 8: the symbol is wrong, the right symbol should be ( in line %d!\n", line);
			ERROR_num++;
			c = fgetc(fp);
			while ((c != ';') && (c != ')')) {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			if (c == ';')
				fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
	}
}

int switch_statement(int sym)
{
	//char idname[IDENTNAMESIZE] = { 0 };
	char c;
	char end_label[20] = { 0 };
	int switch_type;
	sprintf(end_label, "end_label%d", LabelNum++);

	sym = word_Recongize();
	if (sym == 30) {				//(
		sym = word_Recongize();
		if (isfirst_expression(sym)) {			//expr
			sym = expression(sym);
			switch_type = expression_type;
			strcpy(OPList[OPList_top++], "==");
			if (sym == 31) {			//)
				sym = word_Recongize();
				if (sym == 34) {			//{
					sym = word_Recongize();
					if (sym == 9) {				//case
						sym = case_table(sym, end_label, switch_type);
						if (isfirst_default(sym)) {
							sym = default_statement(sym);
							OPList_top = OPList_top - 2;
							free_tReg(OPList[OPList_top]);
							sprintf(CodeList[CodeList_top++], "%s:", end_label);
							if (sym == 35) {
								return word_Recongize();
							}
							else {
								printf("error 8: the symbol is wrong, the right symbol should be } in line %d!\n", line);
								ERROR_num++;
								c = fgetc(fp);
								while (c != '}') {
									if (c == EOF) {
										printf("error 7: Can't find an right symbol before end!\n");
										system("pause");
									}
									else
										c = fgetc(fp);
								}
								//fseek(fp, -1, SEEK_CUR);
								return word_Recongize();
							}
						}
						else {
							printf("error 8: the symbol is wrong, the right symbol should be default in line %d!\n", line);
							ERROR_num++;
							c = fgetc(fp);
							while (c != '}') {
								if (c == EOF) {
									printf("error 7: Can't find an right symbol before end!\n");
									system("pause");
								}
								else
									c = fgetc(fp);
							}
							//fseek(fp, -1, SEEK_CUR);
							return word_Recongize();
						}
					}
					else {
						printf("error 8: the symbol is wrong, the right symbol should be case in line %d!\n", line);
						ERROR_num++;
						c = fgetc(fp);
						while (c != '}') {
							if (c == EOF) {
								printf("error 7: Can't find an right symbol before end!\n");
								system("pause");
							}
							else
								c = fgetc(fp);
						}
						//fseek(fp, -1, SEEK_CUR);
						return word_Recongize();
					}
				}
				else {
					printf("error 8: the symbol is wrong, the right symbol should be { in line %d!\n", line);
					ERROR_num++;
					c = fgetc(fp);
					while (c != ';') {
						if (c == EOF) {
							printf("error 7: Can't find an right symbol before end!\n");
							system("pause");
						}
						else
							c = fgetc(fp);
					}
					//fseek(fp, -1, SEEK_CUR);
					return word_Recongize();
				}
			}
			else {
				printf("error 8: the symbol is wrong, the right symbol should be ) in line %d!\n", line);
				ERROR_num++;
				c = fgetc(fp);
				while (c != ';') {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						system("pause");
					}
					else
						c = fgetc(fp);
				}
				//fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
		}
		else {
			printf("error 8: the symbol is wrong, the right symbol should be an expression in line %d!\n", line);
			ERROR_num++;
			c = fgetc(fp);
			while (c != ';') {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			//fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
	}
	else {
		printf("error 8: the symbol is wrong, the right symbol should be ( in line %d!\n", line);
		ERROR_num++;
		c = fgetc(fp);
		while (c != ';') {
			if (c == EOF) {
				printf("error 7: Can't find an right symbol before end!\n");
				system("pause");
			}
			else
				c = fgetc(fp);
		}
		//fseek(fp, -1, SEEK_CUR);
		return word_Recongize();
	}
}

int return_statement(int sym)
{
	char c;
	sym = word_Recongize();
	if (sym == 30) {		//(
		sym = word_Recongize();
		if (isfirst_expression(sym)) {
			sym = expression(sym);
			if (returnflag != expression_type) {
				ERROR_num++;
				printf("error 16: the return type is error in line %d!\n", line);
				fseek(fp, -1, SEEK_CUR);
				c = fgetc(fp);
				while (c != ';') {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						system("pause");
					}
					else
						c = fgetc(fp);
				}
				fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
			sprintf(CodeList[CodeList_top++], "ret %s", OPList[--OPList_top]);
			if (OPList[OPList_top][0] == '$'&&OPList[OPList_top][1] == 't')
				free_tReg(OPList[OPList_top]);
			if (sym == 31) {		//)
				return word_Recongize();
			}
			else {
				printf("error 8: the symbol is wrong, the right symbol should be ) in line %d!\n", line);
				ERROR_num++;
				c = fgetc(fp);
				while (c != ';') {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						system("pause");
					}
					else
						c = fgetc(fp);
				}
				fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
		}
		else {
			printf("error 8: the symbol is wrong, the right symbol should be an expression in line %d!\n", line);
			ERROR_num++;
			c = fgetc(fp);
			while (c != ';') {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
	}
	else if (sym == 25) {		//;
		if (returnflag != 0) {
			ERROR_num++;
			printf("error 16: the return type is error in line %d!\n", line);
			fseek(fp, -1, SEEK_CUR);
			c = fgetc(fp);
			while (c != ';') {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
		sprintf(CodeList[CodeList_top++], "ret.");
		return 25;
	}
	else {
		printf("error 8: the symbol is wrong, the right symbol should be ; in line %d!\n", line);
		ERROR_num++;
		c = fgetc(fp);
		while (c != ';') {
			if (c == EOF) {
				printf("error 7: Can't find an right symbol before end!\n");
				system("pause");
			}
			else
				c = fgetc(fp);
		}
		fseek(fp, -1, SEEK_CUR);
		return word_Recongize();
	}
}

int condition(int sym)
{
	int left_type, right_type;
	char c;
	if (isfirst_expression(sym)) {
		sym = expression(sym);
		left_type = expression_type;
		if (isop(sym)) {	//关系运算符
			sprintf(OPList[OPList_top++], "%s", getop(sym));
			sym = word_Recongize();
			if (isfirst_expression(sym)) {			//expression
				sym = expression(sym);	
				right_type = expression_type;
				if ((left_type != 1)||(right_type!=1)) {
					ERROR_num++;
					printf("error 14: the type in condition should be int in line %d!\n", line);
					fseek(fp, -1, SEEK_CUR);
					c = fgetc(fp);
					while (c != ')') {
						if (c == EOF) {
							printf("error 7: Can't find an right symbol before end!\n");
							system("pause");
						}
						else
							c = fgetc(fp);
					}
					fseek(fp, -1, SEEK_CUR);
					return word_Recongize();
				}
				sprintf(CodeList[CodeList_top++], "%s %s %s", OPList[OPList_top - 3], OPList[OPList_top - 2], OPList[OPList_top - 1]);
				if (OPList[OPList_top - 1][0] == '$'&&OPList[OPList_top - 1][1] == 't')
					free_tReg(OPList[OPList_top - 1]);
				if (OPList[OPList_top - 3][0] == '$'&&OPList[OPList_top - 3][1] == 't')
					free_tReg(OPList[OPList_top - 3]);
				OPList_top -= 3;
				return sym;
			}
			else {
				ERROR_num++;
				printf("error 8: the symbol is wrong, the right symbol should be an expression in line %d!\n", line);
				ERROR_num++;
				c = fgetc(fp);
				while (c != ')') {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						system("pause");
					}
					else
						c = fgetc(fp);
				}
				fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
		}
		else {
			if (left_type != 1) {
				printf("error 14: the type in condition should be int in line %d!\n",line);
				ERROR_num++;
				fseek(fp, -1, SEEK_CUR); 
				c = fgetc(fp);
				while (c != ')') {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						system("pause");
					}
					else
						c = fgetc(fp);
				}
				fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
			OPList_top--;
			sprintf(CodeList[CodeList_top++], "%s", OPList[OPList_top]);
			if (OPList[OPList_top][0] == '$'&&OPList[OPList_top][1] == 't')
				free_tReg(OPList[OPList_top]);
			return sym;
		}
	}
	else {
		printf("error 8: the symbol is wrong, the right symbol should be an expression in line %d!\n", line);
		ERROR_num++;
		c = fgetc(fp);
		while (c != ')') {
			if (c == EOF) {
				printf("error 7: Can't find an right symbol before end!\n");
				system("pause");
			}
			else
				c = fgetc(fp);
		}
		fseek(fp, -1, SEEK_CUR);
		return word_Recongize();
	}
}

int isop(int sym)
{
	if ((sym > 17) && (sym < 24))
		return 1;
	else
		return 0;
}

int value_para_list(int sym, int paranum, int index)
{
	char c;
	int paranum_all = paranum;
	int left_type, right_type;
	paranum = 0;
	if (isfirst_expression(sym)) {
		sym = expression(sym);

		if (SymList[index+paranum+1].type == expression_type) {
			sprintf(CodeList[CodeList_top++], "push %s", OPList[--OPList_top]);
			paranum++;
			if (OPList[OPList_top][0] == '$'&&OPList[OPList_top][1] == 't')
				free_tReg(OPList[OPList_top]);
		}
		else {
			ERROR_num++;
			printf("error 10: the no.%d parameter in line %d type is wrong!\n", paranum + 1, line);
			fseek(fp, -1, SEEK_CUR);
			c = fgetc(fp);
			while (c != ')') {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
		
		while (sym == 26) {				//,
			sym = word_Recongize();
			if (isfirst_expression(sym)) {
				sym = expression(sym);
				if (SymList[index +paranum+1].type == expression_type) {
					sprintf(CodeList[CodeList_top++], "push %s", OPList[--OPList_top]);
					paranum++;
					if (OPList[OPList_top][0] == '$'&&OPList[OPList_top][1] == 't')
						free_tReg(OPList[OPList_top]);
				}
				else {
					ERROR_num++;
					printf("error 10: the no.%d parameter in line %d type is wrong!\n", paranum + 1, line);
					c = fgetc(fp);
					while (c != ')') {
						if (c == EOF) {
							printf("error 7: Can't find an right symbol before end!\n");
							system("pause");
						}
						else
							c = fgetc(fp);
					}
					fseek(fp, -1, SEEK_CUR);
					return word_Recongize();
				}
			}
			else {
				//ERROR_num++;
				printf("error 8: the symbol is wrong, the right symbol should be an expression in line %d!\n", line);
				ERROR_num++;
				c = fgetc(fp);
				while (c != ')') {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						system("pause");
					}
					else
						c = fgetc(fp);
				}
				fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
		}
		if (sym == 31) {
			if (paranum==paranum_all)
				return 31;
			else {
				ERROR_num++;
				printf("error 11: the number of parameter is wrong in line %d!\n", line);
				c = fgetc(fp);
				while (c != ';') {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						system("pause");
					}
					else
						c = fgetc(fp);
				}
				//fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
		}
		else {
			ERROR_num++;
			printf("error 8: the symbol is wrong, the right symbol should be ) in line %d!\n", line);
			//ERROR_num++;
			c = fgetc(fp);
			while (c != ')') {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
	}
	else if (sym == 31) {
		if (paranum == paranum_all)
			return 31;
		else {
			ERROR_num++;
			printf("the number of parameter is wrong!\n");
			c = fgetc(fp);
			while (c != ';') {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
					
				else
					c = fgetc(fp);
			}
			//fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
	}
	else {
		ERROR_num++;
		printf("error 8: the symbol is wrong, the right symbol should be ) in line %d!\n", line);
		//ERROR_num++;
		c = fgetc(fp);
		while (c != ')') {
			if (c == EOF) {
				printf("error 7: Can't find an right symbol before end!\n");
				system("pause");
			}
			else
				c = fgetc(fp);
		}
		fseek(fp, -1, SEEK_CUR);
		return word_Recongize();
	}
}

int expression(int sym)
{
	if (sym == 14) {		//+
		sym = word_Recongize();
		sym = term(sym);
	}
	else if (sym == 15) {		//-
		sym = word_Recongize();
		strcpy(OPList[OPList_top++], "0");
		strcpy(OPList[OPList_top++], "-");
		sym = term(sym);
		Deal();
	}
	else {
		sym = term(sym);
	}
	while ((sym == 14) || (sym == 15)) {
	
		if (sym == 14)
			strcpy(OPList[OPList_top++], "+");
		else
			strcpy(OPList[OPList_top++], "-");
		sym = word_Recongize();
		sym = term(sym);
		Deal();
		expression_type = 1;
	}
	return sym;
}

int term(int sym)
{
	sym = factor(sym);
	while ((sym == 16) || (sym == 17)) {
	//	expression_type = 1;
		if (sym == 16)
			strcpy(OPList[OPList_top++], "*");
		else
			strcpy(OPList[OPList_top++], "/");
		sym = word_Recongize();
		sym = factor(sym);
		Deal();
		expression_type = 1;
	}
	return sym;
}

int factor(int sym)
{
	char c;
	char idname[IDENTNAMESIZE] = { 0 };
	char reg[10] = { 0 };
	int index;// = getSymIndex_intercode(token);
	if (sym == 38) {		//char
		sprintf(OPList[OPList_top++], "%d",token[1]);
		expression_type = 2;
		return word_Recongize();
	}
	else if (sym == 37) {		//num
		sprintf(OPList[OPList_top++], "%d",NUM0);
		expression_type = 1;
		return word_Recongize();
	}
	else if (sym == 14) {		//+
		sym = word_Recongize();
		if (sym == 37) {
			sprintf(OPList[OPList_top++], "%d", NUM0);
			expression_type = 1;
			return word_Recongize();
		}
	}
	else if (sym == 15) {		//-
		sym = word_Recongize();
		if (sym == 37) {
			sprintf(OPList[OPList_top++], "%d", NUM0*(-1));
			expression_type = 1;
			return word_Recongize();
		}
	}
	else if (sym == 30) {		//(
		sym = word_Recongize();
		if (isfirst_expression(sym)) {
			sym = expression(sym);
			if (sym == 31) {		//)
				expression_type = 1;
				return word_Recongize();
			}
			else {
				printf("error 8: the symbol is wrong, the right symbol should be ) in line %d!\n", line);
				ERROR_num++;
				c = fgetc(fp);
				while (c != ';') {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						system("pause");
					}
					else
						c = fgetc(fp);
				}
				fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
		}
		else {
			;/////////////error 不是表达式
		}
	}
	else if (sym == 36) {		//id
		strcpy(idname, token);
		
		if (id_use_find(idname)) {
			sym = word_Recongize();
			if (sym == 30) {		//有返回值调用
				sym = func_call_factor(sym);
				expression_type = SymList[getSymIndex_intercode(idname)].type;
				return sym;
			}
			else if (sym == 32) {		//数组
				char arrayname[IDENTNAMESIZE];
				strcpy(arrayname, idname);
				sym = word_Recongize();
				if (isfirst_expression(sym)) {
					sym = expression(sym);
					if (sym == 33) {		//]
						//////////////
						sprintf(idname, "%s[%s]", idname, OPList[--OPList_top]);
						if (isnumber(OPList[OPList_top][0])) {
							NUM0 = atoi(OPList[OPList_top]);
							if (NUM0 >= SymList[getSymIndex_intercode(arrayname)].value) {
								printf("error 19: the array is not so big in line %d!\n", line);
								ERROR_num++;
								c = fgetc(fp);
								while (c != ';') {
									if (c == EOF) {
										printf("error 7: Can't find an right symbol before end!\n");
										system("pause");
									}
									else
										c = fgetc(fp);
								}
								fseek(fp, -1, SEEK_CUR);
								return word_Recongize();
							}
						}
						if ((OPList[OPList_top + 1][0] == '$') && (OPList[OPList_top + 1][1] == 't'))
							free_tReg(OPList[OPList_top + 1]);
						strcpy(reg, get_tReg());
						sprintf(CodeList[CodeList_top++], "%s = %s", reg, idname);
						strcpy(OPList[OPList_top++], reg);
						expression_type = SymList[getSymIndex_intercode(arrayname)].type;
						return word_Recongize();
					}
					else {
						printf("error 8: the symbol is wrong, the right symbol should be ] in line %d!\n", line);
						ERROR_num++;
						c = fgetc(fp);
						while (c != ';') {
							if (c == EOF) {
								printf("error 7: Can't find an right symbol before end!\n");
								system("pause");
							}
							else
								c = fgetc(fp);
						}
						fseek(fp, -1, SEEK_CUR);
						return word_Recongize();
					}
				}
			}
			else {			//id
				strcpy(OPList[OPList_top++], idname);
				expression_type = SymList[getSymIndex_intercode(idname)].type;
				return sym;
			}
		}
		else {
			printf("error 18: the idname \"%s\" in line %d is not defined in Symlist!\n", idname, line);
			ERROR_num++;
			sym = word_Recongize();
			if (sym == 32) {
				c = fgetc(fp);
				while (c != ']') {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						system("pause");
					}
					else
						c = fgetc(fp);
				}
				//fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
			else if (sym == 30) {
				c = fgetc(fp);
				while (c != '}') {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						system("pause");
					}
					else
						c = fgetc(fp);
				}
				//fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
			else {
				return word_Recongize();
			}

		}
	}
	else {
		;/////////////error 不是因子
	}
}

int isfirst_expression(int sym)
{
	if ((sym == 14) || (sym == 15) || (sym == 30) || (sym == 36) || (sym == 37) || (sym == 38))
		return 1;
	else
		return 0;
}

int case_table(int sym, char* end_label, int switch_type)
{
	char c;
	char label[20] = { 0 };
	int case_type;
	while (sym == 9) {			//case
		sym = word_Recongize();
		if (sym == 37) {			//常量
			sprintf(OPList[OPList_top++], "%d", NUM0);
			case_type = 1;
		}
		else if (sym == 14) {
			sym = word_Recongize();
			if (sym == 37) {
				sprintf(OPList[OPList_top++], "%d", NUM0);
				case_type = 1;
			}
		}
		else if (sym == 15) {
			sym = word_Recongize();
			if (sym == 37) {
				sprintf(OPList[OPList_top++], "%d", NUM0*(-1));
				case_type = 1;
			}
		}
		else if (sym == 38) {
			sprintf(OPList[OPList_top++], "%d", token[1]);
			case_type = 2;
		}
		else {
			;//////////////error
		}
		if (switch_type == case_type) {
			sprintf(CodeList[CodeList_top++], "%s %s %s", OPList[OPList_top - 3], OPList[OPList_top - 2], OPList[OPList_top - 1]);
			if (OPList[OPList_top - 1][0] == '$'&&OPList[OPList_top - 1][1] == 't')
				free_tReg(OPList[OPList_top - 1]);
			OPList_top -= 1;
			sprintf(label, "label%d", LabelNum++);
			sprintf(CodeList[CodeList_top++], "BZ %s", label);
			sym = word_Recongize();
		}
		else {
			ERROR_num++;
			printf("error 12: the symbol in switch type is %d, but in case type is %d in line %d!\n", switch_type, case_type, line);
			c = fgetc(fp);
			while (c != ';') {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			//fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
		if (sym == 24) {			//：
			sym = word_Recongize();
			if (isfirst_statement(sym)) {
				sym = statement(sym);
				sprintf(CodeList[CodeList_top++], "GOTO %s", end_label);
				sprintf(CodeList[CodeList_top++], "%s:", label);
			}
			else {
				;/////////////error 不是语句
			}
		}
		else {
			ERROR_num++;
			printf("error 8: the symbol is wrong, the right symbol should be ; in line %d!\n", line);
			//ERROR_num++;
			c = fgetc(fp);
			while (c != ';') {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
	}
	return sym;
}

int default_statement(int sym)
{
	char c;
	if (sym == 10) {	//default
		
		sym = word_Recongize();
		if (sym == 24) {
			sym = word_Recongize();
			if (isfirst_statement(sym)) {
				sym = statement(sym);
				return sym;
			}
			else {
				printf("error 8: the symbol is wrong, the right symbol should be a statement in line %d!\n", line);
				ERROR_num++;
				c = fgetc(fp);
				while (c != ')') {
					if (c == EOF) {
						printf("error 7: Can't find an right symbol before end!\n");
						system("pause");
					}
					else
						c = fgetc(fp);
				}
				fseek(fp, -1, SEEK_CUR);
				return word_Recongize();
			}
		}
		else {
			printf("error 8: the symbol is wrong, the right symbol should be : in line %d!\n", line);
			ERROR_num++;
			c = fgetc(fp);
			while (c != ')') {
				if (c == EOF) {
					printf("error 7: Can't find an right symbol before end!\n");
					system("pause");
				}
				else
					c = fgetc(fp);
			}
			fseek(fp, -1, SEEK_CUR);
			return word_Recongize();
		}
	}
	else if (sym == 35) {
		return sym;
	}
	else {
		printf("error 8: the symbol is wrong, the right symbol should be } in line %d!\n", line);
		ERROR_num++;
		c = fgetc(fp);
		while (c != '}') {
			if (c == EOF) {
				printf("error 7: Can't find an right symbol before end!\n");
				system("pause");
			}
			else
				c = fgetc(fp);
		}
		fseek(fp, -1, SEEK_CUR);
		return word_Recongize();
	}
}

int isfirst_default(int sym)
{
	if ((sym == 10)||(sym == 35))
		return 1;
	else
		return 0;
}

int isconst(int sym)
{
	if (sym == 37) {
		return NUM0;
	}
	else if (sym == 14) {
		sym = word_Recongize();
		if (sym == 37)
			return NUM0;
		else {
			return 0;
		}
	}
	else if (sym == 15) {
		sym = word_Recongize();
		if (sym == 37)
			return NUM0*(-1);
		else {
			return 0;
		}
	}
	else if (sym == 38)
		return token[1];
	else
		return 0;
}

int extra_stc(int sym)
{
	char c;
	//ERROR_num++;
	printf("there are some sentence not in the main function in line %d!\n", line);
	ERROR_num++;
	sym = word_Recongize();
	c = fgetc(fp);
	while (c != EOF) {
		c = fgetc(fp);
	}
	return 0;
}

int id_def_check(char * idname, int level)
{
	char c;
	int i = 0;
	for (i = 0; ((i < SymList_top) && (SymList[i].lev == level)); i++) {
		if (strcmp(idname, SymList[i].name) == 0)
			return 1;
	}
	return 0;
}

char * get_kind(int i)
{
	switch (i) {
	case 0: return "const";
	case 1: return "var";
	case 2: return "function";
	case 3: return "parameter";
	//default: return "";		//error
	}
}

void Deal()
{
	char reg[10] = { 0 };
	if (OPList[OPList_top - 3][0] == '$'&&OPList[OPList_top - 3][1] == 't')
		strcpy(reg, OPList[OPList_top - 3]);
	else
		strcpy(reg,get_tReg());
	sprintf(CodeList[CodeList_top++], "%s = %s %s %s", reg, OPList[OPList_top - 3], OPList[OPList_top - 2], OPList[OPList_top - 1]);
	if (OPList[OPList_top - 1][0] == '$'&&OPList[OPList_top - 1][1] == 't')
		free_tReg(OPList[OPList_top - 1]);

	OPList_top -= 2;
	strcpy(OPList[OPList_top - 1], reg);

}

int id_use_find(char* name)		//变量使用时查表，未查到变量声明报错
{
	int i;
	for (i = 0; (i < SymList_top); i++)
	{
		if ((stricmp(name, SymList[i].name) == 0) && (Level_top == SymList[i].lev)){
			return 1;
		}
	}
	for (i = 0; (i < SymList_top); i++)
	{
		if ((stricmp(name, SymList[i].name) == 0) && (0 == SymList[i].lev)) {
			return 1;
		}
	}
	return 0;
}

char * load_reg(char * idname)
{
	char ret[IDENTNAMESIZE] = { 0 };
	char reg1[20] = { 0 };
	char reg2[20] = { 0 };
	char reg3[20] = { 0 };
	int index = getSymIndex_mips(idname);
	if (idname[0]=='$') {
		return idname;
	}
	else if (strcmp(idname, "RET")==0) {
		strcpy(ret, "$v0");
		return ret;
	}
	else if ((isnumber(idname[0]))||(idname[0] == '-')) {
		strcpy(ret, get_sReg());
		sprintf(MIPS_text[MIPS_text_top++], "li %s, %s", ret, idname);
		return ret;
	}
	else if (index == -1) {
		char* p = strchr(idname, '[');
		char* q = strchr(idname, ']');
		if (q != NULL)
			*q = '\0';
		strcpy(reg2, load_reg(p + 1));
		*p = '\0';
		index = getSymIndex_mips(idname);
		if (index == -1)
		{
			;/////////error
		}
		else if (SymList[index].lev == 0)
		{
			strcpy(reg1, get_sReg());
			strcpy(reg3, get_sReg());
			sprintf(MIPS_text[MIPS_text_top++], "la	%s, %s", reg1, idname);
			sprintf(MIPS_text[MIPS_text_top++], "sll %s, %s, 2", reg3, reg2);
			sprintf(MIPS_text[MIPS_text_top++], "add %s, %s, %s", reg1, reg1, reg3);
			sprintf(MIPS_text[MIPS_text_top++], "lw %s, 0(%s)", reg2, reg1);
			free_sReg(reg3);
			free_sReg(reg1);
			return reg2;
		}
		else
		{
			strcpy(reg1, get_sReg());
			strcpy(reg3, get_sReg());
			sprintf(MIPS_text[MIPS_text_top++], "subi %s, $s7, %d", reg1, (SymList[index].index + 4 * SymList[index].value));
			sprintf(MIPS_text[MIPS_text_top++], "sll %s, %s, 2", reg3, reg2);
			sprintf(MIPS_text[MIPS_text_top++], "add %s, %s, %s", reg1, reg1, reg3);
			sprintf(MIPS_text[MIPS_text_top++], "lw %s, 0(%s)", reg2, reg1);
			free_sReg(reg3);
			free_sReg(reg1);
			return reg2;
		}
	}
	else if (SymList[index].lev == 0)		//是全局变量或常量
	{
		strcpy(reg1, get_sReg());
		sprintf(MIPS_text[MIPS_text_top++], "la %s, %s", reg1, idname);
		strcpy(ret, get_sReg());
		sprintf(MIPS_text[MIPS_text_top++], "lw %s, 0(%s)", ret, reg1);
		free_sReg(reg1);
		return ret;
	}
	else if (SymList[index].lev>0)			//是局部变量
	{
		if (SymList[index].kind != 0)
		{
			strcpy(reg1, get_sReg());
			sprintf(MIPS_text[MIPS_text_top++], "subi %s, $s7, %d", reg1, SymList[index].index + 4);
			strcpy(ret, get_sReg());
			sprintf(MIPS_text[MIPS_text_top++], "lw %s, 0(%s)", ret, reg1);
			free_sReg(reg1);
		}
		else
		{
			strcpy(ret, get_sReg());
			sprintf(MIPS_text[MIPS_text_top++], "li %s, %d", ret, SymList[index].value);
		}
		return ret;
	}
}

char * save_reg(char * idname,char* value)
{
	char reg1[10];
	char reg2[10];
	char reg3[10];
	int index;
	int num;
	char ret[20];
	index = getSymIndex_mips(idname);

	if (idname[0] == '$')					//是寄存器
	{
		sprintf(MIPS_text[MIPS_text_top++], "move %s, %s", idname, value);
		return;
	}
	else if (index == -1)				//是数组
	{
		char* p = strchr(idname, '[');			//读取数组内容
		if (p)
		{
			char* q = strchr(idname, ']');
			if (q)
				*q = '\0';
			strcpy(reg2, load_reg(p + 1));
			*p = '\0';
			index = getSymIndex_mips(idname);
			if (index == -1)
			{
				;/////////////error
			}
			else if (SymList[index].lev == 0)
			{
				strcpy(reg1, get_sReg());
				strcpy(reg3, get_sReg());
				sprintf(MIPS_text[MIPS_text_top++], "la	%s, %s", reg1, idname);
				sprintf(MIPS_text[MIPS_text_top++], "sll %s, %s, 2", reg3, reg2);
				sprintf(MIPS_text[MIPS_text_top++], "add %s, %s, %s", reg1, reg1, reg3);
				sprintf(MIPS_text[MIPS_text_top++], "sw %s, 0(%s)", value, reg1);
				free_sReg(reg3);
				free_sReg(reg2);
				free_sReg(reg1);
				return;
			}
			else
			{
				strcpy(reg1, get_sReg());
				strcpy(reg3, get_sReg());
				sprintf(MIPS_text[MIPS_text_top++], "subi %s, $s7, %d", reg1, (SymList[index].index + 4 * SymList[index].value));
				sprintf(MIPS_text[MIPS_text_top++], "sll %s, %s, 2", reg3, reg2);
				sprintf(MIPS_text[MIPS_text_top++], "add %s, %s, %s", reg1, reg1, reg3);
				sprintf(MIPS_text[MIPS_text_top++], "sw %s, 0(%s)", value, reg1);
				free_sReg(reg3);
				free_sReg(reg2);
				free_sReg(reg1);
				return;
			}
		}
	}
	else if (SymList[index].lev == 0)		//是全局变量
	{
		strcpy(reg1, get_sReg());
		sprintf(MIPS_text[MIPS_text_top++], "la	%s, %s", reg1, idname);
		sprintf(MIPS_text[MIPS_text_top++], "sw %s, 0(%s)", value, reg1);
		free_sReg(reg1);
		return;
	}
	else if (SymList[index].lev>0)			//是局部变量
	{
		strcpy(reg1, get_sReg());
		sprintf(MIPS_text[MIPS_text_top++], "subi %s, $s7, %d", reg1, SymList[index].index + 4);
		sprintf(MIPS_text[MIPS_text_top++], "sw %s, 0(%s)", value, reg1);
		free_sReg(reg1);
		return;
	}
	else
	{
		;///////////error
	}
}

char* get_tReg()
{
	//static int t = 0;
	char n[5];
	int i;
	for (i = 0; i <= MAXTREG; i++)
	{
		if (tReg[i] == 0)
		{
			tReg[i] = 1;
			sprintf(n, "$t%d", i);
			return n;
		}
	}
	printf("error 20: T Reg is Full To Use!!\n");
	ERROR_num++;
}

char* get_sReg()
{
	//static int t = 0;
	char n[5];
	int i;
	for (i = 0; i <= MAXSREG; i++)
	{
		if (sReg[i] == 0)
		{
			sReg[i] = 1;
			sprintf(n, "$s%d", i);
			return n;
		}
	}
	printf("error 22: S Reg is Full To Use!!\n");
	ERROR_num++;
	//return n;
}

void  free_tReg(char reg[])		//释放t寄存器
{
	int i = atoi(reg + 2);
	if ((reg[1] == 't') && (reg[0] == '$'))
	{
		if (tReg[i] == 1)
			tReg[i] = 0;
		else
			printf("error 21: $t%d Is Not Used...\n", i);
	}
}

void  free_sReg(char reg[])		//释放s寄存器
{
	int i = atoi(reg + 2);
	if ((reg[1] == 's')&&(reg[0]=='$'))
	{
		if (sReg[i] == 1)
			sReg[i] = 0;
		else
			printf("error 23: $s%d Is Not Used...\n", i);
	}
}

void MIPS_create_data()
{
	int i = 0;
	sprintf(MIPS_data[MIPS_data_top++], ".data");
	for (i = 0; i < SymList_top; i++) {
		if (SymList[i].lev == 0) {
			if (SymList[i].kind == 0) {
				if (SymList[i].type == 1) {
					sprintf(MIPS_data[MIPS_data_top++], "%s: .word %d", SymList[i].name, SymList[i].value);
				}
				else if (SymList[i].type == 2) {
					sprintf(MIPS_data[MIPS_data_top++], "%s: .word %d", SymList[i].name, SymList[i].value);
				}
				else if (SymList[i].type==3){
					sprintf(MIPS_data[MIPS_data_top++], "%s: .asciiz %s", SymList[i].name, StrList[SymList[i].value]);
				}
			}
			else if (SymList[i].kind == 1) {
				if (SymList[i].isarray == 0) {
					sprintf(MIPS_data[MIPS_data_top++], "%s: .space 4", SymList[i].name);
				}
				else if (SymList[i].isarray == 1) {
					sprintf(MIPS_data[MIPS_data_top++], "%s: .space %d", SymList[i].name, SymList[i].value*4);
				}
			}
		}
	}
}

void MIPS_create_text()
{
	int codeNum = 0;
	int para = 0;
	int pushnum = 0;
	int VarSpaceSize = 0;
	int index;
	char code_op[30] = { 0 };
	char mips_op[20] = { 0 };
	char rs[20] = { 0 }, rd[20] = { 0 }, rt[20] = { 0 };
	char label[20] = { 0 };
	char reg1[20] = { 0 };
	char reg2[20] = { 0 };
	char reg3[20] = { 0 };
	char idname[IDENTNAMESIZE] = { 0 };
	char r1[30], r2[30];
	char rr1[30], rr2[30], rr3[30];
	int tregi;

	sprintf(MIPS_text[MIPS_text_top++], ".text");
	VarSpaceSize = FuncStackTop[SymList[getSymIndex_mips("main")].value] + 48;
	sprintf(MIPS_text[MIPS_text_top++], "subi $sp, $sp, %d", VarSpaceSize);
	sprintf(MIPS_text[MIPS_text_top++], "j main");

	for (codeNum = 0; codeNum < CodeList_top; codeNum++) {
		para = div_factor(CodeList[codeNum]);
		if (para == 1) {										//ret
			if (strcmp(CodeList[codeNum], "ret.") == 0) {
				sprintf(MIPS_text[MIPS_text_top++], "jr $ra");
			}
			
			else if (strchr(CodeList[codeNum], ':') != NULL) {				//label:
				sprintf(MIPS_text[MIPS_text_top++], CodeList[codeNum]);
			}
			else {													//t0 
				strcpy(reg1, CodeList[codeNum]);
				sscanf(CodeList[codeNum + 1], "%s %s", code_op, label);
				if (strcmp(code_op, "BNZ") == 0) {
					strcpy(mips_op, "bne");
					sprintf(MIPS_text[MIPS_text_top++], "%s %s, $0, %s", mips_op, reg1, label);
					free_sReg(reg1);
				}
				else if (strcmp(code_op, "BZ") == 0) {
					strcpy(mips_op, "beq");
					sprintf(MIPS_text[MIPS_text_top++], "%s %s, $0, %s", mips_op, reg1, label);
					free_sReg(reg1);
				}
				
			}
		}
		else if (para == 2) {			//2
			sscanf(CodeList[codeNum], "%s %s", code_op, idname);			//操作符 id
			if (strcmp(code_op, "GOTO") == 0) {				//goto
				sprintf(MIPS_text[MIPS_text_top++], "j %s", idname);
			}
			else if (strcmp(code_op, "scan") == 0) {		//scanf
				int type = SymList[getSymIndex_mips(idname)].type;
				if (type == 1)
				{
					sprintf(MIPS_text[MIPS_text_top++], "li $v0, 5");
					sprintf(MIPS_text[MIPS_text_top++], "syscall");
					save_reg(idname, "$v0");							//int
				}
				else if (type == 2)
				{
					sprintf(MIPS_text[MIPS_text_top++], "li $v0, 12");	
					sprintf(MIPS_text[MIPS_text_top++], "syscall");
					save_reg(idname, "$v0");							//char
				}
				
			}
			else if (strcmp(code_op, "push") == 0) {				//传参
				sprintf(MIPS_text[MIPS_text_top++], "subi $sp, $sp, 4");
				strcpy(reg1, load_reg(idname));
				sprintf(MIPS_text[MIPS_text_top++], "sw %s, 0($sp)", reg1);
				free_sReg(reg1);
				pushnum++;
			}
			else if (strcmp(code_op, "call") == 0) {
				int offset = pushnum * 4;		//保存当前上下文时的偏移量
				pushnum = pushnum - FuncParaNum[SymList[getSymIndex_mips(idname)].value];
				for (tregi = 0; tregi < 10; tregi++) {
					if (tReg[tregi] == 1) {
						sprintf(MIPS_text[MIPS_text_top++], "sw $t%d,%d($sp)", tregi, offset + 4 * tregi);
					}
				}
				sprintf(MIPS_text[MIPS_text_top++], "sw $ra, %d($sp)", 40 + offset);
				sprintf(MIPS_text[MIPS_text_top++], "sw $s7, %d($sp)", 44 + offset);
				sprintf(MIPS_text[MIPS_text_top++], "jal %s", idname);
				sprintf(MIPS_text[MIPS_text_top++], "move $sp, $s7");			//恢复栈指针

				offset = pushnum * 4;													//保存当前上下文时的偏移量
				
				for (tregi = 0; tregi < 10; tregi++) {
					if (tReg[tregi] == 1) {
						sprintf(MIPS_text[MIPS_text_top++], "lw $t%d,%d($sp)", tregi ,offset + 4 * tregi);
					}
				}
				sprintf(MIPS_text[MIPS_text_top++], "lw $ra, %d($sp)", 40 + offset);

				sprintf(MIPS_text[MIPS_text_top++], "lw $s7, %d($sp)", 44 + offset);	//恢复$s7寄存器
			}
			else if (strcmp(code_op, "print") == 0) {				//printf
				index = getSymIndex_mips(idname);
				if (index == -1)				//是数组
				{
					char* p = strchr(idname, '[');			//读取数组内容
					if (isnumber(idname[0]))
					{
						sprintf(MIPS_text[MIPS_text_top++], "li $a0, %s", idname);
						sprintf(MIPS_text[MIPS_text_top++], "li $v0, 1");
						sprintf(MIPS_text[MIPS_text_top++], "syscall");
					}
					else if (idname[0] == '$')					//是寄存器
					{
						r2type = FindRegType(idname, codeNum);
						sprintf(MIPS_text[MIPS_text_top++], "move $a0, %s", idname);
						if (r2type == 2)			//如果是字符类型返回值
						{
							sprintf(MIPS_text[MIPS_text_top++], "li $v0, 11");
						}
						else
						{
							sprintf(MIPS_text[MIPS_text_top++], "li $v0, 1");
						}
						sprintf(MIPS_text[MIPS_text_top++], "syscall");
					}
					else if (p)
					{
						char* q = strchr(idname, ']');
						*q = '\0';
						strcpy(reg2, getSymIndex_mips(p + 1));
						*p = '\0';
						index = getSymIndex_mips(idname);
						if (index == -1)
						{
							;//////////error
						}
						else if (SymList[index].lev == 0)
						{
							strcpy(reg1, get_sReg());
							sprintf(MIPS_text[MIPS_text_top++], "la %s, %s", reg1, strlwr(idname));
							sprintf(MIPS_text[MIPS_text_top++], "sll %s, %s, 2", reg2, reg2);
							sprintf(MIPS_text[MIPS_text_top++], "add %s, %s, %s", reg1, reg1, reg2);
							sprintf(MIPS_text[MIPS_text_top++], "lw	%s, 0(%s)", reg2, reg1);
							free_sReg(reg1);
							sprintf(MIPS_text[MIPS_text_top++], "move $a0, %s", reg2);
							if (SymList[index].type == 1)
								sprintf(MIPS_text[MIPS_text_top++], "li $v0, 1");
							else if (SymList[index].type == 2)
								sprintf(MIPS_text[MIPS_text_top++], "li $v0, 11");
							sprintf(MIPS_text[MIPS_text_top++], "syscall");
							free_sReg(reg2);
						}
						else
						{
							strcpy(reg1, get_sReg());
							sprintf(MIPS_text[MIPS_text_top++], "subi %s, $s7, %d", reg1, (SymList[index].index + 4 * SymList[index].value));
							sprintf(MIPS_text[MIPS_text_top++], "sll %s, %s, 2", reg2, reg2);
							sprintf(MIPS_text[MIPS_text_top++], "add %s, %s, %s", reg1, reg1, reg2);
							sprintf(MIPS_text[MIPS_text_top++], "lw %s, 0(%s)", reg2, reg1);
							free_sReg(reg1);
							sprintf(MIPS_text[MIPS_text_top++], "move $a0, %s", reg2);
							if (SymList[index].type == 1)
								sprintf(MIPS_text[MIPS_text_top++], "li $v0, 1");
							else if (SymList[index].type == 2)
								sprintf(MIPS_text[MIPS_text_top++], "li $v0, 11");
							sprintf(MIPS_text[MIPS_text_top++], "syscall");
							free_sReg(reg2);
						}
					}
					else
					{
						strcpy(reg1, get_sReg());
						sprintf(MIPS_text[MIPS_text_top++], "la %s, %s", reg1, idname);
						sprintf(MIPS_text[MIPS_text_top++], "move $a0, %s", reg1);
						sprintf(MIPS_text[MIPS_text_top++], "li $v0, 4");
						sprintf(MIPS_text[MIPS_text_top++], "syscall");
						free_sReg(reg1);
					}
				}
				else if (SymList[index].type == 1)			//如果是数字
				{
					strcpy(reg1, load_reg(idname));
					sprintf(MIPS_text[MIPS_text_top++], "move $a0, %s", reg1);
					sprintf(MIPS_text[MIPS_text_top++], "li $v0, 1");
					sprintf(MIPS_text[MIPS_text_top++], "syscall");
					free_sReg(reg1);
				}
				else if (SymList[index].type == 2)			//如果是字符
				{
					strcpy(reg1, load_reg(idname));
					sprintf(MIPS_text[MIPS_text_top++], "move $a0, %s", reg1);
					sprintf(MIPS_text[MIPS_text_top++], "li $v0, 11");
					sprintf(MIPS_text[MIPS_text_top++], "syscall");
					free_sReg(reg1);
				}
				else if (SymList[index].type == 3)			//如果是字符串
				{
					strcpy(reg1, get_sReg());
					sprintf(MIPS_text[MIPS_text_top++], "la %s, %s", reg1, idname);
					sprintf(MIPS_text[MIPS_text_top++], "move $a0, %s", reg1);
					sprintf(MIPS_text[MIPS_text_top++], "li $v0, 4");
					sprintf(MIPS_text[MIPS_text_top++], "syscall");
					free_sReg(reg1);
				}
				else
				{
					;////////error
				}
			}
			else if (strcmp(code_op, "ret") == 0) {
				strcpy(reg1, load_reg(idname));
				sprintf(MIPS_text[MIPS_text_top++], "move $v0, %s", reg1);		//将返回值存入$v0中
				free_sReg(reg1);
				sprintf(MIPS_text[MIPS_text_top++], "jr $ra");
			}
			else if ((strcmp(code_op, "int") == 0)||(strcmp(code_op, "char")==0)||(strcmp(code_op, "void")==0)) {
				char* p = strchr(idname, '(');
				*p = '\0';
				currentlev = SymList[getSymIndex_mips(idname)].value;
				sprintf(MIPS_text[MIPS_text_top++], "%s:", idname);

				sprintf(MIPS_text[MIPS_text_top++], "subi $sp, $sp, %d",FuncStackTop[SymList[getSymIndex_mips(idname)].value] - 4 * FuncParaNum[SymList[getSymIndex_mips(idname)].value] + 48);
				sprintf(MIPS_text[MIPS_text_top++], "addi $s7, $sp, %d", FuncStackTop[SymList[getSymIndex_mips(idname)].value] + 48);
			}
			
		}
		else if (para == 3) {
			
			sscanf(CodeList[codeNum], "%s %s %s", r1, code_op, r2);
			if (strcmp(code_op, "=") == 0)		//赋值语句 NA
			{
				strcpy(reg3, load_reg(r2));
				save_reg(r1, reg3);
				free_sReg(reg3);
			}
			else if (strcmp(code_op, "==") == 0)		//等于 NA
			{
				sscanf(CodeList[codeNum + 1], "%s %s", code_op, label);
				strcpy(reg1, load_reg(r1));
				strcpy(reg3, load_reg(r2));
				if (strcmp(code_op, "BZ") == 0)
				{
					sprintf(MIPS_text[MIPS_text_top++], "bne %s, %s, %s", reg1, reg3, label);
				}
				else if (strcmp(code_op, "BNZ") == 0)
				{
					sprintf(MIPS_text[MIPS_text_top++], "beq %s, %s, %s", reg1, reg3, label);
				}
				else
					;/////////err0r
				codeNum++;
				free_sReg(reg1);
				free_sReg(reg3);
			}
			else if (strcmp(code_op, "!=") == 0)		//不等于 NA
			{
				sscanf(CodeList[codeNum + 1], "%s %s", code_op, label);
				strcpy(reg1, load_reg(r1));
				strcpy(reg3, load_reg(r2));
				if (strcmp(code_op, "BZ") == 0)
				{
					sprintf(MIPS_text[MIPS_text_top++], "beq %s, %s, %s", reg1, reg3, label);
				}
				else if (strcmp(code_op, "BNZ") == 0)
				{
					sprintf(MIPS_text[MIPS_text_top++], "bne %s, %s, %s", reg1, reg3, label);
				}
				else
					;/////////err0r
				codeNum++;
				free_sReg(reg1);
				free_sReg(reg3);
			}
			else if (strcmp(code_op, ">=") == 0)		//大于等于 NA
			{
				strcpy(reg1, get_sReg());
				strcpy(rr1, load_reg(r1));
				strcpy(rr3, load_reg(r2));
				sprintf(MIPS_text[MIPS_text_top++], "sub %s, %s, %s", reg1, rr1, rr3);
				free_sReg(rr1);
				free_sReg(rr3);

				sscanf(CodeList[codeNum + 1], "%s %s", code_op, label);
				if (strcmp(code_op, "BZ") == 0)
				{
					sprintf(MIPS_text[MIPS_text_top++], "bltz %s, %s", reg1, label);
					free_sReg(reg1);
				}
				else if (strcmp(code_op, "BNZ") == 0)
				{
					sprintf(MIPS_text[MIPS_text_top++], "bgez %s, %s", reg1, label);
					free_sReg(reg1);
				}
				else
					//CodeError(codeNum);
				codeNum++;
			}
			else if (strcmp(code_op, "<=") == 0)		//小于等于 NA
			{
				char op[20], label[20];

				strcpy(reg1, get_sReg());
				strcpy(rr1, load_reg(r1));
				strcpy(rr3, load_reg(r2));
				sprintf(MIPS_text[MIPS_text_top++], "sub %s, %s, %s", reg1, rr1, rr3);
				free_sReg(rr1);
				free_sReg(rr3);

				sscanf(CodeList[codeNum + 1], "%s %s", op, label);
				if (strcmp(op, "BZ") == 0)
				{
					sprintf(MIPS_text[MIPS_text_top++], "bgtz %s, %s", reg1, label);
					free_sReg(reg1);
				}
				else if (strcmp(op, "BNZ") == 0)
				{
					sprintf(MIPS_text[MIPS_text_top++], "blez %s, %s", reg1, label);
					free_sReg(reg1);
				}
				else
					//CodeError(codeNum);
				codeNum++;
			}
			else if (strcmp(code_op, ">") == 0)		//大于 NA
			{
				char op[20], label[20];

				strcpy(reg1, get_sReg());
				strcpy(rr1, load_reg(r1));
				strcpy(rr3, load_reg(r2));
				sprintf(MIPS_text[MIPS_text_top++], "sub %s, %s, %s", reg1, rr1, rr3);
				free_sReg(rr1);
				free_sReg(rr3);

				sscanf(CodeList[codeNum + 1], "%s %s", op, label);
				if (strcmp(op, "BZ") == 0)
				{
					sprintf(MIPS_text[MIPS_text_top++], "blez %s, %s", reg1, label);
					free_sReg(reg1);
				}
				else if (strcmp(op, "BNZ") == 0)
				{
					sprintf(MIPS_text[MIPS_text_top++], "bgtz %s, %s", reg1, label);
					free_sReg(reg1);
				}
				else
					//CodeError(codeNum);
				codeNum++;
			}
			else if (strcmp(code_op, "<") == 0)		//小于 NA
			{
				char op[20], label[20];

				strcpy(reg1, get_sReg());
				strcpy(rr1, load_reg(r1));
				strcpy(rr3, load_reg(r2));
				sprintf(MIPS_text[MIPS_text_top++], "sub %s, %s, %s", reg1, rr1, rr3);
				free_sReg(rr1);
				free_sReg(rr3);

				sscanf(CodeList[codeNum + 1], "%s %s", op, label);
				if (strcmp(op, "BZ") == 0)
				{
					sprintf(MIPS_text[MIPS_text_top++], "bgez %s, %s", reg1, label);
					free_sReg(reg1);
				}
				else if (strcmp(op, "BNZ") == 0)
				{
					sprintf(MIPS_text[MIPS_text_top++], "bltz %s, %s", reg1, label);
					free_sReg(reg1);
				}
				else
					;/////////////error
				codeNum++;
			}
		}
		else if (para == 5) {				//+-*/
			sscanf(CodeList[codeNum], "%s = %s %s %s", rd, rs, code_op, rt);
			if (strcmp(code_op,"+")==0) {
				strcpy(mips_op, "addu");
			}
			else if (strcmp(code_op, "-") == 0) {
				strcpy(mips_op, "subu");
			}
			else if (strcmp(code_op, "*") == 0) {
				strcpy(mips_op, "mul");
			}
			else if (strcmp(code_op, "/") == 0) {
				strcpy(mips_op, "div");
			}
			else {
				;//////////error
			}
			strcpy(reg1, load_reg(rd));
			strcpy(reg2, load_reg(rs));
			strcpy(reg3, load_reg(rt));
			sprintf(MIPS_text[MIPS_text_top++], "%s %s, %s, %s",mips_op, reg1, reg2, reg3);
			free_sReg(reg2);
			free_sReg(reg3);
			save_reg(rd, reg1);
		}
	}
}

void delete_move() {
	int i = 0, j = 0;
	char op[30] = { 0 };
	char r1[30], r2[30];
	char other[100] = { 0 };
	for (i = 0; i < MIPS_text_top; i++) {
		sscanf(MIPS_text[i], "%s %s", op, other);
		if (strcmp(op, "move")==0) {
			sscanf(MIPS_text[i], "%s %s %s", op, r1, r2);
			if ((r1[0]==r2[0])&&(r1[1] == r2[1])&&(r1[2] == r2[2])) {
				for (j = i; j < MIPS_text_top; j++) {
					strcpy(MIPS_text[j], MIPS_text[j+1]);
				}
				MIPS_text_top--;
			}
		}
	}
}

int div_factor(char code[])		//中间代码分离，返回元素个数
{
	int i;
	int n = 1;

	for (i = 0; i<strlen(code); i++)
	{
		if (code[i] == ' ')
		{
			n++;
		}
	}
	return n;
}

char* getop(int sym) {
	char n[3];
	if (sym == 18)
		return "<";
	else if (sym == 19)
		return "<=";
	else if (sym == 20)
		return ">";
	else if (sym == 21)
		return ">=";
	else if (sym == 22)
		return "!=";
	else if (sym == 23)
		return "==";
}

int getSymIndex_mips(char *name) {
	int i;
	for (i = 0; ((i < SymList_top) && (currentlev != 0)); i++)		//首先在局部变量表中搜索
	{
		if (strcmp(name, SymList[i].name) == 0 && (SymList[i].lev == currentlev))
		{
			return i;
		}
	}
	for (i = 0; (i < SymList_top); i++)		//若没有搜索到，则在全局变量表中搜索
	{
		if (strcmp(name, SymList[i].name) == 0 && SymList[i].lev == 0)
		{
			return i;
		}
	}
	return -1;
}

int getSymIndex_intercode(char *name) {
	int i;
	for (i = 0; ((i < SymList_top) && (Level_top != 0)); i++)		//首先在局部变量表中搜索
	{
		if (strcmp(name, SymList[i].name) == 0 && (SymList[i].lev == Level_top))
		{
			return i;
		}
	}
	for (i = 0; (i < SymList_top); i++)		//若没有搜索到，则在全局变量表中搜索
	{
		if (strcmp(name, SymList[i].name) == 0 && SymList[i].lev == 0)
		{
			return i;
		}
	}
	return -1;
}

int FindRegType(char Reg[20], int i)		//寻找寄存器所存值的种类
{
	char idname[IDENTNAMESIZE] = { 0 };
	int j,k=0;
	char r1[30], r2[30], r3[30], op1[2], op2[2];
	char* p = NULL;
	for (j = i - 1; j > 0 && p == NULL; j--)
	{
		p = strstr(CodeList[j], Reg);
	}
	j++;
	if (div_factor(CodeList[j]) == 3)
	{

		sscanf(CodeList[j], "%s %s %s", r1, op1, r2);
		if (strcmp(op1, "=") == 0)
		{
			if (strcmp(r2, "RET") == 0)
			{
				sscanf(CodeList[j - 1], "%s %s", r1, r2);
				if (strcmp(r1, "call") == 0)
					return(SymList[getSymIndex_mips(r2)].type);
			}
			else if (r2[0] == '$')
			{
				return FindRegType(r2, j);
			}
			else
			{
				if (isnumber(r2[0]))
					return 1;
				else
				{
					char* q = strchr(r2, '[');
					if (q)
						while (isnumber(r2[k]) || isletter(r2[k])) {
							idname[k] = r2[k];
							k++;
						}
					return(SymList[getSymIndex_mips(idname)].type);
				}
			}
		}
	}
	else if (div_factor(CodeList[j]) == 5)
	{
		return 1;
	}
	return 1;
}