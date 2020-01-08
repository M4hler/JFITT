	int flag;

%x INITIAL_1 INITIAL_2 COMMENT COMMENT_END ONE_LINE ONE_LINE2 DOC DOC_COMMENT NORMAL_COMMENT

%%
	if(flag == 0)
		BEGIN(INITIAL_1);
	else
		BEGIN(INITIAL_2);

<INITIAL_1>\(.*\".*\".*\) {
	BEGIN(INITIAL_1);
	yymore();
	}
<INITIAL_1>"/*" {
	BEGIN(COMMENT);
	yymore();
	}
<INITIAL_1>"//" {
	BEGIN(ONE_LINE);
	yymore();
	}
<COMMENT>[^\*]*\* {
	BEGIN(COMMENT_END);
	yymore();
	}
<COMMENT_END>"/" {
	BEGIN(INITIAL_1);
	}
<COMMENT_END>[^\/] {
	BEGIN(COMMENT);
	yymore();
	}
<ONE_LINE>.* {
	BEGIN(INITIAL_1);
	}
<ONE_LINE>(.*\\\n).* {
	BEGIN(INITIAL_1);
	}

<INITIAL_2>\(.*\".*\".*\) {
	BEGIN(INITIAL_2);
	yymore();
	}
<INITIAL_2>"/*" {
	BEGIN(DOC);
	yymore();
	}
<INITIAL_2>"//" {
	BEGIN(ONE_LINE2);
	yymore();
	}
<ONE_LINE2>.* {
	BEGIN(INITIAL_2);
	}
<ONE_LINE2>(.*\\\n).* {
	BEGIN(INITIAL_2);
	}
<DOC>\* {
	BEGIN(DOC_COMMENT);
	yymore();
	}
<DOC>"*/" {
	BEGIN(INITIAL_2);
	yymore();
	}
<DOC>[^\*]+\* {
	BEGIN(NORMAL_COMMENT);
	yymore();
	}
<NORMAL_COMMENT>"/" {
	BEGIN(INITIAL_2);
	}
<NORMAL_COMMENT>[^\/] {
	BEGIN(DOC);
	yymore();
	}
<DOC_COMMENT>[.\n]*"*/" {
	BEGIN(INITIAL_2);
	yymore();
	}
%%

int main(int argc, char **argv) 
{
	if(argc > 1)
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
