	int flag;

%x DOC_COMMENTS COMMENTS

OPERATOR	"*"
%%	
		if(flag == 1)
			BEGIN(DOC_COMMENTS);
		else
			BEGIN(COMMENTS);

<DOC_COMMENTS>"//".*	/* Ignore one-line comments */
<DOC_COMMENTS>"//"(.*\\\n)*.*	/*Ignore "multi-line one-line comments" */
<DOC_COMMENTS>"/**/"
<DOC_COMMENTS>"/*"[^{OPERATOR}]+[.\n]*"*/" 
<COMMENTS>"//".*
<COMMENTS>"//"(.*\\\n)*.*
<COMMENTS>"/*"[.\n]*"*/"
%%

int main(int argc, char **argv) 
{
	if(argc > 2)
	{
		if(strcmp(argv[1], "-p") == 0)
			flag = 1;
	}
	else
			flag = 0;

	++argv, --argc;
	if(argc > 0)
		yyin = fopen(argv[0], "r+");
	else 		       
		yyin = stdin;

	yyout = fopen("temporary", "w+");
	yylex();	
}
