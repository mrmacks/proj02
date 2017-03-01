// Author: Max Kimmelman (mkimmelm@u.rochester.edu)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LinkedList.c"

// Tree node
typedef struct Node *Tree;
struct Node{
	char label;
	Tree lc, rs;
};

// Tree functions
Tree makeNode0(char x);
Tree makeNode1(char x, Tree t);
Tree makeNode2(char x, Tree t1, Tree t2);
Tree makeNode3(char x, Tree t1, Tree t2, Tree t3);
void printTree(Tree t, int indent);

// Parse functions
Tree D();
Tree N();
Tree n(); // NT
Tree F();
Tree f(); // FT
Tree T();
Tree t(); // TT
Tree E();

char* firstTerminal;
char* nextTerminal;
int parseFailed = 0;

LinkedList* stack;
LinkedList* table[8][16];
void buildTable();
int isProduction(char x);
int findProduction(char x);
int findInput(char x);

int main(int argc, char* argv[]){
	int numChars = 0;
	for(int i = 1;i < argc;i++){
		char* c = argv[i];
		while(*c != '\0'){
			numChars++;
			c++;
		}
	}

	nextTerminal = malloc(numChars);
	firstTerminal = nextTerminal;
	for(int i = 1;i < argc;i++) strcat(nextTerminal,argv[i]);

	Tree execute = E();
	printf("%i\n",!parseFailed && *nextTerminal == '\0');
	printTree(execute,0);

	nextTerminal = firstTerminal;

	buildTable();

	stack = LinkedList_new();
	LinkedList_add_at_front(stack,'E');
	while(!LinkedList_is_empty(stack)){
		char next = LinkedList_pop(stack);
		
	}
}

// Parse functions
Tree D(){
	if(*nextTerminal - 48 >= 0 && *nextTerminal - 48 <= 9){
		Tree t = makeNode1('D',makeNode0(*nextTerminal));
		nextTerminal++;
		return t;
	}
	parseFailed = 1;
	return NULL;
}

Tree N(){
	Tree DTree, nTree;
	DTree = D();
	if(DTree != NULL){
		nTree = n();
		if(nTree != NULL)
			return makeNode2('N',DTree,nTree);
	}
	parseFailed = 1;
	return NULL;
}

Tree n(){
	if(*nextTerminal - 48 >= 0 && *nextTerminal - 48 <= 9){
		Tree NTree = N();
		if(NTree != NULL)
			return makeNode1('n',NTree);
		parseFailed = 1;
		return NULL;
	}
	return makeNode1('n',makeNode0('e'));
}

Tree F(){
	if(*nextTerminal != '('){
		Tree NTree = N();
		if(NTree != NULL)
			return makeNode1('F',NTree);
	}
	else{
		nextTerminal++;
		Tree ETree = E();
		if(ETree != NULL){
			if(*nextTerminal == ')'){
				nextTerminal++;
				return makeNode3('F',makeNode0('('),ETree,makeNode0(')'));
			}
		}
	}
	parseFailed = 1;
	return NULL;
}

Tree f(){
	if(*nextTerminal == '*' || *nextTerminal == '/'){
		char o = *nextTerminal;
		nextTerminal++;
		Tree FTree = F();
		if(FTree != NULL){
			Tree fTree = f();
			if(fTree != NULL)
				return makeNode3('f',makeNode0(o),FTree,fTree);
		}
		parseFailed = 1;
		return NULL;
	}
	return makeNode1('f',makeNode0('e'));
}

Tree T(){
	Tree FTree = F();
	if(FTree != NULL){
		Tree fTree = f();
		if(fTree != NULL)
			return makeNode2('T',FTree,fTree);
	}
	parseFailed = 1;
	return NULL;
}

Tree t(){
	if(*nextTerminal == '+' || *nextTerminal == '-'){
		char o = *nextTerminal;
		nextTerminal++;
		Tree TTree = T();
		if(TTree != NULL){
			Tree tTree = t();
			if(tTree != NULL)
				return makeNode3('t',makeNode0(o),TTree,tTree);
		}
		parseFailed = 1;
		return NULL;
	}
	return makeNode1('t',makeNode0('e'));
}

Tree E(){
	Tree TTree = T();
	if(TTree != NULL){
		Tree tTree = t();
		if(tTree != NULL)
			return makeNode2('E',TTree,tTree);
	}
	parseFailed = 1;
	return NULL;
}


Tree makeNode0(char x){
	Tree root;
	root = (Tree) malloc(sizeof(struct Node));
	root ->label = x;
	root -> lc = NULL;
	root -> rs = NULL;
	return root;
}

Tree makeNode1(char x, Tree t){
	Tree root;
	root = makeNode0(x);
	root -> lc = t;
	return root;
}

Tree makeNode2(char x, Tree t1, Tree t2){
	Tree root;
	root = makeNode0(x);
	root -> lc = t1;
	t1 -> rs = t2;
	return root;
}

Tree makeNode3(char x, Tree t1, Tree t2, Tree t3){
	Tree root;
	root = makeNode0(x);
	root -> lc = t1;
	t1 -> rs = t2;
	t2 -> rs = t3;
	return root;
}

void printTree(Tree t, int indent){
	if(t == NULL) return;
	for(int i = 0;i < indent;i++) printf("    ");
	printf("%c\n",t -> label);
	Tree child = t -> lc;
	while(child != NULL){
		printTree(child,indent + 1);
		child = child -> rs;
	}
}

int isProduction(char x){
	if(x == 'E') return 1;
	if(x == 't') return 1;
	if(x == 'T') return 1;
	if(x == 'f') return 1;
	if(x == 'F') return 1;
	if(x == 'N') return 1;
	if(x == 'n') return 1;
	if(x == 'D') return 1;
	return 0;
}

int findProduction(char x){
	if(x == 'E') return 0;
	if(x == 't') return 1;
	if(x == 'T') return 2;
	if(x == 'f') return 3;
	if(x == 'F') return 4;
	if(x == 'N') return 5;
	if(x == 'n') return 6;
	if(x == 'D') return 7;
	return -1;
}

int findInput(char x){
	if(x == '(') return 0;
	if(x == ')') return 1;
	if(x == '+') return 2;
	if(x == '-') return 3;
	if(x == '*') return 4;
	if(x == '/') return 5;
	if(x - 48 >= 0 && x - 48 <= 9) return x - 48;
	return -1;
}

void buildTable(){
	LinkedList* assign = LinkedList_new();
	LinkedList_add_at_end(assign,'e');
	for(int i = 0;i < 16;i++) table[1][i] = assign;
	for(int i = 0;i < 16;i++) table[3][i] = assign;
	for(int i = 0;i < 16;i++) table[6][i] = assign;
	assign = LinkedList_new();
	LinkedList_add_at_front(assign,'+');
	LinkedList_add_at_front(assign,'T');
	LinkedList_add_at_front(assign,'t');
	table[1][2] = assign;
	assign = LinkedList_new();
	LinkedList_add_at_front(assign,'-');
	LinkedList_add_at_front(assign,'T');
	LinkedList_add_at_front(assign,'t');
	table[1][3] = assign;
	assign = LinkedList_new();
	LinkedList_add_at_front(assign,'*');
	LinkedList_add_at_front(assign,'F');
	LinkedList_add_at_front(assign,'f');
	table[3][4] = assign;
	assign = LinkedList_new();
	LinkedList_add_at_front(assign,'/');
	LinkedList_add_at_front(assign,'F');
	LinkedList_add_at_front(assign,'f');
	table[3][5] = assign;
	assign = LinkedList_new();
	LinkedList_add_at_front(assign,'N');
	for(int i = 6;i < 16;i++) table[6][i] = assign;
	for(int i = 6;i < 16;i++) table[4][i] = assign;
	assign = LinkedList_new();
	LinkedList_add_at_front(assign,'T');
	LinkedList_add_at_front(assign,'n');
	for(int i = 6;i < 16;i++) table[0][i] = assign;
	assign = LinkedList_new();
	LinkedList_add_at_front(assign,'F');
	LinkedList_add_at_front(assign,'t');
	for(int i = 6;i < 16;i++) table[2][i] = assign;
	assign = LinkedList_new();
	LinkedList_add_at_front(assign,'D');
	LinkedList_add_at_front(assign,'n');
	for(int i = 6;i < 16;i++) table[5][i] = assign;
	for(int j = 0;j < 10;j++){
		assign = LinkedList_new();
		LinkedList_add_at_front(assign,(char)j);
		table[7][j + 6] = assign;
	}
	assign = LinkedList_new();
	LinkedList_add_at_front(assign,'(');
	LinkedList_add_at_front(assign,'E');
	LinkedList_add_at_front(assign,')');
	table[4][0] = assign;
}
