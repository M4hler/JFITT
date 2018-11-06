%x COMMENT ONEDASH TWODASHES

%%
<INITIAL>\<!-- {
	BEGIN(COMMENT);
	yymore();
	}
<COMMENT>[^\-]*\- {
	BEGIN(ONEDASH);
	yymore();
	}
<ONEDASH>\- {
	BEGIN(TWODASHES);
	yymore();
	}
<ONEDASH>[^\-] {
	BEGIN(COMMENT);
	yymore();
	}
<TWODASHES>\> {
	BEGIN(INITIAL);
	}
<TWODASHES>[^\>] {
	BEGIN(COMMENT);
	yymore();	
	}
%%

int main(int argc, char ** argv)
{
	argv++, argc--;
	if(argc > 0)
		yyin = fopen(argv[0], "r+");
	else
		yyin = stdin;

	yyout = fopen("temporary", "w+");

	yylex();
	remove(argv[0]);
	rename("temporary", argv[0]);
}
