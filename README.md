regex       ::= term { '|' term }
term        ::= factor { factor }
factor      ::= atom [ '*' | '+' | '?' ]
atom        ::= char | '(' regex ')'
char        ::= letter | digit | '_' | ...